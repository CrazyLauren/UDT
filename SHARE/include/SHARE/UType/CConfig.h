/*
 * CConfig.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 20.01.2014
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CCONFIG_H_
#define CCONFIG_H_

#ifndef SHARE_CONFIG_DEFINED
#	define SHARE_CONFIG_DEFINED
#endif
namespace NSHARE
{
class SHARE_EXPORT CConfig;
#ifndef SERIALIZE_TEMPLATE_IS_DEFINED
#  define SERIALIZE_TEMPLATE_IS_DEFINED
/*! \brief Serialize type T
 *
 * The Serialized data key is equal "NAME".
 *
 * \param aObject Object
 * \return Serialized object
 * \tparam Serialized object type
 */
template<typename T>
inline NSHARE::CConfig serialize(T const& aObject);

/*! \brief Deserialize type T
 *
 * Deserialize object.
 *
 * @param aConf Serialized data (Child of key "NAME")
 * @return Deserialized object
 * @tparam Deserialized type
 */
template<typename T>
inline T deserialize(NSHARE::CConfig const& aConf);

template<typename T>
inline T deserialize(NSHARE::CConfig const& aConf,NSHARE::IAllocater* aAllocator)
{
	return deserialize<T>(aConf);
}
#endif //#ifndef SERIALIZE_TEMPLATE_IS_DEFINED

class CBuffer;
typedef std::vector<CConfig> ConfigSet;

/**\brief Config - это класс для сериализации данных в формат JSON или XML
 *
 * В начале обэект конвертируется в Config, далее Config конвертируется требуемый формат.
 * CConfig реализован с использованием COW
 */

class SHARE_EXPORT CConfig
{
public:
	static NSHARE::CText const XML_VALUE_NAME;//!< A key of XML value
	static CConfig const& sMGetEmpty();


	CConfig()
	{
	}

	explicit CConfig(const CText& key) :
			FData(data_t(key))
	{
	}

	CConfig(const CText& key, const CText& value) :
		FData(data_t(key,value))
	{
	}
	CConfig(const CText& key,void const* aTo, size_t aMaxLen);
	CConfig(const CText& key,CBuffer const & aTo);


	CConfig(const CText& key, const CConfig& value);//Protection, in case of CConfig(const CText& key, const T& value)
	//with T=CConfig. As the value is to be serialized by CConfig(const CText& key, const T& value)

	template<class T>
	CConfig(const CText& key, const T& value);

	CConfig(const CConfig& rhs);


	CConfig& operator=(const CConfig& rhs);
	~CConfig();
	bool MFromXML(std::istream&);
	bool MToXML(std::ostream&,bool aPretty=false) const;


	bool MToJSON(std::ostream&,bool aPretty=false) const;
	bool MToJSON(NSHARE::CText&,bool aPretty=false) const;
	bool MToJSON(NSHARE::CBuffer&) const;

	bool MFromJSON(NSHARE::CBuffer const&);
	bool MFromJSON(NSHARE::CText const&);
	bool MFromJSON(std::istream&);
	NSHARE::CText MToJSON(bool aPretty=false) const;


	bool MIsEmpty() const
	{
		return FData.MRead().FKey.empty() && FData.MRead().FValue.empty() && FData.MRead().FChildren.empty();
	}
	bool MIsOnlyKey() const
	{
		return FData.MRead().FValue.empty() && FData.MRead().FChildren.empty();
	}
	bool MIsSimple() const //leaf
	{
		return !FData.MRead().FKey.empty() && !FData.MRead().FValue.empty() && FData.MRead().FChildren.empty();
	}

	CText& MKey()
	{
		return FData.MWrite().FKey;
	}
	const CText& MKey() const
	{
		return FData.MRead().FKey;
	}

	const CText& MValue() const
	{
		return FData.MRead().FValue;
	}
	CText& MValue()
	{
		return FData.MWrite().FValue;
	}
	size_t MValueBuf(size_t aMaxLen,void* aTo) const;
	void MValue(CBuffer & aTo) const;

	template<typename T>
	T MValue(T _val=T()) const;

	// populates a primitive value.
	template<typename T>
	T MValue(const CText& key, T const& fallback) const
	{
		CConfig const& _child = MChild(key);
		if (_child.MValue().empty())
			return fallback;
		return _child.MValue<T>(fallback);
	}

