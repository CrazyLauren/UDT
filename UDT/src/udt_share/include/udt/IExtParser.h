/*
 * IExtParser.h
 *
 *  Created on: 21.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef IEXTPARSER_H_
#define IEXTPARSER_H_
#include "udt_types.h"

namespace NUDT
{
/*!\brief Interface for protocol's handler
 *
 */
class UDT_SHARE_EXPORT IExtParser: public NSHARE::IFactory
{
public:

	/*!\brief Message information
	 *
	 */
	struct obtained_dg_t
	{
		required_header_t FType; ///< message type
		uint8_t FErrorCode;/*!<  error code, if The error code isn't zero,
						   the address range from FBegin to FEnd isn't handled.*/
		const uint8_t* FBegin;/*!<  begin of message into buffer,
							  It hasn't to be zero.*/
		const uint8_t* FEnd; /*!<  end of message into buffer,
							 actually The address of next byte after
							 the last byte of messages (equal vector::end())*/
		obtained_dg_t();
	};
	typedef std::vector<obtained_dg_t> result_t;///< Information about messages into the buffer

	/*!\brief  Inheritance messages info
	 *
	 *	The multiple inheritance is illegal.
	 *	The parent message type is inherent from required_header_t.
	 */
	struct msg_inheritance_t: required_header_t
	{
		required_header_t FChildHeader;///< The child message type
		NSHARE::CText FChildProtcol;/*!<  The child message protocol,
										  if is null than the protocol of child and
										  parent is equal.*/

		/*! \brief the default constructor
		 *
		 */
		msg_inheritance_t();

		/*!\brief To constructor may be passed to be copied
		 * a parent message type, a child message type, a child message protocol
		 *
		 *\param aBase A parent message type
		 *\param aChild A child message type
		 *\param aChildProtocol A child message protocol if empty
		 *						the parent and child protocols are equals.
		 */
		explicit msg_inheritance_t(required_header_t const& aBase,
				required_header_t const& aChild,
				NSHARE::CText const& aChildProtocol = NSHARE::CText()
						);

	};

	typedef std::vector<msg_inheritance_t> inheritances_info_t;///< A protocol parents

	/*!\brief Parsing the buffer
	 *
	 *	The splited to messages buffer has't to have skipped blocks and
	 *	has to beginen from \a aItBegin.
	 *	If The address of the last buffer's byte \a aItEnd is not
	 *	equal \a FEnd, than the data from \a FEnd to the
	 *	last buffer's byte \a aItEnd is buffered.
	 *	if no message in the buffer (result_t is empty)
	 *	the buffer is buffered.
	 *
	 *\param aItBegin buffer begin
	 *\param aItEnd buffer end
	 *\param aFrom the sender's buffer uuid (informational)
	 *\param aMask buffer's byte order
	 *
	 *\return messages list into buffer
	 */
	virtual result_t MParserData(const uint8_t* aItBegin, const uint8_t* aItEnd,
			NSHARE::uuid_t aFrom = NSHARE::uuid_t(), uint8_t aMask =
					NSHARE::E_SHARE_ENDIAN)=0;

	/*!\brief Deserialize message type (e.g. for GUI)
	 *
	 *\param  aFrom serialized data
	 *
	 *\return first - message type
	 *			second - true - if successful
	 *\note all exception is catch
	 */
	virtual std::pair<required_header_t, bool> MHeader(
			const NSHARE::CConfig& aFrom) const=0;

	/*!\brief Serialize message type (e.g. for GUI, logging)
	 *
	 *\param  aData message type
	 *
	 *\return serialized message type
	 *\note all exception is catch
	 */
	virtual NSHARE::CConfig MToConfig(const required_header_t& aData) const=0;

	/*!\brief Serialize message (e.g. for GUI)
	 *
	*\param  aHeader message type
	*\param  aItBegin begin data of message
	*\param  aItEnd message end
	 *
	*\return serialized message
	 *
	*\note As then the using \a raw protocol
	 *	the buffer doesn't contain message header,
	 *	the message header is puted obviously.
	 *
	 */
	virtual NSHARE::CConfig MToConfig(const required_header_t& aHeader,
			const uint8_t* aItBegin, const uint8_t* aItEnd) const
	{
		return NSHARE::CConfig();
	}

	/*!\brief Deserialize message (e.g. for GUI)
	 *
	 *\param aFrom a Serialized message
	 *
	 *\return The message
	 */
	virtual NSHARE::CBuffer MFromConfig(const NSHARE::CConfig& aFrom) const
	{
		return NSHARE::CBuffer();
	}

	/*!\brief Protocol description (e.g. for GUI)
	 *
	 *\return description
	 */
	virtual char const* MDescription() const
	{
		return "no description";
	}

	/*!\brief Swap the message byte order
	 *
	 *	if The byte order of the message is
	 *	illegal, than it will changed by
	 *	using this method.
	 *
	 *\param aHeader A message type
	 *\param aItBegin A message begin
	 *\param aItEnd A message end
	 *
	 *\return true - if swapped
	 *
	 *\note Input message byte order
	 *		  is #NSHARE::E_SHARE_OTHER_ENDIAN
	 *		  The header has to swapped too
	 */
	virtual bool MSwapEndian(const required_header_t& aHeader,
			uint8_t* aItBegin, uint8_t* aItEnd) const
	{
		return false;
	}

	/*!\brief Swap endian of the requested message's "header"
	 *
	 *	Because the byte orders of the requested message
	 *	and return value of method MParserData (sent message)
	 *	can be not equal, it's necessary method for
	 *	swaping endian requested message's "header"/
	 *	The method is called for valid data routing.
	 *
	 *\param aHeader A message type
	 *
	 *\return true - if swapped
	 *
	 *\note Input message byte order
	 *		  is #NSHARE::E_SHARE_OTHER_ENDIAN
	 *		  The header has to swapped too
	 */
	virtual bool MSwapEndian(required_header_t* aHeader) const
	{
		return false;
	}

	/*!\brief size of message header
	 *
	 *\return The offset of first byte of data in the message
	 */
	virtual size_t MDataOffset(const required_header_t& aHeader) const
	{
		return sizeof(aHeader.FMessageHeader);
	}
	/*!@brief Returns the size of data message
	 *
	 * The function has to be overload if
	 * overloaded #MGetInheritances method
	 * @param aHeader Type of message
	 * @return The size of the message data or -1 if
	 * it's not defined
	 */
	virtual int MDataSize (const required_header_t& aHeader) const
	{
		return -1;
	}

	/*!@brief Return inheritance messages info
	 *
	 * @return The message parent list
	 * @note The #MDataSize has to be overloaded too
	 */
	virtual inheritances_info_t MGetInheritances() const
	{
		return inheritances_info_t();
	}

protected:

	/*! \brief The default constructor
	 *
	 *\param type An unique protocol name
	 */
	IExtParser(const NSHARE::CText& type) :
			NSHARE::IFactory(type)
	{
	}
};
inline IExtParser::obtained_dg_t::obtained_dg_t() :
		FErrorCode(0),							//
		FBegin(NULL), FEnd(NULL)
{

}
inline IExtParser::msg_inheritance_t::msg_inheritance_t()
{
	;
}
inline IExtParser::msg_inheritance_t::msg_inheritance_t(
		required_header_t const& aParent, //
		required_header_t const& aChild, //
		NSHARE::CText const& aChildProtocol //
		) :
		required_header_t(aParent), //
		FChildHeader(aChild), //
		FChildProtcol(aChildProtocol)
{

}
} //
#endif /* IEXTPARSER_H_ */
