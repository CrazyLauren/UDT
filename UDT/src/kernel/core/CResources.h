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

namespace NUDT
{
class CResources: public NSHARE::CSingleton<CResources>,public IState
{
public:
	static const NSHARE::CText NAME;
	CResources(std::vector<NSHARE::CText> const& aResources,
			NSHARE::CText const& aExtPath = NSHARE::CText());
	~CResources();

	//Loads all resources
	void MLoad(void);
	void MUnload(void);
	bool MIsLoaded(void) const;

	//Load only channels
	void MLoadChannels();
	bool MIsChannelsLoaded(void) const;
	void MUnloadChannels();
	NSHARE::CConfig MSerialize() const;
private:
	struct module_t
	{
		NSHARE::CText FName;
		SHARED_PTR<NSHARE::CDynamicModule> FDynamic;
		//NSHARELLDynamicModule* dynamicModule;
		NSHARE::factory_registry_t* FRegister;
		NSHARE::CConfig MSerialize() const;
	};
	typedef std::list<module_t> mod_channels_t;

	NSHARE::CText FExtLibraryPath;
	mod_channels_t FModules;
};
}//
#endif /* CRESOURCES_H_ */