	const ConfigSet& MChildren() const
	{
		return  FData.MRead().FChildren;
	}
	ConfigSet& MChildren()//todo private
	{
		return FData.MWrite().FChildren;
	}
	const ConfigSet MChildren(const CText& key) const
	{
		ConfigSet r;
		for (ConfigSet::const_iterator i = FData.MRead().FChildren.begin();
				i != FData.MRead().FChildren.end(); ++i)
		{
			if (i->MKey() == key)
				r.push_back(*i);
		}
		return r;
	}

	bool MIsChild(const CText& key) const
	{
		for (ConfigSet::const_iterator i = FData.MRead().FChildren.begin();
				i != FData.MRead().FChildren.end(); ++i)
			if (i->MKey() == key)
				return true;
		return false;
	}

	void MRemove(const CText& key)
	{
		for (ConfigSet::iterator i = FData.MWrite().FChildren.begin(); i != FData.MWrite().FChildren.end();)
		{
			if (i->MKey() == key)
				i = FData.MWrite().FChildren.erase(i);
			else
				++i;
		}
	}

	CConfig const& MChild(const CText& key) const;

	const CConfig* MChildPtr(const CText& key) const;

	CConfig* MMutableChild(const CText& key);

	void MMerge(const CConfig& rhs);
	void MBlendWith(const CConfig& rhs);

	CConfig* MFind(const CText& key);
	const CConfig* MFind(const CText& key) const;

#ifdef SMART_FIELD_EXIST
	template<typename T>
	void MAddIfSet(const CText& key, const smart_field_t<T>& opt)
	{
		if (opt.MIs())
		{
			MAdd(key, to_string<T>(opt.MGetConst()));
		}
	}
#endif
	template<typename T>
	CConfig& MAdd(const CText& key, const T& value);

	CConfig&  MAdd(const CText& key,void const* aTo,size_t aMaxLen);
	CConfig&  MAddTo(const CText& key,CBuffer const & aTo);
	CConfig& MAdd(const CConfig& conf)
	{
		FData.MWrite().FChildren.push_back(conf);
		return *this;
	}

	CConfig& MAdd(const CText& aKey, const CConfig& aConf)
	{
		if(aKey==aConf.MKey())
			return MAdd(aConf);
		else
		{
			CConfig _temp = aConf;
			_temp.MKey() = aKey;
			return MAdd(_temp);
		}
	}

	void MAdd(const ConfigSet& set)
	{
		for (ConfigSet::const_iterator _it = set.begin(); _it != set.end();
				++_it)
			MAdd(*_it);
	}
	template<typename T>
	CConfig& MUpdate(const CText& key, const T& value);

	CConfig& MUpdate(const CConfig& conf)
	{
		MRemove(conf.MKey());
		return MAdd(conf);
	}

	template<typename T>
	CConfig& MSet(const CText& key, const T& value)
	{
		return MUpdate(key, value);
	}

	bool MHasValue(const CText& key) const
	{
		return !MValue(key).empty();
	}

	const CText& MValue(const CText& key) const
	{
		return MChild(key).MValue();
	}




#ifdef SMART_FIELD_EXIST
	// populates the output value iff the Config exists.
	template<typename T>
	bool MGetIfSet(const CText& key, smart_field_t<T>& output) const
	{
		if (MIsChild(key))
		{
			output = MValue<T>(key, T());
			return true;
		}
		else
			return false;
	}
#endif
	template<typename T>
	bool MGetIfSet(const CText& key, T& output) const
	{
		CConfig const& _child = MChild(key);
		if (_child.MValue().empty())
			return false;
		else
		{
			output = _child.MValue<T>(output);
			return true;
		}
	}

	/*!\brief Update the Value if key is exist
	 *
	 *\return true if updated
	 */
	template<typename T>
	bool MUpdateIfSet(const CText& key, const T& value)
	{
		CConfig* _child = MMutableChild(key);
		if (_child==NULL ||_child->MValue().empty())
			return false;
		else
		{
			CConfig const _new(key,value);
			*_child=_new;
			return true;
		}
	}
#ifdef SMART_FIELD_EXIST
	template<typename T>
	bool MUpdateIfSet(const CText& key, const smart_field_t<T>& opt)
	{
		if (opt.MIs())
			return MUpdateIfSet(key,opt.MGetConst());
		else
			return false;
	}
#endif
	// remove everything from (this) that also appears in rhs
	CConfig operator -(const CConfig& rhs) const;
	std::ostream& MPrint(std::ostream & aStream) const;

	NSHARE::CConfig MSerialize() const
	{
		return *this;
	}
	bool MIsValid() const
	{
		return !MIsEmpty();
	}
	static bool sMUnitTest();
private:
	struct SHARE_EXPORT data_t
	{

