/*
 * udt_types.h
 *
 *  Created on: 03.02.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef UDT_TYPES_H_
#define UDT_TYPES_H_

#include <UDT/udt_share_macros.h>
#include <UDT/config/config.h>
namespace NUDT
{
//#define UDT_MAX_MESSAGE_HEADER_SIZE 16
/*!\brief A header of the requirement message
 *
 * The size of the message header is 8 byte.
 * When the kernel is routing message its compare
 * 8 first byte of the message and the requirement
 * header for the receiver (SUBCRIBER).
 * If its are equals when it's compare its
 * version. If the versions of messages are
 * compatible (for detail see NHSARE::version_t)
 * than the kernel is routing
 * the message to the receiver (SUBCRIBER).
 *\warning The header with all zero bytes is considered
 * invalid.
 */
struct UDT_SHARE_EXPORT required_header_t
{
	NSHARE::version_t FVersion;///< A required version of the message

	/*!\brief A message header
	 *
	 */
	union
	{
		struct //header style of
		{
			uint32_t FNumber;///<Useful fields for assignment message type
		};
		uint8_t FMessageHeader[UDT_MAX_MESSAGE_HEADER_SIZE];///< A message header
	};

	/*!\brief The default constructor creates
	 * #FMessageHeader with all zero bytes
	 * to avoid incorrect messages routing.
	 */
	required_header_t();

	/*!\brief A convenience constructor for creating
	 * header from the various object and the message
	 * version.
	 *
	 * \tparam T A message header type
	 * \param aHeader The message header object
	 * \param aVer The message version
	 */
	template<typename T>
	explicit required_header_t(T const& aHeader, NSHARE::version_t const& aVer =
			NSHARE::version_t());

	/*!\brief Two @c required_header_t are equal if headers,
	 * version are equal.
	 *
	 *\param aRht to compare object with
	 *\return true if the objects are equal
	 */
	bool operator==(required_header_t const& aRht) const;

	/*!\brief Two @c required_header_t are compared into some containers
	 * by its headers only.
	 *
	 *\param aRht to compare object with
	 *\return true if the objects are less
	 */
	bool operator<(required_header_t const& aRht) const;

#ifdef SHARE_CONFIG_DEFINED
	static const NSHARE::CText NAME;///< A serializing key
	static const NSHARE::CText KEY_HEADER;///< A key of #FMessageHeader

	/*!\brief Deserialize object
	 *
	 * To check the result of deserialization,
	 * used the MIsValid().
	 *\param aConf Serialized object
	 */
	required_header_t(NSHARE::CConfig const& aConf);
	/*!\brief Serialize object
	 *
	 * The key of serialized object is #NAME
	 *
	 *\return Serialized object.
	 */
	NSHARE::CConfig MSerialize() const;

	/*!\brief Checks object for valid
	 *
	 * Usually It's used after deserializing object(see #required_header_t())
	 *\return true if it's valid.
	 */
	bool MIsValid()const;
#endif
};
inline required_header_t::required_header_t()
{
	for(unsigned i=0;i<sizeof(FMessageHeader);++i)
		FMessageHeader[i]=0x0;
}
;
inline bool required_header_t::operator==(required_header_t const& aRht) const
{
	return memcmp(FMessageHeader, aRht.FMessageHeader, sizeof(aRht.FMessageHeader))==0//
			&& FVersion==aRht.FVersion//
			;
}
inline bool required_header_t::operator<(required_header_t const& aRht) const
{
	return (memcmp(FMessageHeader, aRht.FMessageHeader, sizeof(aRht.FMessageHeader)) < 0);
}

#ifdef SHARE_CONFIG_DEFINED
inline required_header_t::required_header_t(NSHARE::CConfig const& aConf):
FVersion(aConf.MChild(NSHARE::version_t::NAME)) //
{
	for(unsigned i=0;i<sizeof(FMessageHeader);++i)
		FMessageHeader[i]=0x0;

	if(aConf.MHasValue(KEY_HEADER))
	{
		aConf.MChild(KEY_HEADER).MValueBuf(sizeof(FMessageHeader),FMessageHeader);
	}
}
inline NSHARE::CConfig required_header_t::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	_conf.MAdd(KEY_HEADER,FMessageHeader,sizeof(FMessageHeader));
	_conf.MAdd(FVersion.MSerialize());
	return _conf;
}
inline bool required_header_t::MIsValid()const
{
	unsigned i=0;
	for(;i<sizeof(FMessageHeader) //
			&&FMessageHeader[i]==0x0//
			;++i)
		;
	return i!=sizeof(FMessageHeader) ;
}
#endif

template<typename T>
inline required_header_t::required_header_t(T const& aHeader,
		NSHARE::version_t const& aVer)://
			FVersion(aVer)//
{
#if defined( COMPILE_ASSERT) && !defined(NDEBUG)
	COMPILE_ASSERT(sizeof(T)<=sizeof(FMessageHeader),InvalideSizeOfHeader);
#endif
	const unsigned _size =
			sizeof(T) <= sizeof(FMessageHeader) ?
					sizeof(T) : sizeof(FMessageHeader);

	uint8_t const*const _p=reinterpret_cast<uint8_t const*>(&aHeader);

	unsigned i=0;
	for(;i<_size;++i)
		FMessageHeader[i]=_p[i];

	for(;i<sizeof(FMessageHeader);++i)
			FMessageHeader[i]=0x0;
}
/*!\brief Fast less compare of type required_header_t
 *
 */
struct CReqHeaderFastLessCompare
{
	/*!\brief Two @c required_header_t are compared into some containres
	 * by its header message's only.
	 *
	 *\param a first object
	 *\param b first object
	 *\return true if the objects are less
	 */
	bool operator()(const required_header_t& a,
			const required_header_t& b) const
	{
		return a<b;
	}
};

/*!\brief Print the byte buffer to stream in
 * hexadecimal format
 *
 */
template<class T>
inline std::ostream& print_buffer(std::ostream& aStream, T aBegin, T aEnd)
{
	int _i = 0;
	for (; aBegin != aEnd; ++aBegin)
	{
		aStream << *aBegin << " ";
		if (!(++_i % 8))
			aStream << "\n";
	}
	return aStream;
}
}
namespace std
{
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::required_header_t const& aVal)
{

	aStream << "Version:" << aVal.FVersion << std::endl;
	aStream << "Number: ";
	int _i = 0;

	aStream.setf(ios::hex, ios::basefield);
	uint8_t* aBegin=(uint8_t*)aVal.FMessageHeader;
	uint8_t* aEnd=aBegin+ sizeof(aVal.FMessageHeader);
	for (; aBegin != aEnd; ++aBegin)
	{
		aStream << "0x" << static_cast<unsigned const&>(*aBegin) << " ";
		if (!(++_i % 8))
			aStream << "\n";
	}
	aStream.unsetf(ios::hex);
	return aStream;
}
}
#endif /* UDT_TYPES_H_ */
