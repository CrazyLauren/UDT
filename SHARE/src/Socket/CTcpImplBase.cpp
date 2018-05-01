/*
 * CTcpImplBase.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 07.09.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#include <Net.h>
#include <console.h>
#include <Socket/CLoopBack.h>
#include <UType/CDenyCopying.h>
#include <UType/CThread.h>
#include <Socket/CTcpImplBase.h>
#include <Socket/print_socket_error.h>
#if !defined(_WIN32)
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>                     // sockaddr_in
#include <arpa/inet.h>                      // htons, htonl
#include <sys/sysctl.h>
#include <sys/ioctl.h>
#	ifdef __QNX__
#include <netinet/tcp_var.h>//only BSD
#	endif
#else
#include <ws2tcpip.h>
#endif
#define NSHARE_TCP_ERROR_TIMEOUT 1000
namespace NSHARE
{
CSocket CTcpImplBase::MNewSocket()
{
	return static_cast<CSocket::socket_t>(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
}
void CTcpImplBase::MWaitForSend(CSocket const & aTo) const
{
	//NSHARE::usleep(NSHARE_TCP_ERROR_TIMEOUT); //fixme wait 1 ms
	CSelectSocket _sock;
	_sock.MAddSocket(aTo);
	CSelectSocket::socks_t _tmp;
	_sock.MWaitData(_tmp,E_WRITE_ONLY);

}
CTCP::sent_state_t CTcpImplBase::MSendTo(CSocket const & aSock,
		const void* pData, size_t nSize, diagnostic_io_t const & aDiag,
		bool aIsBlockMode, unsigned& aNumberOfRepeats) const
{
	int _repeat_count = 5;

	size_t const _full_size = nSize;
	for (HANG_INIT; _repeat_count && nSize;HANG_CHECK,++aNumberOfRepeats)
	{
		VLOG(2) << "Send to " << aSock;
		VLOG_IF(5,nSize<100) << NSHARE::print_buffer_t<uint8_t const*>(( uint8_t const*)pData, (uint8_t const*) pData + nSize);

		LOG_IF(ERROR,!aSock.MIsValid()) << "try sending to invalide socket "<< aSock;
		if(!aSock.MIsValid())
		return ISocket::sent_state_t(ISocket::E_INVALID_VALUE,0);
#if defined(_WIN32)
		int _val = send(aSock.MGet(), reinterpret_cast<char const*>(pData), static_cast<int>(nSize), 0);
#elif defined(__FreeBSD__) || defined(__NetBSD__) || (defined(__QNX__) && _NTO_VERSION>=640)||defined(__linux__)
		int _val = send(aSock.MGet(), pData, nSize, MSG_NOSIGNAL);
#elif (defined(__QNX__) && _NTO_VERSION<=632)
		signal(SIGPIPE, SIG_IGN );
		VLOG(2) << "Try send to " << aSock;
		int _val = send(aSock.MGet(), pData, nSize, 0);
		VLOG(2) << "Send rval=" << _val;
		signal(SIGPIPE, SIG_DFL );
#elif defined(__linux__)//old kernel
		int _val = send(aSock.MGet(), pData, nSize, MSG_NOSIGNAL);
#else
#error Target not supported
#endif

		if (_val < 0)
		{
#ifdef _WIN32
			int const _errno=::WSAGetLastError();;
			VLOG(2)<<"Error="<<_errno;
#else
			int const _errno=errno;
#endif

			switch (_errno)
			{

#ifdef EWOULDBLOCK
				case EWOULDBLOCK: //the buffer is full
#endif
#ifdef WSAEWOULDBLOCK
				case WSAEWOULDBLOCK: //the buffer is full
#endif
#if defined(EWOULDBLOCK) && defined(EAGAIN) && EAGAIN==EWOULDBLOCK
#else
			case EAGAIN: //The client buffer is full
#endif
				{
					LOG_IF(ERROR,aIsBlockMode)<<"WTF!? Cannot occur in the block mode ";
					VLOG(1) << "The client buffer is full.";
					MWaitForSend(aSock);
					break;
				}
#if defined(EBUSY)/*QNX signsev*/ ||  defined(WSAEFAULT) || defined(EFAULT )
#	ifdef WSAEFAULT
				case WSAEFAULT:
