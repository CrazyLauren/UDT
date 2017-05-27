/*
 * CProperties.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 02.12.2014
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CPROPERTIES_H_
#define CPROPERTIES_H_
//fixme Don't using this class it has to be modifided as it use 
//string for save value. Consequently MGetProperty is slow.
//using 
namespace NSHARE
{
namespace NPropertyImpl
{
typedef CText property_value_type;
struct property_impl_t;
#ifdef STRING_STREAM_MUTEX
static CMutex& sMAntiCrashStringStream();
#endif
static std::locale& sMGetLocale();
}
template<class TMutex = CMutexEmpty>
class   CProperties //TODO Interface
{
	friend struct NPropertyImpl::property_impl_t;
	typedef NPropertyImpl::property_impl_t property_impl_t;
public:
	//!@struct EUnknowenPath
	//!@brief  Р�СЃРєР»СЋС‡РµРЅРёРµ - Р­Р»РјРµРЅС‚Р° РїРѕ СЌС‚РѕРјСѓ РїСѓС‚Рё РЅРµ СЃСѓС‰РµСЃС‚РІСѓРµС‚.
	struct EUnknowenPath	//TODO
	{

	};

	typedef CProperties<TMutex> this_t;
	typedef TMutex property_mutex_t;
	typedef std::map<CText, property_impl_t> properties_t;
	typedef typename properties_t::key_type property_key;
	typedef NSHARE::NPropertyImpl::property_value_type property_value_type;
	typedef std::vector<std::pair<CText, property_value_type> > array_properties_t;

	virtual ~CProperties()
	{
	}

	template<class T>
	inline T MGetProperty(property_key const& aKey, T aDefVal =
			T()) const;
	template<class T>
	inline bool MPutProperty(property_key const& aKey, T const& aVal);
	inline bool MPutProperty(property_key const& aKey,
			char const* const & aVal);
	inline bool MPutProperty(property_key const& aKey,
			char* const & aVal);

	inline bool MPutProperty(property_key const& aKey,
			std::string const & aVal);

	inline bool MIsProperty(property_key const& aKey) const;
	inline bool MResetProperty(property_key const& aKey);
	inline bool MIsPropertyEmpty() const;
	inline std::ostream& MPrintPTree(std::ostream & aStream) const;
	inline std::size_t MGetProperties(array_properties_t* aTo) const;
	template<class Pred>
	inline void MForEach(Pred aPred);
	template<class Pred>
	inline void MForEach(Pred aPred) const;
	static bool sMUnitTest();
protected:
	inline void MRemoveProperty(property_key const& aKey);
	inline void MCleanAllProperties();
	virtual bool MPropertyChanged(property_key const& aKey,	property_value_type const& aPrevVal)
	{
		VLOG(2) << "MPropertyChanged return true";
		return true;
	}

	typedef CSafeData<properties_t> safe_data_t;//FIXME empty Mutex
	typedef typename safe_data_t::template RAccess<> const RAccess;
	typedef typename safe_data_t::template WAccess<> WAccess;

	//properties_t FNodeTree;
private:
	template<class T>
	inline T MGetDecimalProperty(properties_t const& FNodeTree,
			property_key const& aKey, T aDefVal) const;
	template<class T>
	inline T MGetDoubleProperty(properties_t const& FNodeTree,
			property_key const& aKey, T aDefVal) const;
	template<class T>
	inline T MGetPropertyImpl(properties_t const& FNodeTree,
			property_key const& aKey, T aDefVal) const;

	inline property_value_type MGetPropertyImpl(properties_t const& FNodeTree,
			property_key const& aKey, property_value_type aDefVal) const;

	inline std::string MGetPropertyImpl(properties_t const& FNodeTree,
			property_key const& aKey, std::string aDefVal) const;

	inline bool MGetPropertyImpl(properties_t const& FNodeTree,
					property_key const& aKey,bool aDefVal) const;
	inline unsigned char MGetPropertyImpl(properties_t const& FNodeTree,
					property_key const& aKey,unsigned char aDefVal) const;
	inline signed char MGetPropertyImpl(properties_t const& FNodeTree,
					property_key const& aKey,signed char aDefVal) const;
	inline unsigned short int MGetPropertyImpl(properties_t const& FNodeTree,
					property_key const& aKey,unsigned short int aDefVal) const;
	inline short int MGetPropertyImpl(properties_t const& FNodeTree,
					property_key const& aKey,short int aDefVal) const;
	inline unsigned int MGetPropertyImpl(properties_t const& FNodeTree,
				property_key const& aKey,unsigned int aDefVal) const;
	inline int MGetPropertyImpl(properties_t const& FNodeTree,
				property_key const& aKey,int aDefVal) const;
	inline unsigned long int MGetPropertyImpl(properties_t const& FNodeTree,
			property_key const& aKey, unsigned long int aDefVal) const;
	inline long int MGetPropertyImpl(properties_t const& FNodeTree,
			property_key const& aKey, long int aDefVal) const;
	inline double MGetPropertyImpl(properties_t const& FNodeTree,
			property_key const& aKey, double aDefVal) const;
	inline float MGetPropertyImpl(properties_t const& FNodeTree,
			property_key const& aKey, float aDefVal) const;
	inline long double MGetPropertyImpl(properties_t const& FNodeTree,
			property_key const& aKey, long double aDefVal) const;

	safe_data_t FNodeTree;
};
#   if defined(_MSC_VER)
typedef  CProperties<> CPTree;
#else
#	ifndef __QNX__
typedef CProperties<> CPTree __attribute__((deprecated("class CPTree is deprecated, to change its name to CProperties")));
#	endif
#endif
} //namespace USHARE
#include "CProperties.hpp"
#endif /* CPROPERTIES_H_ */
