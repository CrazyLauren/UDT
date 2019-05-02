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
const char CRegistration::SEPERATOR = '@';
const char CAddress::SEPERATOR = '.';
const NSHARE::CText CRegistration::NAME = "name";
COMPILE_ASSERT(static_cast<utf32>(CRegistration::SEPERATOR) == 64u,
		WtfInvalidSymbolAt);
COMPILE_ASSERT(static_cast<utf32>('.') == 46u, WtfInvalidSymbolPoint);
CRegistration::CRegistration()
{

}
CRegistration::CRegistration(const char* aRaw) :
		FName(aRaw)
{
	DCHECK(sMIsValid(aRaw));
}
CRegistration::CRegistration(NSHARE::CText const& aRaw) :
		FName(aRaw)
{
	DCHECK(sMIsValid(aRaw));
}
CRegistration::CRegistration(NSHARE::CText const& aName, CAddress const&aGroup) :
		FName(aName)
{
	DCHECK(sMIsValid(aName));
	MAddTo(aGroup);
}

CRegistration::CRegistration(NSHARE::CConfig const& aConf)
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
NSHARE::CConfig CRegistration::MSerialize() const
{
	return CConfig(NAME, FName);
}
bool CRegistration::MIsValid() const
{
	return !FName.empty();
}
bool CRegistration::MSetRaw(NSHARE::CText const&aName)
{
	VLOG(3) << "Change " << FName << " to " << aName;
	DCHECK(sMIsValid(aName));
	if (!sMIsValid(aName))
		return false;
	FName = aName;
	return true;
}
void CRegistration::MAddTo(CAddress const& aGroup)
{

	MSet(MGetName(), aGroup);
}
void CRegistration::MSet(NSHARE::CText const& aName, CAddress const& aGroup)
{
	DCHECK(sMIsValid(aName));
	FName = sMRegister(aName, aGroup);
}
NSHARE::CText CRegistration::sMRegister(NSHARE::CText const& aName,
		CAddress const& aGroup)
{
	DCHECK(sMIsNameValid(aName));
	VLOG(3) << aName << " added to  " << aGroup;
	if (aGroup.MIsEmpty())
		return aName;
	return aName + SEPERATOR + aGroup.MGetRaw();
}
void CRegistration::MChangeName(NSHARE::CText const& aNewName)
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
NSHARE::CText CRegistration::MGetName() const
{
	CText::size_type const _pos = FName.find(SEPERATOR);
	if (_pos == CText::npos) //no group
		return FName;
	else
		return FName.substr(0, _pos);
}
NSHARE::CText const& CRegistration::MGetRawName() const
{
	return FName;
}
NSHARE::CText CRegistration::MGetAddressText() const
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
CAddress CRegistration::MGetAddress() const
{
	const CText _gr = MGetAddressText();
	if (_gr.empty())
		return CAddress();
	else
		return CAddress(_gr);
}
bool CRegistration::MIsFrom(CAddress const& aGroup) const
{
	return aGroup.MIsSubpathOf(*this);
}
bool CRegistration::MIsFrom(NSHARE::CText const& aGroup) const
{
	const CAddress _gr = MGetAddress();
	return _gr.MIsPathOf(aGroup);
}
bool CRegistration::MIsForMe(NSHARE::CText const& aWhat) const
{
	return sMIsMeImpl(aWhat,MGetRawName());
}
bool CRegistration::sMIsMeImpl(NSHARE::CText const& aFor, NSHARE::CText const& aWhat)
{
	if (aWhat.empty() || aFor.empty())
		return false;

	if (aWhat.length() > aFor.length())
		return false;

	CText::const_iterator _it(aWhat.begin()), _it_end(aWhat.end());

	CText::const_iterator _jt(aFor.begin()), _jt_end(
			aFor.end());

	bool _is_sep = false;

	for (; _it != _it_end && //
			*_it == *_jt; ++_it, ++_jt)
		if (*_it == SEPERATOR)
			_is_sep = true;
	VLOG(5) << " IsSep=" << _is_sep << " end=" << (_it == _it_end)
						<< " jt_end=" << (_jt == _jt_end);
	if (_it != _it_end)
		return false;
	else if	( _jt == _jt_end)//full equal
		return true;
	else if(*_jt==CRegistration::SEPERATOR)//only name
		return true;
	else if (_is_sep && *_jt == CAddress::SEPERATOR) //more shorter address
		return true;
	else
		VLOG(5)<<"Next  "<<*_jt;

	return false;
}
bool CRegistration::MIsMe(NSHARE::CText const& aWhat) const
{
	return sMIsMeImpl(MGetRawName(),aWhat);
}
bool CRegistration::MIsMe(CRegistration const& aWhat) const
{
	return MIsMe(aWhat.MGetRawName());
}
bool CRegistration::MIsNameEqual(CRegistration const& aWho) const
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
bool CRegistration::MIsName() const
{
	return !FName.empty() && (*FName.begin() != SEPERATOR);
}
bool CRegistration::MIsAddress() const
{
	bool const _res = FName.find(SEPERATOR) != CText::npos;
	CHECK((!_res) || ((utf32 )SEPERATOR != *FName.end()));
	return _res;
}
bool CRegistration::operator<(const CRegistration&aRht) const
{
	return std::less<CText>()(FName, aRht.FName);
}
bool CRegistration::operator==(const CRegistration& aRht) const
{
	return std::equal_to<CText>()(FName, aRht.FName);
}

