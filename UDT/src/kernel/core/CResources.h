/*
 * CResources.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 19.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CRESOURCES_H_
#define CRESOURCES_H_

#include "ICore.h"
namespace NUDT
{
class CResources: public NSHARE::CSingleton<CResources>,public ICore
{
public:
	static const NSHARE::CText NAME;///< A serializing key
	static const NSHARE::CText MODULES_PATH;/*!< A name of key for specification
										where looking for additional libraries*/
	static const NSHARE::CText LIST_OF_LOADED_LIBRARY;/*!< A name of key for specification
										what library has to be loaded*/
	static const NSHARE::CText ONLY_SPECIFIED_LIBRARY;/*!< A name of key for specification
										loaded only library which is specified by #LIST_OF_LOADED_LIBRARY*/

	/*!\brief Deserialize object
	 *
	 * To check the result of deserialization,
	 * used the MIsValid().
	 *\param aConf Serialized object
	 */
	CResources(NSHARE::CConfig const& aConf);

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
	bool MIsValid() const;

	bool MStart();
	~CResources();

	//Loads all resources
	void MLoad(void);
	void MUnload(void);
	bool MIsLoaded(void) const;

private:
	/*! \brief Information about plugin
	 *
	 *\note keyword "mutable" is used as it's used in "set"
	 */
	struct module_t
	{
		enum eError
		{
			E_NO_ERROR=0,
			E_NO_FUNCTION=-255,
			E_CANNOT_OPEN,
			E_NO_FACTORY_REGISTER,
			INVALID_NAME_OF_FACTORY
		};

		static const NSHARE::CText NAME;///< A serializing key


		module_t(NSHARE::CText const& aName);

		bool MLoad(NSHARE::CText const& aPath) const;

		NSHARE::CConfig MSerialize() const;

		bool operator<(module_t const& aRht) const;

		NSHARE::CText const FName;///<A name of module without .so(dll)

		mutable SHARED_PTR<NSHARE::CDynamicModule> FDynamic;///< A loaded library

		mutable NSHARE::factory_registry_t* FRegister;///< A pointer to main function in library

		NSHARE::CConfig FConfig;///< passed configure information
		mutable eError FError;///< Error Code
	};
	typedef std::set<module_t> mod_channels_t;

	void MLookingForLibraries();
	bool MPutModule(NSHARE::CText const& aName,NSHARE::CConfig const& aConf=NSHARE::CConfig());
	//Load only channels
	void MLoadLibraries();
	void MUnloadLibraries();

	NSHARE::CText FExtLibraryPath;
	mod_channels_t FModules;
	bool FDontSearchLibrary;///< No looking for library
};
}//
#endif /* CRESOURCES_H_ */
