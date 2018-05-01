/*
 * cb_t.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 22.03.2013
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CB_T_H_
#define CB_T_H_
namespace NSHARE
{
/** \brief Значения, которые должен возврщать callback(CB) функция
 *
 */
enum eCBRval
{
	E_CB_REMOVE = -1, //!<удалить текущий CB из списка CB события
	E_CB_SAFE_IT = 0, //!<Оставить текущий в списке CB события
	E_CB_BLOCING_OTHER = 2 //!< Остановить вызов остальных  CB, этого события
};

/** \brief Сигнатуры методов CB по умолчанию
 * \{
 */
typedef eCBRval signal_t(void* WHO, void* WHAT, void* YOU_DATA);
typedef eCBRval (*psignal_t)(void* WHO, void* WHAT, void* YOU_DATA);
// \}

/** \brief шаблон типовой реализации CB для CEvent
 *
 * Класс содержит два поля указатель на вызоваимый CB и указатель
 * на данные, передаваемые в CB третьим аргументом
 *
 * \tparam сигнатура CB метода
 */
template<typename TSignal,typename TArg=void>
struct  Callback_t
{
	typedef TSignal pM;
	typedef TArg* arg_t;
	Callback_t() :
			FSignal(NULL), FYouData(NULL)
	{
		;
	}
	Callback_t(TSignal const& aSignal, void * const aData) :
			FSignal(aSignal), FYouData(aData)
	{
		;
	}
	Callback_t(Callback_t<TSignal> const& aCB) :
			FSignal(aCB.FSignal), FYouData(aCB.FYouData)
	{
		;
	}
	TSignal FSignal;//!< Указатель на метод CB
	void* FYouData;//!< Указатель на данные
#if __cplusplus >= 201103

	explicit operator bool() const
	{
		return MIs();
	}
#endif
	bool MIs()const
	{
		return FSignal!=NULL;
	}
	template<class T, class Y>
	eCBRval operator ()(T* aWho, Y * const aArgs) const
	{
		if (FSignal)
			return (eCBRval) (*FSignal)(aWho, aArgs, FYouData);
		return E_CB_REMOVE;
	}
	template<class T>
	eCBRval operator ()(T* aWho, arg_t const aArgs) const
	{
		if (FSignal)
			return (eCBRval)(*FSignal)(aWho, aArgs, FYouData);
		return E_CB_REMOVE;
	}
//	eCBRval operator ()(void* aWho, void * const aArgs) const
//	{
//		if (FSignal)
//			return (eCBRval)(*FSignal)(aWho, aArgs, FYouData);
//		return E_CB_REMOVE;
//	}
	bool operator ==(Callback_t const& rihgt) const
	{
		return FSignal == rihgt.FSignal && FYouData == rihgt.FYouData;
	}
};
template struct SHARE_EXPORT Callback_t<psignal_t>;
typedef Callback_t<psignal_t> CB_t;

template<bool (*psignal_t)(void* WHO, void* WHAT, void* YOU_DATA)>
struct  CB_static_t
{
	typedef void* arg_t;
	CB_static_t() :
			FYouData(NULL)
	{
		;
	}
	CB_static_t(void * const aData) :
			FYouData(aData)
	{
		;
	}
	void* FYouData;
	bool operator ()(void* aWho, void * const aArgs)
	{
		return psignal_t(aWho, aArgs, FYouData);
	}
};
} //namespace USHARE
namespace std
{
template<typename TSignal,typename TArg>
inline std::ostream& operator<<(std::ostream & aStream,
		NSHARE::Callback_t<TSignal,TArg> const& aCb)
{
	aStream.setf(ios::hex, ios::basefield);
	aStream << "Pointer to data:" << aCb.FYouData << "; Pointer to cb handler: "
			<< aCb.FSignal;
	aStream.unsetf(ios::hex);
	return aStream;
}
}
#endif /* CB_T_H_ */
