/*
 * socket_parser.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 25.06.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  
 
#include <deftype>
#include <share_socket.h>
#include <Socket/socket_parser.h>

#ifdef DEX_SOCKET_EXIST
#define KEY_DEX CDex::NAME
#else
#	define KEY_DEX "dex"
#endif
#undef KEY_TCP
#define KEY_TCP CTCP::NAME
#undef KEY_PIPE
#define KEY_PIPE CFifo::NAME
#define KEY_PIPE_SERVER CFIFOServer::NAME

#undef KEY_UNIX
#ifdef UNIX_SOCKET_EXIST
#define KEY_UNIX CUnixDGRAM::NAME
#else
#define KEY_UNIX "unix_dgram"
#endif


#undef KEY_FILE
#define KEY_FILE CSocketFile::NAME
#undef KEY_UDP
#define KEY_UDP CUDP::NAME
#define KEY_RS485 "rs485"
#define KEY_RS422 "rs422"
#define KEY_RS232 "rs232"

#define KEY_SM_SERVER CSharedMemoryServer::NAME
#define KEY_SM_CLIENT CSharedMemoryClient::NAME
#define KEY_LOOP_BACK CLoopBack::NAME

namespace NSHARE
{

std::vector<ISocket*> get_sockets(CConfig const& aChild)
{
	std::vector<ISocket*> _rval;
	const ConfigSet& _child = aChild.MChildren();
	VLOG(2)<<"The number of socket child :"<<_child.size();
	for (ConfigSet::const_iterator _it_up = _child.begin();
			_it_up != _child.end(); ++_it_up)
	{
		NSHARE::CText _name = (*_it_up)->MKey();
		VLOG(2)<<"Name Socket "<<_name;
		if (_name == KEY_DEX)
		{
#ifdef DEX_SOCKET_EXIST
			CDex* _dex_in = NULL;
			CDex* _dex_out = NULL;

			ConfigSet const& _child = _it_up->MChildren();

			ConfigSet::const_iterator _it = _child.begin();
			ConfigSet::const_iterator _it_end = _child.end();
			for (; _it != _it_end; ++_it)
			{
				CDex*_dex = new CDex(_it->MRead());
				if (!_dex->MIsOpen())
				{
					std::cerr << "Cannot open dex channel." << std::endl;
					LOG(DFATAL)<<"Cannot open dex channel by param "<<(*_it);
					if (_dex_in)
						delete _dex_in;
					if (_dex_out)
						delete _dex_out;
					_dex_in = NULL;
					_dex_out = NULL;
					break;
				}
				else
				{
					if (_dex->MGetParam().FDirect == CDex::eIn)
					{
						DCHECK(!_dex_in);
						if (_dex_in)
							delete _dex_in;
						_dex_in = _dex;
					}
					else
					{
						CHECK_EQ(_dex->MGetParam().FDirect, CDex::eOut);

						DCHECK(!_dex_out);
						if (_dex_out)
							delete _dex_out;
						_dex_out = _dex;
					}
				}
			}
			if (_dex_in && _dex_out)
				_rval.push_back(new CDexDuplex(_dex_in, _dex_out));
			else if (_dex_in)
				_rval.push_back(_dex_in);
			else if (_dex_out)
				_rval.push_back(_dex_out);
#else
			LOG(FATAL)<<"Dex is not available";
			std::cerr << "Dex is not available" << std::endl;
			throw std::domain_error("error");
#endif
		}
		else if (_name == KEY_TCP)
		{
#ifdef TCP_SOCKET_EXIST
			NSHARE::net_address _addr(*_it_up);

			if (!_addr.MIsValid())
			{
				LOG(ERROR)<<"net addr is not valid "<<(*_it_up);
				continue;
			}
			if (_addr.ip.MIs())
				_rval.push_back(new NSHARE::CTCP(_addr));
			else
				_rval.push_back(new NSHARE::CTCPServer(_addr));

#else
			LOG(FATAL) << "Tcp is not available";
			std::cerr << "Tcp is not avaliable" << std::endl;
			throw std::domain_error("error");
#endif
		}
		else if (_name == KEY_PIPE)
		{
#ifdef PIPE_SOCKET_EXIST
			CFifo* _fifo = new CFifo(*_it_up);
			if (_fifo->MIsOpen())
				_rval.push_back(_fifo);
			else
			{
				LOG(DFATAL)<<"Cannot open fifo "<<(*_it_up);
				delete _fifo;
			}
#else
			LOG(FATAL) << "Pipe is not available";
			std::cerr << "Pipe is not avaliable" << std::endl;
			throw std::domain_error("error");
#endif
		}else if(_name == KEY_PIPE_SERVER)
		{
#ifdef PIPE_SOCKET_EXIST
			CFIFOServer* _fifo = new CFIFOServer(*_it_up);
			if (_fifo->MIsOpen())
				_rval.push_back(_fifo);
			else
			{
				LOG(DFATAL)<<"Cannot open fifo "<<(*_it_up);
				delete _fifo;
			}
#else
			LOG(FATAL) << "The pipe server is not available";
			std::cerr << "The pipe server is not avaliable" << std::endl;
			throw std::domain_error("error");
#endif
		}
		else if (_name == KEY_UNIX)
		{
#	ifdef UNIX_SOCKET_EXIST
			CUnixDGRAM* _unix = new CUnixDGRAM(_it_up->MRead());
			if(_unix->MIsOpen())
			_rval.push_back( _unix);
			else
			{
				LOG(DFATAL)<<"Cannot open fifo "<<(*_it_up);
				delete _unix;
			}
#	else
			LOG(FATAL) << "unix socket is not available";
			std::cerr << "unix socket is not avaliable" << std::endl;
			throw std::domain_error("error");
#	endif	//#	ifdef UNIX_SOCKET_EXIST
		}
		else if (_name == KEY_RS485 || _name == KEY_RS232 || _name == KEY_RS422)
		{
#ifdef SERIAL_SOCKET_EXIST
			using namespace NSHARE::NSerial;
			NSerial::ISerial* _p = NULL;
			if (_name == KEY_RS485)
			_p =new CRS485(*_it_up);
			else if (_name == KEY_RS232 || _name == KEY_RS422)
			_p = new CSerialPort(*_it_up);

			if (!_p->MIsOpen())
			{
				LOG(DFATAL)<<"Cannot open serial port "<<(*_it_up);
				delete _p;
				_p=NULL;
			}
			else
			_rval.push_back( _p);
#else
			LOG(FATAL) << "serial socket is not available";
			std::cerr << "serial socket is not avaliable" << std::endl;
			throw std::domain_error("error");
#endif
		}
		else if (_name == KEY_UDP)
		{
#ifdef UDP_SOCKET_EXIST
			CUDP* _udp = new CUDP(*_it_up);
			if(_udp->MIsOpen())
			_rval.push_back( _udp);
			else
			{
				LOG(DFATAL)<<"Cannot open udp "<<(*_it_up);
				delete _udp;
			}
#else
			LOG(FATAL) << "udp socket is not available";
			std::cerr << "udp socket is not avaliable" << std::endl;
			throw std::domain_error("error");
#endif
		}
		else if (_name == KEY_FILE)
		{
#ifdef FILE_SOCKET_EXIST
			CSocketFile* _file_in = NULL;
			CSocketFile* _file_out = NULL;

			ConfigSet const& _child = (*_it_up)->MChildren();

			ConfigSet::const_iterator _it = _child.begin();
			ConfigSet::const_iterator _it_end = _child.end();
			for (; _it != _it_end; ++_it)
			{

				CSocketFile*_file = new CSocketFile(*_it);

				if (!_file->MIsOpen())
				{
					std::cerr << "Cannot open file channel." << std::endl;
					LOG(DFATAL)<<"Cannot open file channel by param "<<(*_it);
					delete _file_in;
					delete _file_out;
					_file_in = NULL;
					_file_out = NULL;
					break;
				}
				else
				{
					if (_file->MGetSetting().FDirect == CSocketFile::eIn)
					{
						DCHECK(!_file_in);
						delete _file_in;
						_file_in = _file;
					}
					else
					{
						CHECK_EQ(_file->MGetSetting().FDirect, CSocketFile::eOut);

						DCHECK(!_file_out);
						delete _file_out;
						_file_out = _file;
					}
				}
			}

			if (_file_in && _file_out)
				_rval.push_back( new CSocketFileDuplex(_file_in, _file_out));
			else if (_file_in)
				_rval.push_back( _file_in);
			else if (_file_out)
				_rval.push_back( _file_out);
#else
			LOG(FATAL)<<"File is not available";
			std::cerr << "File is not avaliable" << std::endl;
			throw std::domain_error("error");
#endif
		}
		else if (_name == KEY_SM_SERVER)
		{
#ifdef SM_SOCKET_EXIST
			CSharedMemoryServer* _sm = new CSharedMemoryServer(*_it_up);
			if(_sm->MIsOpen())	_rval.push_back( _sm);
			else
			{
				LOG(DFATAL)<<"Cannot open sm server "<<(*_it_up);
				delete _sm;
			}
#else
			LOG(FATAL)<<"The Shared Memory server is not available";
			std::cerr << "Shared Memory server is not avaliable" << std::endl;
			throw std::domain_error("error");
#endif
		}
		else if (_name == KEY_SM_CLIENT)
		{
#ifdef SM_SOCKET_EXIST
			CSharedMemoryClient* _sm = new CSharedMemoryClient(*_it_up);
			if(_sm->MIsOpen())_rval.push_back( _sm);
			else
			{
				LOG(DFATAL)<<"Cannot open sm client "<<(*_it_up);
				delete _sm;
			}
#else
			LOG(FATAL)<<"The Shared Memory client is not available";
			std::cerr << "Shared Memory client is not avaliable" << std::endl;
			throw std::domain_error("error");
#endif
		}
		else if (_name == KEY_LOOP_BACK)
		{
#ifdef LOOP_BACK_SOCKET_EXIST
			CLoopBack* _loop = new CLoopBack(*_it_up);
			if(_loop->MIsOpen())_rval.push_back( _loop);
			else
			{
				LOG(DFATAL)<<"Cannot open loop back "<<(*_it_up);
				delete _loop;
			}
#else
			LOG(FATAL)<<"The Loop back is not available";
			std::cerr << "Loop back is not avaliable" << std::endl;
			throw std::domain_error("error");
#endif
		}
	}
	return _rval;
}
}
