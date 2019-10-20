/*
 * programm_id.h
 *
 *  Created on: 17.12.2015
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef PROGRAMM_ID_H_
#define PROGRAMM_ID_H_

#include "udt_share_macros.h"

#ifdef uuid_t
#error "Fucking programmer"
#endif
namespace NUDT
{
/*!\brief A program identifier
 *
 * Any program has its identifier which consisting from
 * an non-unique name and a unique UUID.
 * The non-unique name consists from non-unique
 *	name which is defined as string is made up of a lowercase
 *	name ( Latin letters a to z, digits 0 to 9,
 *	!#$%&'+-/=?^_`{|}~), an @ symbol,
 *	and group which is defined using a hierarchical
 *	naming pattern, with some levels in the hierarchy
 *	separated by periods (."dot").
 *	For example:
 *		name@group1,
 *		name,
 *		name@group1.group2.
 */
struct UDT_SHARE_EXPORT id_t
{
	NSHARE::uuid_t FUuid;///< An unique program ID
	NSHARE::CText FName;///< An program name

    /*!\brief The default constructor creates fields using their
     * respective default constructors.
     */
	id_t();

	/*!\brief The UUID may be passed to a constructor to be copied.
	 *
	 *\param aVal UUID
	 */
	explicit id_t(NSHARE::uuid_t const& aVal);

	/*!\brief Two @c id_t are equal if UUID are equal.
	 *
	 *\param aRht to compare object with
	 *\return true if the objects are equal
	 */
	bool operator==(id_t const& aRht) const;

	/*!\brief Two @c id_t are equal if UUID aren't equal.
	 *
	 *\param aRht to compare object with
	 *\return true if the objects aren't equal
	 */
	bool operator!=(id_t const& aRht) const;

	/*!\brief Two @c id_t are compared into some containres
	 * by its UUID only.
	 *
	 *\param aRht to compare object with
	 *\return true if the objects are less
	 */
	bool operator<(id_t const& aRht) const;
#ifdef SHARE_CONFIG_DEFINED
	static const NSHARE::CText NAME;///< A serializing key
	static const NSHARE::CText KEY_NAME;///< A key of #FName
	
	/*!\brief Deserialize object
	 *
	 * To check the result of deserialization,
	 * used the MIsValid().
	 *\param aConf Serialized object
	 */
	id_t(NSHARE::CConfig const& aConf);

	/*!\brief Serialize object
	 *
	 * The key of serialized object is #NAME
	 *
	 *\return Serialized object.
	 */
	NSHARE::CConfig MSerialize() const;

	/*!\brief Checks object for valid
	 *
	 * Usually It's used after deserializing object(see #id_t())
	 *\return true if it's valid.
	 */
	bool MIsValid()const;
#endif

};
/*!\brief A program type
 *
 */
enum eProgramType
{
	E_KERNEL = 1, ///< It's a kernel
	E_CONSUMER = 2, ///< It's a user program
	//E_GUI = 3, //todo front end
	//todo gui
};
/*!\brief An information about program
 *
 */
struct UDT_SHARE_EXPORT program_id_t
{

	id_t FId;///< A program identifier
	NSHARE::version_t FVersion;///< A program version
	unsigned FTime;///< A start time
	uint64_t FPid;///< A program process id
	NSHARE::CText FPath;///< A path to the program
	eProgramType FType;///<A type of program
	NSHARE::eEndian FEndian;///A byte order used by program

	/*!\brief The default constructor creates fields using their
	 * respective default constructors.
	 */
	program_id_t();
#ifdef SHARE_CONFIG_DEFINED
	static const NSHARE::CText NAME;///< A serializing key
	static const NSHARE::CText KEY_TIME;///< A key of #FTime
	static const NSHARE::CText KEY_PID;///< A key of #FPid
	static const NSHARE::CText KEY_PATH;///< A key of #FPath
	static const NSHARE::CText KEY_TYPE;///< A key of #FType
	static const NSHARE::CText KEY_ENDIAN;///< A key of #FEndian
	
	/*!\brief Deserialize object
	 *
	 * To check the result of deserialization,
	 * used the MIsValid().
	 *\param aConf Serialized object
	 */
	program_id_t(NSHARE::CConfig const& aConf);

	/*!\brief Serialize object
	 *
	 * The key of serialized object is #NAME
	 *
	 *\return Serialized object.
	 */
	NSHARE::CConfig MSerialize() const;

	/*!\brief Checks object for valid
	 *
	 * Usually It's used after deserializing object(see #program_id_t())
	 *\return true if it's valid.
	 */
	bool MIsValid() const;
#endif

