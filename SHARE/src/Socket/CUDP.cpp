// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CUDP.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 26.03.2013
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  
#include <deftype>
#include <Net.h>
#include <uclock.h>
#include <string.h>
#include <console.h>
#include <errno.h>
#if !defined(_WIN32)
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>                     // sockaddr_in
#include <arpa/inet.h>                      // htons, htonl
#include <unistd.h>
#else
#include <ws2tcpip.h> //socklen_t
#endif //#ifndef WIN32
#include <Socket/print_socket_error.h>
#define UDP_BUFFER_SIZE USHRT_MAX

/*
 #ifdef __MINGW32__
 #	define _WIN32
 #endif
 */
namespace NSHARE
{
const NSHARE::CText CUDP::NAME="udp";
CUDP::CUDP(NSHARE::CConfig const& aConf) :
		FSock(-1)
{
	FBuffer = NULL;
	FBufferSize = 0;
	FMaxSize = 0;
	param_t _param(aConf);
	LOG_IF(DFATAL,!_param.MIsValid())<<"Configure for udp is not valid "<<aConf;
	MOpen(_param);
}
CUDP::CUDP(const param_t&  aParam) :
		FSock(-1)
{
	FBuffer = NULL;
	FBufferSize = 0;
	FMaxSize = 0;
	if(aParam.MIsValid())
		MOpen(aParam);
}
CUDP::~CUDP()
{
	MClose();
	if (FBufferSize)
		delete[] FBuffer;
}
void CUDP::MClose()
{
	if (MIsOpen())
	{
		FSock.MClose();
		//FSockSelect.MRemoveAll();
	}
	/*
	 #if defined(_WIN32)
	 closesocket(MGetSocket());
	 WSACleanup();
	 #else
	 close(MGetSocket());
	 #endif
	 */
//	FPort = 0;
}

inline CSocket CUDP::MNewSocket()
{
	return static_cast<CSocket::socket_t>(socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP));
}
bool CUDP::MOpen(const param_t& aParam, int aFlags)
{
	if(aParam.FAddr.MGetConst().MIsValid())
		MSetSendAddress(aParam.FAddr.MGetConst());
	FParam=aParam;
	return MOpen();
}
void CUDP::MSetSendAddress(net_address const& aAddress)
{
	VLOG(2) << "Setting  " << aAddress << " as default address";
	if (aAddress.MIsValid())
	{
		FParam.FAddr = aAddress;
		MSetAddress(aAddress, &FAddr.MGet());
	}else
	{
		FParam.FAddr.MUnSet();
		FAddr.MUnSet();
	}
}
bool CUDP::MOpen()
{
	VLOG(2)<<"Open UDP "<<FParam.FPort;
	LOG_IF(WARNING, FSock.MIsValid())
			<< "Host socket is valid. Leak socket can occur! "<<FSock;
	if (FSock.MIsValid())
		return false;
	if(!FParam.MIsValid())//auto generate
		FParam.FPort=0;
	FSock = MNewSocket();
	if (!FSock.MIsValid())
	{
		LOG(ERROR)<< FSock << print_socket_error();
		return false;
	}
	if (FParam.FPort)
		MReUseAddr(FSock);

	FMaxSize = 0;
	sockaddr_in _addr;
	net_address _ip(FParam.FPort);
	MSetAddress(_ip, &_addr);

	MSettingBufSize(FSock);
	if (MBindSocket(MGetSocket().MGet(), _addr) < 0)
	{
		LOG(ERROR)<<"Binding "<<FParam.FPort<<" failed "<< print_socket_error();
		FSock.MClose();
		return false;
	}
	if (!FParam.FPort) //if Port is 0 it means select any available port.
	{
		//struct sockaddr_in _addr;
#ifdef _WIN32
		int _len = sizeof(_addr);
#else
		socklen_t _len = sizeof(_addr);
#endif
		int _val = getsockname(MGetSocket().MGet(), (sockaddr*) &_addr, &_len);
		(void) _val;
		LOG_IF(FATAL,_val!=0) << print_socket_error();
		FParam.FPort = ntohs(_addr.sin_port);
		LOG(INFO)<<"Chosen port number "<<FParam.FPort<<" as the random port.";
	}
	//FSockSelect.MAddSocket(FSock);
	VLOG(0) << "The UDP socket has been opened successfully.";
	FMaxSize = static_cast<size_t>(MGetSendBufSize(MGetSocket()));
	return true;
}
bool CUDP::MReOpen()
{
	MClose();
	return MOpen();
}
bool CUDP::MIsOpen() const
{
	return MGetSocket().MIsValid();
}
size_t CUDP::MAvailable() const
{
	return CNetBase::MAvailable(FSock);
}
ssize_t CUDP::MReceiveData(net_address* aFrom, data_t * aBuf, float const aTime)
{
	VLOG(2) << "Receive data to " << aBuf << ", max time " << aTime;

	if (!MIsOpen())
	{
		LOG(ERROR)<< "The socket is not working";
		return 0;
	}
	int _recvd = 0;
	_recvd = recvfrom(MGetSocket().MGet(), (char*) &_recvd, 1, MSG_PEEK,
			NULL, NULL);
#ifdef _WIN32
	//WTF? Fucking windows. If the received msg is more than the buffer size,
	//it generates the WSAEMSGSIZE error. But The buffer is always small as
	//it is 1 byte!!!!!
	if (_recvd >= 0//
					|| (_recvd == SOCKET_ERROR && WSAEMSGSIZE == ::WSAGetLastError()))
#else
	if (_recvd >= 0)
#endif
	{
		if (MGetSocket().MIsValid()) //closed
		{
			struct sockaddr_in _addr;
			socklen_t _len = sizeof(_addr);
			const size_t _befor = aBuf->size();
			const size_t _size = MAvailable();

			VLOG(2) << "Available " << _size << " bytes";
			VLOG_IF(1,!_size) << "No data on socket " << MGetSocket();
			aBuf->resize(_befor + _size);
			CHECK_GT(aBuf->size(), 0);
			CHECK_GE(aBuf->size(), _size);
			data_t::value_type* _pbegin=aBuf->ptr()+(aBuf->size() - _size);
#ifdef _WIN32
			_recvd = recvfrom(MGetSocket().MGet(),
					(char*) _pbegin, (int)_size,
					0, (struct sockaddr*) &_addr, &_len);
#else
			_recvd = recvfrom(MGetSocket().MGet(),
					_pbegin, _size,
					0, (struct sockaddr*) &_addr, &_len);
#endif
			VLOG(2) << "Recvd=" << _recvd;
			if (aFrom)
			{
				LOG_IF(ERROR,_len==0) << "Length of address is 0.";
				if (_len > 0)
				{
					aFrom->ip = inet_ntoa(_addr.sin_addr);
					aFrom->port = ntohs(_addr.sin_port);
					VLOG(2) << "From " << *aFrom;
				}
			}
			aBuf->resize(_befor + _recvd);
			VLOG(0) << "Reads " << _recvd << " bytes";
			FDiagnostic.MRecv(_recvd);
		}
		else
			_recvd = -1;
	}
	if (_recvd <= 0)
	{
#ifdef _WIN32
			int const _errno=::WSAGetLastError();
			if(_errno==WSAECONNRESET)//Thank you Bill Gates for your care!
				//It's error  mean that  The packet has been SENT to closed port
				{
					VLOG(1)<< "WSAECONNRESET"<<_errno;
					_recvd=0;
				}
			else
			{
				LOG(ERROR)<< "Unknown error:"<<_errno;
				_recvd=-1;
			}
#else
			LOG(ERROR)<< "Unknown error:" << print_socket_error();
			_recvd=-1;
#endif
	}
	return _recvd;
}
CUDP::sent_state_t CUDP::MSend(void const* pData, size_t nSize, NSHARE::CConfig const& aTo)
{
	net_address _addr(aTo);
	LOG_IF(DFATAL,!_addr.MIsValid()) << "Invalide type of smart_addr";
	if (!_addr.MIsValid())
		return sent_state_t(E_INVALID_VALUE,0);
	return MSend(pData, nSize, _addr);
}
CUDP::sent_state_t CUDP::MSend(void const* pData, size_t nSize)
{
	if (!FParam.FAddr.MIs())
	{
		LOG(ERROR)<<"The Default address is not setting";
		return sent_state_t(E_INVALID_VALUE,0);
	}
	return MSend(pData, nSize, FAddr.MGet());
}
CUDP::sent_state_t CUDP::MSend(void const* pData, size_t nSize, net_address const& aAddress)
{
	VLOG(1) << "Send " << pData << " size=" << nSize << " to " << aAddress;
	struct sockaddr_in _Sa;
	MSetAddress(aAddress, &_Sa);
	return MSend(pData, nSize, _Sa);
}
CUDP::sent_state_t CUDP::MSend(void const* pData, size_t nSize,
		struct sockaddr_in const& aAddress)
{
	if (!MIsOpen())
	{
		LOG(WARNING)<<"The Port is not opened.";
		return sent_state_t(E_NOT_OPENED,0);
	}
	VLOG(2)<<"Sending "<<pData<<" of "<<nSize;
	if(FMaxSize && nSize>FMaxSize)
	FMaxSize=static_cast<size_t>(MGetSendBufSize(MGetSocket()));

