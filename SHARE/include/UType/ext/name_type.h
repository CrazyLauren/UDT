/*
 * name_type.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 11.09.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef NAME_TYPE_H_
#define NAME_TYPE_H_

#include <stdexcept>
namespace NSHARE
{
//!@struct name_t
//!@brief  Имя типа идентификатора класса
//!@brief  Если номер <0 то он не учитывается
struct  name_t
{
	typedef  CText string_t;
	struct  type_t: string_t
	{
		type_t();
		type_t(string_t const& aName, string_t const& aSubName = string_t(),
				string_t const& aSubSubName = string_t(),
				string_t const& aSubSubSubName = string_t());
		type_t(const char* aName);
		template<class T>
		type_t&operator=(const T& __str);
		//is the Object subtype of aMainType
		bool MIsSubTypeOf(type_t const& aMainType)const;
		//is aSubType subtype of Object
		bool MIsMainTypeOf(type_t const& aSubType)const;

		static string_t sMCreateFullType(string_t const& aName,
				string_t const& aSubName=string_t(), string_t const& aSubSubName=string_t(),
				string_t const& aSubSubSubName=string_t())
		{
			string_t _type = aName;
			if (!aSubName.empty())
			{
				_type += sMSpecifier();
				_type += aSubName;

				if (!aSubSubName.empty())
				{
					_type += sMSpecifier();
					_type += aSubSubName;
					if (!aSubSubSubName.empty())
					{
						_type += sMSpecifier();
						_type += aSubSubSubName;
					}
				}
			}
			return _type;
		}
	};
	typedef string_t full_name_t;
	typedef int number_t;
	//_-_
	COMPILE_ASSERT(static_cast<utf32>('@')==64u,WtfInvalidSymbolCoding);
	static const char sMSpecifier()
	{
		return '@';
	}
	name_t(type_t const&, number_t);
	name_t();
	name_t(const name_t&);
	name_t & operator=(const name_t&);

	bool MSetName(type_t const&, number_t);
	type_t const& MGetType() const;

	full_name_t const& MGetFullName() const;
	bool MSetByFullName(full_name_t const&);

	static type_t sMGenType();
	static bool sMIsTypeCorrect(type_t const& aType);

	static full_name_t sMGetFullName(type_t const&, number_t const &);//спрефиксом
	static bool sMGetByFullName(full_name_t const&, type_t*, number_t*);//спрефиксом

	bool operator<(const name_t&) const;
	bool operator==(const name_t&) const;
	bool operator<(const full_name_t&) const;

	number_t MGetNumber() const;
private:
	static const char sMSpace()
	{
		return '#';
	}
	type_t FType;
	number_t FNumber;
	full_name_t FFullName;//Optimize

	//friend std::ostream& operator <<(std::ostream&, const NSHARE::name_t&);
	friend full_name_t const& operator>>(full_name_t const&, NSHARE::name_t&);
};
template<class T>
inline name_t::type_t &name_t::type_t::operator=(const T& __str)
{
	string_t::operator =(__str);
	return *this;
}
inline name_t::type_t::type_t()
{
	VLOG(2)<<"New empty type"<<" :"<<this;;
}
inline  name_t::type_t::type_t(string_t const& aName, string_t const& aSubName,
		string_t const& aSubSubName,
		string_t const& aSubSubSubName ) :
		string_t(sMCreateFullType(aName,aSubName,aSubSubName,aSubSubSubName))
{
	VLOG(2)<<"New type "<<aName<<" :"<<this;;
}
inline name_t::type_t::type_t(const char* aName) :
		string_t(aName)
{
	VLOG(2)<<"New type "<<aName<<" :"<<this;
}
inline bool name_t::type_t::MIsSubTypeOf(type_t const& aMainType) const
{
	VLOG(2)<<"Is " << *this <<" Sub of  "<<aMainType<<" :"<<this;
	if(aMainType.length()>length())
		return false;
	string_t::const_iterator _sub_it=begin();
	string_t::const_iterator _main_it=aMainType.begin();
	for(;_main_it!=aMainType.end();++_main_it,++_sub_it)
	{
		if(*_main_it!=*_sub_it)
			return false;
	}
	if(aMainType.length()==length())
		return true;

	if(*_sub_it!=static_cast<utf32>(sMSpecifier()))
		return false;

	return true;
}
inline bool name_t::type_t::MIsMainTypeOf(type_t const& aSubType) const
{
	VLOG(2)<<"Is " << *this <<" Main of  "<<aSubType<<" :"<<this;

	return aSubType.MIsSubTypeOf(*this);
}

inline name_t::name_t(type_t const&aVal, number_t aVal2)
{
	VLOG(2)<<"New name. Type "<<aVal<<" Number:"<<aVal2<<" :"<<this;
	if(!MSetName(aVal, aVal2))
		throw std::invalid_argument(("type_t:"+aVal).MToStdString());
}

inline name_t::name_t()
{
	VLOG(2)<<"New empty name. :"<<this;
	type_t _type=sMGenType();
	if (!MSetName(_type,std::rand()))
		throw std::invalid_argument(_type.MToStdString());
}

inline name_t::name_t(const name_t& aRht)
{
	VLOG(2)<<"New from name. :"<<&aRht<<" :"<<this;
	CHECK_NE(&aRht , this);
	CHECK_NE(aRht.FType.empty(),true);
	bool _is=MSetName(aRht.FType, aRht.FNumber);
	MASSERT(_is,true);
}
inline name_t & name_t::operator=(const name_t& aRht)
{
	VLOG(2)<<"Assignment name from :"<<&aRht<<" :"<<this;
	CHECK_NE(&aRht , this);
	CHECK_NE(aRht.FType.empty(),true);
	bool _is=MSetName(aRht.FType,aRht.FNumber);
	MASSERT(_is,true);
	return *this;
}
inline bool name_t::MSetName(type_t const& aVal, number_t aNumber)
{
	VLOG(2)<<"Set name. Type "<<aVal<<" Number:"<<aNumber<<" :"<<this;
	if (!sMIsTypeCorrect(aVal))
	{
		LOG(WARNING)<<"Name isn't correct."<<"Type "<<aVal<<" Number:"<<aNumber<<" :"<<this;
		return false;
	}
	FType = aVal;
	FNumber = aNumber;
	FFullName=sMGetFullName(MGetType(), MGetNumber());

	VLOG(2)<<"Fullname "<<FFullName<<" :"<<this;
	return true;
}
inline bool name_t::MSetByFullName(full_name_t const& aVal)
{
	VLOG(2)<<"Set name by full name: "<<aVal<<" :"<<this;
	type_t _type;
	number_t _number=-1;
	if (sMGetByFullName(aVal, &_type, &_number))
		return MSetName(_type,_number);
	return false;
}

inline name_t::type_t const& name_t::MGetType() const
{
	return FType;
}
inline name_t::full_name_t const& name_t::MGetFullName() const
{
	return FFullName;
}

inline name_t::type_t name_t::sMGenType()
{
	VLOG(2)<<"Gen random type";
	type_t _type;
	_type.MMakeRandom(7);
	VLOG(2)<<"Random type:"<<_type;
	return _type;
}
inline bool name_t::sMIsTypeCorrect(type_t const& aType)
{
	if (aType.empty())
		return false;
	if(aType.isalnum())
		return true;
	type_t::const_iterator _it = aType.begin();
	VLOG(2)<<"The special symbol is "<<static_cast<utf32>(sMSpecifier());
	for (; _it != aType.end(); ++_it)
		if(!CText(1,*_it).isalnum()&& static_cast<utf32>(sMSpecifier())!=*_it)
			{
				LOG(WARNING)<<"The Symbol "<<CText(1,*_it)<< "("<<*_it<<")isn't correct.";
				return false;
			}
	if (*aType.begin() == static_cast<utf32>(sMSpecifier())
			|| *aType.end() == static_cast<utf32>(sMSpecifier()))
		return false;
	return true;
}
inline name_t::number_t name_t::MGetNumber() const
{
	return FNumber;
}


inline name_t::full_name_t name_t::sMGetFullName(type_t const& aVal,
		number_t const & aNumber)
{
/*	std::stringstream _new_name;
	if (aNumber < 0)
		_new_name << aVal;
	else
		_new_name << aVal << sMSpace() << aNumber;

	return _new_name.str();*/

	 name_t::full_name_t _name(aVal);

	if (aNumber >= 0)
	{
		_name += sMSpace();
		NSHARE::num_to_str(aNumber, _name);
	}
	return 	_name;

}

