/*
 * CProperties.hpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 12.09.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CPROPERTIES_HPP_
#define CPROPERTIES_HPP_
#include <limits>       // std::numeric_limits
#include <cctype> //std::isdigit
namespace NSHARE
{
namespace NPropertyImpl
{
struct property_impl_t
{
	property_impl_t() :
			pMCheck(NULL)
	{
		;
	}
	template<class T>
	inline bool MMake(const T &aVal);

	inline bool MMake(char const* const &aVal);
	inline bool MMake(char* const &aVal);

	template<class _T>
	static bool sMCheck(property_value_type const&aVal);


	static bool sMCheckAlwaysTrue(property_value_type const&aVal);

	static int sMIsHex(property_value_type const&aVal);
	property_value_type const& MGet() const
	{
		return FVal;
	}
	inline bool MCheck(property_value_type const&aVal) const
	{
		MASSERT_1(pMCheck);
		return pMCheck(aVal);
	}
	void MReset()
	{
		FVal = property_value_type();
	}
	template<class T>
	inline bool MMakeString(const T &aVal,property_value_type & aTo);
protected:
	property_value_type FVal;
	bool (*pMCheck)(property_value_type const&aVal);
};
template<class _T>
inline bool property_impl_t::MMakeString(const _T &aVal,property_value_type & aTo)
{
#ifdef STRING_STREAM_MUTEX
	CRAII<CMutex> _fix(NPropertyImpl::sMAntiCrashStringStream());
#endif
	std::ostringstream _str_buf;
	_str_buf.imbue(NPropertyImpl::sMGetLocale());
	_str_buf << aVal;
	if (_str_buf.fail())
		return false;
	aTo=_str_buf.str();
	return true;
}
template<>
inline bool property_impl_t::MMakeString<bool>(const bool &aVal,
		property_value_type & aTo)
{
	return bool_to_str(aVal, aTo);
}
template<>
inline bool property_impl_t::MMakeString<unsigned char>(const unsigned char &aVal,
		property_value_type & aTo)
{
	return num_to_str(aVal, aTo,16);
}
template<>
inline bool property_impl_t::MMakeString<signed char>(const signed char &aVal,
		property_value_type & aTo)
{
	return num_to_str(aVal, aTo);
}
template<>
inline bool property_impl_t::MMakeString<unsigned short int>(const unsigned short int &aVal,
		property_value_type & aTo)
{
	return num_to_str(aVal, aTo);
}
template<>
inline bool property_impl_t::MMakeString<short int>(const short int &aVal,
		property_value_type & aTo)
{
	return num_to_str(aVal, aTo);
}
template<>
inline bool property_impl_t::MMakeString<unsigned int>(const unsigned int &aVal,
		property_value_type & aTo)
{
	return num_to_str(aVal, aTo);
}
template<>
inline bool property_impl_t::MMakeString<int>(const int &aVal,
		property_value_type & aTo)
{
	return num_to_str(aVal, aTo);
}
template<>
inline bool property_impl_t::MMakeString<unsigned long int>(const unsigned long int &aVal,
		property_value_type & aTo)
{
	return num_to_str(aVal, aTo);
}
template<>
inline bool property_impl_t::MMakeString<long int>(const long int &aVal,
		property_value_type & aTo)
{
	return num_to_str(aVal, aTo);
}
template<>
inline bool property_impl_t::MMakeString<double>(const double &aVal,
		property_value_type & aTo)
{
	return float_to_str(aVal, aTo);
}
template<>
inline bool property_impl_t::MMakeString<float>(const float &aVal,
		property_value_type & aTo)
{
	return float_to_str(aVal, aTo);
}
template<>
inline bool property_impl_t::MMakeString<long double>(const long double &aVal,
		property_value_type & aTo)
{
	return float_to_str(aVal, aTo);
}
template<class _T>
inline bool property_impl_t::MMake(const _T &aVal)
{
	property_value_type _str;
	if (!MMakeString(aVal, _str))
		return false;
	if (FVal.empty() && !pMCheck)
		pMCheck = sMCheck<_T>;
	else if (!MCheck(_str))
		return false;

	FVal = _str;
	return true;
}

template<>
inline bool property_impl_t::MMake(const property_value_type &aVal)
{
	if (FVal.empty() && !pMCheck)
		pMCheck = sMCheckAlwaysTrue;
	else if (!MCheck(aVal))
		return false;
	FVal = aVal;
	return true;
}

inline bool property_impl_t::MMake(char const* const &aVal)
{
	return MMake<property_value_type>(property_value_type(aVal));
}

inline bool property_impl_t::MMake(char* const &aVal)
{
	return MMake<property_value_type>(property_value_type(aVal));
}

template<class _T>
inline bool property_impl_t::sMCheck(property_value_type const&aVal)
{
	MASSERT_1(!aVal.empty());
#ifdef STRING_STREAM_MUTEX
	CRAII<CMutex> _fix(NPropertyImpl::sMAntiCrashStringStream());
#endif
	std::istringstream _stream(aVal.MToStdString());
	_stream.imbue(NPropertyImpl::sMGetLocale());
	_T _val;
	_stream >> _val;
	if (_stream.fail())
		return false;
	else if (_stream.get() == std::istringstream::traits_type::eof())
		return true;
	else
		return false;
}

template<>
inline bool property_impl_t::sMCheck<signed char>(
		property_value_type const&aVal)
{
	return is_decimal_correct<signed char>(aVal);
}

template<>
inline bool property_impl_t::sMCheck<short int>(property_value_type const&aVal)
{
	return is_decimal_correct<short int>(aVal);
}

template<>
inline bool property_impl_t::sMCheck<int>(property_value_type const&aVal)
{
	return is_decimal_correct<int>(aVal);
}

template<>
inline bool property_impl_t::sMCheck<long int>(property_value_type const&aVal)
{
	return is_decimal_correct<long int>(aVal);
}

template<>
inline bool property_impl_t::sMCheck<unsigned char>(
		property_value_type const&aVal)
{
	return is_decimal_correct<unsigned char>(aVal);
}

template<>
inline bool property_impl_t::sMCheck<unsigned short int>(
		property_value_type const&aVal)
{
	return is_decimal_correct<unsigned short int>(aVal);
}

template<>
inline bool property_impl_t::sMCheck<unsigned int>(
		property_value_type const&aVal)
{
	return is_decimal_correct<unsigned int>(aVal);
}

template<>
inline bool property_impl_t::sMCheck<unsigned long int>(
		property_value_type const&aVal)
{
	return is_decimal_correct<unsigned long int>(aVal);
}

template<>
inline bool property_impl_t::sMCheck<bool>(property_value_type const&aVal)
{
	return is_bool_correct(aVal);
}
template<>
inline bool property_impl_t::sMCheck<double>(property_value_type const&aVal)
{
	return is_float_correct<double>(aVal);
}
template<>
inline bool property_impl_t::sMCheck<float>(property_value_type const&aVal)
{
	return is_float_correct<float>(aVal);
}
template<>
inline bool property_impl_t::sMCheck<long double>(
		property_value_type const&aVal)
{
	return is_float_correct<long double>(aVal);
}

inline bool property_impl_t::sMCheckAlwaysTrue(property_value_type const&aVal)
{
	if (aVal.empty())
		return false;
	return true;
}


inline int property_impl_t::sMIsHex(property_value_type const&aVal)
{
	if (aVal.size() > 2)
	{
		if (aVal[0] == '0' && aVal[1] == 'x')
			return 1;
	}
	return 0;
}
} //namespace NPropertyImpl
template<class TPropes>
template<class T>
inline T CProperties<TPropes>::MGetProperty(property_key const& aKey,
		T aDefVal) const
{
	RAccess _node = FNodeTree.MGetRAccess();
	return MGetPropertyImpl(_node, aKey, aDefVal);
	//return MGetPropertyImpl(FNodeTree,aKey, aDefVal);
}
template<class TPropes>
template<class T>
inline T CProperties<TPropes>::MGetPropertyImpl(properties_t const& aNodeTree,
		property_key const& aKey, T aDefVal) const
{
	typename properties_t::const_iterator _it = aNodeTree.find(aKey);
	if (_it != aNodeTree.end())
	{
#ifdef STRING_STREAM_MUTEX	
		CRAII<CMutex> _fix(NPropertyImpl::sMAntiCrashStringStream());
#endif		
		std::istringstream _stream(_it->second.MGet().MToStdString());
		_stream.imbue(NPropertyImpl::sMGetLocale());
		MASSERT_1(_it->second.MGet().length());
		T _val;
		_stream >> _val;
		if (_stream.fail() || !_stream.eof())
			return aDefVal;
		else
			return _val;

	}
	return aDefVal;
}
template<class TPropes>
inline typename CProperties<TPropes>::property_value_type CProperties<TPropes>::MGetPropertyImpl(
		typename CProperties<TPropes>::properties_t const& aNodeTree,
		typename CProperties<TPropes>::property_key const& aKey,
		typename CProperties<TPropes>::property_value_type aDefVal) const
{
	typename properties_t::const_iterator _it = aNodeTree.find(aKey);
	if (_it != aNodeTree.end())
		return _it->second.MGet();
	return aDefVal;
}

template<class TPropes>
inline std::string CProperties<TPropes>::MGetPropertyImpl(
		properties_t const& aNodeTree, property_key const& aKey,
		std::string aDefVal) const
{
	property_value_type _prope=MGetPropertyImpl<property_value_type>(aNodeTree,aKey,property_value_type(aDefVal));
	return _prope.MToStdString();
}
template<class TPropes>
inline bool CProperties<TPropes>::MGetPropertyImpl(
		properties_t const& aNodeTree, property_key const& aKey,
		bool aDefVal) const
{
	typename properties_t::const_iterator _it = aNodeTree.find(aKey);
	if (_it != aNodeTree.end())
	{
		bool _rval=aDefVal;
		if(str_to_bool(_it->second.MGet(),_rval))
			return _rval;
		else
			return aDefVal;
	}
	return aDefVal;
}
template<class TPropes>
inline unsigned char CProperties<TPropes>::MGetPropertyImpl(
		properties_t const& aNodeTree, property_key const& aKey,
		unsigned char aDefVal) const
{
	return MGetDecimalProperty(aNodeTree, aKey, aDefVal);
}
template<class TPropes>
inline signed char CProperties<TPropes>::MGetPropertyImpl(
		properties_t const& aNodeTree, property_key const& aKey,
		signed char aDefVal) const
{
	return MGetDecimalProperty(aNodeTree, aKey, aDefVal);
}
template<class TPropes>
inline unsigned short int CProperties<TPropes>::MGetPropertyImpl(
		properties_t const& aNodeTree, property_key const& aKey,
		unsigned short int aDefVal) const
{
	return MGetDecimalProperty(aNodeTree, aKey, aDefVal);
}
template<class TPropes>
inline short int CProperties<TPropes>::MGetPropertyImpl(
		properties_t const& aNodeTree, property_key const& aKey,
		short int aDefVal) const
{
	return MGetDecimalProperty(aNodeTree, aKey, aDefVal);
}
template<class TPropes>
inline unsigned int CProperties<TPropes>::MGetPropertyImpl(
		properties_t const& aNodeTree, property_key const& aKey,
		unsigned int aDefVal) const
{
	return MGetDecimalProperty(aNodeTree, aKey, aDefVal);
}
template<class TPropes>
inline int CProperties<TPropes>::MGetPropertyImpl(properties_t const& aNodeTree,
		property_key const& aKey, int aDefVal) const
{
	return MGetDecimalProperty(aNodeTree, aKey, aDefVal);
}
template<class TPropes>
inline unsigned long int CProperties<TPropes>::MGetPropertyImpl(
		properties_t const& aNodeTree, property_key const& aKey,
		unsigned long int aDefVal) const
{
	return MGetDecimalProperty(aNodeTree, aKey, aDefVal);
}
template<class TPropes>
inline long int CProperties<TPropes>::MGetPropertyImpl(
		properties_t const& aNodeTree, property_key const& aKey,
		long int aDefVal) const
{
	return MGetDecimalProperty(aNodeTree, aKey, aDefVal);
}
template<class TPropes>
inline double CProperties<TPropes>::MGetPropertyImpl(
		properties_t const& aNodeTree, property_key const& aKey,
		double aDefVal) const
{
	return MGetDoubleProperty(aNodeTree, aKey, aDefVal);
}
template<class TPropes>
inline float CProperties<TPropes>::MGetPropertyImpl(
		properties_t const& aNodeTree, property_key const& aKey,
		float aDefVal) const
{
	return MGetDoubleProperty(aNodeTree, aKey, aDefVal);
}
template<class TPropes>
inline long double CProperties<TPropes>::MGetPropertyImpl(
		properties_t const& aNodeTree, property_key const& aKey,
		long double aDefVal) const
{
	return MGetDoubleProperty(aNodeTree, aKey, aDefVal);
}
template<class TPropes>
template<class T>
inline T CProperties<TPropes>::MGetDecimalProperty(
		properties_t const& aNodeTree, property_key const& aKey,
		T aDefVal) const
{
	typename properties_t::const_iterator _it = aNodeTree.find(aKey);
	if (_it != aNodeTree.end())
	{
		T _rval;
		if (str_to_decimal(_it->second.MGet(), _rval))
			return _rval;
		else
			return aDefVal;
		//::strtol()
	}
	return aDefVal;
}


template<class TPropes>
template<class T>
inline T CProperties<TPropes>::MGetDoubleProperty(properties_t const& aNodeTree,
		property_key const& aKey, T aDefVal) const
{
	typename properties_t::const_iterator _it = aNodeTree.find(aKey);
	if (_it != aNodeTree.end())
	{
		//atof
		T _rval;
		if (str_to_float(_it->second.MGet(), _rval))
			return _rval;
		else
			return aDefVal;
	}
	return aDefVal;
}
template<class TPropes>
template<class T>
inline bool CProperties<TPropes>::MPutProperty(property_key const& aKey,
		T const& aVal)
{
	property_impl_t _prev_val;
	bool _is_new=false;
	{
		WAccess _node = FNodeTree.MGetWAccess();
		properties_t& _propers = _node.MGet();
		typename properties_t::iterator _it = _propers.find(aKey);
		if (_it != _propers.end())
		{
			_is_new=false;
			_prev_val = _it->second;
			if (!_it->second.MMake(aVal))
				return false;
		}else
		{
			_is_new=true;
			property_impl_t _new;
			if (!_new.MMake(aVal))
				return false;
			_it = _propers.insert(
					typename properties_t::value_type(aKey, _new)).first;
		}
	}

	if (!MPropertyChanged(aKey, _is_new?property_value_type():_prev_val.MGet()))
	{
		if (_is_new)
			MRemoveProperty(aKey);
		else
		{
			WAccess _node = FNodeTree.MGetWAccess();
			properties_t& _propers = _node.MGet();
			typename properties_t::iterator _it = _propers.find(aKey);
			MASSERT_1(_it != _propers.end());
			_it->second = _prev_val;
		}
		return false;
	}
	return true;
}
template<class TPropes>
inline bool CProperties<TPropes>::MPutProperty(property_key const& aKey,
		char const* const & aVal)
{
	property_value_type _val(aVal);
	return MPutProperty<property_value_type>(aKey, _val);
}
template<class TPropes>
inline bool CProperties<TPropes>::MPutProperty(property_key const& aKey,
		char* const & aVal)
{
	property_value_type _val(aVal);
	return MPutProperty<property_value_type>(aKey, _val);
}
template<class TPropes>
inline bool CProperties<TPropes>::MPutProperty(property_key const& aKey,
		std::string const & aVal)
{
	property_value_type _val(aVal);
	return MPutProperty<property_value_type>(aKey, _val);
}
template<class TPropes>
inline bool CProperties<TPropes>::MIsProperty(property_key const& aKey) const
{
	RAccess _node = FNodeTree.MGetRAccess();
	typename properties_t::const_iterator _it = _node.MGet().find(aKey);
	return _it != _node.MGet().end();
}
template<class TPropes>
inline bool CProperties<TPropes>::MResetProperty(property_key const& aKey)
{
	WAccess _node = FNodeTree.MGetWAccess();
	typename properties_t::iterator _it = _node.MGet().find(aKey);
	if (_it != _node.MGet().end())
	{
		property_impl_t _tmp = _it->second;
		_it->second.MReset();
		if (!MPropertyChanged(aKey, _tmp.MGet()))
		{
			_it->second = _tmp;
			return false;
		}
	}
	return true;
}
template<class TPropes>
inline std::ostream& CProperties<TPropes>::MPrintPTree(
		std::ostream & aStream) const
{
	RAccess _node = FNodeTree.MGetRAccess();
	typename properties_t::const_iterator _it = _node.MGet().begin();
	for (; _it != _node.MGet().end(); ++_it)
		aStream << _it->first << "= " << _it->second.MGet() << std::endl;
	return aStream;
}
template<class TPropes>
inline void CProperties<TPropes>::MRemoveProperty(property_key const& aKey)
{
	WAccess _node = FNodeTree.MGetWAccess();
	_node.MGet().erase(aKey);
}
template<class TPropes>
inline void CProperties<TPropes>::MCleanAllProperties()
{
	WAccess _node = FNodeTree.MGetWAccess();
	_node.MGet().clear();
}
template<class TPropes>
inline bool CProperties<TPropes>::MIsPropertyEmpty() const
{
	return FNodeTree.MGetRAccess().MGet().empty();
}
template<class TPropes>
inline std::size_t CProperties<TPropes>::MGetProperties(
		array_properties_t* aTo) const
{
	RAccess _node = FNodeTree.MGetRAccess();
	typename properties_t::const_iterator _it = _node.MGet().begin();
	for (; _it != _node.MGet().end(); ++_it)
	{
		array_properties_t::value_type _val(_it->first, _it->second.MGet());
		aTo->push_back(_val);
	}
	return aTo->size();
}
template<class TPropes>
template<class TPred>
inline void CProperties<TPropes>::MForEach(TPred aPred)
{
	WAccess _node = FNodeTree.MGetWAccess();
	typename properties_t::iterator _it = _node.MGet().begin();
	for (; _it != _node.MGet().end(); ++_it)
		aPred(_it->first, _it->second.MGet());
}
template<class TPropes>
template<class TPred>
inline void CProperties<TPropes>::MForEach(TPred aPred) const
{
	RAccess _node = FNodeTree.MGetRAccess();
	typename properties_t::const_iterator _it = _node.MGet().begin();
	for (; _it != _node.MGet().end(); ++_it)
		aPred(_it->first, _it->second.MGet());
}
namespace NPropertyImpl
{
#ifdef STRING_STREAM_MUTEX
inline CMutex& sMAntiCrashStringStream()
{
	static CMutex _mutex;
	return _mutex;
}
#endif
inline std::locale& sMGetLocale(){
static std::locale _locale("");
return _locale;
}
}

template<class TPropes>
inline bool CProperties<TPropes>::sMUnitTest()
{
	CProperties _tree;
	{
			_tree.MPutProperty("float", 2.0);
			MASSERT_1(_tree.MGetProperty<std::string>("float") == "2");
			_tree.MPutProperty("float", 2.1);
			MASSERT_1(_tree.MGetProperty<std::string>("float") == "2.1");
	}
	{
		_tree.MPutProperty("unsigned", 5u);
		_tree.MPutProperty("unsigned", -2);
		MASSERT_1(_tree.MGetProperty<unsigned>("unsigned") == 5);
		_tree.MPutProperty("unsigned", std::numeric_limits<unsigned>::max());
		MASSERT_1(
				_tree.MGetProperty<unsigned>("unsigned")
						== std::numeric_limits<unsigned>::max());

		std::stringstream _check_max_unsigned;
		_check_max_unsigned << std::numeric_limits<unsigned>::max();
		_tree.MPutProperty("unsigned", _check_max_unsigned.str() + "0");
		MASSERT_1(
				_tree.MGetProperty<unsigned>("unsigned")
						== std::numeric_limits<unsigned>::max());
		_tree.MPutProperty("unsigned", "0xFF");
		MASSERT_1(_tree.MGetProperty<std::string>("unsigned") == "0xFF");
		MASSERT_1(_tree.MGetProperty<unsigned>("unsigned") == 255);
	}
	{
		_tree.MPutProperty<unsigned char>("unsigned char", 0xA5);
		MASSERT_1(_tree.MGetProperty<unsigned>("unsigned char") == 0xA5);
		MASSERT_1(_tree.MGetProperty<std::string>("unsigned char") == "0xA5");
	}
	{
		_tree.MPutProperty("int", 5);
		_tree.MPutProperty("int", -2);
		MASSERT_1(_tree.MGetProperty<int>("int") == -2);
		_tree.MPutProperty("int", std::numeric_limits<int>::max());
		MASSERT_1(
				_tree.MGetProperty<int>("int")
						== std::numeric_limits<int>::max());

		_tree.MPutProperty("int", std::numeric_limits<int>::min());
		MASSERT_1(
				_tree.MGetProperty<int>("int")
						== std::numeric_limits<int>::min());

		std::stringstream _check_max_unsigned;
		_check_max_unsigned << std::numeric_limits<int>::max();
		_tree.MPutProperty("int", _check_max_unsigned.str() + "0");
		MASSERT_1(
				_tree.MGetProperty<int>("int")
						== std::numeric_limits<int>::min());
	}
	{
		_tree.MPutProperty("char*", "string");
		MASSERT_1(_tree.MGetProperty<std::string>("char*") == "string");
		_tree.MPutProperty("char*", "0123456789");
		MASSERT_1(_tree.MGetProperty<std::string>("char*") == "0123456789");
	}
	{
		_tree.MPutProperty<bool>("bool", true);
		MASSERT_1(_tree.MGetProperty<bool>("bool") == true);
		_tree.MPutProperty("bool", "false");
		MASSERT_1(_tree.MGetProperty<bool>("bool") == false);
		_tree.MPutProperty("bool", "1");
		MASSERT_1(_tree.MGetProperty<bool>("bool") == true);
		_tree.MPutProperty("bool", "0");
		MASSERT_1(_tree.MGetProperty<bool>("bool") == false);
		_tree.MPutProperty("bool", "true");
		MASSERT_1(_tree.MGetProperty<bool>("bool") == true);
		MASSERT_1(_tree.MGetProperty<std::string>("bool") == "true");
	}
	{
		struct _test_t
		{
			CProperties<CMutex> _property;
			NSHARE::CThread _pthread;
			int _count;
			_test_t()
			{
				_count = 0;
				_pthread += NSHARE::CB_t(sMTest, this);
				_pthread.MCreate(NULL);
			}
			~_test_t()
			{
				_pthread.MCancel();
			}
			static int sMTest(void* aWho, void* aWhat, void*aData)
			{
				_test_t* _test = static_cast<_test_t*>(aData);
				for (; _test->_property.MIsPropertyEmpty(); usleep(100))
					;
				for (; _test->_count < 10; usleep(100))
					_test->_property.MPutProperty("count", ++_test->_count);
				return 0;
			}
			inline void gen_random_string(char* aS, size_t aLen)
			{
				static char const _alphanum[] = "0123456789"
						"QWERTYUIOPASDFGHJKLZXCVBNM"
						"qwertyuiopasdfghjklzxcvbnm";
				static size_t const _alphanum_size = sizeof(_alphanum) - 1;
				if (!aLen)
					return;
				aS[aLen] = 0;
				for (; aLen != 0;) //aLen всегда >0
					aS[--aLen] = _alphanum[std::rand() % _alphanum_size];
			}
			bool MTest()
			{
				_property.MPutProperty<int>("count", _count);
				char _str[256];
				for (; _count != 10;)
				{
					gen_random_string(_str, 256);
					_property.MPutProperty<int>(_str, 0);
					usleep(1);
				}
				usleep(1000);
				//if(_pthread.MIsRunning())_pthread.MJoin();
				if (_count == _property.MGetProperty<int>("count"))
					return true;
				else
					return false;
			}
		} _test;

		MASSERT_1(_test.MTest());
	}
	return true;
}
} //namespace USHARE
#endif /* CPROPERTIES_HPP_ */
