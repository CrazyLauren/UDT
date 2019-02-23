/*
 * CParserFactory.h
 *
 *  Created on: 21.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef CPARSERFACTORY_H_
#define CPARSERFACTORY_H_

#include <IExtParser.h>
namespace NSHARE
{
//#if  defined( _WIN32 )
//template class UDT_SHARE_EXPORT CFactoryManager<NUDT::IExtParser> ;
//#endif
}
namespace NUDT
{
class UDT_SHARE_EXPORT CParserFactory: public NSHARE::CFactoryManager<
		NUDT::IExtParser>
{
public:
	static const NSHARE::CText NAME;
	static const NSHARE::CText PARSER;
	CParserFactory();
	NSHARE::CConfig MSerialize() const;
};
} //
//easy-to-use macro registrator
#define REGISTRE_ONLY_ONE_PROTOCOL_MODULE(aClassName, aLibraryName)\
	namespace{struct CRegisterImpl:public NSHARE::CFactoryRegisterer{\
	CRegisterImpl():NSHARE::CFactoryRegisterer(NSHARE::CText().MMakeRandom(10),NSHARE::version_t(0,1)){};\
			void MAdding() const{NUDT::CParserFactory::sMAddFactory<aClassName>();}\
			void MUnregisterFactory() const{NUDT::CParserFactory::sMGetInstance().MRemoveFactory(aClassName::NAME);};\
			bool MIsAlreadyRegistered() const\
			{ if (NUDT::CParserFactory::sMGetInstancePtr())\
					return NUDT::CParserFactory::sMGetInstance().MIsFactoryPresent(aClassName::NAME);\
				return false;};\
	};};\
	static NSHARE::factory_registry_t g_factory;\
	extern "C" aLibraryName##_EXPORT NSHARE::factory_registry_t* get_factory_registry()\
	{if (g_factory.empty())g_factory.push_back(new CRegisterImpl());\
		return &g_factory;}\
		/*END macro*/

#endif /* CPARSERFACTORY_H_ */
