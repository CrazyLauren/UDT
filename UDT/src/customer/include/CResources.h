/*
 * CResources.h
 *
 *  Created on: 19.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CRESOURCES_H_
#define CRESOURCES_H_

namespace NUDT
{
class CResources: public NSHARE::CSingleton<CResources>
{
public:
    static const NSHARE::CText NAME;///< A serializing key
    static const NSHARE::CText MODULES_PATH;/*!< A name of key for specification
										where looking for additional libraries*/
    static const NSHARE::CText LIST_OF_LOADED_LIBRARY;/*!< A name of key for specification
										what library has to be loaded*/
    static const NSHARE::CText ONLY_SPECIFIED_LIBRARY;/*!< A name of key for specification
										loaded only library which is specified by #LIST_OF_LOADED_LIBRARY*/

    static const NSHARE::CText ENV_MODULE_PATH;/*!< THe Enviroment name  which is hold path to looking for modules*/

	CResources(NSHARE::CConfig const& aConf);

	~CResources();

	//Loads all resources
	void MLoad(void);
	void MUnload(void);
	bool MIsLoaded(void) const;

	//Load only channels
	void MLoadLibrariess();
	bool MIsChannelsLoaded(void) const;
	void MUnloadChannels();

private:
	struct module_t
	{
		NSHARE::CText FName;
		SHARED_PTR<NSHARE::CDynamicModule> FDynamic;
		//NSHARELLDynamicModule* dynamicModule;
		NSHARE::factory_registry_t* FRegister;
	};
	typedef std::list<module_t> mod_channels_t;

	void MAddStaticFactory();

    NSHARE::Strings  FExtLibraryPath;
	mod_channels_t FModules;
};
}//
#endif /* CRESOURCES_H_ */