CAddress::CAddress()
{

}
CAddress::CAddress(const char* aGroup) :
		FAddress(aGroup)
{
	DCHECK(sMIsValid(aGroup));
}
CAddress::CAddress(NSHARE::CText const& aGroup) :
		FAddress(aGroup)
{
	DCHECK(sMIsValid(aGroup));
}
CAddress::CAddress(std::vector<NSHARE::CText> const& aGroups)
{
	MSet(aGroups);
}
bool CAddress::MSet(NSHARE::CText const& aGroups)
{
	DCHECK(sMIsValid(aGroups));
	if (!sMIsValid(aGroups))
		return false;
	FAddress = aGroups;
	VLOG(3) << "Change group from " << FAddress << " to " << aGroups;
	return true;
}
void CAddress::MSet(std::vector<NSHARE::CText> const& aGroups)
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
CAddress& CAddress::operator+=(NSHARE::CText const& aName)
{
	return MAddPath(aName);
}
CAddress& CAddress::operator+=(const char* aName)
{
	return MAddPath(aName);
}
CAddress& CAddress::MAddPath(NSHARE::CText const& aName)
{
	DCHECK(sMIsNameValid(aName));
	if (FAddress.empty())
		FAddress = aName;
	else if (!aName.empty())
		FAddress += SEPERATOR + aName;
	else
		LOG(ERROR)<<"Cannot add empty subgroup to "<<FAddress;
	return *this;
}
CAddress& CAddress::MRemoveLastPath()
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
NSHARE::CText const& CAddress::MGetRaw() const
{
	return FAddress;
}
std::vector<NSHARE::CText> CAddress::MGetPath() const
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
bool CAddress::MIsSubpathOfForRegistration(CText const& aName) const
{
	if (MIsEmpty())
		return true;

	CText::size_type const _pos = aName.find(CRegistration::SEPERATOR);
	if (_pos == CText::npos) //no group
		return false;
	CText::size_type const _size_gr= aName.length()-_pos-1;
	if(_size_gr < MGetRaw().length())
		return false;

	return aName.compare(_pos+1,MGetRaw().length(),MGetRaw())==0 &&//
			(_size_gr == MGetRaw().length()
					|| aName[_pos + 1 + MGetRaw().length()]
							== CAddress::SEPERATOR);
}
bool CAddress::MIsSubpathOf(CRegistration const& aName) const
{
	if (MIsEmpty())
		return true;
	return MIsSubpathOfForRegistration(aName.MGetRawName());
}
bool CAddress::MIsSubpathOf(CAddress const& aVal) const
{
	return MIsSubpathOf(aVal.MGetRaw());
}
bool CAddress::MIsSubpathOf(CText const& _gr) const
{
	if (MIsEmpty())
		return true;

	if (_gr.empty() || _gr.length() < MGetRaw().length())
		return false;
	return _gr.compare(0, MGetRaw().length(), MGetRaw()) == 0 &&//
			(_gr.length() == MGetRaw().length()
					|| _gr[MGetRaw().length()] == CAddress::SEPERATOR);
}
bool CAddress::MIsPathOf(NSHARE::CText const& aRawGroup) const
{
	if (aRawGroup.empty())
		return true;

	if (MGetRaw().empty() || MGetRaw().length() < aRawGroup.length())
		return false;
	return MGetRaw().compare(0, aRawGroup.length(), aRawGroup) == 0 && //
			(MGetRaw().length() == aRawGroup.length()
					|| MGetRaw()[aRawGroup.length()] == CAddress::SEPERATOR);

}
bool CAddress::MIsPathOf(CAddress const& aVal) const
{
	return MIsPathOf(aVal.MGetRaw());
}
bool CAddress::MIsPathOf(CRegistration const& aReg) const
{
	return MIsPathOfForRegistration(aReg.MGetRawName());
}
bool CAddress::MIsPathOfForRegistration(CText const& aName) const
{
	if (aName.empty())
		return true;

	CText::size_type const _pos = aName.find(CRegistration::SEPERATOR);
	if (_pos == CText::npos) //no group
		return true;
	CText::size_type const _size_gr= aName.length()-_pos-1;
	if(MGetRaw().length()<_size_gr)
		return false;
	return MGetRaw().compare(0, _size_gr, aName, _pos + 1, _size_gr) == 0 && //
			(_size_gr == MGetRaw().length()
					|| MGetRaw()[_pos + 1 + _size_gr] == CAddress::SEPERATOR);
}
bool CAddress::MIsEmpty() const
{
	return FAddress.empty();
}
bool CAddress::operator<(const CAddress&aRht) const
{
	return std::less<NSHARE::CText>()(FAddress, aRht.FAddress);
}
bool CAddress::operator==(const CAddress& aRht) const
{
	return std::equal_to<CText>()(FAddress, aRht.FAddress);
}

