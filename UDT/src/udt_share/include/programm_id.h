/*
 * programm_id.h
 *
 *  Created on: 17.12.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 *	Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef PROGRAMM_ID_H_
#define PROGRAMM_ID_H_

#include <udt_share_macros.h>

#ifdef uuid_t
#error "Fucking programmer"
#endif
namespace NUDT
{
struct UDT_SHARE_EXPORT id_t
{
	NSHARE::uuid_t FUuid;
	NSHARE::CText FName;

	id_t():FUuid(0)
	{

	}
	explicit id_t(NSHARE::uuid_t const& aVal):FUuid(aVal)
	{

	}

	bool operator==(id_t const& aRht) const
	{
		return FUuid == aRht.FUuid;
	}
	bool operator!=(id_t const& aRht) const
	{
		return !operator==(aRht);
	}

	bool operator<(id_t const& aRht) const
	{
		return FUuid.FVal < aRht.FUuid.FVal;
	}
#ifdef SHARE_CONFIG_DEFINED
	static const NSHARE::CText NAME;
	static const NSHARE::CText KEY_NAME;
	
	id_t(NSHARE::CConfig const& aConf):FUuid(aConf.MChild(NSHARE::uuid_t::NAME))
	{
		aConf.MGetIfSet(KEY_NAME, FName);
	}
	NSHARE::CConfig MSerialize() const
	{
		NSHARE::CConfig _conf(NAME);
		_conf.MAdd(FUuid.MSerialize());
		_conf.MSet(KEY_NAME, FName);
		return _conf;
	}
	bool MIsValid()const{
		return !FName.empty()|| FUuid.MIsValid();
	}
#endif

};
enum eType
{
	E_KERNEL = 1, //
	E_CONSUMER = 2, //
	//E_GUI = 3, //todo front end
	//todo gui
};
struct UDT_SHARE_EXPORT program_id_t
{

	id_t FId;
	NSHARE::version_t FKernelVersion;
	unsigned FTime;
	uint64_t FPid;
	NSHARE::CText FPath;
	eType FType;
	NSHARE::eEndian FEndian;

	program_id_t():
		FTime(0),
		FPid(0),
		FType (E_KERNEL),
		FEndian(NSHARE::E_SHARE_ENDIAN)
	{
	}
#ifdef SHARE_CONFIG_DEFINED
	static const NSHARE::CText NAME;
	static const NSHARE::CText KEY_TIME;
	static const NSHARE::CText KEY_PID;
	static const NSHARE::CText KEY_PATH;
	static const NSHARE::CText KEY_TYPE;
	static const NSHARE::CText KEY_ENDIAN;
	
	program_id_t(NSHARE::CConfig const& aConf):
		FId(aConf.MChild(id_t::NAME)),//
		FKernelVersion(aConf.MChild(NSHARE::version_t::NAME)),//
		FTime(0),//
		FPid(0)//
	{
		aConf.MGetIfSet(KEY_TIME, FTime);
		aConf.MGetIfSet(KEY_PID, FPid);
		aConf.MGetIfSet(KEY_PATH, FPath);
		FType=static_cast<eType>(aConf.MValue(KEY_TYPE, 0));
		FEndian=static_cast<NSHARE::eEndian>(aConf.MValue<unsigned>(KEY_ENDIAN, NSHARE::E_SHARE_ENDIAN));
	}
	NSHARE::CConfig MSerialize() const
	{
		NSHARE::CConfig _conf(NAME);
		_conf.MSet(KEY_TIME, FTime);
		_conf.MSet(KEY_PID, FPid);
		_conf.MAdd(KEY_PATH, FPath);
		_conf.MAdd<unsigned>(KEY_TYPE, FType);
		_conf.MAdd<unsigned>(KEY_ENDIAN, FEndian);
		_conf.MAdd(FId.MSerialize());
		_conf.MAdd(FKernelVersion.MSerialize());
		return _conf;
	}
	bool MIsValid() const
	{
		return FId.MIsValid() &&FTime &&FPid;
	}

#endif

	bool operator<(program_id_t const& aRht) const
	{
		return FId < aRht.FId;
	}
	bool operator==(program_id_t const& aRht) const
	{
		return FId == aRht.FId && FPath == aRht.FPath && FPid == aRht.FPid;
	}
	bool operator!=(program_id_t const& aRht) const
	{
		return !operator==(aRht);
	}

};


} //
namespace std
{
inline std::ostream& operator<<(std::ostream & aStream, NUDT::id_t const& aVal)
{
	aStream << "Name=" << aVal.FName << "(" << aVal.FUuid << ")";
	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream, NUDT::eType const& aVal)
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

	aStream << "Version :" << aVal.FKernelVersion << std::endl;
	aStream << "Path:" << aVal.FPath << std::endl;
	aStream << "Pid:" << aVal.FPid << std::endl;
	aStream << "Type:" << static_cast<NUDT::eType const>(aVal.FType) << std::endl;

	aStream << "Connect Time:"
			<< aVal.FTime ;

	return aStream;
}

}
#endif /* PROGRAMM_ID_H_ */