#	endif
#	ifdef EBUSY
				case EBUSY:
#	endif
#	ifdef EFAULT
				case EFAULT:
#	endif
				LOG(ERROR)<<"Send invalid buffer by tcp(signsev)";
				return ISocket::sent_state_t(ISocket::E_INVALID_VALUE,0);
				break;
#endif
#ifdef __QNX__
				//H.z.
				case ESRCH:
				{
					VLOG(1) << "No such process. The server died.";
					--_repeat_count;
					if(!_repeat_count)
					{
						LOG(ERROR)<< "Cann't Send " << pData << " size=" << nSize << " to "
						<< aSock << " as " << print_socket_error();
						return ISocket::sent_state_t(ISocket::E_ERROR,_full_size-nSize);
					}
					else
					{
						MWaitForSend(aSock.MGet());
					}
				}
				break;
#endif

#ifdef	EMSGSIZE
				case EMSGSIZE:
#endif
#ifdef	WSAEMSGSIZE
				case WSAEMSGSIZE:
#endif
#ifdef	ENOMEM
				case ENOMEM:
#endif
				{
					LOG(ERROR)<<"The msg to large "<<nSize<<" bytes";
					return ISocket::sent_state_t(ISocket::E_TOO_LARGE,0);
					break;
				}
#ifdef EPIPE
				case EPIPE:
				{
					VLOG(2)<<"Socket is closed.";
					return ISocket::sent_state_t(ISocket::E_ERROR,_full_size-nSize);
					break;
				}
#endif

#ifdef ENOBUFS
				case ENOBUFS:
#endif
#ifdef WSAENOBUFS
				case WSAENOBUFS:
#endif
				{
#ifdef __linux__
					LOG(ERROR)<<"WTF!? ENOBUFS in linux";
#endif
					LOG(ERROR)<<"The system couldn't allocate an internal buffer."
					"Checking ARP for overloading.";
					--_repeat_count;
					if(_repeat_count)
					NSHARE::usleep(NSHARE_TCP_ERROR_TIMEOUT);
					break;
				}
#ifdef WSANOTINITIALISED
				case WSANOTINITIALISED:
				LOG(FATAL) <<"WTF!? The WSA is not initialized";
				return ISocket::sent_state_t(ISocket::E_ERROR,0);
				break;