bool CRegistration::sMIsValid(NSHARE::CText const& aName)
{
	if (aName.empty())
		return true;
	if (/**aName.begin() == CAddress::SEPERATOR || // ".some"*/
	*(--aName.end()) == CAddress::SEPERATOR || //"s@some."
			*(--aName.end()) == CRegistration::SEPERATOR || //some@
			(*aName.begin() == CRegistration::SEPERATOR && aName.size() == 1) || // "@"
			(*aName.begin() == CRegistration::SEPERATOR
					&& aName[1] == CAddress::SEPERATOR) //"@.some"
			)
		return false;

	CText::const_iterator _it = aName.begin(), _it_end(aName.end());

	bool _is_name_sep = false;

	CText::const_iterator _prev_gr_it = _it;
	--_prev_gr_it;

	for (; _it != _it_end; ++_it)
		if (*_it == CRegistration::SEPERATOR)
		{
			if (_is_name_sep) //"some@some@"
				break;
			else
			{
				_is_name_sep = true;
				if (*(_it + 1) == CAddress::SEPERATOR) //some@.some
					break;
			}
		}
		else if (*_it == CAddress::SEPERATOR)
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
bool CAddress::sMIsValid(NSHARE::CText const& aName)
{
	if (aName.empty())
		return true;
	if (*aName.begin() == CAddress::SEPERATOR || // ".some"
			*(--aName.end()) == CAddress::SEPERATOR || //"some."
			*(--aName.end()) == CRegistration::SEPERATOR || //
			(*aName.begin() == CRegistration::SEPERATOR && aName.size() == 1) || // "@"
			(*aName.begin() == CRegistration::SEPERATOR
					&& aName[1] == CAddress::SEPERATOR) //"@.some"
			)
		return false;
	const NSHARE::CText _double(2, SEPERATOR); //..

	return aName.find(_double) == CText::npos
			&& aName.find(CRegistration::SEPERATOR) == CText::npos;
}
bool CRegistration::sMIsNameValid(NSHARE::CText const& aName)
{
	CText::const_iterator _it = aName.begin(), _it_end(aName.end());

	for (; _it != _it_end && //
			*_it != SEPERATOR; ++_it)
		;
	return _it == _it_end;
}
bool CAddress::sMIsNameValid(NSHARE::CText const& aName)
{
	CText::const_iterator _it = aName.begin(), _it_end(aName.end());

	for (; _it != _it_end && //
			*_it != CRegistration::SEPERATOR && //
			*_it != CAddress::SEPERATOR; ++_it)
		;
	return _it == _it_end;
}
bool CRegistration::sMUnitTest()
{
	{
		CRegistration const _m("raw@q.w.e.r.t");
		CAddress const _g("q.w.e.r.t");
		CRegistration const _m2("raw", _g);
		CHECK(_m == _m2);

		CAddress _g2;
		_g2.MAddPath("q").MAddPath("w").MAddPath("e").MAddPath("r");
		_g2 += "t";
		CHECK(_g2 == _g);

		CHECK(_m.MIsFrom(_g));
		CHECK(_g.MIsSubpathOf(_m));
		_g2.MRemoveLastPath();

		CHECK(_g.MIsPathOf(_g2));
		_g2 += "f";
		CHECK(!_g.MIsPathOf(_g2));
		CHECK(!_m.MIsFrom(_g2));

		CHECK(_m.MIsFrom(CAddress()));

		CRegistration const _m3("@q.w.e.r.t");
		CHECK(_m3.MIsFrom(_g));
	}
	return true;
}
}

