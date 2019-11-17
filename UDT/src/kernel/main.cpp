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
#include <SHARE/share_socket.h>
#include <SHARE/revision.h>

#include <internel_protocol.h>
#include <parser_in_protocol.h>
#include <udt/programm_id.h>
#include <udt/CParserFactory.h>
#include <tclap/CmdLine.h>
#include <SHARE/logging/CShareLogArgsParser.h>

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
#include "services/CAutoSearchByEthernet.h"
#include "services/CTimeDispatcher.h"

using namespace NUDT;
DECLARATION_VERSION_FOR(Kernel)
static NSHARE::version_t const g_version(MAJOR_VERSION_OF(Kernel), MINOR_VERSION_OF(Kernel), REVISION_OF(Kernel));
CAutoSearchByEthernet* g_CAutoSearchByEthernet = NULL;
CTimeDispatcher* g_CTimeDispatcher = NULL;
bool g_wait_for_input=false;

void initialize_def_main_channels();
void remove_def_main_channels();
void initialize_def_links();
void initialize_def_io_managers();
//void initialize_extern_modules();
void initialize_def_sevices();
void remove_def_sevices();
void remove_def_io_managers();
void initialize_core(int argc, char* argv[]);
void initalize_io();
void remove_io();
void perpetual_loop();
void start();
void stop();

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

	//initialize_extern_modules();

	initalize_io();
	//-----------------------------

	start();
	perpetual_loop();
	stop();
//	remove_extern_modules();
	remove_def_io_managers();
	remove_def_main_channels();
	remove_io();
	remove_def_sevices();

	return EXIT_SUCCESS;
}

CMainChannelFactory* g_main_channel_factory=NULL;
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
		
	g_main_channel_factory=new CMainChannelFactory();
}
void remove_def_main_channels()
{
	delete g_main_channel_factory;
	g_main_channel_factory = NULL;
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

	if (CConfigure::sMGetInstance().MGet().MFind(CAutoSearchByEthernet::NAME))
		g_CAutoSearchByEthernet=new CAutoSearchByEthernet();
	else
		VLOG(2)<<"No "<<CAutoSearchByEthernet::NAME;

	if (CConfigure::sMGetInstance().MGet().MFind(CTimeDispatcher::NAME))
		g_CTimeDispatcher=new CTimeDispatcher();
	else
		VLOG(2)<<"No "<<CTimeDispatcher::NAME;
}
void remove_def_io_managers()
{
	if(g_CAutoSearchByEthernet)
		delete g_CAutoSearchByEthernet;
	g_CAutoSearchByEthernet = NULL;

	if(g_CTimeDispatcher)
		delete g_CTimeDispatcher;
	g_CTimeDispatcher = NULL;
}
/*
void initialize_extern_modules()
{
	NUDT::CResources::sMGetInstance().MLoad();
}
*/

CParserFactory* g_CParserFactory=NULL;
CRoutingService* g_CRoutingService=NULL;
CInfoService* g_CInfoService=NULL;
CPacketDivisor* g_CPacketDivisor=NULL;


void initialize_def_sevices()
{
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
}
CKernelIo* g_CKernelIo=NULL;
void remove_io()
{
	delete g_CKernelIo;
	g_CKernelIo=NULL;
}
void initalize_io()
{
	g_CKernelIo=new CKernelIo();
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

    ValueArg<bool> _wait_for("w", "wait",
        "Stop if any character is entered", false, g_wait_for_input,
        "true false", _cmd);

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
        g_wait_for_input=_wait_for.getValue();
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
NSHARE::CCondvar g_condvar_wait;
NSHARE::CMutex g_condvar_mutex(NSHARE::CMutex::MUTEX_NORMAL);
bool g_double_ctrl_c=false;

#ifdef _WIN32
BOOL WINAPI consoleHandler(DWORD signal)
{
    if (signal == CTRL_C_EVENT)
    {
#else
#   include <signal.h>
void consoleHandler(int s)
{
    {
#endif
        if (g_double_ctrl_c)
        {
            std::cout << "Double Ctrl-C..." << std::endl;
            std::cout << "Force stopped..." << std::endl;
            exit(EXIT_FAILURE);
        }

        g_double_ctrl_c = true;
        std::cout << "Ctrl-C handled..." << std::endl;
        std::cout << "Run stopping kernel..." << std::endl;
        g_condvar_wait.MSignal();
    }
#ifdef _WIN32
    return TRUE;
#endif
}

void perpetual_loop()
{
    LOG(INFO)<<"The kernel is started";

    if(g_wait_for_input)
    {
        std::cout << "Press any key ..." << std::endl;
        getchar();
    }
    else
    {

#ifdef _WIN32
        if (!SetConsoleCtrlHandler(consoleHandler, TRUE))
        {
            std::cerr<<"ERROR: Could not set control handler. Continue ..."<<std::endl;
        }
#else
        struct sigaction sigIntHandler;

        sigIntHandler.sa_handler = consoleHandler;
        sigemptyset(&sigIntHandler.sa_mask);
        sigIntHandler.sa_flags = 0;

        sigaction(SIGINT, &sigIntHandler, NULL);
#endif
        NSHARE::CRAII<NSHARE::CMutex> _lock(g_condvar_mutex);

        g_condvar_wait.MTimedwait(&g_condvar_mutex);
    }
}
void stop()
{

	LOG(INFO)<<"Stopping the kernel";

	std::cout << "Stopping ..." << std::endl;
	CCore::sMGetInstance().MStop();
}
void start()
{
	std::cout << "Starting..." << std::endl;
	CCore::sMGetInstance().MStart();
	std::cout << "Working ..." << std::endl;
}
