// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * sniffer.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 24.06.2016
 *      Author:  (https://github.com/CrazyLauren/UDT)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */

#include <deftype>
#include <console.h>
#include <algorithm>
#include <share_socket.h>
#include <Socket/socket_parser.h>

#define DEFAULT_CFG_FILE "./sniffer.cfg"

static bool g_is_console_out = true;

void console_out();

NSHARE::CConfig FConfig;
#define XML_PREFIX "sniffer"

void get_sniffers(std::vector<NSHARE::ISocket*>& aTo,NSHARE::CConfig const & _sniffer)
{
	LOG(INFO)<<"init sniffer";
	if(!_sniffer.MIsEmpty())
		aTo=NSHARE::get_sockets(_sniffer);
	else
	{
		LOG(WARNING) << "The sniffer from Socket isn't exist."<<_sniffer;
	}
	LOG(INFO)<<"The number of sniffers "<<aTo.size();
}
std::vector<NSHARE::ISocket*> gSock_sniffer_from;
std::vector<NSHARE::ISocket*> gSock_sniffer_to;
NSHARE::ISocket* gSock_from = NULL;
NSHARE::ISocket* gSock_to = NULL;
void setting(char* argv[])
{
	LOG(INFO)<< "Initialization configuration file.Path:" << DEFAULT_CFG_FILE << ".";
	std::fstream _stream;
	_stream.open(DEFAULT_CFG_FILE);
	LOG_IF(FATAL,!_stream.is_open())<<"***ERROR***:Configuration file - "<<DEFAULT_CFG_FILE<<".";

	FConfig.MFromXML(_stream);
	_stream.close();
	VLOG(2)<<FConfig;
	if(FConfig.MIsEmpty())
	{
		LOG(FATAL)<<"***ERROR***:Configuration file - "<<DEFAULT_CFG_FILE<<".";
		std::abort();
	}

	{
		NSHARE::CConfig _config=FConfig.MChild("first");
		if(_config.MIsEmpty())
		{
			LOG(FATAL)<<"***ERROR***:There is not <first> tag in the Configuration file  "<<DEFAULT_CFG_FILE <<".";
			std::abort();
		}

		std::vector<NSHARE::ISocket*> _rval= NSHARE::get_sockets(_config);
		LOG(INFO)<<"Number of sockets "<<_rval.size();
		if(!_rval.empty())
			gSock_from =_rval.front();
		if (gSock_from)
			LOG(INFO)<< *gSock_from;
		else
		LOG(WARNING) << "The first Socket isn't exist.";

		get_sniffers(gSock_sniffer_from,_config.MChild( "sniffer"));
	}
	{
		NSHARE::CConfig _config=FConfig.MChild("second");
		if(_config.MIsEmpty())
		{
			LOG(FATAL)<<"***ERROR***:There is not <second> tag in the Configuration file  "<<DEFAULT_CFG_FILE <<".";
			std::abort();
		}

		std::vector<NSHARE::ISocket*> _rval= NSHARE::get_sockets(_config);
		LOG(INFO)<<"Number of sockets "<<_rval.size();
		if(!_rval.empty())
		gSock_to =_rval.front();

		if (gSock_to)
		LOG(INFO)<< *gSock_to;
		else
		LOG(WARNING) << "The second Socket isn't exist.";

		get_sniffers(gSock_sniffer_to,_config.MChild( "sniffer"));
	}
}

int main(int argc, char *argv[])
{
#ifdef _WIN32
	WSADATA _data;
	WSAStartup(MAKEWORD(2,2), &_data);
#endif
	init_trace(argc, argv);
	setting(argv);

	console_out();
	std::cout << "Arrivederci!" << std::endl;

	return EXIT_SUCCESS;
}

class CWondefull: NSHARE::CAsyncSocket
{
public:
	NSHARE::ISocket* FSockTo;
	std::vector<NSHARE::ISocket*> const FSockSniffer;
	CWondefull(NSHARE::ISocket*& aSockFrom, NSHARE::ISocket* aSockTo,
			std::vector<NSHARE::ISocket*> aSockSniffer) :
			NSHARE::CAsyncSocket(aSockFrom), FSockTo(aSockTo), FSockSniffer(
					aSockSniffer)
	{
		FBytesRead = 0;
		FBytesSend = 0;
		FCountPackets = 0;
		NSHARE::CAsyncSocket::MStart();
		FMyNumer = ++FCounter;
		LOG(WARNING)<<"Channel "<<FMyNumer<<" is "<<*aSockFrom;
		LOG(INFO)<<"Sniffers "<<aSockSniffer.size();
		std::vector<NSHARE::ISocket*>::const_iterator _it =
				FSockSniffer.begin();
		for (; _it != FSockSniffer.end(); ++_it)
		{
			(void)0;
			LOG(INFO)<<*(*_it);
		}
	}
	virtual void MReceivedData(const NSHARE::ISocket::data_t& aData)
	{
		NSHARE::CRAII<NSHARE::CMutex> _blocked(FMutex);
		LOG(INFO)<<"From Channel "<<FMyNumer<<":\n"<<aData;
		++FCountPackets;
		FBytesRead += aData.size();
		if(!aData.empty())
		{
			VLOG(2)<<"Send "<<aData.size();
			if (FSockTo&&FSockTo->MSend(aData).MIs())
				FBytesSend += aData.size();
			else
			{
				LOG_IF(ERROR,FSockTo)<<"Cannot send data to:"<<FSockTo;
				LOG_IF(ERROR,!FSockTo)<<"Cannot send data as null pointer.";
			}
			if(!FSockSniffer.empty())
			{
				std::vector<NSHARE::ISocket*>::const_iterator _it=FSockSniffer.begin();
				for(;_it!=FSockSniffer.end();++_it)
				{
					CHECK_NOTNULL(*_it);
					if((*_it)->MSend(aData).MIs())
						LOG(INFO)<<"Send to sniffer."<<*(*_it);
					else
						LOG(ERROR)<<"Cannot send to sniffer"<<*(*_it);
				}
			}
		}
	}
	std::ostream & MPrint(std::ostream & aStream) const
	{
		if(!MGetSocket())
		return aStream;
		aStream<<" Channel "<<FMyNumer<<". "<<*MGetSocket()<<"\n";
		aStream<<"The number of packet being received:"<<FCountPackets<<".\t Received bytes:"<<FBytesRead<<". Forwarded bytes:"<<FBytesSend<<".\n";
		return aStream;
	}
	std::size_t FCountPackets;
	std::size_t FBytesRead;
	std::size_t FBytesSend;

	static NSHARE::CMutex FMutex;
	int FMyNumer;
	static int FCounter;
};
NSHARE::CMutex CWondefull::FMutex;
int CWondefull::FCounter = 0;
inline void console_out()
{
	time_t _time;
	g_is_console_out = true;
	CWondefull _pipe1(gSock_from, gSock_to, gSock_sniffer_from);
	CWondefull _pipe2(gSock_to, gSock_from, gSock_sniffer_to);
	for (; g_is_console_out;)
	{
		_time = time(NULL);
		NSHARE::NCONSOLE::clear();
		std::cout << ctime(&_time) << "\n";
		_pipe1.MPrint(std::cout);
		_pipe2.MPrint(std::cout);
		std::cout << std::endl;

		for (int i = 0; ++i < 10; NSHARE::usleep(100000))
			if (!g_is_console_out)
				break;
	}
	NSHARE::NCONSOLE::clear();
}
