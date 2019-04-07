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

namespace NSHARE
{
/**\brief Класс динамического подключения библиотек
 *
 */
class SHARE_EXPORT CDynamicModule
{
public:
	static const NSHARE::CText NAME;
	typedef CText string_t;

    CDynamicModule(const string_t& name,const string_t& aPath =string_t());

    ~CDynamicModule();

    const string_t& MGetModuleName() const;

    void* MGetSymbolAddress(const string_t& symbol) const;

    template<class T>
    T* MGet(const string_t& symbol) const
    {
    	return (T*)MGetSymbolAddress(symbol);
    }
    NSHARE::CConfig MSerialize() const;
private:
    struct CImpl;
    CImpl* FPimpl;
};
}
#endif /* DYMAMIC_MODULES_H_ */
