// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CFIFOServerWin32Impl.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 27.06.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifdef _WIN32
#include <deftype>
#include <winerror.h>
//#include <ev.h>
#include <SHARE/fdir.h>
#include <SHARE/Pipe.h>
#include <SHARE/Socket/CFIFOServerWin32Impl.h>

namespace NSHARE
{
const unsigned CFIFOServer::ATOMIC_FIFO_BUUFER =
		8192;
CFIFOServer::CImpl::CImpl(CFIFOServer& aThis) :
		FThis(aThis), FResetEvent(INVALID_HANDLE_VALUE)
{
	VLOG(2) << "Construct CFIFOServer::CImpl: " << this;
	memset(FBufferPipe,0,sizeof(FBufferPipe));
	FIsOpen = false;
}

CFIFOServer::CImpl::~CImpl()
{
	VLOG(2) << "Destruct CFIFOServer::CImpl: " << this;
	MClose();
}
bool CFIFOServer::CImpl::MIsMade() const
{
	return NSHARE::is_the_file_exist(FThis.MGetSetting().FPath.c_str());
}
void CFIFOServer::CImpl::MClose()
{
	VLOG(2) << "Close fifo socket: " << this;
	if (!MIsOpen())
	{
		LOG(ERROR)<< "FIFO has already been close.";
		return;
	}
	FIsOpen = false;

	PulseEvent(FResetEvent);
	CloseHandle(FResetEvent);
	FResetEvent = INVALID_HANDLE_VALUE;
	{
		safe_clients_t::WAccess<> _acsess = FClients.MGetWAccess();
		clients_data_t::iterator _it = _acsess->begin(), _it_end(
				_acsess->end());
		for (; _it != _it_end; ++_it)
		{
			MClose(*_it);
		}
		_acsess->clear();
	}
}
inline void CFIFOServer::CImpl::MReconnectClient(client_data_t & aPipe)
{
	LOG(INFO)<<"Reconnect fifo client "<<aPipe.FPath;
	DisconnectNamedPipe(aPipe.pipe);
	ConnectNamedPipe(aPipe.pipe, &aPipe.ovConnect);
	ResetEvent(aPipe.ovConnect.hEvent);
	ResetEvent(aPipe.ovRead.hEvent);
	aPipe.pipeConnected = false;
}
void CFIFOServer::CImpl::MClose(SHARED_PTR<client_data_t>& aClient)
{
	LOG(INFO)<<"Closing fifo client "<<aClient->FPath;
	DisconnectNamedPipe(aClient->pipe);
	CloseHandle(aClient->ovConnect.hEvent);
	CloseHandle(aClient->ovRead.hEvent);
	CloseHandle(aClient->pipe);
	aClient->pipeConnected=false;
}
void CFIFOServer::CImpl::MClose(CFIFOServer::path_t const& aClientPath)
{
	VLOG(2) << "Client is closing " << aClientPath;
	safe_clients_t::WAccess<> _acsess = FClients.MGetWAccess();
	clients_data_t::iterator _it = _acsess->begin(), _it_end(_acsess->end());
	for (; _it != _it_end; ++_it)
	{
		if ((*_it)->FPath == aClientPath)
		{
			MClose(*_it);
			_acsess->erase(_it);
			VLOG(2) << "The Client " << aClientPath << " is closed.";
			break;
		}
	}
	LOG_IF(WARNING,_it==_it_end) << "The client " << aClientPath
											<< " is not exist.";
}
bool CFIFOServer::CImpl::MOpen()
{
	VLOG(2) << "Open socket :" << this;
	if (!MReconfigure())
		MClose();
	else
	{
		FResetEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		CHECK_NE(FResetEvent, INVALID_HANDLE_VALUE);
	}
	FIsOpen = true;
	return FIsOpen;
}
#define NUMBER_OF_EVENT 1024
int CFIFOServer::CImpl::MWaitForEvent(HANDLE hEvents[])
{
	unsigned _max = 0;
	{
		safe_clients_t::RAccess<> const _acsess = FClients.MGetRAccess();
		clients_data_t::const_iterator _it = _acsess->begin(), _it_end(
				_acsess->end());
		CHECK_LT((_acsess->size() * 2 + 1), NUMBER_OF_EVENT);
		for (; _it != _it_end; ++_it)
		{
			hEvents[_max++] = (*_it)->ovConnect.hEvent;
			hEvents[_max++] = (*_it)->ovRead.hEvent;
		}
	}
	VLOG(2) << "Max: " << _max;
	CHECK_GT(_max, 0);
	//hEvents[_max++]=FResetEvent;
	DWORD numEvent = WaitForMultipleObjects(_max, hEvents, FALSE, INFINITE);
	VLOG(2) << "Events: " << numEvent;
	int _select = numEvent - WAIT_OBJECT_0;
	return _select;
}
ssize_t CFIFOServer::CImpl::MReceiveData(data_t * aBuf, float const aTime,
		CFIFOServer::recvs_from_t *aFrom)
{
	CRAII<CMutex> _block(FMutexRead);
	return MReceiveDataImpl(aBuf, aTime, aFrom);
}
ssize_t CFIFOServer::CImpl::MReceiveDataImpl(data_t * aBuf, float const aTime,
		CFIFOServer::recvs_from_t *aFrom)
{
	VLOG(2) << "Receive data to " << aBuf << " from socket " << "; aTime "
						<< aTime << ":" << this;
	unsigned _count = 0;
	if (MIsOpen())
		do
		{
			HANG_INIT;
			HANG_CHECK;
			HANDLE hEvents[NUMBER_OF_EVENT];
			int _select = MWaitForEvent(hEvents);
			if(_select<0)
			{
				LOG(ERROR)<<"Unknown error "<<GetLastError();
				return -1;
			}
			if(hEvents[_select]==FResetEvent)
			{
				VLOG(2)<<"Reset events";
				continue;
			}
			unsigned _array_pos=_select/2;

			smart_client_t _sm_client;
			bool _is_connect_event=(_select%2==0);
			{
				safe_clients_t::RAccess<> const _acsess = FClients.MGetRAccess();
				if(_array_pos<_acsess->size())
				_sm_client=_acsess->operator[](_array_pos);

				if(!_sm_client/*.get()*/|| //
						(hEvents[_select]!=_sm_client->ovConnect.hEvent && hEvents[_select]!=_sm_client->ovRead.hEvent))
				{
					LOG(ERROR)<<"Try locking for event manually.";
					if(_sm_client.get()) _sm_client=smart_client_t();
					clients_data_t::const_iterator _it = _acsess->begin(), _it_end(
							_acsess->end());
					for (; _it != _it_end; ++_it)
					{
						if((*_it)->ovConnect.hEvent==hEvents[_select] || (*_it)->ovRead.hEvent==hEvents[_select] )
						{
							VLOG(2)<<"Valid event is founded ";
							_sm_client=(*_it);
							break;
						}
					}
				}
			}
			DCHECK_NOTNULL(_sm_client.get());
			if(!_sm_client.get()) continue;

			client_data_t& _client=*_sm_client.get();
			DWORD dwBytesPipe=0;
			//WaitForMultipleObjects работает как всё в виндоусе)))
			//Если у нас событие подключения то
			//сбрасываем все событие и вызываем ф-ию Read.
			//Если вы подумали что она считывает данные то вы ошиблись.
			//Это ведь Виндоус!!! Мы просто регистрируем событие которое будет вызванно
			//когда данные будут прочитаны!!!
			if(_is_connect_event)
			{
				VLOG(2)<<"pipe connecting ...";
				if (GetOverlappedResult(_client.pipe, &_client.ovConnect, &dwBytesPipe, FALSE)==FALSE)
				{
					LOG(ERROR)<<"Error pipe connecting "<<GetLastError();
					continue;
				}
				VLOG(2)<<"Pipe Connected";
				ResetEvent(_client.ovConnect.hEvent);
				ResetEvent(_client.ovRead.hEvent);
				_client.pipeConnected = true;
			}
			else
			{
				if (GetOverlappedResult(_client.pipe, &_client.ovRead, &dwBytesPipe, FALSE)==FALSE)
				{
					DWORD err = GetLastError();
					VLOG(2)<<" An error occurred "<<err;
					switch (err)
					{
						case ERROR_BROKEN_PIPE:
						{
							VLOG(2)<<"Disconnect and reconnect pipe...";
							MReconnectClient(_client);
							continue;
							break;
						}
						default:
						LOG(ERROR)<<"An error occurred."<<err;
						return -1;
						break;
					}
				}
				else
				{
					VLOG(1)<<"Receive "<<dwBytesPipe<<" bytes from "<<_client.FPath;

					_count+=dwBytesPipe;
					CHECK_GT(dwBytesPipe,0);
					data_t::const_iterator _it_beg((data_t::value_type*) FBufferPipe);
					aBuf->insert(aBuf->end(), _it_beg, _it_beg + dwBytesPipe);
					if(aFrom)
					{
						CFIFOServer::recvs_from_t::value_type _rfrom;
						_rfrom.FAddr.MAdd("path",_client.FPath);
						_rfrom.FBufBegin=aBuf->begin();
						_rfrom.FSize=dwBytesPipe;
					}

					ResetEvent(_client.ovRead.hEvent);
				}
				VLOG(2)<<"Receive data "<<dwBytesPipe;
			}

			VLOG(2)<<"Number Of Bytes Transferred "<<dwBytesPipe;

			_client.ovRead.Offset = 0;
			_client.ovRead.OffsetHigh = 0;
			for(;ReadFile(_client.pipe, FBufferPipe, sizeof(FBufferPipe), &dwBytesPipe,
							&_client.ovRead)!=FALSE;)
			{
				CHECK_GT(dwBytesPipe,0);
				_count+=dwBytesPipe;
				data_t::const_iterator _it_beg((data_t::value_type*) FBufferPipe);
				aBuf->insert(aBuf->end(), _it_beg, _it_beg + dwBytesPipe);
				if(aFrom)
				{
					CFIFOServer::recvs_from_t::value_type _rfrom;
					_rfrom.FAddr.MAdd("path",_client.FPath);
					_rfrom.FBufBegin=aBuf->begin();
					_rfrom.FSize=dwBytesPipe;
				}

			}
			DWORD dwErr = GetLastError();
			if( (dwErr == ERROR_IO_PENDING))		//
			{
				VLOG(2)<<"The read operation is still pending";
			}
			else
			{
				LOG(ERROR)<<" An error occurred; disconnect from the client. "<<dwErr;
				MReconnectClient(_client);
			}

		}while(MIsOpen() && !_count);
	return (_count > 0 ? _count : -1);
}
bool CFIFOServer::CImpl::MReconfigure()
{
	VLOG(2) << "Reconfigure socket :" << this;
	CText const& _path = FThis.MGetSetting().FPath;
	clients_t _new_clients = FThis.MGetSetting().FClients;

	CHECK(!_path.empty());

	VLOG_IF(1,_new_clients.empty()) << " There are not clients.";

	//todo count up the number of path. It necessary for MAddNewClient function

	//add Server path
	clients_t::iterator _kt = _new_clients.begin();
	for (; _kt != _new_clients.end(); ++_kt)
		if (_path == *_kt)
			break;
	if (_kt == _new_clients.end())
	{
		_new_clients.push_back(_path);
	};
	//Looking for opened clients what there is in clients_t.
	{
		safe_clients_t::WAccess<> _acsess = FClients.MGetWAccess();
		clients_data_t::iterator _it = _acsess->begin();

		for (; _it != _acsess->end();)
		{

			clients_t::iterator _jt = _new_clients.begin();
			for (; _jt != _new_clients.end(); ++_jt)
				if ((*_it)->FPath == *_jt)
					break;

			if (_jt != _new_clients.end())
			{
				//erase from _clients
				_new_clients.erase(_jt);
			}
			else
			{
				//close client
				MClose(*_it);
				if (_it != _acsess->begin())
				{
					clients_data_t::iterator _r_it = _it;
					--_it;
					_acsess->erase(_r_it);
				}
				else
				{
					_acsess->erase(_it);
					_it = _acsess->begin();
					continue;
				}
			}
			++_it;
		}
		//add new client

		clients_t::iterator _jt = _new_clients.begin();
		for (; _jt != _new_clients.end(); ++_jt)
		{
			smart_client_t _client(MAddNewClientImpl(*_jt));
			if (_client.get())
				_acsess->push_back(_client);
		}
	}
	if (MIsOpen())
		PulseEvent(FResetEvent);
	return true;

}
CText CFIFOServer::CImpl::MGetValidPath(CText const& _path)
{
	const CText _prefix("\\\\");
	CHECK_EQ(_prefix.length(), 2);
	if (_path.compare(0, _prefix.length(), _prefix) != 0)
	{
		LOG(WARNING)<<"There is not prefix "<<_prefix<<" in the fifo path:"<<_path;
		CText _new_path("\\\\.\\pipe\\");
		CText::size_type _pos=_path.find_last_of("\\");
		if(_pos==CText::npos)
		{
			_pos=_path.find_last_of("/");
		}
		if(_pos==CText::npos)
		_new_path+=_path;
		else
		_new_path+=_path.substr(_pos+1,CText::npos);

		return _new_path;
	}
	return _path;
}
void CFIFOServer::CImpl::MAddNewClient(CFIFOServer::path_t const& aTo)
{
	smart_client_t _client(MAddNewClientImpl(aTo));
	if (_client.get())
	{
		VLOG(2) << "Add new client " << aTo;
		safe_clients_t::WAccess<> _acsess = FClients.MGetWAccess();
		_acsess->push_back(_client);
	}
	if (MIsOpen())
		PulseEvent(FResetEvent);

}
CFIFOServer::CImpl::smart_client_t CFIFOServer::CImpl::MAddNewClientImpl(
		const CFIFOServer::path_t& aTo)
{
	VLOG(1) << " Add new client " << aTo;
	smart_client_t _pipe(new client_data_t);
	_pipe->FPath = MGetValidPath(aTo);
	_pipe->pipeConnected = false;
	_pipe->pipe = CreateNamedPipe(_pipe->FPath.c_str(),
			PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, 0, 1, ATOMIC_FIFO_BUUFER,
			ATOMIC_FIFO_BUUFER, NMPWAIT_WAIT_FOREVER, NULL);
	if (_pipe->pipe == INVALID_HANDLE_VALUE)
	{
		LOG(DFATAL)<<"Could not create named pipe "<< GetLastError();
		return smart_client_t();
	}
	_pipe->ovConnect.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	_pipe->ovRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (_pipe->ovConnect.hEvent == INVALID_HANDLE_VALUE
			|| _pipe->ovRead.hEvent == INVALID_HANDLE_VALUE)
	{
		LOG(DFATAL)<<"Could not create named pipe "<< GetLastError();
		CloseHandle(_pipe->pipe);
		CloseHandle(_pipe->ovConnect.hEvent);
		CloseHandle(_pipe->ovRead.hEvent);
		return smart_client_t();
	}
	if (ConnectNamedPipe(_pipe->pipe, &_pipe->ovConnect) == FALSE)
	{
		DWORD error = GetLastError();
		if (error != ERROR_IO_PENDING)
		{
			LOG(DFATAL)<<"Error ConnectNamedPipe event "<< error;
			return smart_client_t();
		}
		else
		{
			VLOG(2)<<"Pedding";
		}
	}
	_pipe->FTime = ::time(NULL);
	return _pipe;

}

bool CFIFOServer::CImpl::MIsOpen() const
{
	return FIsOpen;
}

size_t CFIFOServer::CImpl::MAvailable() const
{
	size_t _val = 0;

	safe_clients_t::RAccess<> const _acsess = FClients.MGetRAccess();
	clients_data_t::const_iterator _it = _acsess->begin(), _it_end(
			_acsess->end());
	for (; _it != _it_end; ++_it)
	{
		if ((*_it)->pipeConnected)
		{
			DWORD avail = 0;
			if (PeekNamedPipe(&(*_it)->ovRead, NULL, 0, NULL, &avail,
					NULL) == FALSE)
			{
				VLOG(2) << "Available error:" << GetLastError();
			}
			else
			{
				VLOG(2) << avail << " bytes available for reading from "
									<< (*_it)->FPath;
				_val += static_cast<size_t>(avail);
			}
		}
	}
	VLOG(2) << _val << " bytes available for reading from all";
	return static_cast<size_t>(_val);

}
CFIFOServer::sent_state_t CFIFOServer::CImpl::MSend(void const* const aData, std::size_t aSize,
		NSHARE::CConfig const& aTo)
{
	CText _path;
	if(!aTo.MGetIfSet("path",_path))
		return sent_state_t(sent_state_t::E_ERROR,0);
	CRAII<CMutex> _block(FMutexWrite);
	return MSendImpl(aData, aSize, _path);
}
CFIFOServer::sent_state_t CFIFOServer::CImpl::MSend(const void* const aData, std::size_t aSize, CFIFOServer::path_t const& aTo)
{
	CRAII<CMutex> _block(FMutexWrite);
	return MSendImpl(aData, aSize, aTo);
}
CFIFOServer::sent_state_t CFIFOServer::CImpl::MSendImpl(void const* const aData, std::size_t aSize,
		CFIFOServer::path_t const& aPath)
{
	VLOG(2) << "Send data " << aData << " (size=" << aSize << ") to " << aPath
						<< ":" << this;
	if (!MIsOpen())
		return sent_state_t(sent_state_t::E_ERROR,0);

	CText _path=MGetValidPath(aPath);
	clients_data_t _clients;
	{
		safe_clients_t::RAccess<> const _acsess = FClients.MGetRAccess();
		clients_data_t::const_iterator _it = _acsess->begin(), _it_end(
				_acsess->end());
		for (; _it != _it_end; ++_it)
		{
			if ((*_it)->FPath == _path)
				_clients.push_back(*_it);
		}
	}
	if (_clients.empty())
		return sent_state_t(sent_state_t::E_ERROR,0);
	clients_data_t::const_iterator _it = _clients.begin();
	for (; _it != _clients.end(); ++_it)
	{
		int _size=(int)aSize;
		for(HANG_INIT;_size>0;HANG_CHECK)
		{
			VLOG(2)<<"Send data to "<<(*_it)->FPath;
			DWORD dwWritten;
			CHECK_LE(aSize, std::numeric_limits<DWORD>::max());
			if(WriteFile((*_it)->pipe, aData, static_cast<DWORD>(aSize), &dwWritten, NULL)==FALSE)
			{
				LOG(ERROR)<<"Error during write "<<GetLastError();
				return sent_state_t(sent_state_t::E_ERROR,aSize-_size);
			}
			_size-=dwWritten;
		}
	}

	return sent_state_t(sent_state_t::E_SENDED,aSize);
}

const CSocket& CFIFOServer::CImpl::MGetSocket() const
{
	return CSocket::sMNullSocket();
}
void CFIFOServer::CImpl::MFlush(const eFlush&)
{
	LOG(WARNING)<< "FIFO flush method is not implemented.";
	safe_clients_t::RAccess<> const _acsess = FClients.MGetRAccess();
	clients_data_t::const_iterator _it = _acsess->begin(), _it_end(
			_acsess->end());
	for (; _it != _it_end;++_it)
	{
		FlushFileBuffers((*_it)->pipe);
	}
}
void CFIFOServer::CImpl::MSerialize(NSHARE::CConfig & aConf) const
{
	aConf.MAdd("buf",sizeof(FBufferPipe));
	aConf.MAdd("con",FIsOpen);
	const safe_clients_t::RAccess<> _acs=FClients.MGetRAccess();
	clients_data_t const& _cls = _acs.MGet();
	clients_data_t::const_iterator _it = _cls.begin(), _it_end(_cls.end());

	for (; _it != _it_end; ++_it)
	{
		CHECK_NOTNULL(_it->get());

		NSHARE::CConfig _conf("cl");
		_conf.MAdd("path", _it->get()->FPath);
		_conf.MAdd("time", _it->get()->FTime);
		_conf.MAdd("con", _it->get()->pipeConnected);
		aConf.MAdd(_conf);
	}
}
} // namespace NSHARE
#endif //#ifdef _WIN32