	LOG_IF(ERROR,(FMaxSize>0)&&FMaxSize<(size_t)nSize)<<" Message may be too long. Max size = "<<FMaxSize<<". Sending "<<nSize;
#ifdef _WIN32
	int _rval=sendto(MGetSocket().MGet(), reinterpret_cast<char const*>(pData), static_cast<int>(nSize), 0,
			(struct sockaddr *) &aAddress, sizeof(aAddress));
	LOG_IF(ERROR,_rval<0)<<"Send error "<<print_socket_error();
	VLOG_IF(2,_rval>=0)<<"Sent "<<_rval<<" bytes.";
	bool _is= _rval>0;
#else
	int _rval= sendto(MGetSocket().MGet(), pData, nSize, 0, (struct sockaddr *) &aAddress,
			sizeof(aAddress));

	LOG_IF(ERROR,_rval<0)<<"Send error "<<print_socket_error();
	VLOG_IF(2,_rval>=0)<<"Sent "<<_rval<<" bytes.";
	bool const _is= _rval>0;
#endif
	if(_is) FDiagnostic.MSend(_rval);
	return _is?sent_state_t(E_SENDED,_rval):sent_state_t(E_ERROR,0);
}
CUDP::param_t const& CUDP::MGetInitParam() const
{
	return FParam;
}
const CSocket& CUDP::MGetSocket(void) const
{
	return FSock;
}
in_port_t CUDP::MGetPort() const
{
	return FParam.FPort;
}
NSHARE::CConfig CUDP::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	_conf.MAdd(MGetInitParam().MSerialize());
	_conf.MAdd("open",MIsOpen());
	_conf.MAdd(FDiagnostic.MSerialize());
	return _conf;
}
std::ostream & CUDP::MPrint(std::ostream & aStream) const
{
	if (MIsOpen())
		aStream << NSHARE::NCONSOLE::eFG_GREEN << "Opened.";
	else
		aStream << NSHARE::NCONSOLE::eFG_RED << "Close.";
	aStream << NSHARE::NCONSOLE::eNORMAL;
	aStream << " Type: UDP. " << "Parameters: local port=" << FParam.FPort;
	if (FParam.FAddr.MIs())
		aStream << "; " << FParam.FAddr.MGetConst() << ".";
	else
		aStream<<"There is not sending ip.";
	return aStream;
}