		data_t(){
			;
		}
		data_t(const CText& key);
		data_t(const CText& key, const CText& value);

		CText FKey;
		CText FValue;
		ConfigSet FChildren;
		bool operator==(const data_t& __lhs) const;
	};
	template<class T>
	inline void MReadFrom(T const&, bool aFirst = true); //ptree - boost

	template<class T>
	inline void MWriteTo(T&, bool aFirst = true) const; //ptree - boost

	CCOWPtr<data_t> FData;

	friend bool operator==(const CConfig& __lhs, const CConfig& __rhs);
};
namespace impl
{
namespace cconfig
{
template<class T>
inline NSHARE::CConfig serialize_standard_types(T const& aObject)
{
	NSHARE::CConfig _conf;
	#ifdef		NUM_TO_STRING_EXIST
	_conf.MValue()=to_string<T>(aObject);
#else
	std::stringstream out;
	out << aObject;
	_conf.MValue() = out.str();
#endif
	return _conf;
}
template<class T>
inline T deserialize_standard_types(NSHARE::CConfig const& aConf)
{
	T _val;
#ifdef FROM_STRING_EXIST
	bool _is = from_string(aConf.MValue(), _val);
	LOG_IF(ERROR, !_is) << "Cannot convert " << aConf.MValue() << " to "
	<< CTypeInfo(CTypeInfo::_info<T>());
	(void)_is;
#else
	std::istringstream _stream(aConf.MValue().MToStdString());
	if (!_stream.eof())
		_stream >> _val;
	LOG_IF(ERROR, _stream.fail()) << "Cannot convert " << aConf.MValue();
#endif
	return _val;
}
typedef char is_method_t;
typedef int nobody_t;

template<typename T>
struct serialize_check
{
#ifdef CAN_USE_SFINAE_METHODS_DETECTOR
    template<typename U,NSHARE::CConfig (U::*)() const> struct serialize_1{};
    template<typename U,NSHARE::CConfig const& (U::*)() const> struct serialize_2{};

    template <typename U> static is_method_t test(serialize_1<U, &U::MSerialize>*);
    template <typename U> static is_method_t test(serialize_2<U, &U::MSerialize>*);
#endif
    template <typename U> static nobody_t test(...);
    enum
    {
#ifdef CAN_USE_SFINAE_METHODS_DETECTOR
        result = sizeof(test<T>(0))
#else
		result = sizeof(nobody_t)
#endif	//	#ifdef CAN_USE_SFINAE_METHODS_DETECTOR
    };
};
template <typename T, unsigned = serialize_check<T>::result >
struct ser_t
{
	template <typename U>
	static
	inline NSHARE::CConfig serialize(U const& aObject)
	{
		return aObject.MSerialize();
	}
};
template <typename T >
struct ser_t<T, sizeof(nobody_t)>
{
	template <typename U>
	static
	inline NSHARE::CConfig serialize(U const& aObject)
	{
		return serialize_standard_types<U>(aObject);
	}
};
template<typename T>
struct deserialize_check
{
#ifdef CAN_USE_SFINAE_METHODS_DETECTOR
	template<typename U>
	static is_method_t test(U const& );
#endif
	template<typename U> static nobody_t test(...);
	enum
	{
#ifdef CAN_USE_SFINAE_METHODS_DETECTOR
        result = sizeof(test<T>(NSHARE::CConfig()))
#else
		result = sizeof(is_method_t)
#endif	//	#ifdef CAN_USE_SFINAE_METHODS_DETECTOR
	};
	/*#ifdef CAN_USE_SFINAE_METHODS_DETECTOR
	template<typename U>
	static U test2(U const& = U(NSHARE::CConfig()));

	template<std::size_t>
	struct dummy
	{
	};
	template<typename U>
	static is_method_t test(dummy< sizeof(test2<U>()) > *);
#endif
	template<typename U> static nobody_t test(...);
	enum eConstant
	{
		result = sizeof(test<T>(0))
	};*/
};
template<>
struct deserialize_check<bool>
{
	enum
	{
		result = sizeof(nobody_t)
	};
};
template<>
struct deserialize_check<unsigned char>
{
	enum
	{
		result = sizeof(nobody_t)
	};
};
template<>
struct deserialize_check<signed char>
{
	enum
	{
		result = sizeof(nobody_t)
	};
};
template<>
struct deserialize_check<unsigned short int>
{
	enum
	{
		result = sizeof(nobody_t)
	};
};
template<>
struct deserialize_check<short int>
{
	enum
	{
		result = sizeof(nobody_t)
	};
};
template<>
struct deserialize_check<unsigned int>
{
	enum
	{
		result = sizeof(nobody_t)
	};
};
template<>
struct deserialize_check<int>
{
	enum
	{
		result = sizeof(nobody_t)
	};
};
template<>
struct deserialize_check<unsigned long int>
{
	enum
	{
		result = sizeof(nobody_t)
	};
};
template<>
struct deserialize_check<long int>
{
	enum
	{
		result = sizeof(nobody_t)
	};
};
#ifdef SIZE_OF_LONG_LONG_INT
template<>
struct deserialize_check<long long int>
{
	enum
	{
		result = sizeof(nobody_t)
	};
};
template<>
struct deserialize_check<unsigned long long int>
{
	enum
	{
		result = sizeof(nobody_t)
	};
};
#endif
template<>
struct deserialize_check<double>
{
	enum
	{
		result = sizeof(nobody_t)
	};
};
#ifdef SIZE_OF_LONG_DOUBLE
template<>
struct deserialize_check<long double>
{
	enum
	{
		result = sizeof(nobody_t)
	};
};
#endif
template<>
struct deserialize_check<float>
{
	enum
	{
		result = sizeof(nobody_t)
	};
};

template <typename T, unsigned = deserialize_check<T>::result >
struct der_t
{
	//template <typename U>
	static
	inline T deserialize(NSHARE::CConfig const& aConf)
	{
		return T(aConf);
	}
};
template <typename T >
struct der_t<T, sizeof(nobody_t)>
{
	//template <typename U>
	static
	inline T deserialize(NSHARE::CConfig const& aConf)
	{
		return deserialize_standard_types<T>(aConf);
	}
};
}
};
template<typename T>
inline NSHARE::CConfig serialize(T const& aObject)
{
	return impl::cconfig::ser_t<T>::serialize(aObject);
}
template<typename T>
inline T deserialize(NSHARE::CConfig const& aConf)
{
	typedef impl::cconfig::der_t<T> _t;
	return _t::deserialize(aConf);
	//return T(aConf);
}
/** Serailize specialization for CBuffer
 *
 * @note defined in CConfig, realized in CBuffer
 */
