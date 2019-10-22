// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * tasks.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 12.04.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
# ifdef __QNX__
#include <deftype>
#include <SHARE/tasks.h>
#include <SHARE/fdir.h>
#include <sys/procfs.h>
#include <string.h>
#include <fcntl.h>

namespace NSHARE
{
	void process_name_list(Strings *aOut)
	{
		id_of_all_process_t _to;
		pid_list(&_to);
		for (id_of_all_process_t::iterator _it = _to.begin();
				_it != _to.end(); ++_it)
		aOut->push_back(process_name(*_it));
	}
	void pid_list(id_of_all_process_t* aOut)
	{
		std::list<std::string> _to;
		NSHARE::get_name_of_files_of_dir(&_to, "/proc");

		for (std::list<std::string>::iterator _it = _to.begin(); _it != _to.end();
				++_it)
		{
			std::string::const_iterator _jt = _it->begin();
			for (; _jt != _it->end(); ++_jt)
			if (!isdigit(*_jt))
			break;
			if (_jt == _it->end() && !_it->empty())
			{
				aOut->push_back(atoi(_it->c_str()));
			}
		}
	}

	NSHARE::CText process_name(CThread::process_id_t pid)
	{

		char _paths[PATH_MAX];

		int _fd;
// 1) set up structure
		struct
		{
			procfs_debuginfo info;
			char buff[PATH_MAX];
		}_name;

		sprintf(_paths, "/proc/%d/as", pid);
		if ((_fd = open(_paths, O_RDONLY)) == -1)
		{
			return "";
		}
// 2) ask for the name
		NSHARE::CText _str_name;
		if (devctl(_fd, DCMD_PROC_MAPDEBUG_BASE, &_name, sizeof(_name), 0) != EOK)
		{
			if (pid == 1)
			{
				_str_name += "(procnto)";
			}
			else
			{
				_str_name += "(n/a)";
			}
		}
		else
		{
			_str_name = _name.info.path;
			_str_name.erase(0, _str_name.find_last_of("/") + 1);
		}

		close(_fd);
		return _str_name;
	}
} //namespace USHARE
# elif _WIN32
#include <deftype>
#include <SHARE/tasks.h>
#include <psapi.h>
//#pragma comment(lib,"Psapi.lib")
namespace NSHARE
{
	void pid_list(id_of_all_process_t* aOut)
	{
		// Get the list of process identifiers.
		DWORD aProcesses[2048], cbNeeded = 0;
		memset(aProcesses, 0, sizeof(aProcesses));
		//This returns a list of handles to processes running on the system as an array.
		if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
		return;

		// Calculate how many process identifiers were returned.
		DWORD cProcesses = cbNeeded / sizeof(DWORD);
		for (unsigned i = 0; i < cProcesses; i++)
		if (aProcesses[i] != 0)
		aOut->push_back(aProcesses[i]);
	}
	void process_name_list(Strings *aOut)
	{
		id_of_all_process_t _to;
		pid_list(&_to);
		for (id_of_all_process_t::iterator _it = _to.begin();
				_it != _to.end(); ++_it)
		aOut->push_back(process_name((int)*_it));
	}

	NSHARE::CText process_name(CThread::process_id_t processID)
	{
		TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

		// Get a handle to the process.
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
				FALSE, processID);

		// Get the process name.
		if (INVALID_HANDLE_VALUE != hProcess)
		{
			HMODULE hMod;
			DWORD cbNeeded;

			//Given a handle to a process, this returns all the modules running within the process.
			//The first module is the executable running the process,
			//and subsequent handles describe DLLs loaded into the process.
			if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
			{
				//This function returns the short name for a module,
				//typically the file name portion of the EXE or DLL
				GetModuleBaseName(hProcess, hMod, szProcessName,
						sizeof(szProcessName) / sizeof(TCHAR));
			}
		}
		NSHARE::CText _text(szProcessName);
		//close the process handle
		CloseHandle(hProcess);
		return _text;
	}
}
# elif unix
//readproc-code-48dc5a750d6306d6088ca924123a62d009b4bdf2.zip
#include <deftype>
#include <SHARE/tasks.h>
#include <SHARE/fdir.h>
#include <signal.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h> // for opendir(), readdir(), closedir()
#include <sys/stat.h> // for stat()
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#define PROC_DIRECTORY "/proc/"
#define CASE_SENSITIVE    1
#define CASE_INSENSITIVE  0
#define EXACT_MATCH       1
#define INEXACT_MATCH     0

namespace NSHARE
{

void pid_list(id_of_all_process_t* aOut)
{
	std::list<std::string> _to;
	NSHARE::get_name_of_files_of_dir(&_to, PROC_DIRECTORY);

	for (std::list<std::string>::iterator _it = _to.begin(); _it != _to.end();
			++_it)
	{
		std::string::const_iterator _jt = _it->begin();
		for (; _jt != _it->end(); ++_jt)
			if (!isdigit(*_jt))
				break;
		if (_jt == _it->end() && !_it->empty())
		{
			aOut->push_back(atoi(_it->c_str()));
		}
	}
}
void process_name_list(Strings *aOut)
{
	id_of_all_process_t _to;
	pid_list(&_to);
	for (id_of_all_process_t::iterator _it = _to.begin();
			_it != _to.end(); ++_it)
	aOut->push_back(process_name(*_it));
}
NSHARE::CText process_name(CThread::process_id_t pid)
{

	char _paths[PATH_MAX];
	char chrarry_NameOfProcess[300];
	char* chrptr_StringToCompare = NULL;
	sprintf(_paths, "%s%d/cmdline",PROC_DIRECTORY, pid);

	FILE* fd_CmdLineFile = fopen(_paths, "rt"); // open the file for reading text
	if (fd_CmdLineFile)
	{
		fscanf(fd_CmdLineFile, "%s", chrarry_NameOfProcess); // read from /proc/<NR>/cmdline
		fclose(fd_CmdLineFile); // close the file prior to exiting the routine
		char* const _slash=strrchr(chrarry_NameOfProcess, '/');
		if (_slash!=NULL)
			chrptr_StringToCompare = _slash + 1;
		else
			chrptr_StringToCompare = chrarry_NameOfProcess;

		//printf("Process name: %s\n", chrarry_NameOfProcess);
		//printf("Pure Process name: %s\n", chrptr_StringToCompare );

		return CText(chrptr_StringToCompare);

	}
	return CText();
}
}
#endif

namespace NSHARE
{
bool is_process_exist(CThread::process_id_t pID)
{
	VLOG(2) << "Checking  process " << pID << " exist.";
#if defined(_WIN32)
	HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION, false, pID);
	if (!handle)
	{
		VLOG(2)<<"The process "<<pID<<" is not exist.";
		return false;
	}
	else
	{
		DWORD _status=0;
		bool _result=GetExitCodeProcess(handle,&_status)!=FALSE;
		CloseHandle(handle);
		VLOG_IF(2,_result)<<"The process "<<pID<<" is   exist.";
		VLOG_IF(2,!_result)<<"The process "<<pID<<" is not   exist.";
		return _status==STILL_ACTIVE;
	}
#else
	if (kill(pID, 0) != 0 && errno != EPERM)
	{
		VLOG(2) << "The process " << pID << " is not  exist.";
		return false;
	}
	else
	{
		VLOG(2) << "The process " << pID << " is  exist.";
		return true;
	}
#endif
}
}
