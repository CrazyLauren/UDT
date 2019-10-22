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
	CConfig(const CText& key, const T& value) :
		FData(key)
	{
#ifdef		NUM_TO_STRING_EXIST
		FData.MWrite().FValue=to_string<T>(value);
#else
		std::stringstream out;
		out << value;
		FData.MWrite().FValue=out.str();
#endif
	}

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
	T MValue(T _val=T()) const
	{
#ifdef FROM_STRING_EXIST
		bool _is = from_string(MValue(), _val);
		LOG_IF(ERROR, !_is) << "Cannot convert " << MValue() << " to "
		<< CTypeInfo(CTypeInfo::_info<T>());
		(void)_is;
#else
		std::istringstream _stream(MValue().MToStdString());
		if (!_stream.eof())
			_stream >> _val;
		LOG_IF(ERROR, _stream.fail()) << "Cannot convert " << MValue();
#endif
		return _val;
	}
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
	CConfig& MAdd(const CText& key, const T& value)
	{
#ifdef		NUM_TO_STRING_EXIST
		FData.MWrite().FChildren.push_back(CConfig(key, to_string<T>(value)));
#else
		std::stringstream out;
		out << value;
		FData.MWrite().FChildren.push_back(CConfig(key, out.str()));
#endif
		return *this;
	}

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
	CConfig& MUpdate(const CText& key, const T& value)
	{
#ifdef		NUM_TO_STRING_EXIST
		return MUpdate(CConfig(key, to_string<T>(value)));
#else
		std::stringstream out;
		out << value;
		return MUpdate(CConfig(key, out.str()));
#endif

	}


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
	};
	template<class T>
	inline void MReadFrom(T const&, bool aFirst = true); //ptree - boost

	template<class T>
	inline void MWriteTo(T&, bool aFirst = true) const; //ptree - boost

	CCOWPtr<data_t> FData;
};

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
CConfig& CConfig::MAdd<CText>(const CText& key, const CText& value)
{
	FData.MWrite().FChildren.push_back(CConfig(key, value));
	return *this;
}
template<> inline
CText CConfig::MValue<CText>(CText _val) const
{
	return MValue();
}
template<>
CBuffer CConfig::MValue<CBuffer>(CBuffer _val) const;

template<> inline
CConfig& CConfig::MAdd<CBuffer>(const CText& key,CBuffer const & aTo)
{
	return MAddTo(key,aTo);
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
