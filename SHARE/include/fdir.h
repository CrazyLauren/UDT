/*
 * fdir.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 08.10.2013
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef FDIR_H_
#define FDIR_H_

namespace NSHARE
{

enum eFilesOfDirFlags
{
	E_INCLUDES_SUBDIR = 1 << 0,
};
extern SHARE_EXPORT std::string get_path();
extern SHARE_EXPORT bool get_files_of_dir(std::list<std::string> *_out,
		const std::string &directory, NSHARE::CFlags<unsigned> const& =
				NSHARE::CFlags<unsigned>(E_INCLUDES_SUBDIR));
extern SHARE_EXPORT bool get_path_of_files_of_dir(std::list<std::string> *out,
		const std::string &directory);
extern SHARE_EXPORT bool get_name_of_files_of_dir(std::list<std::string> *out,
		const std::string &directory); //FIXME get_nameS_of_files_of_dir=get_path_of_files_of_dir
extern SHARE_EXPORT bool is_diretory(const std::string &_name);
extern bool is_the_file_exist(const std::string &_name);
SHARE_EXPORT int filecmp(FILE* aF1, FILE* aF2);
SHARE_EXPORT int filecmp(std::string const& aF1, std::string const& aF2);

} //namespace USHARE

#endif /* FDIR_H_ */
