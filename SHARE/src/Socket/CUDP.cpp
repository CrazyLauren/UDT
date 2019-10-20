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
#include <share/Net.h>
#include <share/Socket/CNetBase.h>
#include <share/uclock.h>
#include <string.h>
#include <share/console.h>
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
#include "print_socket_error.h"
#define UDP_BUFFER_SIZE USHRT_MAX

#ifdef _WIN32
typedef char raw_type_t;
#else
typedef void raw_type_t;
#endif

namespace NSHARE
{
const NSHARE::CText CUDP::NAME = "udp";

CUDP::CUDP(NSHARE::CConfig const& aConf) :
		FMaxSendSize(0)
{

	FIsWorking=0;
	FIsReceiving=0;
	settings_t _param(aConf);
	LOG_IF(DFATAL,!_param.MIsValid()) << "Configure for udp is not valid "
												<< aConf;
	MOpen(_param);
}
CUDP::CUDP(const settings_t& aParam)
{
	FMaxSendSize = 0;
	FIsWorking=0;
	FIsReceiving=0;
	if (aParam.MIsValid())
		MOpen(aParam);
}
CUDP::~CUDP()
{
	MClose();

	CRAII<CMutex> _lock(FReceiveThreadMutex);///< Wait for receive operation stopped
}
void CUDP::MClose()
{
	if (MIsOpen())
	{
		FIsWorking=0;

		//force unlock socket
		if(FIsReceiving.MIsOne())
		{
			net_address const _addr(net_address::LOCAL_HOST,
					MGetSetting().FPort);
			DCHECK(_addr.MIsValid());
			char _val=1;
			sent_state_t const _is=MSend(&_val, sizeof(_val), _addr);
			DCHECK(_is.MIs());
		}
		FSock.MClose();
	}
}
CUDP::list_of_broadcast_addr_t CUDP::sMGetBroadcast()
{
	list_of_broadcast_addr_t _rval;
	interfaces_addr_t _to;
	if(loocal_addr_ip4(&_to))
	{
		VLOG(2)<<"Interfaces "<<_to;
		for (NSHARE::interfaces_addr_t::const_iterator _it = _to.begin();
				_it != _to.end(); ++_it)
		{
			_rval.insert(broadcast(_it->FIp,_it->FMask));
		}
	}
	return _rval;
}

/** @brief Synchronize info about receivers of message
 * from settings with  #FSendToAddr
 *
 */
void CUDP::MUpdateSendList()
{
	FSendToAddr.resize(FParam.FSendTo.size());
	net_addresses_t::const_iterator _it=FParam.FSendTo.begin();
	for (unsigned i=0; i< FSendToAddr.size();++_it, ++i)
	{
		CNetBase::MSetAddress(*_it,	&FSendToAddr[i]);
	}
}
bool CUDP::MSettingUp(const settings_t& aParam)
{
	if(MIsOpen())
		return false;
	FParam = aParam;

	MUpdateSendList();
	return true;
}

bool CUDP::MOpen(const settings_t& aParam)
{
	return MSettingUp(aParam)&&MOpen();
}
bool CUDP::MSetSendAddress(net_address const& aAddress)
{
	VLOG(2) << "Set send address:  " << aAddress;
	MCleanSendAddress();

	if (aAddress.MIsValid())
	{
		return MAddSendAddress(aAddress);
	}else
	{
		return true;
	}

}
bool CUDP::MAddSendAddress(net_address const& aAddress)
{
	VLOG(2) << "Adds  " << aAddress << " to send list";

	if(!FParam.MAddSendAddr(aAddress))
		return false;
	MUpdateSendList();
	return true;
}
bool CUDP::MRemoveSendAddress(net_address const& aAddress)
{
	VLOG(2) << "Removes  " << aAddress << " from semd list";

	if(!FParam.MRemoveSendAddr(aAddress))
		return false;
	MUpdateSendList();
	return true;
}
bool CUDP::MCleanSendAddress()
{
	VLOG(2) << "Removes all send addresses";

	FParam.FSendTo.clear();

	MUpdateSendList();
	return true;
}

/** Check for exist the net interface
 * which is in keeping with broadcast
 *
 *
 * @return true if exist valid broadcast ip addresses
 */
bool CUDP::MRemoveInvalidBroadcastAddresses()
{
	bool _is_broadcast_exist = false;

	/** Receives list of network interface address
	 *
	 */
	interfaces_addr_t _to;
	loocal_addr_ip4(&_to);

	if (!FParam.FSendTo.empty())
	{
		/** Looking for broadcast addresses in FParam
		 *
		 */
		net_addresses_t::iterator _it = FParam.FSendTo.begin();
		bool _is_full_broadcast=false;
		do
		{
			DCHECK(_it->FIp.MIs());

			network_ip_t const& _ip = _it->FIp.MGetConst();
			if (is_broadcast_ip(_ip))
			{
				/** If 255.255.255.255 broadcast addresses when remove it
				 * and add network interface  broadcast addresses
				 *
				 */
				if (FParam.FFlags.MGetFlag(
						settings_t::E_REPLACE_FULL_BROADCAST_ADDR) && _ip==net_address::BROAD_CAST_ADDR)
				{
					_is_full_broadcast=true;
					FParam.FSendTo.erase(_it++); //warning! only post increment
					continue;
				}
				else
				{
					/** Check if exist corresponding interface
					 * to broadcast address
					 *
					 */
					bool _is = false;
					interfaces_addr_t::const_iterator _it_int = _to.begin();
					for (; _it_int != _to.end() && !_is; ++_it_int)
					{
						_is = is_in(_ip, _it_int->FIp, _it_int->FMask);
					}

					/** if corresponding interface
					 * is not  exist removing the address from #FParam
					 *
					 */
					if (_is)
					{
						VLOG(2) << "The network interface for broadcast ip:"
											<< _ip << " is exist " << *_it_int;
						_is_broadcast_exist = true;
					}
					else
					{
						LOG(DFATAL)
												<< "The network intreface for broadcast ip:"
												<< _ip << " is not exist.";

						if (FParam.FFlags.MGetFlag(
								settings_t::E_REMOVE_INVALID_SEND_IP))
						{
							FParam.FSendTo.erase(_it++); //warning! only post increment
							continue;
						}
					}
				}
			}

			++_it;
		} while (_it != FParam.FSendTo.end());

		if (_is_full_broadcast)
		{
			LOG(INFO)<<"Replace full broadcast address to interface broadcast addresses";
			if(MAddBroadcastInterfaceAddresses()>0)
				_is_broadcast_exist=true;
		}
	}
	return _is_broadcast_exist;
}
/** Adds to #FParam list of broadcast interface addresses
 *
 *@return amount of broadcast ip
 */
unsigned CUDP::MAddBroadcastInterfaceAddresses()
{
	const list_of_broadcast_addr_t _addrs = sMGetBroadcast();
	list_of_broadcast_addr_t::const_iterator _it = _addrs.begin(), _it_end(
			_addrs.end());
	unsigned _count=0;
	for (; _it != _it_end; ++_it)
	{
		const net_address _new_addr(*_it, FParam.FPort);
		const bool _is = FParam.MAddSendAddr(_new_addr);
		LOG_IF(DFATAL,!_is) << "Cannot add broadcast address " << _new_addr;
		LOG_IF(INFO,_is) << "Adds broadcast address " << _new_addr;
		if(_is)
			++_count;
	}
	return _count;
}

/** Adds broadcast addresses to send list if
 * it no exist
 *
 * If broadcast is exist in send list
 * it checks
 *
 *	@return true if no error
 */
bool CUDP::MAddBroadcastAddresses()
{
	bool _is_broadcast_exist=false;

	if(!MRemoveInvalidBroadcastAddresses())
	{
		LOG(INFO)<<"As no broadcast addresses adding it";
		if(FParam.FFlags.MGetFlag(settings_t::E_REPLACE_FULL_BROADCAST_ADDR))
			MAddBroadcastInterfaceAddresses();
		else
			FParam.MAddSendAddr(net_address(net_address::BROAD_CAST_ADDR,FParam.FPort));
	}

	MUpdateSendList();
	return true;
}
bool CUDP::MOpen()
{
	VLOG(2) << "Open UDP " << FParam.FPort;
	LOG_IF(WARNING, FSock.MIsValid())
												<< "Host socket is valid. Leak socket can occur! "
												<< FSock;
	if (FSock.MIsValid())
		return false;

	if (!FParam.MIsValid()) //auto generate
		FParam.FPort = net_address::RANDOM_NETWORK_PORT;

	FSock = CNetBase::MNewSocket(SOCK_DGRAM,IPPROTO_UDP);
	if (!FSock.MIsValid())
	{
		LOG(DFATAL) << FSock << print_socket_error();
		return false;
	}

	FMaxSendSize = 0;

	CNetBase::MSettingSocket(FSock,FParam.FSocketSetting);

	net_address _bind_addr(FParam.FPort);

	switch (FParam.FType)
	{
	case settings_t::eUNICAST:
	{
		if (!FParam.FReceiveFrom.empty())
		{
			DCHECK_EQ(FParam.FReceiveFrom.size(), 1);
			_bind_addr.FIp = *FParam.FReceiveFrom.begin();
		}
		break;
	}
	case settings_t::eBROADCAST:
	{
		if (!CNetBase::MMakeAsBroadcast(FSock))
		{
			LOG(DFATAL) << "Cannot setup broadcast addresses";
			return false;
		}

		if (!MAddBroadcastAddresses())
		{
			LOG(DFATAL) << "Cannot adds broadcast addresses";
			return false;
		}

		if (!FParam.FReceiveFrom.empty())
		{
			DCHECK_EQ(FParam.FReceiveFrom.size(), 1);
			_bind_addr.FIp = *FParam.FReceiveFrom.begin();
		}
	}
		break;

	case settings_t::eMULTICAST:
	{
		LOG(FATAL) << "Multicast address is not implemented";
	}
		break;
	};

	if (CNetBase::MSetLocalAddrAndPort(MGetSocket().MGet(), _bind_addr) < 0)
	{
		LOG(ERROR) << "Binding " << FParam.FPort << " failed "
								<< print_socket_error();
		FSock.MClose();
		return false;
	}
	if (FParam.FPort == net_address::RANDOM_NETWORK_PORT)
	{
		FParam.FPort=CNetBase::MGetLocalAddress(MGetSocket()).FPort;
		LOG(INFO) << "Chosen port number " << FParam.FPort
								<< " as the random port.";
	}

	VLOG(0) << "The UDP socket has been opened successfully.";
	FMaxSendSize = static_cast<size_t>(CNetBase::MGetSendBufSize(MGetSocket()));
	FIsWorking=1;
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
	DCHECK_NOTNULL(aBuf);

	VLOG(2) << "Receive data to " << aBuf << ", max time " << aTime;
	CRAII<CMutex> _lock(FReceiveThreadMutex);

	if (!MIsOpen())
	{
		LOG(ERROR) << "The socket is not working";
		FDiagnostic+=sent_state_t(sent_state_t::E_SOCKET_CLOSED,0);
		FIsReceiving=0;
		return -1;
	}
	int _recvd = -1;

	if (FIsWorking.MIsOne() && ((FIsReceiving = 1)) && FIsWorking.MIsOne()) //-V501
		//As FIsWorking and FIsReceiving is atomic, need to be sure
		//that is during of changing the value of FIsReceiving
		//the value of FIsWorking hasn't  be changed
	{
		_recvd = recvfrom(MGetSocket().MGet(), (raw_type_t*) &_recvd, 1,
				MSG_PEEK,
				NULL, NULL);

#ifdef _WIN32
		//WTF? Fucking windows. If the received msg is more than the buffer size,
		//it generates the WSAEMSGSIZE error. But The buffer is always small as
		//it is 1 byte!!!!!
		if (_recvd > 0	//
		|| (_recvd == SOCKET_ERROR && WSAEMSGSIZE == ::WSAGetLastError()))
		#else
		if (_recvd > 0)
#endif
		{
			if (FIsWorking.MIsOne()) //closed
			{
				struct sockaddr_in _addr;
				socklen_t _len = sizeof(_addr);
				const size_t _befor = aBuf->size();
				const size_t _size = MAvailable();

				VLOG(2) << "Available " << _size << " bytes";
				VLOG_IF(1,!_size) << "No data on socket " << MGetSocket();
				aBuf->resize(_befor + _size);
				DCHECK_GT(aBuf->size(), 0);
				DCHECK_GE(aBuf->size(), _size);
				data_t::value_type* _pbegin = aBuf->ptr()
						+ (aBuf->size() - _size);
				_recvd = recvfrom(MGetSocket().MGet(), (raw_type_t*) _pbegin,
						(int) _size,
						0, (struct sockaddr*) &_addr, &_len);
				if (FIsWorking.MIsOne())
				{
					DCHECK_GT(_recvd, 0);

					VLOG(2) << "Recvd=" << _recvd;
					if (aFrom)
					{
						LOG_IF(ERROR,_len==0) << "Length of address is 0.";
						if (_len > 0)
						{
							aFrom->FIp = get_ip(_addr.sin_addr);
							aFrom->FPort = ntohs(_addr.sin_port);
							VLOG(2) << "From " << *aFrom;
						}
					}
					aBuf->resize(_befor + _recvd);
					VLOG(0) << "Reads " << _recvd << " bytes";
					FDiagnostic.MRecv(_recvd);
				}
				else
				{
					_recvd = -1;
					aBuf->resize(_befor);
				}
			}
			else
				_recvd = -1;
		}
		if (_recvd <= 0)
		{
			if (FIsWorking.MIsOne())
			{
#ifdef _WIN32
				int const _errno = ::WSAGetLastError();
				if (_errno == WSAECONNRESET) //Thank you Bill Gates for your care!
				//It's error  mean that  The packet has been SENT to closed port
				{
					VLOG(1) << "WSAECONNRESET" << _errno;
					_recvd = 0;
				}
				else
				{
					LOG(ERROR) << "Unknown error:" << _errno;
					_recvd = -1;
				}
#else
				LOG(ERROR)<< "Unknown error:" << print_socket_error();
				_recvd=-1;
#endif
				FDiagnostic += sent_state_t(sent_state_t::E_ERROR, 0);
			}
			else
				FDiagnostic += sent_state_t(sent_state_t::E_SOCKET_CLOSED, 0);
		}
	}
	FIsReceiving=0;
	return _recvd;
}
CUDP::sent_state_t CUDP::MSend(void const* pData, size_t nSize,
		NSHARE::CConfig const& aTo)
{
	net_address _addr(aTo);
	LOG_IF(DFATAL,!_addr.MIsValid()) << "Invalide type of smart_addr";
	if (!_addr.MIsValid())
		return sent_state_t(sent_state_t::E_INVALID_VALUE, 0);

	return MSend(pData, nSize, _addr);
}
CUDP::sent_state_t CUDP::MSend(void const* pData, size_t nSize)
{
	if (FSendToAddr.empty())
	{
		LOG(ERROR) << "The Default address is not setting";
		return sent_state_t(sent_state_t::E_INVALID_VALUE, 0);
	}

	send_to_t::const_iterator _it=FSendToAddr.begin(),_it_end(FSendToAddr.end());

	sent_state_t _state(sent_state_t::E_ERROR, 0);

	for (; _it != _it_end; ++_it)
	{
		_state = MSend(pData, nSize,*_it);
	}
	return _state;
}
CUDP::sent_state_t CUDP::MSend(void const* pData, size_t nSize,
		net_address const& aAddress)
{
	VLOG(1) << "Send " << pData << " size=" << nSize << " to " << aAddress;
	struct sockaddr_in _Sa;
	CNetBase::MSetAddress(aAddress, &_Sa);
	return MSend(pData, nSize, _Sa);
}
CUDP::sent_state_t CUDP::MSend(void const* pData, size_t nSize,
		struct sockaddr_in const& aAddress)
{
	if (!MIsOpen())
	{
		LOG(WARNING) << "The Port is not opened.";
		return sent_state_t(sent_state_t::E_NOT_OPENED, 0);
	}
	VLOG(2) << "Sending " << pData << " of " << nSize<<" to "<<net_address(aAddress);