	/*!\brief Two @c program_id_t are compared into some containers
	 * by its UUID only.
	 *
	 *\param aRht to compare object with
	 *\return true if the objects are less
	 */
	bool operator<(program_id_t const& aRht) const;

	/*!\brief Two @c program_id_t are equal if ID,
	 * path, pid are equals.
	 *
	 *\param aRht to compare object with
	 *\return true if the objects are equal
	 */
	bool operator==(program_id_t const& aRht) const;

	/*!\brief Two @c program_id_t aren't equal if aren't equal.
	 *
	 *\param aRht to compare object with
	 *\return true if the objects aren't equal
	 */
	bool operator!=(program_id_t const& aRht) const;
};


inline id_t::id_t():
	FUuid(0)
{

}
inline id_t::id_t(NSHARE::uuid_t const& aVal):FUuid(aVal)
{

}
inline bool id_t::operator==(id_t const& aRht) const
{
	return FUuid == aRht.FUuid;
}

inline bool id_t::operator!=(id_t const& aRht) const
{
	return !operator==(aRht);
}

inline bool id_t::operator<(id_t const& aRht) const
{
	return FUuid.FVal < aRht.FUuid.FVal;
}
#ifdef SHARE_CONFIG_DEFINED
inline id_t::id_t(NSHARE::CConfig const& aConf):FUuid(aConf.MChild(NSHARE::uuid_t::NAME))
{
	aConf.MGetIfSet(KEY_NAME, FName);
}
inline NSHARE::CConfig id_t::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	_conf.MAdd(FUuid.MSerialize());
	_conf.MSet(KEY_NAME, FName);
	return _conf;
}
inline bool id_t::MIsValid()const
{
	return !FName.empty()|| FUuid.MIsValid();
}
#endif

inline program_id_t::program_id_t() :
	FTime(0),
	FPid(0),
	FType (E_KERNEL),
	FEndian(NSHARE::E_SHARE_ENDIAN)
{
}
#ifdef SHARE_CONFIG_DEFINED
inline program_id_t::program_id_t(NSHARE::CConfig const& aConf):
	FId(aConf.MChild(id_t::NAME)),//
	FVersion(aConf.MChild(NSHARE::version_t::NAME)),//
	FTime(0),//
	FPid(0)//
{
	aConf.MGetIfSet(KEY_TIME, FTime);
	aConf.MGetIfSet(KEY_PID, FPid);
	aConf.MGetIfSet(KEY_PATH, FPath);
	FType=static_cast<eProgramType>(aConf.MValue(KEY_TYPE, 0));
	FEndian=static_cast<NSHARE::eEndian>(aConf.MValue<unsigned>(KEY_ENDIAN, NSHARE::E_SHARE_ENDIAN));
}
inline NSHARE::CConfig program_id_t::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	_conf.MSet(KEY_TIME, FTime);
	_conf.MSet(KEY_PID, FPid);
	_conf.MAdd(KEY_PATH, FPath);
	_conf.MAdd<unsigned>(KEY_TYPE, FType);
	_conf.MAdd<unsigned>(KEY_ENDIAN, FEndian);
	_conf.MAdd(FId.MSerialize());
	_conf.MAdd(FVersion.MSerialize());
	return _conf;
}
inline bool program_id_t::MIsValid() const
{
	return FId.MIsValid() &&FTime &&FPid;
}
#endif

inline bool program_id_t::operator<(program_id_t const& aRht) const
{
	return FId < aRht.FId;
}
inline bool program_id_t::operator==(program_id_t const& aRht) const
{
	return FId == aRht.FId && FPath == aRht.FPath && FPid == aRht.FPid;
}
inline bool program_id_t::operator!=(program_id_t const& aRht) const
{
	return !operator==(aRht);
}
} //
namespace std
{
inline std::ostream& operator<<(std::ostream & aStream, NUDT::id_t const& aVal)
{
	aStream << "Name=" << aVal.FName << "(" << aVal.FUuid << ")";
	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream, NUDT::eProgramType const& aVal)
{
	using namespace NUDT;
	switch (aVal)
	{
	case E_KERNEL:
		aStream << "Kernel";
		break;
	case E_CONSUMER:
		aStream << "consumer";
		break;
//	case E_FRONT_END:
//			aStream << "front end";
//			break;
	}
	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::program_id_t const& aVal)
{
	aStream << aVal.FId << std::endl;

	aStream << "Version :" << aVal.FVersion << std::endl;
	aStream << "Path:" << aVal.FPath << std::endl;
	aStream << "Pid:" << aVal.FPid << std::endl;
	aStream << "Type:" << static_cast<NUDT::eProgramType const>(aVal.FType) << std::endl;

	aStream << "Connect Time:"
			<< aVal.FTime ;

	return aStream;
}

}
#endif /* PROGRAMM_ID_H_ */
