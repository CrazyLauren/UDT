// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * main.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 12.04.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <cstdlib>
#include <iostream>
#include <deftype>
#include <share_socket.h>
#include <revision.h>

#include <internel_protocol.h>
#include <parser_in_protocol.h>
#include <programm_id.h>
#include <CParserFactory.h>
#include <tclap/CmdLine.h>
#include <logging/CShareLogArgsParser.h>

#include "core/CCore.h"

#include "core/CDescriptors.h"
#include "core/CResources.h"
#include "core/CConfigure.h"
#include "core/CDataObject.h"

#include "io/CParserFactoryState.h"
#include "io/CLinkDiagnostic.h"
#include "io/CChannelDiagnostics.h"
#include "io/CKernelIo.h"
#include "io/tcp/ITcpIOManager.h"
#include "io/tcp/CKernelIOByTCP.h"
#include "io/tcp/CKernelIOByTCPClient.h"
#include "io/frontend/CExternalChannel.h"
#include "io/main/CMainChannelFactory.h"
#include "io/main/CMainChannelFactoryRegister.h"
#include "io/main/CUdpMainChannel.h"
#include "io/main/CTcpServerMainChannel.h"
#include "io/main/CTcpClientMainChannel.h"
#include "io/main/CSmMainChannel.h"
#include "io/tcp/CConnectionHandlerFactory.h"

#include "io/tcp/customer/CLocalLinkRegister.h"
#include "io/http/CHttpIOManagerRegister.h"
#include "io/tcp/kernel/CKernelServerLinkRegister.h"
#include "io/tcp/kernel/CKernelLinkRegister.h"

#include "services/CRoutingService.h"
#include "services/CInfoService.h"
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
	CConfig _main_settings = CConfigure::sMGetInstance().MGet().MChild(IMainChannel::CONFIGURE_NAME);
	LOG_IF(DFATAL, _main_settings.MIsEmpty()) << "The main channel settings is not exist. See configure file, tag " << IMainChannel::CONFIGURE_NAME;
	if(!_main_settings.MIsEmpty())
	{

		//udp
		if(_main_settings.MIsChild(CUdpMainChannel::NAME))
			CMainFactoryRegisterer<CUdpMainChannel>().MRegisterFactory();
		//sm
		if (_main_settings.MIsChild(CSmMainChannel::NAME))
			CMainFactoryRegisterer<CSmMainChannel>().MRegisterFactory();
		
		//tcp server
		if (_main_settings.MIsChild(CTcpServerMainChannel::NAME))
			CMainFactoryRegisterer<CTcpServerMainChannel>().MRegisterFactory();
		
		//tcp client
		if (_main_settings.MIsChild(CTcpClientMainChannel::NAME))
			CMainFactoryRegisterer<CTcpClientMainChannel>().MRegisterFactory();
	}
		
<<<<<<< HEAD
	new CMainChannelFactory();
=======
	g_main_channel_factory=new CMainChannelFactory();
}
void remove_def_main_channels()
{
	delete g_main_channel_factory;
	g_main_channel_factory = NULL;
>>>>>>> 3b9273e... See ChangeLog.txt
}
void initialize_def_links()
{
	new CConnectionHandlerFactory();
	//links --------
	// clink
	CIOLocalLinkRegister _link;
	_link.MRegisterFactory();
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
	if (CConfigure::sMGetInstance().MGet().MFind(CKernelIOByTCP::NAME))
		CKernelIOByTCPRegister().MRegisterFactory();
	else
		VLOG(2) << "No " << CKernelIOByTCPRegister::NAME;

	if (CConfigure::sMGetInstance().MGet().MFind(
		CKernelIOByTCPClient::NAME))
		CKernelIOByTCPClientRegister().MRegisterFactory();
	else
			VLOG(2)<<"No "<<CKernelIOByTCPClient::NAME;

	if (CConfigure::sMGetInstance().MGet().MFind(
		CExternalChannel::NAME))
		CExternalChannelRegister().MRegisterFactory();
	else
		VLOG(2)<<"No "<<CExternalChannelRegister::NAME;

	if (CConfigure::sMGetInstance().MGet().MFind(CHttpIOManagerRegister::NAME))
		CHttpIOManagerRegister().MRegisterFactory();
	else
		VLOG(2)<<"No "<<CHttpIOManagerRegister::NAME;
}
void initialize_extern_modules()
{
	NUDT::CResources::sMGetInstance().MLoad();
}
void initialize_def_sevices()
{
<<<<<<< HEAD
	new CParserFactory(); //todo IState
	new CRoutingService();
	new CInfoService();
	new CPacketDivisor();
=======
	g_CParserFactory=new CParserFactory(); //todo IState
	g_CRoutingService=new CRoutingService();
	g_CInfoService=new CInfoService();
	g_CPacketDivisor=new CPacketDivisor();
}
void remove_def_sevices()
{
	delete g_CPacketDivisor;
	g_CPacketDivisor=NULL;

	delete g_CInfoService;
	g_CInfoService=NULL;

	delete g_CRoutingService;
	g_CRoutingService=NULL;

	delete g_CParserFactory;
	g_CParserFactory=NULL;
>>>>>>> 3b9273e... See ChangeLog.txt
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
	
	char const * _pname = argv[0];
	char const * const _ps = strrchr(_pname, '/');
	_pname = _ps ? (_ps + 1) : _pname;
	char const * const _pbs = strrchr(_pname, '\\');
	_pname = _pbs ? (_pbs + 1) : _pname;
	char const* _def_name = "?";
	_pname = _pname ? _pname : _def_name;

	ValueArg<std::string> _name("n", "name",
			"id name of kernel (by default program name)", false, _pname,
			"string", _cmd);

	ValueArg<std::string> _config("c", "config",
			"configure path (json or xml). By default ./default_kernel_config.json.", false,
			"./default_kernel_config.json", "path to json or xml config", _cmd);

	try
	{
		_cmd.parse(argc, argv);
		init_share_trace(_cmd.getProgramName().c_str());
		init_id(_name.getValue().c_str(), E_KERNEL, g_version);

	} catch (ArgException &e)  // catch any exceptions
	{
		cerr << "Invalid argument : " << e.error() << " for  " << e.argId()
				<< endl;
		exit(EXIT_FAILURE);
	}
	new CCore();
	new CDescriptors();

	//default configure
	{
		NSHARE::CConfig aConf;
		NSHARE::CText _path(_config.getValue());
		aConf.MGetIfSet("path", _path);
		if (_path.find(".xml") != NSHARE::CText::npos)
		{
			new CConfigure(_path, CConfigure::XML);
		}
		else if (_path.find(".json") != NSHARE::CText::npos)
		{
			new CConfigure(_path, CConfigure::JSON);
		}
		else
			LOG(DFATAL)<<"Invalid type of Configuration file - "<<_path<<".";
		CConfigure::sMGetInstance().MGet().MBlendWith(aConf);
	}

	new CDataObject();

	//parsers
	{
		const CConfig& _p = CConfigure::sMGetInstance().MGet().MChild(
				NUDT::CResources::NAME);
		new NUDT::CResources(_p);
	}
	new CParserFactoryState();
}

void perpetual_loop()
{
	for (;; NSHARE::sleep(10000))
		;
}
void start()
{
	new CKernelIo();

	std::cout << "Starting..." << std::endl;
	CCore::sMGetInstance().MStart();
	std::cout << "Working ..." << std::endl;
}