#endif

				default:
				{
					LOG(ERROR)<< "Cann't Send " << pData << " size=" << nSize << " to "
					<< aSock << " as " << print_socket_error();
					return ISocket::sent_state_t(ISocket::E_ERROR,_full_size-nSize);
				}
				break;
			}
		}
		else
		{
			if(_val==0)
			{
				LOG(ERROR)<<"WTF? Send 0 bytes";
				return ISocket::sent_state_t(ISocket::E_ERROR,_full_size-nSize);
			}
			CHECK_GE((int)nSize,_val);
			nSize-=_val;
			pData=((char const*)pData)+_val;
			VLOG_IF(2,nSize)<<"Send more";
		}
	}
	if (_repeat_count == 0 || nSize > 0)
		return ISocket::sent_state_t(ISocket::E_ERROR, _full_size - nSize);

	aDiag.MSend(_full_size - nSize);
	return ISocket::sent_state_t(ISocket::E_SENDED, _full_size - nSize);
}
int CTcpImplBase::MReadData(ISocket::data_t* aBuf, CSocket const& aSock) const
{
	const size_t _befor = aBuf->size();

	do
	{
		const size_t _avalable = CNetBase::MAvailable(aSock);
		VLOG(2) << "Befor " << _befor << " bytes";
		VLOG_IF(1,(!_avalable)) << "No data on socket " << aSock
											<< ", may be it has been closed already."
													"Assuming the size equal 1 byte.";

		const size_t _size = (_avalable ? _avalable : 1) + _befor;
		VLOG(2) << "Available  " << (_size - _befor) << " bytes";
		aBuf->resize(_size);


		ISocket::data_t::value_type* _pbegin = aBuf->ptr() + _befor;
		CHECK_GT(_size, 0);
		int _recvd = 0;
#ifndef _WIN32
#	ifdef __linux__
		_recvd = recv(aSock.MGet(), (void*) _pbegin, _avalable, MSG_NOSIGNAL);
#  else
		_recvd = recv(aSock.MGet(), (void*) _pbegin, _avalable, 0);
#endif
#else
		_recvd = recv(aSock.MGet(), (char*) _pbegin,
				static_cast<int>(_avalable), 0);
#endif
		VLOG(1) << "Reads " << _recvd << " bytes from " << aSock;
		LOG_IF(ERROR,(_recvd<(int)_avalable)&&(_recvd>0) ) << "Available "
																	<< _avalable
																	<< " read "
																	<< _recvd;
		VLOG_IF(1, _recvd == 0) << aSock << " has been disconnected";
		VLOG_IF(1,_recvd <0) << "Error on socket " << aSock << "; "
									<< print_socket_error();

		//Set real size
		if (_recvd <= 0)
		{
			aBuf->resize(_befor);
		}
		else
		{
			VLOG_IF(5,_recvd<100)
							<< NSHARE::print_buffer_t<uint8_t const*>(
									(uint8_t const*) _pbegin,
									(uint8_t const*) _pbegin + _recvd);
			aBuf->resize(_befor + (size_t) (_recvd));

		}

		if (_recvd < 0)
		{
#ifdef _WIN32
			int const _errno = ::WSAGetLastError();;
			VLOG(2) << "Error=" << _errno;
#else
			int const _errno=errno;
#endif

			switch (_errno)
			{

#ifdef EWOULDBLOCK
			case EWOULDBLOCK: //the buffer is full
#endif
#ifdef WSAEWOULDBLOCK
			case WSAEWOULDBLOCK: //the buffer is full
#endif
#if defined(EWOULDBLOCK) && defined(EAGAIN) && EAGAIN==EWOULDBLOCK
#else
			case EAGAIN: //the buffer is full
#endif
			{
				LOG(ERROR)<< "The buffer is full.";
				continue;
				break;
			}
#if defined(EBUSY)/*QNX signsev*/ ||  defined(WSAEFAULT) || defined(EFAULT )
#	ifdef WSAEFAULT
			case WSAEFAULT:
#	endif
#	ifdef EBUSY
			case EBUSY:
#	endif
#	ifdef EFAULT
			case EFAULT:
#	endif
			LOG(ERROR)<<"Recv to invalid buffer by tcp(signsev)";
			break;
#endif

#ifdef	WSAEMSGSIZE
			case WSAEMSGSIZE:
#endif

			{
				LOG(ERROR)<<"The buffer is small.";
				continue;
				break;
			}
#ifdef EPIPE
			case EPIPE:
			{
				VLOG(2)<<"Socket is closed.";
				break;
			}
#endif

#ifdef WSANOTINITIALISED
			case WSANOTINITIALISED:
			LOG(FATAL) <<"WTF!? The WSA is not initialized";
			break;
#endif

			default:
			{
				LOG(ERROR)<< "Cann't recv  to "	<< aSock << " as " << print_socket_error();
			}
			break;
		}
	}//if error

	//if the error could be handled, the return _recvd is not called as the continue has been called
		return _recvd;
	} while (1);


	CHECK(false);
	return -1;//suppress warning only
}
} /* namespace NSHARE */
