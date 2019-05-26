/*
 * vlog_is_on.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 15.10.2015
 *      Author:  https://github.com/CrazyLauren
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


#if defined(__GNUC__) || defined (STATEMENTS_WITH_INITIALIZER)

#ifndef HAVE_STATEMENTS_WITH_INITIALIZER
#	define ENABLE_EXTENTION  __extension__
#else
#	define ENABLE_EXTENTION
#endif

/*! A magic fast macro for definition of logging
 * level in this code area
 *
 * if compiler is supported
 * initialization variable in the if code block
 * than the macro have O(1) logical complexity
 * in the other case O(n) where
 * n - the number of unique level definition (vmodule parameter of
 * command line)
 *
 */
#define VLOG_IS_ON(verboselevel)                                \
  ENABLE_EXTENTION  \
  ({ static int* _vlocal = &NSHARE::logging_impl::vlog_uniitialized();/*vlog_uniitialized() is very big num*/\
     (*_vlocal >= verboselevel) &&                          \
     (\
/*as _vlocal is "static" when if _vlocal was inited, comparison "*_vlocal >= verboselevel" is valid.*/\
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
/*! @brief Returns unique login level of file if exist
 *
 * See "module" parameter of command string
 *
 * @param aLevel [out] Pointer to file level value
 * @param aFile A name of checked file
 * @return true if level exist
 */
extern SHARE_EXPORT  bool is_file_level(int** aLevel, const char* aFile);

/*! @brief Returns login level of file
 *
 * @param aFile A name of file
 * @return level of logging
 */
extern SHARE_EXPORT  int get_file_level(const char* aFile);

/*! @brief Check if "verbose level" logging macros
 * has to work in the file
 *
 * @param aFile A name of file
 * @param aLevel A maximal verbose level
 * @return true if has to work
 *
 */
extern SHARE_EXPORT  bool is_v_log_on(const char* aFile, int verboselevel);

/*! @brief Returns reference to very big number
 * which is used for checking initialization status
 * of logging level in macro #VLOG_IS_ON macro
 *
 *
 * @return reference for big number
 *
 */
extern SHARE_EXPORT   int& vlog_uniitialized();

/*! @brief Initialize "verbose logging"
 * for compiler which supported initialize
 * variable in if block;
 *
 */
extern SHARE_EXPORT   bool init_gnu_vlog(int**, int*, const char* aFile, int verboselevel);

/*! @brief Returns reference for variable
 * in which saved logging
 * Initialization status
 *
 */
extern SHARE_EXPORT  bool& is_inited();

/*! @brief Parses command line
 * and return list of files
 * which has unique information
 * about it logging level
 *
 * @return information about unique logging level
 * of files
 */
extern SHARE_EXPORT  std::list<module_info_t> init_modules();

/*! @brief Returns information about unique logging level
 * of files
 *
 * @return information about unique logging level
 * of files
 */
extern SHARE_EXPORT  std::list<module_info_t>& get_modules();

/*! @brief Returns reference for vmodule flags
 * which is passed to command line
 *
 */
extern SHARE_EXPORT std::string& flags_vmodule();


/*! @brief Returns reference for vlevel flags
 * which is passed to command line
 *
 */
extern SHARE_EXPORT int& flags_v();

/*! @brief Returns reference for level flags
 * which is passed to command line
 *
 */
extern SHARE_EXPORT  int& flags_minloglevel();
}
}



#endif /* VLOG_IS_ON_H_ */
