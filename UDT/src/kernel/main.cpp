/*
 * main.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 12.04.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <cstdlib>
#include <iostream>
#include <deftype>
#include <Socket.h>
#include <revision.h>

#include <internel_protocol.h>
#include <parser_in_protocol.h>
#include <programm_id.h>
#include <CParserFactory.h>
#include <tclap/CmdLine.h>
#include <logging/CShareLogArgsParser.h>

#include "core/IState.h"

#include "core/CDescriptors.h"
#include "core/CResources.h"
#include "core/CDiagnostic.h"
#include "core/CConfigure.h"
#include "core/CDataObject.h"

#include "io/ITcpIOManager.h"
#include "io/CLinkDiagnostic.h"
#include "io/CChannelDiagnostics.h"
#include "io/CKernelIo.h"
#include "io/CKernelIOByTCP.h"
#include "io/CKernelIOByTCPClient.h"
#include "io/frontend/CExternalChannel.h"
#include "io/main/CMainChannelFactory.h"
#include "io/main/CMainChannelFactoryRegister.h"
#include "io/main/CUdpMainChannel.h"
#include "io/main/CTcpServerMainChannel.h"
#include "io/main/CTcpClientMainChannel.h"
#include "io/main/CSmMainChannel.h"
#include "io/CConnectionHandlerFactory.h"

#include "io/customer/CLocalLinkRegister.h"
#include "io/http/CHttpNewConncetion.h"
#include "io/kernel/CKernelServerLinkRegister.h"
#include "io/kernel/CKernelLinkRegister.h"

#include "services/CRoutingService.h"
#include "services/CInfoService.h"
#include "services/CHttpRequestHandler.h"
#include "services/CPacketDivisor.h"

using namespace NUDT;
DECLARATION_VERSION_FOR(Kernel)
static NSHARE::version_t const g_version(MAJOR_VERSION_OF(Kernel), MINOR_VERSION_OF(Kernel), REVISION_OF(Kernel));

void initialize_def_main_channels();
void initialize_def_links();
void initialize_def_io_managers();
void initialize_extern_modules();
void initialize_def_sevices();
void initialize_core(int argc, char* argv[]);
void perpetual_loop();
void start();

int main(int argc, char *argv[])
{
#ifdef _WIN32
	WSADATA _data;
	WSAStartup(MAKEWORD(2,2), &_data);
#endif

	initialize_core(argc, argv);

	initialize_def_sevices();

	initialize_def_main_channels();

	initialize_def_io_managers();

	initialize_def_links();

	initialize_extern_modules();

	//-----------------------------

	start();
	perpetual_loop();

	return EXIT_SUCCESS;
}

void initialize_def_main_channels()
{
	{
		//udp
		CMainFactoryRegisterer<CUdpMainChannel> _udp;
		_udp.MRegisterFactory();
		//sm
		CMainFactoryRegisterer<CSmMainChannel> _sm;
		_sm.MRegisterFactory();
		//tcp server
		CMainFactoryRegisterer<CTcpServerMainChannel> _tcpser;
		_tcpser.MRegisterFactory();
		//tcp client
		CMainFactoryRegisterer<CTcpClientMainChannel> _tcpcl;
		_tcpcl.MRegisterFactory();
	}
	new CMainChannelFactory();
}
void initialize_def_links()
{
	new CConnectionHandlerFactory();
	//links --------
	// clink
	CIOLocalLinkRegister _link;
	_link.MRegisterFactory();
	//http
	CIOHttplLinkRegister _http;
	_http.MRegisterFactory();
	//kernel client
	CIOKernelLinkRegister _ker;
	_ker.MRegisterFactory();
	//kernel serv
	CKernelServerLinkRegister _ker2;
	_ker2.MRegisterFactory();
}

void initialize_def_io_managers()
{
	//tcp
	CKernelIOByTCPRegister _tcp_io;
	_tcp_io.MRegisterFactory();
	CKernelIOByTCPClientRegister _tcp_client;
	_tcp_client.MRegisterFactory();

	CExternalChannelRegister _front;
	_front.MRegisterFactory();
}
void initialize_extern_modules()
{

	CResources::sMGetInstance().MLoad();
}
void initialize_def_sevices()
{
	new CParserFactory(); //todo IState
	new CRoutingService();
	new CInfoService();
	new CPacketDivisor();
	new CHttpRequestHandler();
}

void initialize_core(int argc, char* argv[])
{
	using namespace NSHARE;
	using namespace TCLAP;
	using namespace std;

	std::stringstream _ver;
	_ver << g_version;

	CmdLine _cmd("Kernel of UDT...", ' ', _ver.str(), true);
	CShareLogArgsParser _logging("v", "verbose");
	_cmd.add(_logging);

	ValueArg<std::string> _name("n", "name",
			"id name of kernel (by default program name)", false, argv[0],
			"string", _cmd);

	ValueArg<std::string> _config("c", "config",
			"configure path (json or xml). By default ./kernel.json.", false,
			"./kernel.json", "path to json or xml config", _cmd);

	try
	{
		_cmd.parse(argc, argv);

		init_id(_name.getValue().c_str(), E_KERNEL, g_version);
		init_share_trace(_cmd.getProgramName().c_str());

	} catch (ArgException &e)  // catch any exceptions
	{
		cerr << "Invalid argument : " << e.error() << " for  " << e.argId()
				<< endl;
		exit(EXIT_FAILURE);
	}
	new CDiagnostic();
	new CDescriptors();

	//default configure
	{
		NSHARE::CConfig aConf;
		NSHARE::CText _path(_config.getValue());
		aConf.MGetIfSet("path", _path);
		CConfigure* _p_conf = NULL;
		if (_path.find(".xml") != NSHARE::CText::npos)
		{
			_p_conf = new CConfigure(_path, CConfigure::XML);
		}
		else if (_path.find(".json") != NSHARE::CText::npos)
		{
			_p_conf = new CConfigure(_path, CConfigure::JSON);
		}
		else
			LOG(DFATAL)<<"Invalid type of Configuration file - "<<_path<<".";
		_p_conf->MGet().MBlendWith(aConf);
	}

	new CDataObject();

	//parsers
	{
		const CConfig* _p = CConfigure::sMGetInstance().MGet().MChildPtr(
				"modules");
		//LOG_IF(DFATAL,!_p) << "Invalid config file.Key modules is not exist.";
		std::vector<NSHARE::CText> _text;
		if (_p)
		{
			ConfigSet::const_iterator _it = _p->MChildren().begin();

			for (; _it != _p->MChildren().end(); ++_it)
				_text.push_back(_it->MKey());
		}
		new CResources(_text);
	}
}
void perpetual_loop()
{
	for (;; NSHARE::sleep(10000))
		;
}
void start()
{
	CKernelIo* _io = new CKernelIo();

	std::cout << "Starting..." << std::endl;
	_io->MInit();
	std::cout << "Working ..." << std::endl;
}