const CText CUDP::param_t::UDP_PORT = "port";
const CText CUDP::param_t::ADDR_TO = "toip";

CUDP::param_t::param_t(in_port_t aPort, net_address const& aParam)
{
	FPort=aPort;
	if(aParam.MIsValid())
		FAddr=aParam;
}
CUDP::param_t::param_t()
{
	FPort = std::numeric_limits<in_port_t>::max();
}
CUDP::param_t::param_t(NSHARE::CConfig const& aConf)
{
	FPort = std::numeric_limits<in_port_t>::max();

	if (aConf.MGetIfSet(UDP_PORT, FPort))
	{
		if (aConf.MIsChild(ADDR_TO))
		{
			net_address _addr(aConf.MChild(ADDR_TO));
			if(_addr.MIsValid())
				FAddr=_addr;
			else
				LOG(DFATAL)<<"Invalid param "<<aConf.MChild(ADDR_TO);
		}
	}
}
bool CUDP::param_t::MIsValid() const
{
	return FPort!=std::numeric_limits<in_port_t>::max();
}
CConfig CUDP::param_t::MSerialize() const
{
	CConfig _conf("param");
	if (MIsValid())
	{
		_conf.MSet(UDP_PORT, FPort);
		if (FAddr.MIs())
			_conf.MAdd(/*ADDR_TO,*/ FAddr.MGetConst().MSerialize());
	}
	return _conf;
}
} //namespace NSHARE

