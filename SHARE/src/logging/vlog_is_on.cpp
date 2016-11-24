/*
 * vlog_is_on.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 15.10.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <logging/vlog_is_on.h>

namespace NSHARE
{
namespace logging_impl
{
extern SHARE_EXPORT std::string& flags_vmodule()
{
	static std::string _v;
	return _v;
}
extern SHARE_EXPORT int& flags_v()
{
	static int _v=-1;
	return _v;
}
extern SHARE_EXPORT int& flags_minloglevel()
{
	static int _v=FATAL;
	return _v;
}
// Implementation of fnmatch that does not need 0-termination
// of arguments and does not allocate any memory,
// but we only support "*" and "?" wildcards, not the "[...]" patterns.
static bool fnmatch(const char* pattern, size_t patt_len, const char* str,
		size_t str_len) //from google
{
	size_t p = 0;
	size_t s = 0;
	while (1)
	{
		if (p == patt_len && s == str_len)
			return true;
		if (p == patt_len)
			return false;
		if (s == str_len)
			return p + 1 == patt_len && pattern[p] == '*';
		if (pattern[p] == str[s] || pattern[p] == '?')
		{
			p += 1;
			s += 1;
			continue;
		}
		if (pattern[p] == '*')
		{
			if (p + 1 == patt_len)
				return true;
			do
			{
				if (fnmatch(pattern + (p + 1), patt_len - (p + 1), str + s,
						str_len - s))
				{
					return true;
				}
				s += 1;
			} while (s != str_len);
			return false;
		}
		return false;
	}
	return false;
}
extern SHARE_EXPORT bool is_file_level(int** aVal, const char* aFile)
{
	MASSERT_1(is_inited());
	const char* base = strrchr(aFile, '/');
#ifdef _WIN32
	if (!base)
		base = strrchr(aFile, '\\');
#endif
	base = base ? (base + 1) : aFile;
	const char* base_end = strchr(base, '.');
	size_t base_length = base_end ? size_t(base_end - base) : strlen(base);
	if (base_length >= 4 && (memcmp(base + base_length - 4, "-inl", 4) == 0))
	{
		base_length -= 4;
	}
	std::list<module_info_t>::const_iterator _jt = get_modules().begin();

	for (; _jt != get_modules().end(); ++_jt)
		if (fnmatch(_jt->FPattern.c_str(), _jt->FPattern.size(), base,
				base_length))
		{
			*aVal = &_jt->FLevel;
			return true;
		}
	return false;
}
extern SHARE_EXPORT int get_file_level(const char* aFile)
{
	static std::map<std::string, int> _levels;

	CHECK(!get_modules().empty());
	std::map<std::string, int>::const_iterator _it = _levels.find(aFile);
	if (_it != _levels.end())
		return _it->second;
	int* _pv = NULL;
	if (is_file_level(&_pv, aFile))
		_levels[aFile] = *_pv;
	else
		_levels[aFile] = FLAGS_v;
	return FLAGS_v;
}
SHARE_EXPORT int& vlog_uniitialized()
{
	static int _val = 50000;
	return _val;
}
extern SHARE_EXPORT bool is_v_log_on(const char* aFile, int verboselevel)
{
	if (!is_inited())
	{
		return FLAGS_v>= verboselevel;
	}
	return get_file_level(aFile) >= verboselevel;
}

		extern SHARE_EXPORT bool init_gnu_vlog(int** aLocal, int* aDefault,
		const char* aFile, int verboselevel)
{
	if (!is_inited())
	{
		return is_v_log_on(aFile, verboselevel);
	}

	int* _pv = NULL;
	if (is_file_level(&_pv, aFile))
		*aLocal = _pv;
	else
		*aLocal = aDefault;
	return *(*aLocal) >= verboselevel;
}
extern SHARE_EXPORT bool& is_inited()
{
	static bool _is = false;
	return _is;
}
extern SHARE_EXPORT std::list<module_info_t>& get_modules()
{
	static std::list<module_info_t> _modules(init_modules());
	return _modules;
}
extern SHARE_EXPORT std::list<module_info_t> init_modules()
{
	MASSERT_1(is_inited());
	const char* vmodule = FLAGS_vmodule.c_str();
	const char* sep;
	std::list<module_info_t> _modules;
	while ((sep = strchr(vmodule, '=')) != NULL)
	{
		std::string pattern(vmodule, sep - vmodule);
		int module_level;
		if (sscanf(sep, "=%d", &module_level) == 1)
		{
			module_info_t info;
			info.FPattern = pattern;
			info.FLevel = module_level;
			_modules.push_back(info);
		}
		// Skip past this entry
		vmodule = strchr(sep, ';');
		if (vmodule == NULL)
			break;
		vmodule++; // Skip past ","
	}
	return _modules;
}
}
}

