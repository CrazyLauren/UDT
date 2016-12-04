/*
 * CAddress.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 12.09.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CADDRESS_H_
#define CADDRESS_H_

namespace NSHARE
{
class  CRegistration;
class SHARE_EXPORT CAddress
{

public:
	static const char SEPERATOR;
	CAddress();
	explicit CAddress(const char*);//raw name
	explicit CAddress(NSHARE::CText const&);//raw name
	explicit CAddress(std::vector<NSHARE::CText> const& aGroups);

	bool MSet(NSHARE::CText const&);
	void MSet(std::vector<NSHARE::CText> const& aGroups);

	CAddress& operator+=(NSHARE::CText const& aName);
	CAddress& operator+=(const char* aName);
	CAddress& MAddPath(NSHARE::CText const& aName);
	CAddress& MRemoveLastPath();

	NSHARE::CText const& MGetRaw()const;
	std::vector<NSHARE::CText>  MGetPath() const;


	bool MIsSubpathOf(CRegistration const&) const;
	bool MIsSubpathOfForRegistration(CText const&) const;

	bool MIsSubpathOf(CAddress const&)const;
	bool MIsSubpathOf(NSHARE::CText const& aPath)const;//true if aPath==q.w.r and FAddress==q.w

	bool MIsPathOf(NSHARE::CText const& aSubPath)const;//true if aSubPath==q.w and FAddress==q.w.r
	bool MIsPathOf(CAddress const&)const;
	bool MIsPathOf(CRegistration const&) const;
	bool MIsPathOfForRegistration(CText const&) const;

	bool MIsEmpty()const;
	static bool sMIsValid(NSHARE::CText const&);
	static bool sMIsNameValid(NSHARE::CText const&);

	bool operator<(const CAddress&) const;
	bool operator==(const CAddress&) const;

private:
	NSHARE::CText FAddress;
};
class  SHARE_EXPORT CRegistration
{

public:
	static const NSHARE::CText NAME;
	static const char SEPERATOR;
	CRegistration();
	explicit CRegistration(const char*);//raw name
	explicit CRegistration(NSHARE::CText const&);//raw name
	explicit CRegistration(NSHARE::CText const& aName,CAddress const&aGroup);
	explicit CRegistration(NSHARE::CConfig const& aConf);
	NSHARE::CConfig MSerialize() const;
	bool MIsValid()const;

	bool MSetRaw(NSHARE::CText const&);
	void MChangeName(NSHARE::CText const&);
	void MAddTo(CAddress const&);
	void MSet(NSHARE::CText const& aName,CAddress const&);


	NSHARE::CText MGetName()const;
	NSHARE::CText const& MGetRawName()const;
	NSHARE::CText MGetAddressText()const;
	CAddress MGetAddress() const;

	bool MIsFrom(CAddress const&) const;
	bool MIsFrom(NSHARE::CText const&) const;

	bool MIsMe(CRegistration const&) const;
	bool MIsMe(NSHARE::CText const&) const;
	bool MIsForMe(NSHARE::CText const&) const;


	bool MIsName()const;
	bool MIsAddress()const;
	bool MIsNameEqual(CRegistration const&) const;


	static NSHARE::CText sMRegister(NSHARE::CText const& aName,CAddress const&);
	static bool sMIsValid(NSHARE::CText const&);
	static bool sMIsNameValid(NSHARE::CText const&);
	static bool sMUnitTest();

	bool operator<(const CRegistration&) const;
	bool operator==(const CRegistration&) const;

private:
	static bool sMIsMeImpl(NSHARE::CText const&,NSHARE::CText const&) ;
	NSHARE::CText FName;
};
}
namespace std
{
inline std::ostream& operator <<(std::ostream& aStream, const NSHARE::CRegistration& aBase)
{
	return aStream << aBase.MGetRawName();
}
inline std::ostream& operator <<(std::ostream& aStream, const NSHARE::CAddress& aBase)
{
	return aStream << aBase.MGetRaw();
}
}
#endif /* CADDRESS_H_ */