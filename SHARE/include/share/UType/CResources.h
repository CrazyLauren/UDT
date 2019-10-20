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

#ifndef SHARE_CRESOURCES_H_
#define SHARE_CRESOURCES_H_


namespace NSHARE
{
/**\brief Класс для динамической загрузки фабрик из библиотек
 *
 * В библиотеках должна быть определена функция с сигнатурой factory_registry_func_t (по умолчанию,  её
 * индетфикатор в библиотеке должен быть  FACTORY_REGISTRY_FUNC_NAME) . Если
 * необходимо добавить фабрики статически то можно воспользоваться методом
 * static_factory_registry. Считаеся, что CFactoryRegisterer* является указателем
 * на глобальную область памяти.
 */
class SHARE_EXPORT CResources: public NSHARE::CSingleton<CResources>
{
public:
	static const NSHARE::CText NAME;

	/**\brief инициализирующие параметры
	 *
	 */
	struct SHARE_EXPORT param_t
	{
		/**\brief параметры необх. для динам. загрузки
		 */
		struct SHARE_EXPORT reg_t
		{
			NSHARE::CText library;///<имя библиотеки без префиксов и суфиксов
			NSHARE::CText reg_func;///<идентификатор функции регистратора, по умолчнанию он равен FACTORY_REGISTRY_FUNC_NAME
			NSHARE::CConfig config;///<настройки передаваемые при вызове reg_func
		};
		typedef std::vector<reg_t> libraries_registrators_t;

		static const NSHARE::CText NAME;
		static const NSHARE::CText LIBRARY;
		static const NSHARE::CText SEARCH_PATH;
		static const NSHARE::CText REGISTRY_FUNC_NAME;
		static const NSHARE::CText REG_CONFIG;

		libraries_registrators_t libraries;///< Library registator function name
		NSHARE::CText search_path;///< library's search path

		param_t();

		explicit param_t(std::vector<NSHARE::CText> const& aLibraries,
				NSHARE::CText const& aPath = NSHARE::CText());

		param_t(NSHARE::CConfig const& aConf);
		bool MIsValid() const;
		NSHARE::CConfig MSerialize() const;
	};

	explicit CResources(param_t const & aParam =param_t());
	explicit CResources(NSHARE::CConfig const& aConfig);
	~CResources();

	bool MSetParam(param_t const & aParam);

	void MLoad(void);
	void MUnload(void);
	bool MIsLoaded() const;
	NSHARE::CConfig MSerialize() const;
private:
	struct module_t
	{
		module_t (param_t::reg_t const&);

		param_t::reg_t const FRegInfo;
		SHARED_PTR<NSHARE::CDynamicModule> FDynamic;
		NSHARE::factory_registry_t* FRegister;
		NSHARE::CConfig MSerialize() const;
	};
	typedef std::list<module_t> mod_channels_t;//todo to set

	mod_channels_t FModules;
	bool FIsLoad;
	NSHARE::CText FSearchPath;
};
inline bool CResources::MIsLoaded() const
{
	return FIsLoad;
}
/**\brief функция используется для статического добавления фабрик
 *
 */
extern "C" SHARE_EXPORT  NSHARE::factory_registry_t* static_factory_registry(
		NSHARE::CFactoryRegisterer* aVal);
}//


#endif /* SHARE_CRESOURCES_H_ */
