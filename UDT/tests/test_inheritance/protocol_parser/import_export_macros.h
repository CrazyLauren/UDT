/*
 * import_export_macros.h
 *
 *  Created on: 05.05.2019
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2019  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef IMPORT_EXPORT_MACROS_H_
#define IMPORT_EXPORT_MACROS_H_

#if  defined( _WIN32 )
#   ifdef INHERITANCE_PROTOCOL_PARSER_EXPORTS
#       define PROTOCOL_PARSER_EXPORT __declspec(dllexport)
#   else
#       define PROTOCOL_PARSER_EXPORT __declspec(dllimport)
#   endif
#else
#       define PROTOCOL_PARSER_EXPORT
#endif



#endif /* IMPORT_EXPORT_MACROS_H_ */
