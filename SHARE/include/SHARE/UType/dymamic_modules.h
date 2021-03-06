/*
 * dymamic_modules.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 12.09.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef DYMAMIC_MODULES_H_
#define DYMAMIC_MODULES_H_

#include <SHARE/macro_attributes.h>

namespace NSHARE
{
/**\brief Class for dynamically loading libraries
 *
 */
class SHARE_EXPORT CDynamicModule
{
public:
	static const NSHARE::CText NAME;///< A serializing key
	static const NSHARE::CText LIBRARY_UNIQUE_POSTFIX;/*!<unique postfix specified in cmake file (usually
	 	 	 	 	 	 	 	 	 	 	 	 	 	for debug "d" for release empty)*/
	static const NSHARE::CText LIBRARY_EXTENSION;///<Extension of library (for Win32: .dll, for unix: .so)
	static const NSHARE::CText LIBRARY_PREFIX;///<Prefix of library (for Win32: "", for unix: "lib")
	typedef CText string_t;///< Type of used string

	/*!\brief Creates object and loads the library
	 *
	 *\param aName - aName of library (without .dll or .so)
	 *\param aName - aPath to library
	 */
    CDynamicModule(const string_t& aName,const string_t& aPath =string_t());

    ~CDynamicModule();

    /*!\brief Returns library name
     *
     */
    const string_t& MGetModuleName() const;

	/*!\brief Returns pointer to exported symbol
	 *
	 *\param aSymbol a export name of function or member
	 */
    void* MGetSymbolAddress(const string_t& aSymbol) const;

	/*!\brief Returns pointer to exported symbol
	 *
	 *\param aSymbol a export name of function or member
	 *\tparam T typeof symbol
	 */
    template<class T>
    T* MGet(const string_t& symbol) const
    {
    	return (T*)MGetSymbolAddress(symbol);
    }

	/*!\brief Serialize object
	 *
	 * The key of serialized object is #NAME
	 *
	 *\return Serialized object.
	 */
    NSHARE::CConfig MSerialize() const;

	/*!\brief Checks object for valid
	 *
	 * Usually It's used after deserializing object
	 *\return true if it's valid.
	 */
	bool MIsValid()const;

	/*!\brief Return true if is library
	 *
	 * @param name library name
	 * @param aAllowPostifx if true take account #LIBRARY_UNIQUE_POSTFIX
	 */
	static bool sMIsNameOfLibrary(const CText& name,bool aAllowPostifx=true);

	/*! Returns library name without prefix and postfix
	 *
	 * @param name a full library name
	 * @return library name
	 */
	static CText sMGetLibraryName(CText name);

	/*! Returns full library with prefix and postfix
	 *
	 * @param name a  library name
	 * @return a full library name
	 */
	static CText sMGetLibraryNameInSystem(CText name);
private:
    struct CImpl;
    CImpl* FPimpl;
};
}
#endif /* DYMAMIC_MODULES_H_ */