	if (FMaxSendSize && nSize > FMaxSendSize)
		FMaxSendSize = static_cast<size_t>(CNetBase::MGetSendBufSize(MGetSocket()));

	LOG_IF(ERROR,(FMaxSendSize>0)&&FMaxSendSize<(size_t)nSize)
																<< " Message may be too long. Max size = "
																<< FMaxSendSize
																<< ". Sending "
																<< nSize;

	return MSendImpl(pData,nSize,aAddress);

}
CUDP::sent_state_t  CUDP::MSendImpl(void const* pData, size_t nSize,
		struct sockaddr_in const& aAddress)
{
	int const _rval = sendto(MGetSocket().MGet(),
			reinterpret_cast<raw_type_t const*>(pData), static_cast<int>(nSize), 0,
			(struct sockaddr *) &aAddress, sizeof(aAddress));
	LOG_IF(ERROR,_rval<0) << "Send error " << print_socket_error();
	VLOG_IF(2,_rval>=0) << "Sent " << _rval << " bytes.";
	bool _is = _rval > 0;
	sent_state_t const _trval(
			_is ? sent_state_t::E_SENDED :
							sent_state_t::E_ERROR, nSize);

	FDiagnostic.MSend(_trval);
	return _trval;
}
CUDP::settings_t const& CUDP::MGetSetting() const
{
	return FParam;
}
NSHARE::CConfig CUDP::MSettings(void) const
{
	return MGetSetting().MSerialize();
}
const CSocket& CUDP::MGetSocket(void) const
{
	return FSock;
}
network_port_t CUDP::MGetPort() const
{
	return FParam.FPort;
}
NSHARE::CConfig CUDP::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	_conf.MAdd(MGetSetting().MSerialize());
	_conf.MAdd("open", MIsOpen());
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

