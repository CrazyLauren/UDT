// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CLoopBack.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 02.08.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
 
#include <share/Net.h>
#include <share/console.h>
#include <share/Socket/CLoopBack.h>

namespace NSHARE
{
const CText CLoopBack::MAX_SPEED = "max_speed";
const NSHARE::CText CLoopBack::NAME="loop";
CLoopBack::CLoopBack(NSHARE::CConfig const& aConf) :
		FMaxSpeed(0)
{
	aConf.MGetIfSet(MAX_SPEED, FMaxSpeed);
	MOpen();
	FCurrentSpeed = 0;
	FCurrentTime = NSHARE::get_time();
}
CLoopBack::CLoopBack(bool aNeedOpen, unsigned aMaxSpeed) :
		FMaxSpeed(aMaxSpeed)
{
	FIsOpened = false;
	if (aNeedOpen)
		MOpen();
	FCurrentSpeed = 0;
	FCurrentTime = NSHARE::get_time();
}
CSocket const& CLoopBack::MGetSocket() const
{
	return FClient.MGetSocket();
}
bool CLoopBack::MReOpen()
{
	MClose();
	return MOpen();
}
void CLoopBack::MClose()
{
	VLOG(2) << "Try close loop socked." << FServer.MIsOpen() << " Inside:"
						<< FClient.MIsOpen();

	if (FServer.MIsOpen())
	{
		FServer.MClose();
	}
	else
		LOG(WARNING)<<"WTF? The Inside loop has been closed.";

	if (FClient.MIsOpen())
	{
		FClient.MClose();
	}
	else
		LOG(WARNING)<<"WTF? The loop has been closed.";
	CHECK(!FServer.MIsOpen());
	CHECK(!FClient.MIsOpen());
	FIsOpened = false;
}
CLoopBack::sent_state_t CLoopBack::MSend(data_t const& aVal)
{
	if (!FServer.MIsOpen())
	{
		LOG(ERROR)<< "The Loop back socket is invalid";
		return sent_state_t(sent_state_t::E_NOT_OPENED,0);
	}
	CRAII<CMutex> _block(FMutex);
	bool const _empty=FData.empty();
	if(FMaxSpeed && _empty)
	FCurrentTime = NSHARE::get_time();
	FData.push_back(aVal);
	VLOG(5)<<"Buffers length= "<<FData.size();
	int _some=0;

	sent_state_t const _state=FServer.MSend(&_some,sizeof(_some));

	sent_state_t const _rval(_state.MIs()?sent_state_t::E_SENDED:sent_state_t::E_ERROR,aVal.size());
	FDiagnostic.MSend(_rval);
	return _rval;
}
CLoopBack::sent_state_t CLoopBack::MSend(const void* pData, size_t nSize)
{
	VLOG(1) << "Send " << pData << " size=" << nSize << " to loop back";
	NSHARE::CBuffer _buffer(nSize, 0);
	memcpy(_buffer.ptr(), pData, nSize);

	return MSend(_buffer);
}

size_t CLoopBack::MAvailable() const
{
	CRAII<CMutex> _block(FMutex);
	size_t _size = 0;
	std::list<data_t>::const_iterator _it = FData.begin(), _it_end(FData.end());
	for (; _it != _it_end; ++_it)
	{
		_size += _it->size();
	}
	return _size;
}
bool CLoopBack::MOpen()
{
	LOG_IF(ERROR, FClient.MIsOpen())
												<< "Loop socket is valid. Leak socket can occur!";

	//	LOG_IF(WARNING, FIsLoopConnected)
	//			<< "Loop socket is connected. Leak socket can occur!";
	LOG_IF(ERROR, FServer.MIsOpen())
												<< "Loop socket is connected. Leak socket can occur!";

	FClient.MOpen();
	CHECK(FClient.MGetPort());
	net_address _addr(INADDR_LOOPBACK, FClient.MGetPort());
	VLOG(2) << "Client port:" << FClient.MGetPort();
	FServer.MOpen(CUDP::settings_t(0, _addr));

	CHECK(FClient.MIsOpen());
	CHECK(FServer.MIsOpen());

	CHECK(FServer.MGetPort());
	VLOG(2) << "Server port:" << FServer.MGetPort();
	_addr.FPort = FServer.MGetPort();
	FClient.MSetSendAddress(_addr);

	size_t _rval = 0;
	//cheking connection
	CHECK_EQ(FClient.MAvailable(), 0u);
	FServer.MSend(&_rval, sizeof(_rval));
	usleep(10000);		//fixme wait for received
	CHECK_GE(FClient.MAvailable(), sizeof(_rval));
	return FIsOpened = MReadAll();
}
bool CLoopBack::MIsOpen() const
{
	return FIsOpened;
}
ssize_t CLoopBack::MReceiveData(data_t *aData, float const aTime)
{
	ssize_t _size = 0;

	do
	{
		_size = FClient.MReceiveData(aData, aTime);
		if (_size > 0)
		{
			CHECK_NOTNULL(aData);
			CRAII<CMutex> _block(FMutex);

			if (FData.empty())
			{
				LOG(ERROR)<< "The Data is not exist.";
				_size = 0;
				continue;
			}
			FData.front().MMoveTo(*aData);
			FData.pop_front();
			_size = aData->size();
			if (FMaxSpeed)
			{
				double _current = NSHARE::get_time();
				double _expect = FCurrentTime
						+ (double) _size / (double) FMaxSpeed;		//sec

						//As the mutex has been locked.
						//The Send method can be completed while the sleep
						//is not timeout.
				for (; _expect > _current; _current = NSHARE::get_time())
				{
					if ((_expect - _current) >= 1.0)
						NSHARE::sleep(
								static_cast<unsigned>(_expect - _current));
					else
						NSHARE::usleep(
								static_cast<unsigned>((_expect - _current)
										* 1000 * 1000));
				}
				VLOG(5) << "Receive " << _size << " bytes time= "
									<< ((_expect - FCurrentTime) * 1000 * 1000)
									<< " ms";
				FCurrentTime = NSHARE::get_time();
				DCHECK_GE(FCurrentTime, _expect);

			}
		}
		else
		{
			LOG(ERROR)<<"Unknown error";
			break;
		}
	} while (_size == 0);
	if(_size>0)
		FDiagnostic.MRecv(_size);
	return _size;
}
bool CLoopBack::MReadAll()
{
	CHECK(FClient.MIsOpen());
	data_t _data;
	VLOG(2) << "Receiving loop back msg.";
	ssize_t _recvd = FClient.MReceiveData(&_data, 0);
	{
		CRAII<CMutex> _block(FMutex);
		FData.clear();
	}
	VLOG(2) << "Rval=" << _recvd;
	if (_recvd <= 0)
	{
		FClient.MClose();
		LOG(DFATAL)<<"Error during receiving loop back msg.";
		return false;
	}

	VLOG(2) << "It's loop back.";
	return true;
}
NSHARE::CConfig CLoopBack::MSettings(void) const
{
	NSHARE::CConfig _conf(NAME);
	_conf.MAdd(MAX_SPEED,FMaxSpeed);
	_conf.MAdd("curt",FCurrentTime);
	_conf.MAdd("curs",FCurrentSpeed);
	return _conf;
}
NSHARE::CConfig CLoopBack::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	_conf.MAdd(MAX_SPEED,FMaxSpeed);
	_conf.MAdd("open",MIsOpen());
	_conf.MAdd("curt",FCurrentTime);
	_conf.MAdd("curs",FCurrentSpeed);
	_conf.MAdd(FDiagnostic.MSerialize());
	return _conf;
}
std::ostream & CLoopBack::MPrint(std::ostream & aStream) const
{
	if (MIsOpen())
		aStream << NSHARE::NCONSOLE::eFG_GREEN << "Opened.";
	else
		return aStream << NSHARE::NCONSOLE::eFG_RED << "Closed."
				<< NSHARE::NCONSOLE::eNORMAL;
	aStream << NSHARE::NCONSOLE::eNORMAL;
	aStream << " Type: LoopBack. Speed=";
	if (FMaxSpeed)
		aStream << FMaxSpeed << " b/s";
	else
		aStream << "Unlimited";
	aStream << ". Using ";
	aStream << FServer;
	aStream << ". And " << FClient;
	return aStream;
}
} /* namespace NSHARE */