SHARE_EXPORT NSHARE::CConfig serialize_cbuffer(NSHARE::CBuffer const& aObject);
template<>
inline NSHARE::CConfig serialize<NSHARE::CBuffer>(NSHARE::CBuffer const& aObject)
{
	return serialize_cbuffer(aObject);
}
/*
#define SPECIALIZE_SERIALIZE_FOR(aDATA_TYPE)\
template<>\
inline NSHARE::CConfig serialize<aDATA_TYPE>(aDATA_TYPE const& aObject)\
{\
	return serialize_standard_types<aDATA_TYPE>(aObject);\
}\
template<>\
inline aDATA_TYPE deserialize(NSHARE::CConfig const& aConf)\
{\
	return deserialize_standard_types<aDATA_TYPE>(aConf);\
}
*/

/*SPECIALIZE_SERIALIZE_FOR(bool)
SPECIALIZE_SERIALIZE_FOR(unsigned char)
SPECIALIZE_SERIALIZE_FOR(signed char)
SPECIALIZE_SERIALIZE_FOR(unsigned short int)
SPECIALIZE_SERIALIZE_FOR(short int)
SPECIALIZE_SERIALIZE_FOR(unsigned int)
SPECIALIZE_SERIALIZE_FOR(int)
SPECIALIZE_SERIALIZE_FOR(long int)
SPECIALIZE_SERIALIZE_FOR(unsigned long int)
#if __cplusplus >= 201103L
SPECIALIZE_SERIALIZE_FOR(unsigned long long)
SPECIALIZE_SERIALIZE_FOR(long long int)
#endif
SPECIALIZE_SERIALIZE_FOR(double)
SPECIALIZE_SERIALIZE_FOR(float)
SPECIALIZE_SERIALIZE_FOR(long double)*/

template<class T>
inline CConfig::CConfig(const CText& key, const T& value) :
	FData(serialize<T>(value).FData)
{
	MKey() = key;
//#ifdef NUM_TO_STRING_EXIST
//	FData.MWrite().FValue=to_string<T>(value);
//#else
//	std::stringstream out;
//	out << value;
//	FData.MWrite().FValue=out.str();
//#endif
}

