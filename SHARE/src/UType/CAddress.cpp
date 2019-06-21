// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CAddress.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 12.09.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
namespace NSHARE
{
const char CProgramName::SEPERATOR = '@';
const char CProgramGroup::SEPERATOR = '.';
const NSHARE::CText CProgramName::NAME = "name";
COMPILE_ASSERT(static_cast<utf32>(CProgramName::SEPERATOR) == 64u,
		WtfInvalidSymbolAt);
COMPILE_ASSERT(static_cast<utf32>('.') == 46u, WtfInvalidSymbolPoint);
CProgramName::CProgramName()
{

}
CProgramName::CProgramName(const char* aRaw) :
		FName(aRaw)
{
	DCHECK(sMIsValid(aRaw));
}
CProgramName::CProgramName(NSHARE::CText const& aRaw) :
		FName(aRaw)
{
	DCHECK(sMIsValid(aRaw));
}
CProgramName::CProgramName(NSHARE::CText const& aName, CProgramGroup const&aGroup) :
		FName(aName)
{
	DCHECK(sMIsValid(aName));
	MAddTo(aGroup);
}

CProgramName::CProgramName(NSHARE::CConfig const& aConf)
{
	VLOG(3) << "Create CRegistration from " << aConf;
	if (!aConf.MValue().empty())
	{
		FName = aConf.MValue<CText>(CText());
		const bool _is = sMIsValid(FName);
		LOG_IF(ERROR,!_is) << "The name " << FName << " is not valid";
		if (!_is)
			FName.clear();
	}
	VLOG(4) << "Value = " << FName;

}
NSHARE::CConfig CProgramName::MSerialize() const
{
	return CConfig(NAME, FName);
}
bool CProgramName::MIsValid() const
{
	return !FName.empty();
}
bool CProgramName::MSetRaw(NSHARE::CText const&aName)
{
	VLOG(3) << "Change " << FName << " to " << aName;
	DCHECK(sMIsValid(aName));
	if (!sMIsValid(aName))
		return false;
	FName = aName;
	return true;
}
void CProgramName::MAddTo(CProgramGroup const& aGroup)
{

	MSet(MGetName(), aGroup);
}
void CProgramName::MSet(NSHARE::CText const& aName, CProgramGroup const& aGroup)
{
	DCHECK(sMIsValid(aName));
	FName = sMRegister(aName, aGroup);
}
NSHARE::CText CProgramName::sMRegister(NSHARE::CText const& aName,
		CProgramGroup const& aGroup)
{
	DCHECK(sMIsNameValid(aName));
	VLOG(3) << aName << " added to  " << aGroup;
	if (aGroup.MIsEmpty())
		return aName;
	return aName + SEPERATOR + aGroup.MGetRaw();
}
void CProgramName::MChangeName(NSHARE::CText const& aNewName)
{
	VLOG(3) << "Change name from " << FName << " to " << aNewName;
	DCHECK(sMIsNameValid(aNewName));
	if (FName.empty())
		FName = aNewName;
	else
	{
		CText::size_type const _pos = FName.find(SEPERATOR);
		if (_pos == CText::npos) //no group
			FName = aNewName;
		else
			FName.replace(0, _pos, aNewName);
	}
}
NSHARE::CText CProgramName::MGetName() const
{
	CText::size_type const _pos = FName.find(SEPERATOR);
	if (_pos == CText::npos) //no group
		return FName;
	else
		return FName.substr(0, _pos);
}
NSHARE::CText const& CProgramName::MGetRawName() const
{
	return FName;
}
NSHARE::CText CProgramName::MGetAddressText() const
{
	CText::size_type const _pos = FName.find(SEPERATOR);
	if (_pos == CText::npos) //no group
		return CText();
	else
	{
		CHECK_NE((utf32 )SEPERATOR, *(--FName.end()));
		return FName.substr(_pos + 1);
	}
}
CProgramGroup CProgramName::MGetAddress() const
{
	const CText _gr = MGetAddressText();
	if (_gr.empty())
		return CProgramGroup();
	else
		return CProgramGroup(_gr);
}
unsigned CProgramName::MIsFrom(CProgramGroup const& aGroup) const
{
	return aGroup.MIsSubGroupOf(*this);
}
unsigned CProgramName::MIsFrom(NSHARE::CText const& aGroup) const
{
	const CProgramGroup _gr = MGetAddress();
	return _gr.MIsInGroup(aGroup);
}
unsigned CProgramName::MIsForMe(CProgramName const& aWhat) const
{
	return MIsForMe(aWhat.MGetRawName());
}
unsigned CProgramName::MIsForMe(NSHARE::CText const& aWhat) const
{
	return sMCompare(aWhat,MGetRawName());
}

unsigned CProgramName::sMCompare(NSHARE::CText const& aWhat, NSHARE::CText const& aWith)
{
	if (aWith.empty() || aWhat.empty())
		return 0;

	if (aWith.length() > aWhat.length())
		return 0;

	CText::const_iterator _it(aWith.begin()), _it_end(aWith.end());

	CText::const_iterator _jt(aWhat.begin()), _jt_end(
			aWhat.end());

	bool _is_sep = false;

	for (; _it != _it_end && //
			*_it == *_jt; ++_it, ++_jt)
		if (*_it == SEPERATOR)
			_is_sep = true;

	VLOG(5) << " IsSep=" << _is_sep << " end=" << (_it == _it_end)
						<< " jt_end=" << (_jt == _jt_end);

	if (_it != _it_end)//some char of path is not equal
		return 0;
	else if	( _jt == _jt_end)//full equal
		return 1;
	else if(*_jt==CProgramName::SEPERATOR)//aWith does not have group
			//aWhat have group, thus aWith is related with  aWhat
		return std::numeric_limits<unsigned>::max();
	else if (_is_sep && *_jt == CProgramGroup::SEPERATOR)
	{
		//The Names are equal and group of aWith is
		//subgroup of aWhat

		unsigned _level=1;//minimal level 1

		for (; _jt != _jt_end; ++_jt)
			if (*_jt == CProgramGroup::SEPERATOR)
				++_level;
		return _level;
	}
	else
	{
		VLOG(5)<<"The group is not equal  "<<*_jt;
		return 0;
	}
}
unsigned CProgramName::MIsMe(NSHARE::CText const& aWhat) const
{
	return sMCompare(MGetRawName(),aWhat);
}
unsigned CProgramName::MIsMe(CProgramName const& aWhat) const
{
	return MIsMe(aWhat.MGetRawName());
}
bool CProgramName::MIsNameEqual(CProgramName const& aWho) const
{

	CText::const_iterator _it(aWho.MGetRawName().begin()), _it_end(
			aWho.MGetRawName().end());
	CText::const_iterator _jt(MGetRawName().begin()), _jt_end(
			MGetRawName().end());

	for (; _it != _it_end && //
			_jt != _jt_end && //
			*_it == *_jt && //
			*_it != SEPERATOR; ++_it, ++_jt)
		;
	if (_it == _it_end && _jt == _jt_end)
		return true;
	else if (((_it == _it_end) && (*_jt == SEPERATOR)) || //
			((_jt == _jt_end) && (*_it == SEPERATOR)))
		return true;
	else
		return false;
}
bool CProgramName::MIsName() const
{
	return !FName.empty() && (*FName.begin() != SEPERATOR);
}
bool CProgramName::MIsAddress() const
{
	bool const _res = FName.find(SEPERATOR) != CText::npos;
	CHECK((!_res) || ((utf32 )SEPERATOR != *FName.end()));
	return _res;
}
bool CProgramName::operator<(const CProgramName&aRht) const
{
	return std::less<CText>()(FName, aRht.FName);
}
bool CProgramName::operator==(const CProgramName& aRht) const
{
	return std::equal_to<CText>()(FName, aRht.FName);
}

CProgramGroup::CProgramGroup()
{

}
CProgramGroup::CProgramGroup(const char* aGroup) :
		FAddress(aGroup)
{
	DCHECK(sMIsValid(aGroup));
}
CProgramGroup::CProgramGroup(NSHARE::CText const& aGroup) :
		FAddress(aGroup)
{
	DCHECK(sMIsValid(aGroup));
}
CProgramGroup::CProgramGroup(std::vector<NSHARE::CText> const& aGroups)
{
	MSet(aGroups);
}
bool CProgramGroup::MSet(NSHARE::CText const& aGroups)
{
	DCHECK(sMIsValid(aGroups));
	if (!sMIsValid(aGroups))
		return false;
	FAddress = aGroups;
	VLOG(3) << "Change group from " << FAddress << " to " << aGroups;
	return true;
}
void CProgramGroup::MSet(std::vector<NSHARE::CText> const& aGroups)
{
	FAddress.clear();
	if (!aGroups.empty())
	{
		std::vector<NSHARE::CText>::const_iterator _it = aGroups.begin(),
				_it_end = aGroups.end();
		for (; _it != _it_end; ++_it)
			MAddPath(*_it);
	}
	VLOG(2) << "Result=" << FAddress;
}
CProgramGroup& CProgramGroup::operator+=(NSHARE::CText const& aName)
{
	return MAddPath(aName);
}
CProgramGroup& CProgramGroup::operator+=(const char* aName)
{
	return MAddPath(aName);
}
CProgramGroup& CProgramGroup::MAddPath(NSHARE::CText const& aName)
{
	DCHECK(sMIsGroupNameValid(aName));
	if (FAddress.empty())
		FAddress = aName;
	else if (!aName.empty())
		FAddress += SEPERATOR + aName;
	else
		LOG(ERROR)<<"Cannot add empty subgroup to "<<FAddress;
	return *this;
}
CProgramGroup& CProgramGroup::MRemoveLastPath()
{
	CText::size_type const _pos = FAddress.find_last_of(SEPERATOR);
	if (_pos == CText::npos)
		FAddress.clear();
	else
	{
		CHECK_NE(_pos, 0);
		FAddress.resize(_pos);
	}
	VLOG(2) << "Result:" << FAddress;
	return *this;
}
NSHARE::CText const& CProgramGroup::MGetRaw() const
{
	return FAddress;
}
std::vector<NSHARE::CText> CProgramGroup::MGetPath() const
{

	std::vector<NSHARE::CText> _rval;

	if (!FAddress.empty())
	{
		NSHARE::CText::size_type _begin = 0;
		do
		{
			NSHARE::CText::size_type const _pos_val = FAddress.find_first_of(
					SEPERATOR, _begin);
			_rval.push_back(
					FAddress.substr(_begin,
							_pos_val == NSHARE::CText::npos ?
									NSHARE::CText::npos : _pos_val - _begin));
			_begin =
					_pos_val == NSHARE::CText::npos ?
							NSHARE::CText::npos : _pos_val + 1;
		} while (_begin != NSHARE::CText::npos);
	}
	return _rval;
}
inline unsigned CProgramGroup::sMIsSubGroupOf(CText const& aWhat,CText const& aWith)
{
	CText::size_type  _group_pos = aWhat.find(CProgramName::SEPERATOR);

	if (_group_pos == CText::npos) //no group
		_group_pos=0;
	else
		++_group_pos; ///skips SEPERATOR

	if(aWith.empty())
		return aWhat.empty()?1:std::numeric_limits<unsigned>::max();//empty group is max

	CText::size_type const _size_gr= aWhat.length()-_group_pos;
	if(aWhat.empty() || _size_gr < aWith.length())//too small
		return 0;

	size_t const _end_of_group=_group_pos + aWith.length();
	bool const _is_equal=aWhat.compare(_group_pos,aWith.length(),aWith)==0&&//
			(_size_gr == aWith.length()
								|| aWhat[_end_of_group]
										== CProgramGroup::SEPERATOR);
	if (_is_equal)
	{
		size_t _level = 1; //minimal level 1

		for (size_t i = _end_of_group; i < aWhat.length(); ++i)
			if (aWhat[i] == CProgramGroup::SEPERATOR)
				++_level;
		return (unsigned)_level;
	}
	else
		return 0;
}
unsigned CProgramGroup::MIsSubGroupOf(CText const& aName) const
{
	return sMIsSubGroupOf(aName,MGetRaw());
}
unsigned CProgramGroup::MIsSubGroupOf(CProgramName const& aName) const
{
	return MIsSubGroupOf(aName.MGetRawName());
}
unsigned CProgramGroup::MIsSubGroupOf(CProgramGroup const& aVal) const
{
	return MIsSubGroupOf(aVal.MGetRaw());
}
unsigned CProgramGroup::MIsInGroup(NSHARE::CText const& aRawGroup) const
{
	return sMIsSubGroupOf(MGetRaw(),aRawGroup);
/*	if (aRawGroup.empty())
		return true;

	if (MGetRaw().empty() || MGetRaw().length() < aRawGroup.length())
		return false;
	return MGetRaw().compare(0, aRawGroup.length(), aRawGroup) == 0 && //
			(MGetRaw().length() == aRawGroup.length()
					|| MGetRaw()[aRawGroup.length()] == CProgramGroup::SEPERATOR);*/

}
unsigned CProgramGroup::MIsInGroup(CProgramGroup const& aVal) const
{
	return sMIsSubGroupOf(MGetRaw(),aVal.MGetRaw());
}
unsigned CProgramGroup::MIsInGroup(CProgramName const& aReg) const
{
	return sMIsSubGroupOf(MGetRaw(),aReg.MGetRawName());
}

bool CProgramGroup::MIsEmpty() const
{
	return FAddress.empty();
}
bool CProgramGroup::operator<(const CProgramGroup&aRht) const
{
	return std::less<NSHARE::CText>()(FAddress, aRht.FAddress);
}
bool CProgramGroup::operator==(const CProgramGroup& aRht) const
{
	return std::equal_to<CText>()(FAddress, aRht.FAddress);
}

bool CProgramName::sMIsValid(NSHARE::CText const& aName)
{
	if (aName.empty())
		return true;
	if (/**aName.begin() == CAddress::SEPERATOR || // ".some"*/
	*(--aName.end()) == CProgramGroup::SEPERATOR || //"s@some."
			*(--aName.end()) == CProgramName::SEPERATOR || //some@
			(*aName.begin() == CProgramName::SEPERATOR && aName.size() == 1) || // "@"
			(*aName.begin() == CProgramName::SEPERATOR
					&& aName[1] == CProgramGroup::SEPERATOR) //"@.some"
			)
		return false;

	CText::const_iterator _it = aName.begin(), _it_end(aName.end());

	bool _is_name_sep = false;

	CText::const_iterator _prev_gr_it = _it;
	--_prev_gr_it;

	for (; _it != _it_end; ++_it)
		if (*_it == CProgramName::SEPERATOR)
		{
			if (_is_name_sep) //"some@some@"
				break;
			else
			{
				_is_name_sep = true;
				if (*(_it + 1) == CProgramGroup::SEPERATOR) //some@.some
					break;
			}
		}
		else if (*_it == CProgramGroup::SEPERATOR)
		{
			if (_is_name_sep)
			{
				if ((_it - _prev_gr_it) == 1) //"some..some"
					break;
				_prev_gr_it = _it;
			}
		}
	return _it == _it_end;
}
bool CProgramGroup::sMIsValid(NSHARE::CText const& aName)
{
	if (aName.empty())
		return true;
	if (*aName.begin() == CProgramGroup::SEPERATOR || // ".some"
			*(--aName.end()) == CProgramGroup::SEPERATOR || //"some."
			*(--aName.end()) == CProgramName::SEPERATOR || //
			(*aName.begin() == CProgramName::SEPERATOR && aName.size() == 1) || // "@"
			(*aName.begin() == CProgramName::SEPERATOR
					&& aName[1] == CProgramGroup::SEPERATOR) //"@.some"
			)
		return false;
	const NSHARE::CText _double(2, SEPERATOR); //..

	return aName.find(_double) == CText::npos
			&& aName.find(CProgramName::SEPERATOR) == CText::npos;
}
bool CProgramName::sMIsNameValid(NSHARE::CText const& aName)
{
	CText::const_iterator _it = aName.begin(), _it_end(aName.end());

	for (; _it != _it_end && //
			*_it != SEPERATOR; ++_it)
		;
	return _it == _it_end;
}
bool CProgramGroup::sMIsGroupNameValid(NSHARE::CText const& aName)
{
	CText::const_iterator _it = aName.begin(), _it_end(aName.end());

	for (; _it != _it_end && //
			*_it != CProgramName::SEPERATOR && //
			*_it != CProgramGroup::SEPERATOR; ++_it)
		;
	return _it == _it_end;
}
bool CProgramName::sMUnitTest()
{
	{
		CProgramName const _m("raw@q.w.e.r.t");
		CProgramGroup const _g("q.w.e.r.t");
		CProgramName const _m2("raw", _g);
		CHECK(_m == _m2);

		CProgramGroup _g2;
		_g2.MAddPath("q").MAddPath("w").MAddPath("e").MAddPath("r");
		_g2 += "t";
		CHECK(_g2 == _g);

		CHECK(_m.MIsFrom(_g));
		CHECK(_g.MIsSubGroupOf(_m));
		_g2.MRemoveLastPath();

		CHECK(_g.MIsInGroup(_g2));
		CHECK(_g2.MIsSubGroupOf(_g));
		_g2 += "f";
		CHECK(!_g.MIsInGroup(_g2));
		CHECK(!_m.MIsFrom(_g2));


		CHECK(_m.MIsFrom(CProgramGroup()));

		CProgramName const _m3("@q.w.e.r.t");
		CHECK(_m3.MIsFrom(_g));

		CProgramName const _m4("raw",CProgramGroup("q.w.e.r"));
		CHECK_EQ(_m4.MIsForMe(_m),2);
		CHECK_EQ(_m.MIsMe(_m4),2);
		CHECK_EQ(_m.MIsMe(_m2),1);

		CProgramName const _m5("raw");
		CHECK_EQ(_m.MIsMe(_m5),std::numeric_limits<unsigned>::max());

		CProgramName const _m6("raw@q");
		CHECK_EQ(_m.MIsMe(_m6),5);

		CProgramName const _m7("raw@q.w.e.r.t.y");
		CHECK_EQ(_m.MIsMe(_m7),0);
	}
	return true;
}
}

