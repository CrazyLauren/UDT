/*
 * CBuffering.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 10.08.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <udt_share.h>
#include <shared_types.h>
#include "../core/kernel_type.h"
#include "CBuffering.h"

namespace NUDT
{
const NSHARE::CText CBuffering::NAME="buf";

const NSHARE::CText CBuffering::MAX_BUFFER_SIZE_FOR_CHANNEL =
		"maxbuf";
const NSHARE::CText CBuffering::MAX_SM_BUFFER = "maxsmbuf";
const NSHARE::CText CBuffering::SM_FLAG = "sm";

static unsigned _buf_channel_size(NSHARE::CConfig const& aConf)
{
	unsigned _size = 0;
	if (aConf.MGetIfSet(CBuffering::MAX_BUFFER_SIZE_FOR_CHANNEL, _size))
	{
		VLOG(2) << "Buffer size " << _size;
	}
	return _size;
}
static unsigned _buf_sm_size(NSHARE::CConfig const& aConf)
{
	unsigned _size = 0;
	bool _is_sm=false;
	aConf.MGetIfSet(CBuffering::SM_FLAG,_is_sm);
	if (!_is_sm &&aConf.MGetIfSet(CBuffering::MAX_SM_BUFFER, _size))
	{
		VLOG(2) << "Max sm size " << _size;
	}
	return _size;

}
static uint32_t g_current_SM_buffer=0;
CBuffering::CBuffering(NSHARE::CConfig const& aConf) :
		FMaxBufferSizeForChannel(_buf_channel_size(aConf)), //
		FMaxSMBufferSize( _buf_sm_size(aConf)), //
		FCurrentSMBuffer(g_current_SM_buffer)
{
	VLOG(5)<<MSerialize().MToJSON(true);
	MInit();
}

void CBuffering::MInit()
{
	FSMBuffer = 0;
	FBuffer = 0;
	FHandlingBuffer = 0;
	FDoingThread = 0;
	memset(&FDiagnostic, 0, sizeof(FDiagnostic));
}

CBuffering::CBuffering(unsigned const aMaxBufferSizeForChannel) : //for sm only
		FMaxBufferSizeForChannel(aMaxBufferSizeForChannel),//
		FMaxSMBufferSize(0), //no limitation
		FCurrentSMBuffer(g_current_SM_buffer)
{
	MInit();
}
CBuffering::CBuffering(unsigned const aMaxBufferSizeForChannel,
		unsigned const aMaxSMBufferSize, uint32_t& aSMBuffer) :
		FMaxBufferSizeForChannel(aMaxBufferSizeForChannel), //
		FMaxSMBufferSize(aMaxSMBufferSize), //
		FCurrentSMBuffer(aSMBuffer) //
{
	MInit();
}

CBuffering::~CBuffering()
{
}
bool CBuffering::MTry(data_list_t & aTo)
{
	if(MIsEmpty())
		return false;

	aTo.splice(aTo.end(), FData);
	FDiagnostic.FMaxSmBuffering=std::max(FDiagnostic.FMaxSmBuffering,FSMBuffer);


	FHandlingBuffer=FBuffer;
	FCurrentSMBuffer-=FSMBuffer;//todo atomic
	FSMBuffer=0;

	CHECK_EQ(FDoingThread,0);

	FDoingThread = NSHARE::CThread::sMThreadId();
	VLOG(2)<<"Start handling by "<<FDoingThread;
	return true;
}
void CBuffering::MFinish(data_list_t & aNoSent,bool aTryAgain)
{
	FDiagnostic.FMaxBuffering = std::max(FDiagnostic.FMaxBuffering,
			FBuffer);
	FDiagnostic.FHandledDataSize += FHandlingBuffer;

	if (aNoSent.empty())
	{

		VLOG(2) << "Finish handling by " << FDoingThread << " Buffer="
							<< (FBuffer-FHandlingBuffer) << " bytes, Sent="
							<< FHandlingBuffer << " bytes.";
		FBuffer -= FHandlingBuffer;
	}
	else
	{
		data_list_t::const_iterator _it = aNoSent.begin(), _it_end =
				aNoSent.end();
		size_t _num=0;
		for(;_it!=_it_end;++_it)
		{
			_num += _it->FData.size();
			LOG(ERROR)<<"Cannot handle "<<_it->FDataId;
		}
		FDiagnostic.FHandledDataSize-=_num;
		FBuffer+=_num;
		if(aTryAgain) FData.splice(FData.begin(), aNoSent);
		LOG(ERROR)<<"Could not handle "<<_num<<" bytes.";
	}


	CHECK_EQ(FDoingThread,NSHARE::CThread::sMThreadId());
	FDoingThread=0;
	FHandlingBuffer=0;
}
bool CBuffering::MIsWorking() const
{
	return FDoingThread>0;
}
bool CBuffering::MPut(user_data_t const& aData)
{
	//Thread blocking protection and buffering limitation.
	//if another thread is handling data to aFor, Let its put our buffer.
	const size_t _b_size = aData.FData.size();
	if (FMaxBufferSizeForChannel)
	{
		if ((FBuffer + _b_size) > FMaxBufferSizeForChannel)
		{
			//todo set overload flag
			LOG(ERROR)<<"Cannot put data. The buffering is overloaded. Cur="<<FBuffer<<" bytes; MaxSize="<<FMaxBufferSizeForChannel;
			return false;
		}
		FBuffer+=_b_size;
	}
	if (FMaxSMBufferSize
			&& (FMaxSMBufferSize < FMaxBufferSizeForChannel
					|| !FMaxBufferSizeForChannel))
	{
		if (!aData.FData.MIsAllocatorEqual(NSHARE::CBuffer::sMDefAllaocter()))
		{
			VLOG(2) << "Handle by SM";
			if ((FCurrentSMBuffer + _b_size) < FMaxSMBufferSize)
			{
				FCurrentSMBuffer += _b_size;//todo atomic
				FSMBuffer += _b_size;

				VLOG(2) << "Increment sm buffer " << FCurrentSMBuffer;
			}
			else
			{
				LOG(WARNING)<<"SM limitation="<<FCurrentSMBuffer<<"; max="<<FMaxSMBufferSize;
				user_data_t _fix_data;
				_fix_data.FData.deep_copy(aData.FData);
				_fix_data.FDataId=aData.FDataId;
				FData.push_back(_fix_data);
				return true;
			}
		}
	}
	VLOG(2)<<"Push data to buffer.";
	FData.push_back(aData);
	return true;
}
NSHARE::CConfig CBuffering::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	_conf.MAdd(MAX_BUFFER_SIZE_FOR_CHANNEL,FMaxBufferSizeForChannel);
	_conf.MAdd(MAX_SM_BUFFER,FMaxSMBufferSize);

	_conf.MAdd("hmaxbuf",FDiagnostic.FMaxBuffering);
	_conf.MAdd("hmaxsmbuf",FDiagnostic.FMaxSmBuffering);
	_conf.MAdd("hsize",FDiagnostic.FHandledDataSize);
	_conf.MAdd("cursmbuf",g_current_SM_buffer);
	return _conf;
}
} /* namespace NUDT */
