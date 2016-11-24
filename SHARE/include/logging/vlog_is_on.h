/*
 * vlog_is_on.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 15.10.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef VLOG_IS_ON_H_
#define VLOG_IS_ON_H_

#define FLAGS_vmodule NSHARE::logging_impl::flags_vmodule()
#define FLAGS_v NSHARE::logging_impl::flags_v()
// Log suppression level: messages logged at a lower level than this
// are suppressed.
#define FLAGS_minloglevel NSHARE::logging_impl::flags_minloglevel()


#if defined(__GNUC__)
#define VLOG_IS_ON(verboselevel)                                \
  __extension__  \
  ({ static int* _vlocal = &NSHARE::logging_impl::vlog_uniitialized();/*vlog_uniitialized() is very big num*/\
     (*_vlocal >= verboselevel) &&                          \
     (\
/*if _vlocal was inited, comparison "*_vlocal >= verboselevel" is valid.*/\
(_vlocal != &NSHARE::logging_impl::vlog_uniitialized() ) ||   \
/*try to init _vlocal, and comparing "_vlocal" and  "verboselevel" again */\
(NSHARE::logging_impl::is_inited() &&NSHARE::logging_impl::init_gnu_vlog(&_vlocal, &FLAGS_v, __FILE__, verboselevel)) \
/*Cann't init _vlocal, using share flag*/\
|| (!NSHARE::logging_impl::is_inited() && FLAGS_v >= (verboselevel))\
	 ); })
#else
#define VLOG_IS_ON(verboselevel)\
		(FLAGS_v >= (verboselevel) || ((NSHARE::logging_impl::is_inited() && !NSHARE::logging_impl::get_modules().empty())&& NSHARE::logging_impl::is_v_log_on(__FILE__,verboselevel)))

#endif

namespace NSHARE
{
namespace logging_impl
{
struct module_info_t
{
	std::string FPattern;
	mutable int FLevel;
};
extern SHARE_EXPORT  bool is_file_level(int**, const char* aFile);
extern SHARE_EXPORT  int get_file_level(const char* aFile);
extern SHARE_EXPORT  bool is_v_log_on(const char* aFile, int verboselevel);
extern SHARE_EXPORT   int& vlog_uniitialized();
extern SHARE_EXPORT   bool init_gnu_vlog(int**, int*, const char* aFile, int verboselevel);
extern SHARE_EXPORT  bool& is_inited();
extern SHARE_EXPORT  std::list<module_info_t> init_modules();
extern SHARE_EXPORT  std::list<module_info_t>& get_modules();
extern SHARE_EXPORT std::string& flags_vmodule();
extern SHARE_EXPORT int& flags_v();
extern SHARE_EXPORT  int& flags_minloglevel();
}
}



#endif /* VLOG_IS_ON_H_ */
