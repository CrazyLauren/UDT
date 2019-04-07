/*
 * CAddress.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 12.09.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CADDRESS_H_
#define CADDRESS_H_

namespace NSHARE
{
class  CRegistration;


/**\brief имя домена записанного слева направо (ru.kremlin)
 *
 */
class SHARE_EXPORT CAddress
{

public:
	static const char SEPERATOR;
	CAddress();

	/**\brief constructor
	 *
	 *\param aAddress address in format: a.b.c
	 */
	explicit CAddress(const char* aAddress);

	/**\brief constructor
	 *
	 *\param aAddress address in format: a.b.c
	 */
	explicit CAddress(NSHARE::CText const& aAddress);

	/**\brief constructor
	 *
	 *\param aGroups list of domain
	 */
	explicit CAddress(std::vector<NSHARE::CText> const& aGroups);

	bool MSet(NSHARE::CText const&);
	void MSet(std::vector<NSHARE::CText> const& aGroups);

	CAddress& operator+=(NSHARE::CText const& aName);
	CAddress& operator+=(const char* aName);
	CAddress& MAddPath(NSHARE::CText const& aName);
	CAddress& MRemoveLastPath();

	NSHARE::CText const& MGetRaw()const;
	std::vector<NSHARE::CText>  MGetPath() const;


<<<<<<< HEAD
	bool MIsSubpathOf(CRegistration const&) const;
	bool MIsSubpathOfForRegistration(CText const&) const;

	bool MIsSubpathOf(CAddress const&)const;
	bool MIsSubpathOf(NSHARE::CText const& aPath)const;//true if aPath==q.w.r and FAddress==q.w

	bool MIsPathOf(NSHARE::CText const& aSubPath)const;//true if aSubPath==q.w and FAddress==q.w.r
	bool MIsPathOf(CAddress const&)const;
	bool MIsPathOf(CRegistration const&) const;
	bool MIsPathOfForRegistration(CText const&) const;

=======
	/*!\brief Returns if The name is in my group
	 *
	 * Returns 2 if aName==q.w and my group is q.w.r
	 *
	 *
	 *\param aName - the program name or group
	 *
	 *\note if the group empty then returns unsigned max
	 *
	 *\return 0 - if no
	 *         > 0 the group of aName is a part of my group,
	 * 		   		 the return value is equal
	 * 		   		 the level of aName subgroup in
	 * 		   		 my group.
	 * 		   		 for detail see sMIsSubGroupOf
	 *
	 */
	unsigned MIsSubGroupOf(CText const& aName) const;

	/*!\brief Returns if The name is in my group
	 *
	 * Returns 2 if aName==q.w and my group is q.w.r.
	 *
	 *\param aName - the program name
	 *\note if the group empty then returns unsigned max
	 *
	 *\return 0 - if no
	 *         > 0 the group of aName is a part of my group,
	 * 		   		 the return value is equal
	 * 		   		 the level of aName subgroup in
	 * 		   		 my group.
	 * 		   		 for detail see sMIsSubGroupOf
	 *
	 */
	unsigned MIsSubGroupOf(CProgramName const& aName) const;

	/*!\brief Returns if The my group is subgroup of aGroup
	 *
	 * Returns 2 if aName==q.w and my group is q.w.r
	 *
	 *\param aGroup - the group
	 *
	 *\return 0 - if no
	 *         > 0 the group of aGroup is a part of my group,
	 * 		   		 the return value is equal
	 * 		   		 the level of aGroup subgroup in
	 * 		   		 my group.
	 * 		   		 for detail see sMIsSubGroupOf
	 */
	unsigned MIsSubGroupOf(CProgramGroup const& aGroup)const;

	/*!\brief Check for is in group
	 *
	 * Returns 2 if aGroup==q.w.r and my group is q.w
	 *
	 *\param aGroup - the group
	 *
	 *\return 0 - if no
	 *         > 0 if in, for detail see sMIsSubGroupOf
	 */
	unsigned MIsInGroup(NSHARE::CText const& aGroup)const;

	/*!\brief Check for is in group
	 *
	 * Returns 2 if aGroup==q.w.r and my group is q.w
	 *
	 *\param aGroup - the group
	 *
	 *\return 0 - if no
	 *         > 0 if in, for detail see sMIsSubGroupOf
	 */
	unsigned MIsInGroup(CProgramGroup const& aGroup)const;

	/*!\brief Check for is in group
	 *
	 * Returns 2 if aGroup==q.w.r and my group is q.w
	 *
	 *\param aName - the group
	 *
	 *\return 0 - if no
	 *         > 0 if in, for detail see sMIsSubGroupOf
	 */
	unsigned MIsInGroup(CProgramName const& aName) const;

	/*!\brief Returns if group is exist
	 *
	 *\return true if group is exist.
	 */
>>>>>>> f3da2cc... see changelog.txt
	bool MIsEmpty()const;
	static bool sMIsValid(NSHARE::CText const&);
	static bool sMIsNameValid(NSHARE::CText const&);

<<<<<<< HEAD
	bool operator<(const CAddress&) const;
	bool operator==(const CAddress&) const;

private:
=======
	/*!\brief Checks for the group validity
	 *
	 *\param aGroup the group
	 *
	 *\return true if valid
	 */
	static bool sMIsValid(NSHARE::CText const& aGroup);

