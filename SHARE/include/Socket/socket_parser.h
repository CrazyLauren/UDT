/*
 * socket_parser.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 12.03.2014
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  
#ifndef SOCKET_PARESER_H_
#define SOCKET_PARESER_H_
#ifdef OLD_PARSER
//fixme remove
#define KEY_DEX "dex"
#define KEY_DEX_CHANNEL_NAME "name"
#define KEY_DEX_QUANTITY "quantity"
#define KEY_DEX_SIZE "size"
#define KEY_DEX_IN "in"
#define KEY_DEX_OUT "out"

#define KEY_FILE "file"
#define KEY_FILE_PATH "path"
#define KEY_FILE_PERIOD "period"
#define KEY_FILE_SIZE "size"
#define KEY_FILE_IN "in"
#define KEY_FILE_OUT "out"

#define KEY_TCP "tcp"
#define KEY_TCP_IP "ip"
#define KEY_TCP_PORT "port"

#define KEY_UDP "udp"
#define KEY_UDP_PORT "port"
#define KEY_UDP_TO_IP "toip"
#define KEY_UDP_TO_PORT "toport"

#define KEY_PIPE "pipe"
#define KEY_PIPE_PATH "path"
#define KEY_PIPE_TYPE "type"
#define KEY_PIPE_READ_ONLY "readonly"
#define KEY_PIPE_WRITE_ONLY "writeonly"

#define KEY_UNIX "unix"
#define KEY_UNIX_FROM "from"
#define KEY_UNIX_TO   "to"
#define KEY_UNIX_STREAM "stream"
#define KEY_UNIX_DGRAM "dgram"

#define KEY_RS485 "rs485"
#define KEY_RS422 "rs422"
#define KEY_RS232 "rs232"
#define KEY_SERIAL_NOTATION "notation"
#define KEY_SERIAL_SPEED "speed"
#define KEY_SERIAL_PATH "path"

#define KEY_SM "sm"
#endif
namespace NSHARE
{
SHARE_EXPORT std::vector<ISocket*> get_sockets(CConfig const& aChild);
#ifdef OLD_PARSER
template<class TTree>
SHARED_DEPRECATED_F2 inline std::vector<ISocket*> get_sockets(TTree const& aChild)
{
	std::vector<ISocket*> _rval;
	for (typename TTree::const_iterator _it_up = aChild.begin();
			_it_up != aChild.end(); ++_it_up)
	{
		NSHARE::String _name = _it_up->first;
		if (_name == KEY_DEX)
		{
#ifdef DEX_SOCKET_EXIST
			CDex* _dex_in = NULL;
			CDex* _dex_out = NULL;
			CDex::param_t _param;
			try
			{
				TTree const& _child=_it_up->second;

				std::string _str, _str_prefix;
				typename TTree::const_iterator _it =
				_child.begin();
				typename TTree::const_iterator _it_end = _child.end();
				for (; _it != _it_end; ++_it)
				{
					CDex*_dex = new CDex();
					if (_it->first == KEY_DEX_OUT)
					{
						_param.FDirect = CDex::eOut;
						_str_prefix += KEY_DEX_OUT;
						_str_prefix += ".";
						_dex_out = _dex;
					}
					else if (_it->first == KEY_DEX_IN)
					{
						_param.FDirect = CDex::eIn;
						_str_prefix += KEY_DEX_IN;
						_str_prefix += ".";
						_dex_in = _dex;
					}
					else
					std::abort();

					_param.FName = _child.template get<NSHARE::String>(
							_str_prefix + KEY_DEX_CHANNEL_NAME);
					_param.FQuantity = _child.template get<size_t>(
							_str_prefix + KEY_DEX_QUANTITY);
					_param.FSize = _child.template get<size_t>(
							_str_prefix + KEY_DEX_SIZE);
					_dex->MOpen(_param);
					if (!_dex->MIsOpen())
					{
						if (_dex_in)
						delete _dex_in;
						if (_dex_out)
						delete _dex_out;
						_dex_in=NULL;
						_dex_out=NULL;
						break;
					}
				}
			}
			catch (...)
			{
				if (_dex_in)
				delete _dex_in;
				if (_dex_out)
				delete _dex_out;
				continue;
			}
			if (_dex_in && _dex_out)
			_rval.push_back( new CDexDuplex(_dex_in, _dex_out));
			else if (_dex_in)
			_rval.push_back( _dex_in);
			else if (_dex_out)
			_rval.push_back( _dex_out);
#else
			std::cerr << "Dex is not avaliable" << std::endl;
			std::abort();
#endif
		}
		else if (_name == KEY_TCP)
		{
#ifdef TCP_SOCKET_EXIST
			//NSHARE::NNet::CTCP* _tcp = new NSHARE::NNet::CTCP();
			NSHARE::net_address _addr;
			TTree const& _child=_it_up->second;
			try
			{
				_addr.port = _child.template get<in_port_t>(KEY_TCP_PORT);
			}
			catch (...)
			{
				continue;
			}
			try
			{
				_addr.ip = _child.template get<std::string>(KEY_TCP_IP);
			}
			catch (...)
			{
				std::cerr<<KEY_TCP_IP<<" is not exist "<<std::endl;
			}
			if(!_addr.MIsValid())
			continue;
			if(_addr.ip.MIs())
			_rval.push_back( new NSHARE::CTCP(_addr));
			else
			_rval.push_back( new NSHARE::CTCPServer(_addr));

#else
			std::cerr << "Tcp is not avaliable" << std::endl;
			std::abort();
#endif
		}
		else if (_name == KEY_PIPE)
		{
#ifdef PIPE_SOCKET_EXIST
			TTree const& _child=_it_up->second;
			std::string _path;
			std::string _type;
			_path = _child.template get<std::string>(KEY_PIPE_PATH, "");
			_type = _child.template get<std::string>(KEY_PIPE_TYPE, "");
			if (_path == "")
			continue;
			NSHARE::CFifo::setting_t _setting;
			_setting.FPath=_path.c_str();

			if (_type == KEY_PIPE_READ_ONLY)
			{
				_setting.FType=CFifo::READ_ONLY;
			}
			else if (_type == KEY_PIPE_WRITE_ONLY)
			{
				_setting.FType=CFifo::WRITE_ONLY;
			}
			else
			{
				_setting.FType=CFifo::READ_WRITE;
			}
			_rval.push_back( new CFifo(_setting));
#else
			std::cerr << "Pipe is not avaliable" << std::endl;
			std::abort();
#endif
		}
		else if (_name == KEY_UNIX)
		{
#if defined (__QNX__) ||defined (unix)
#	ifdef UNIX_SOCKET_EXIST
			ISocket* _unix = NULL;
			try
			{
				std::string _str, _str_prefix;
				TTree const& _child=_it_up->second;
				typename TTree::const_iterator _it =
				_child.begin();
				typename TTree::const_iterator _it_end = _child.end();
				for (; _it != _it_end; ++_it)
				{
					if (_it->first == KEY_UNIX_DGRAM)
					{
						_str_prefix += KEY_UNIX_DGRAM;
						_str_prefix += ".";

						std::string _from;
						std::string _to;
						_from = _child.template get<std::string>(
								_str_prefix + KEY_UNIX_FROM, "");
						_to = _child.template get<std::string>(
								_str_prefix + KEY_UNIX_TO, "");
						CUnixDGRAM::param_t _param;
						_param.FFrom=_from.c_str();
						_param.FTo=_to.c_str();
						CUnixDGRAM* _p = new CUnixDGRAM(_param);
						_unix=_p;
						_p->MOpen();
						break;
					}
					else if (_it->first == KEY_UNIX_STREAM)
					{
						//TODO
						break;
					}
					else
					{
						std::cerr<<"Unix socket error"<<std::endl;
						std::abort();
					}

				}
			}
			catch (...)
			{
				if (_unix)
				delete _unix;
				_unix=NULL;
				continue;
			}
			if (_unix && !_unix->MIsOpen())
			{
				delete _unix;
				_unix=NULL;
				continue;
			}
			if(_unix)
			_rval.push_back(_unix);
#	else
			std::cerr << "unix socket is not avaliable" << std::endl;
			std::abort();
#	endif	//#	ifdef UNIX_SOCKET_EXIST
#else
			std::abort();
#endif //#if defined (__QNX__) ||defined (unix)
		}
		else if (_name == KEY_RS485 || _name == KEY_RS232 || _name == KEY_RS422)
		{
#ifdef SERIAL_SOCKET_EXIST
			using namespace NSHARE::NSerial;
			NSerial::ISerial* _p = NULL;
			TTree const& _child=_it_up->second;

			String _str_prefix;
			String _notation = _child.template get<std::string>(
					_str_prefix + String(KEY_SERIAL_NOTATION), "");
			if (_notation == "")
			continue;

			uint32_t _speed = _child.template get<uint32_t>(
					_str_prefix + String(KEY_SERIAL_SPEED), 0u);
			if (_speed == 0)
			continue;
			String _dev_name = _child.template get<std::string>(
					_str_prefix + String(KEY_SERIAL_PATH), "");
			if (_dev_name == "")
			continue;

			if (_name == KEY_RS485)
				 _p=new CRS485();
			else if (_name == KEY_RS232 || _name == KEY_RS422)
				_p=new CSerialPort();
			else
			continue;
			if (!_p->MOpen(_dev_name))
			{
				delete _p;
				_p=NULL;
				continue;
			}
			try
			{
				NSerial::port_settings_t _setting(*NSerial::CNotation::sCreateNotation(_notation),_speed);
				_p->MSetUP(_setting);
			}
			catch (...)
			{
				delete _p;
				_p=NULL;
				continue;
			}
			if(_p)
			_rval.push_back(_p);
#else
			std::cerr << "serial socket is not avaliable" << std::endl;
			std::abort();
#endif
		}
		else if (_name == KEY_UDP)
		{
#ifdef UDP_SOCKET_EXIST
			NSHARE::CUDP* _udp = new NSHARE::CUDP();
			NSHARE::net_address _addr;
			in_port_t _port;
			try
			{
				TTree const& _child=_it_up->second;
				_addr.ip = _child.template get<std::string>(KEY_UDP_TO_IP);
				_addr.port = _child.template get<in_port_t>(KEY_UDP_TO_PORT);
				_port = _child.template get<in_port_t>(KEY_UDP_PORT);
			}
			catch (...)
			{
				delete _udp;
				_udp=NULL;
				continue;
			}
			NSHARE::CUDP::param_t _param(_port, _addr);
			_udp->MOpen(_param);
			if (!_udp->MIsOpen())
			{
				delete _udp;
				_udp=NULL;
				continue;
			}
			if(_udp)
			_rval.push_back(_udp);
#else
			std::cerr << "udp socket is not avaliable" << std::endl;
			std::abort();
#endif
		}
		else if (_name == KEY_FILE)
		{
#ifdef FILE_SOCKET_EXIST
			CSocketFile* _file_in = NULL;
			CSocketFile* _file_out = NULL;
			CSocketFile::param_t _param;
			try
			{
				TTree const& _child=_it_up->second;
				std::string  _str_prefix;
				typename TTree::const_iterator _it =
				_child.begin();
				typename TTree::const_iterator _it_end = _child.end();
				for (; _it != _it_end; ++_it)
				{
					CSocketFile*_file = new CSocketFile();
					if (_it->first == KEY_FILE_OUT)
					{
						_param.FDirect = CSocketFile::eOut;
						_str_prefix += KEY_FILE_OUT;
						_str_prefix += ".";
						_file_out = _file;
					}
					else if (_it->first == KEY_FILE_IN)
					{
						_param.FDirect = CSocketFile::eIn;
						_str_prefix += KEY_FILE_IN;
						_str_prefix += ".";
						_file_in = _file;
						_param.FTime = _child.template get<size_t>(
								_str_prefix + KEY_FILE_PERIOD);
						_param.FSize = _child.template get<size_t>(
								_str_prefix + KEY_FILE_SIZE);
					}
					else
					{
						std::cerr << "File  avaliable config" << std::endl;
						std::abort();
					}
					_param.FPath = _child.template get<NSHARE::String>(
							_str_prefix + KEY_FILE_PATH);

					_file->MOpen(_param);
					if (!_file->MIsOpen())
					{
						delete _file_in;
						delete _file_out;
						_file_in=NULL;
						_file_out=NULL;
						break;
					}
				}
			}
			catch (...)
			{
				delete _file_in;
				_file_in=NULL;
				delete _file_out;
				_file_out=NULL;
				continue;
			}
			if (_file_in && _file_out)
			_rval.push_back( new CSocketFileDuplex(_file_in, _file_out));
			else if (_file_in)
			_rval.push_back( _file_in);
			else if (_file_out)
			_rval.push_back( _file_out);
#else
			std::cerr << "File is not avaliable" << std::endl;
			std::abort();
#endif
		}
	}
	return _rval;
}
#endif //OLD_PARSER
}

#endif /* SOCKET_PARESER_H_ */