#ifdef SMART_FIELD_EXIST
// specialization for Config
template<> inline
void CConfig::MAddIfSet<CConfig>(const CText& key,
		const smart_field_t<CConfig>& opt)
{
	if (opt.MIs())
	{
		CConfig conf = opt.MGetConst();
		conf.MKey() = key;
		MAdd(conf);
	}
}

template<> inline
bool CConfig::MGetIfSet<CConfig>(const CText& key,
		smart_field_t<CConfig>& output) const
{
	if (MIsChild(key))
	{
		output = MChild(key);
		return true;
	}
	else
	return false;
}
#endif
template<> inline
bool CConfig::MUpdateIfSet<CConfig>(const CText& key, const CConfig& value)
{
	CConfig* _child = MMutableChild(key);
	if (_child==NULL || _child->MIsEmpty())
		return false;
	else
	{
		CConfig const _new(key,value);
		*_child=_new;
		return true;
	}
}
template<> inline
bool CConfig::MGetIfSet<CConfig>(const CText& key,
		CConfig& output) const
{
	if (MIsChild(key))
	{
		output = MChild(key);
		return true;
	}
	else
		return false;
}
template<typename T> inline
CConfig& CConfig::MAdd(const CText& key, const T& value)
{
	FData.MWrite().FChildren.push_back(CConfig(key, serialize<T>(value)));
//#ifdef		NUM_TO_STRING_EXIST
//	FData.MWrite().FChildren.push_back(CConfig(key, to_string<T>(value)));
//#else
//	std::stringstream out;
//	out << value;
//	FData.MWrite().FChildren.push_back(CConfig(key, out.str()));
//#endif
	return *this;
}
template<> inline
CConfig& CConfig::MAdd<CText>(const CText& key, const CText& value)
{
	FData.MWrite().FChildren.push_back(CConfig(key, value));
	return *this;
}
template<typename T>
inline T CConfig::MValue(T _val) const
{
	return MIsOnlyKey()? _val: deserialize<T>(*this);
//#ifdef FROM_STRING_EXIST
//	bool _is = from_string(MValue(), _val);
//	LOG_IF(ERROR, !_is) << "Cannot convert " << MValue() << " to "
//	<< CTypeInfo(CTypeInfo::_info<T>());
//	(void)_is;
//#else
//	std::istringstream _stream(MValue().MToStdString());
//	if (!_stream.eof())
//		_stream >> _val;
//	LOG_IF(ERROR, _stream.fail()) << "Cannot convert " << MValue();
//#endif
//	return _val;
}
template<> inline
CText CConfig::MValue<CText>(CText _val) const
{
	return MValue();
}
//template<>
//CBuffer CConfig::MValue<CBuffer>(CBuffer _val) const;
#ifdef SHARE_BUFFER_DEFINED
template<> inline
CBuffer CConfig::MValue<CBuffer>(CBuffer _val) const
{
	MValue(_val);
	return _val;
}
#endif
template<> inline
CConfig& CConfig::MAdd<CBuffer>(const CText& key,CBuffer const & aTo)
{
	return MAddTo(key,aTo);
}
template<typename T> inline
CConfig& CConfig::MUpdate(const CText& key, const T& value)
{
	return MUpdate(CConfig(key, serialize<T>(value)));
//#ifdef		NUM_TO_STRING_EXIST
//	return MUpdate(CConfig(key, to_string<T>(value)));
//#else
//	std::stringstream out;
//	out << value;
//	return MUpdate(CConfig(key, out.str()));
//#endif

}

template<> inline
CConfig& CConfig::MUpdate<CText>(const CText& key, const CText& value)
{
	MRemove(key);
	return MAdd(CConfig(key, value));
}

template<> inline
CConfig& CConfig::MUpdate<CConfig>(const CText& key, const CConfig& conf)
{
	MRemove(key);
	CConfig temp = conf;
	temp.MKey() = key;
	return MAdd(temp);
}
inline bool operator==(const CConfig& __lhs, const CConfig& __rhs)
{
	return __lhs.FData == __rhs.FData;
}
inline bool operator!=(const CConfig& __lhs, const CConfig& __rhs)
{
	return !operator==(__lhs, __rhs);
}

} /* namespace NSHARE */
namespace std
{
inline std::ostream& operator <<(std::ostream& aStream,
		NSHARE::CConfig const& aInfo)
{
	aInfo.MPrint(aStream);
	return aStream;
}
}
#endif /* CCONFIG_H_ */