	/*!\brief Checks for group name validity
	 *
	 * The group name does not have characters  CProgramName::SEPERATOR and
	 * CProgramGroup::SEPERATOR.
	 *
	 *\param aGroup group
	 *
	 *\return true if valid
	 */
	static bool sMIsGroupNameValid(NSHARE::CText const&);

	/*!\brief Fast group compare
	 *
	 *\return true if less
	 */
	bool operator<(const CProgramGroup&) const;

	/*!\brief Fast group compare
	 *
	 *\return true if equal
	 */
	bool operator==(const CProgramGroup&) const;

private:

	/*!\brief Check for subgroup
	 *
	 * Return > 0 if aWhat==q.w.r and aWith==q.w
	 *
	 *\param aWith - the group with which is compared
	 *\param aName - the program name or group which is checked
	 *
	 *\note if the aWith is empty then returns unsigned max
	 *
	 *\return 0 - if is not subgroup
	 *         > 0 the group of aWhat is a part of aWith group,
	 * 		   		 the return value is equal
	 * 		   		 the level of aWhat subgroup in
	 * 		   		 aWith.
	 * 		   		 for detail see CRegistration::sMCompare
	 *
	 */
	static unsigned sMIsSubGroupOf(CText const& aWhat,CText const& aWith);
>>>>>>> f3da2cc... see changelog.txt
	NSHARE::CText FAddress;
};

/**\brief идентификатор ПО наподобие почты (program@ru.kremlin)
 *
 */
class  SHARE_EXPORT CRegistration
{

public:
	static const NSHARE::CText NAME;
	static const char SEPERATOR;

	CRegistration();

	/**\brief constructor
	 *
	 *\param aAddress address in format: xz@a.b.c
	 */
	explicit CRegistration(const char*);

	/**\brief constructor
	 *
	 *\param aAddress address in format: xz@a.b.c
	 */
	explicit CRegistration(NSHARE::CText const&);

	/**\brief constructor
	 *
	 *\param aName name
	 *\param aGroup domains
	 */
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
<<<<<<< HEAD
	CAddress MGetAddress() const;

	bool MIsFrom(CAddress const&) const;
	bool MIsFrom(NSHARE::CText const&) const;

	bool MIsMe(CRegistration const&) const;
	bool MIsMe(NSHARE::CText const&) const;
	bool MIsForMe(NSHARE::CText const&) const;
=======
	CProgramGroup MGetAddress() const;

	/*!\brief Check for is in group
	 *
	 *\param aGroup - the group
	 *
	 *\return 0 - if no
	 *         > 0 if in, for detail see CProgramGroup::sMIsSubGroupOf
	 */
	unsigned MIsFrom(CProgramGroup const& aGroup) const;

	/*!\brief Check for is in group
	 *
	 *\param aName - the group
	 *
	 *\return 0 - if no
	 *         > 0 if in, for detail see CProgramGroup::sMIsSubGroupOf
	 */
	unsigned MIsFrom(NSHARE::CText const& aGroup) const;

	/*!\brief Checks for my Address is a part of the other address
	 *
	 *\param aWith the Address with which is compared
	 *
	 *
	 *\return see sMCompare
	 */
	unsigned MIsMe(CProgramName const& aWith) const;

	/*!\brief Checks for my Address is a part of the other address
	 *
	 *\param aWith the Address with which is compared
	 *
	 *
	 *\return see sMCompare
	 */
	unsigned MIsMe(NSHARE::CText const& aWith) const;


	/*!\brief Checks for the Address is a part of my address
	 *
	 *\param aWhat the compared Address
	 *
	 *
	 *\return see sMCompare
	 */
	unsigned MIsForMe(NSHARE::CText const& aWhat) const;

	/*!\brief Checks for the Address is a part of my address
	 *
	 *\param aWhat the compared Address
	 *
	 *
	 *\return see sMCompare
	 */
	unsigned MIsForMe(CProgramName const& aWhat) const;
>>>>>>> f3da2cc... see changelog.txt


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
<<<<<<< HEAD
	static bool sMIsMeImpl(NSHARE::CText const&,NSHARE::CText const&) ;
=======
	/*!\brief Checks for the Address is a part of the other address
	 *
	 *\param aWhat - the compared Address
	 *\param aWith - the Address with which aWhat is compared
	 *\return 0 - if no
	 * 		   > 0 - aWhat is a part of aWith,
	 * 		   		 the return value is equal
	 * 		   		 the level of aWhat subgroup in
	 * 		   		 aWith.
	 * 		   1 - if the addresses are equal
	 * 		   2 - if for example aWhat == a@x
	 * 		   		  aWith == a@x.y
	 * 		   etc.
	 * 		   unsigned max - if aWith does not have group
	 * 		   				  aWhat have group
	 */
	static unsigned sMCompare(NSHARE::CText const& aWhat,NSHARE::CText const& aWith) ;
>>>>>>> f3da2cc... see changelog.txt
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