inline bool name_t::sMGetByFullName(full_name_t const& aVal, type_t* aType,
		number_t* aNumber)
{
	full_name_t::const_iterator _it = aVal.begin();
	for (; _it != aVal.end(); ++_it)
		if (*_it == '_')
		{
			*aType = aVal.substr(0, _it - aVal.begin());
			++_it;
			if (_it != aVal.end())
			{
/*
				std::istringstream _stream(aVal.substr(_it - aVal.begin()).MToStdString());
				_stream >> (*aNumber);
				return !_stream.fail() && _stream.eof();
*/
				return NSHARE::str_to_decimal(aVal.substr(_it-aVal.begin()),*aNumber);
			}
			else
			{
				*aNumber = -1;
				return true;
			}
		}
	return false;
}

inline bool name_t::operator<(const name_t& aRhs) const
{
	if(FNumber>=0)
		return FType < aRhs.FType && FNumber < aRhs.FNumber;
	else
		return FType < aRhs.FType;
}
inline bool name_t::operator==(const name_t& aRhs) const
{
	if (FNumber >= 0)
		return FType == aRhs.FType && FNumber == aRhs.FNumber;
	else
		return FType == aRhs.FType;
}
inline bool name_t::operator<(const full_name_t& aVal) const
{
	return MGetFullName() < aVal;
}
}
namespace std
{
inline std::ostream& operator <<(std::ostream& aStream, const NSHARE::name_t& aBase)
{
	return aStream << aBase.MGetFullName();
}

inline NSHARE::name_t::full_name_t const& operator>>(
		NSHARE::name_t::full_name_t const& aName, NSHARE::name_t& aBase)
{
	aBase.MSetByFullName(aName);
	return aName;
}
}

#endif /* NAME_TYPE_H_ */