	aStream<<"Send to:" <<FParam.FSendTo;

	return aStream;
}
diagnostic_io_t const& CUDP::MGetDiagnosticState() const
{
	return FDiagnostic;
}

const CText CUDP::settings_t::NAME = "param";
const CText CUDP::settings_t::UDP_PORT = "port";
const CText CUDP::settings_t::ADDR_TO = "toip";
const CText CUDP::settings_t::RECEIVE_FROM = "fromip";
const CText CUDP::settings_t::TYPE = "type";
const CText CUDP::settings_t::TYPE_UNICAST = "unicast";
const CText CUDP::settings_t::TYPE_BROADCAST = "broadcast";
const CText CUDP::settings_t::TYPE_MULTICAST = "multicast";
const CText CUDP::settings_t::REMOVE_INVALID_SEND_IP = "remove_invalid_send_ip";
const CText CUDP::settings_t::REPLACE_FULL_BROADCAST_ADDR = "replace_full_broadcast_addr";

CUDP::settings_t::settings_t(network_port_t aPort, net_address const& aParam,
		eType const& aType) :
		FType(aType),//
		FFlags(E_DEFAULT_FLAGS)
{
	FSocketSetting.FFlags.MSetFlag(socket_setting_t::E_SET_BUF_SIZE, false);
	FPort = aPort;

	MAddSendAddr(aParam);
}
CUDP::settings_t::settings_t() :
		FPort(std::numeric_limits<network_port_t>::max()),//
		FType(eUNICAST),//
		FFlags(E_DEFAULT_FLAGS)//
{
	FSocketSetting.FFlags.MSetFlag(socket_setting_t::E_SET_BUF_SIZE, false);
}
bool CUDP::settings_t::MRemoveSendAddr(net_address const&  aParam)
{
	VLOG(2)<<"Removes address "<<aParam;
	return FSendTo.erase(aParam)>0;
}
bool CUDP::settings_t::MRemoveReceiveAddr(network_ip_t const& aAddress)
{
	return FReceiveFrom.erase(aAddress)>0;;
}
bool CUDP::settings_t::MSetReceiveAddr(network_ip_t const& aNewAddress)
{
	FReceiveFrom.clear();
	return FReceiveFrom.insert(aNewAddress).second;
}
bool CUDP::settings_t::MAddReceiveAddr(network_ip_t const& aNewAddress)
{
	switch (FType)
	{
	case eUNICAST:
	case eBROADCAST:
		if(!FReceiveFrom.empty())
			return false;
		break;
	case eMULTICAST:
		break;
	};
	return FReceiveFrom.insert(aNewAddress).second;
}
bool CUDP::settings_t::MAddSendAddr(net_address aParam)
{
	switch (FType)
	{
	case eUNICAST:
		if (aParam.MIsValid())
		{
			if (aParam.MIsIPValid())
			{
				if (aParam.FIp.MGetConst().rfind(net_address::BROAD_CAST_SUBADDR) != CText::npos)
				{
					LOG(INFO) << "The address is broadcast:"
											<< aParam.FIp.MGetConst();

					FType = eBROADCAST;
				}
			}
			else if (!aParam.FIp.MIs())
			{
				FType = eBROADCAST;
				aParam.FIp = net_address::BROAD_CAST_ADDR;
				LOG(INFO) << "The set up broadcast ip:"
										<< aParam.FIp.MGetConst();
			}
			else
				return false;
		}else
			return false;
		break;

	case eBROADCAST:
	case eMULTICAST:
	{
		if (!aParam.MIsValid())
		{
			aParam.FPort = FPort;
			aParam.FIp = net_address::BROAD_CAST_ADDR;
			LOG(INFO) << "The set up broadcast ip:"
									<< aParam.FIp;
		}
		else
		{
			if (!aParam.FIp.MIs())
			{
				aParam.FIp = net_address::BROAD_CAST_ADDR;

			}else if (!aParam.MIsIPValid())
				return false;
		}
	}
		break;
	}
	VLOG(2)<<"Adds address "<<aParam;
	return FSendTo.insert(aParam).second;
}
CUDP::settings_t::settings_t(NSHARE::CConfig const& aConf) :
		FType(eUNICAST),//
		FFlags(E_REMOVE_INVALID_SEND_IP),//
		FSocketSetting(aConf.MChild(socket_setting_t::NAME))
{
	FPort = std::numeric_limits<network_port_t>::max();

	const CText _type=aConf.MValue(TYPE, TYPE_UNICAST);
	if (_type == TYPE_UNICAST)
		FType = eUNICAST;
	else if (_type == TYPE_BROADCAST)
		FType = eBROADCAST;
	else if (_type == TYPE_MULTICAST)
		FType = eMULTICAST;

	FFlags.MSetFlag(E_REMOVE_INVALID_SEND_IP,
			aConf.MValue(REMOVE_INVALID_SEND_IP,
					FFlags.MGetFlag(E_REMOVE_INVALID_SEND_IP)));
	FFlags.MSetFlag(E_REPLACE_FULL_BROADCAST_ADDR,aConf.MValue(REPLACE_FULL_BROADCAST_ADDR, FFlags.MGetFlag(E_REPLACE_FULL_BROADCAST_ADDR)));

	if (aConf.MGetIfSet(UDP_PORT, FPort))
	{
		{
			ConfigSet const _set = aConf.MChildren(ADDR_TO);
			ConfigSet::const_iterator _it = _set.begin();
			for (; _it != _set.end(); ++_it)
			{
				bool const _is = MAddSendAddr(net_address(*_it));
				LOG_IF(DFATAL,!_is) << "Invalid param " << *_it;
			}
		}
		{
			ConfigSet const _set = aConf.MChildren(RECEIVE_FROM);
			ConfigSet::const_iterator _it = _set.begin();
			for (; _it != _set.end(); ++_it)
			{
				bool const _is = MAddReceiveAddr(_it->MValue());
				LOG_IF(DFATAL,!_is) << "Invalid param " << *_it;
			}
		}
	}
}
bool CUDP::settings_t::MIsValid() const
{
	return FPort != std::numeric_limits<network_port_t>::max()//
			&& FSocketSetting.MIsValid();
}
CConfig CUDP::settings_t::MSerialize() const
{
	CConfig _conf(NAME);
	if (MIsValid())
	{
		_conf.MAdd(FSocketSetting.MSerialize());

		_conf.MSet(UDP_PORT, FPort);
		if (!FReceiveFrom.empty())
		{
			NSHARE::network_ips_t::const_iterator _it(FReceiveFrom.begin()), _it_end(
					FReceiveFrom.end());
			for(;_it!=_it_end;++_it)
				_conf.MAdd(RECEIVE_FROM,*_it);
		}

		if (!FSendTo.empty())
		{
			NSHARE::net_addresses_t::const_iterator _it(FSendTo.begin()), _it_end(
					FSendTo.end());
			for(;_it!=_it_end;++_it)
				_conf.MAdd(ADDR_TO,_it->MSerialize());
		}
		_conf.MAdd(TYPE, FType);

		_conf.MAdd(REMOVE_INVALID_SEND_IP, FFlags.MGetFlag(E_REMOVE_INVALID_SEND_IP));
		_conf.MAdd(REPLACE_FULL_BROADCAST_ADDR, FFlags.MGetFlag(E_REPLACE_FULL_BROADCAST_ADDR));
	}
	return _conf;
}
} //namespace NSHARE

