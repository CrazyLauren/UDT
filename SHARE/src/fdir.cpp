// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * fdir.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 27.04.2014
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <malloc.h>
#include <SHARE/fdir.h>
#include <exception>
//todo replace to boost realization

#define FILE_COMPARE_BUFFER_SIZE 1024*512
#include <errno.h>
#ifdef _MSC_VER
#	include <direct.h>
#else
#	include <dirent.h>
#endif
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
namespace NSHARE
{

extern bool get_path_of_files_of_dir(std::list<std::string> *out,
		const std::string &directory)
{
	if (!get_name_of_files_of_dir(out, directory))
		return false;

	for (std::list<std::string>::iterator _it = out->begin(); _it != out->end();
			++_it)
	{
		std::string _str = directory;
		if (_str.compare(_str.length() - 1, 1, "/")!=0)
			_str += "/";
		_str += *_it;
		*_it = _str;
	}
	return true;
}
extern bool get_name_of_files_of_dir(std::list<std::string> *out,
		const std::string &directory)
{
#ifdef _WIN32
	::WIN32_FIND_DATA _data;
	HANDLE _find = ::FindFirstFile((directory+"\\*").c_str(), &_data);
	if (_find == INVALID_HANDLE_VALUE)
		return false;
	do
	{
		if (!(_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			out->push_back(_data.cFileName);
	} while (::FindNextFile(_find, &_data));
	::FindClose(_find);
	return true;
#else
	DIR *_dir;
	struct dirent *_ent;
	if ((_dir = opendir(directory.c_str())) != NULL)
	{
		for (; (_ent = readdir(_dir)) != NULL;)
		{
			std::string _name = _ent->d_name;
			if (_name.size() == 2 && !_name.compare(".."))
			continue;
			if (_name.size() == 1 && !_name.compare("."))
			continue;
			out->push_back(_name);
		}
		closedir(_dir);
	}
	else
	return false;
	return true;
#endif
}
extern bool get_files_of_dir(std::list<std::string> *out,
		const std::string &directory, NSHARE::CFlags<unsigned> const& aFlags)
{

	if (get_path_of_files_of_dir(out, directory))
	{
		if (aFlags.MGetFlag(E_INCLUDES_SUBDIR))
		{
			for (std::list<std::string>::iterator _it = out->begin();
					_it != out->end(); ++_it)
			{
				if (is_diretory(*_it))
				{
					get_path_of_files_of_dir(out, *_it);
					_it = out->erase(_it);
					continue;
				}
			}
		}
		return true;
	}
	return false;
}
extern bool is_diretory(const std::string &_name)
{
#ifdef _WIN32
	::WIN32_FIND_DATA _data;
	HANDLE _find = ::FindFirstFile(_name.c_str(), &_data);
	if (_find == INVALID_HANDLE_VALUE)
		return false;
	bool const _r =( _data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)!=0;
	::FindClose(_find);
	return _r;
#else

	struct stat buf;
	int rc = stat(_name.c_str(), &buf);
	if (rc != 0)
		throw std::runtime_error("stat calling error");
	return S_ISDIR(buf.st_mode);
#endif
}
#ifdef _WIN32
extern bool is_the_file_exist(const std::string &_name)
{
	DWORD dwAttrib = GetFileAttributes(_name.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES
			&& !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
#else
extern bool is_the_file_exist(const std::string &_name)
{
	struct stat buf;
	errno = 0;
	int rc = stat(_name.c_str(), &buf);
	if (rc != 0)
	{
		if (errno == ENOENT)
			return false;
		else
			throw std::runtime_error("stat calling error");
	}
	return true;
}
#endif
extern int filecmp(std::string const& aF1, std::string const& aF2)
{
	FILE *_f1 = fopen(aF1.c_str(), "r");
	if (_f1 == NULL)
		return -1;
	FILE *_f2 = fopen(aF2.c_str(), "r");
	if (_f2 == NULL)
	{
		fclose(_f1);
		return -1;
	}
	int _rval = filecmp(_f1, _f2);
	fclose(_f1);
	fclose(_f2);
	return _rval;
}

extern int filecmp(FILE* aF1, FILE* aF2)
{
	int _rval=0;
	uint8_t* _buf1=(uint8_t*)malloc (FILE_COMPARE_BUFFER_SIZE);
	uint8_t* _buf2=(uint8_t*)malloc (FILE_COMPARE_BUFFER_SIZE); 
	
	CHECK_NOTNULL(_buf1);
	CHECK_NOTNULL(_buf2);

	fseek(aF1, 0, SEEK_END);
	fseek(aF2, 0, SEEK_END);
	int _s1 = ftell(aF1), _s2 = ftell(aF2);
	if (_s1 < 0 || _s2 < 0)
	{
		_rval = -1;
		goto exit;
	}
	if (_s1 != _s2)
	{
		_rval = 1;
		goto exit;
	}
	fseek(aF1, 0, SEEK_SET);
	fseek(aF2, 0, SEEK_SET);
	do
	{
		_s1 = (int)fread(_buf1, sizeof(uint8_t), FILE_COMPARE_BUFFER_SIZE, aF1);
		_s2 = (int)fread(_buf2, sizeof(uint8_t), FILE_COMPARE_BUFFER_SIZE, aF2);
		if (!_s1 || _s1 != _s2 || (memcmp(_buf1, _buf2, _s1)!=0))
		{
			_rval = 1;
			goto exit;
		}
	} while (!feof(aF1) && !feof(aF2));

exit:
	free(_buf1);
	free(_buf2);
	return _rval;
}
extern std::string get_path()
{
	char _path[FILENAME_MAX];
#ifdef _WIN32
	if (!_getcwd(_path, sizeof(_path)))
#else
	if(!getcwd(_path,sizeof(_path)))
#endif
	{
		return std::string();
	}
	_path[sizeof(_path) - 1] = '\0'; //be on safe side
	return std::string(_path);
}
} //namespace USHARE
