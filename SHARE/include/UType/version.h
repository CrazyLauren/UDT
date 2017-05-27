/*
 * version.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 22.03.2013
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef VERSION_H_
#define VERSION_H_

namespace NSHARE
{
#ifdef SHARE_CONFIG_DEFINED
#define	SHARED_CONFIG_MSVC_FIX NSHARE::CConfig
#else
#define	SHARED_CONFIG_MSVC_FIX int
#endif
/** \brief Класс для хранения версии в формате
 * (Страшая версия.Младшая версия.Номер ревизии или хэш репозит.)
 *
 */
SHARED_PACKED(struct SHARE_EXPORT version_t
{
	static const NSHARE::CText NAME;

	uint8_t FMajor;//<! - Major version
	uint8_t FMinor;//<! - Minor version
	mutable uint16_t FRelease;//<! - Revision(SVN)  or repository Hash(Git)


	version_t(uint8_t aMajor=0,uint8_t aMinor=0,uint16_t aRelease=0);

	inline bool MIsExist() const;
	/** \brief Проверяет совместимость версии aVer с текущей
	 *
	 *	\param aVer - проверяемая версия
	 */
	inline bool MIsCompatibleWith(version_t const& aVer) const;
	inline bool operator==(version_t const& aRht) const;
	inline bool operator!=(version_t const& aRht) const;
	version_t( SHARED_CONFIG_MSVC_FIX const& aConf);
	SHARED_CONFIG_MSVC_FIX MSerialize() const;
});

inline bool  version_t::MIsExist() const
{
	return FMajor!=0 || FMinor!=0 || FRelease!=0;
}
inline version_t::version_t(uint8_t aMajor,uint8_t aMinor,uint16_t aRelease):
	FMajor(aMajor),
	FMinor(aMinor),
	FRelease(aRelease)
{
}
inline bool version_t::operator==(version_t const& aRht) const
{
	return FMajor==aRht.FMajor&&FMinor==aRht.FMinor&&FRelease==aRht.FRelease;
}
inline bool version_t::operator!=(version_t const& aRht) const
{
	return !operator==(aRht);
}

inline bool version_t::MIsCompatibleWith(version_t const& aVal) const
{
	return FMajor == aVal.FMajor && FMinor <= aVal.FMinor;
}
#ifdef SHARE_CONFIG_DEFINED
inline version_t::version_t(NSHARE::CConfig const& aConf):
			FMajor(0),
			FMinor(0),
			FRelease(0)
{
	FMajor=aConf.MValue<unsigned>("mj", 0);
	FMinor=aConf.MValue<unsigned>("mn", 0);
	FRelease=aConf.MValue<unsigned>("rs",0);
}
inline NSHARE::CConfig version_t::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	if(MIsExist())
	{
		_conf.MSet<unsigned>("mj", FMajor);
		_conf.MSet<unsigned>("mn", FMinor);
		_conf.MSet<unsigned>("rs", FRelease);
	}
	return _conf;
}
#endif
} //namespace USHARE
namespace std
{
inline std::ostream& operator<<(std::ostream & aStream,
		const NSHARE::version_t& aVersion)
{
	aStream << (int) aVersion.FMajor << '.'<< (int) aVersion.FMinor << '.'
			<< (int) aVersion.FRelease;
	return aStream;
}
inline std::istream& operator>>(std::istream& aStream,
		NSHARE::version_t& aVersion) //TODO
{
	char _c;
	 aStream>>aVersion.FMajor;
	if (aStream.fail() || aStream.eof())
	{
		aStream.setstate(std::istream::failbit);
		return aStream;
	}
	 aStream>>_c;
	if (_c != '.')
	{
		aStream.setstate(std::istream::failbit);
		return aStream;
	}
	aStream>>aVersion.FMinor;
	if (aStream.fail() || aStream.eof())
	{
		aStream.setstate(std::istream::failbit);
		return aStream;
	}
	_c = 0;
	aStream>>_c;
	if (_c != '.')
	{
		aStream.setstate(std::istream::failbit);
		return aStream;
	}
	unsigned _val=0;
	aStream>>_val;
	if (aStream.fail())
	{
		aStream.setstate(std::istream::failbit);
		return aStream;
	}
	aVersion.FRelease=_val;
	return aStream;
}
}
#endif /* VERSION_H_ */
