/*
 * diagnostic_io_t.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 11.08.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  

#ifndef DIAGNOSTIC_IO_T_H_
#define DIAGNOSTIC_IO_T_H_

namespace NSHARE
{
/** Info about send errors
 *
 */
struct sent_state_t
{
	static const NSHARE::CText NAME;///<A serialization key
	static const NSHARE::CText KEY_ERROR;///<A serialization key of #FError
	static const NSHARE::CText KEY_BYTE;///<A serialization key of #FBytes

	/** @brief Error type
	 *
	 */
	enum eSendState
	{
		E_SENDED 		= 0x0, ///< no error
		E_INVALID_VALUE	=0x1<<0, ///< Cannot send as no the specified receiver
		E_NOT_OPENED	=0x1<<1 , ///< Socket is closed
		E_TOO_LARGE		=0x1<<2, /*!< This error is occured than the sent message size is more//!< E_TOO_LARGE
				 	 	 	 	 	 * than maximal send message size or the memory is full (no
				 	 	 	 	 	 * empty memory blocks)
				 	 	 	 	 	 */
		E_AGAIN			=0x1<<3, ///< This error is occured than the buffer of receiver is full
		E_ERROR			=0x1<<4, ///< Unknown error
		E_MAX_BITWISE_CODE = 4,///< Technology value
	};

	typedef NSHARE::CFlags<eSendState,uint32_t> error_t;///< An error type

	/** The error type and size of
	 * unsent message will be passed to constructo
	 *
	 * @param eError a erorr type
	 * @param aBytes size of unsent message
	 */
	sent_state_t(eSendState eError, size_t aBytes);

	/** @brief default constructor
	 *
	 */
	sent_state_t();

	/** @brief Test error state
	 *
	 *	@return true if no error
	 */
	bool MIs() const;

	/** @brief Test error state flags
	 *
	 *  @param aFlag a test type
	 *	@return true if flag true
	 */
	bool MIs(eSendState const& aFlag) const;

	/** @brief Adds error
	 *
	 *	@param aError An error
	 */
	void operator+=(const sent_state_t &aError);

	/** @brief Adds bytes
	 *
	 *	@param aBytes A bytes
	 */
	void operator+=(const size_t &aBytes);

	/** Returns error code
	 *
	 *	@return error code
	 */
	error_t const& MGetError() const;

	/** Returns count of sent (unsent) bytes
	 *
	 * @return count of bytes
	 */
	size_t const& MSentByte() const;

	/*! @brief Serialize object
	 *
	 * The key of serialized object is #NAME
	 *
	 * @return Serialized object.
	 */
	CConfig MSerialize() const;
private:
	error_t FError; ///<Send state
	size_t FBytes; ///< Amount of unsent (sent) bytes
};
/** Information about IO
 *
 * @note full mutable object - the "const" is ignored
 */
struct SHARE_EXPORT diagnostic_io_t
{
	static const NSHARE::CText NAME;///<A serialization key
	static const NSHARE::CText KEY_RECVDATA;///<A serialization key of #FRecvData
	static const NSHARE::CText KEY_SENTDATA;///<A serialization key of #FRecvCount
	static const NSHARE::CText KEY_RECVCOUNT;///<A serialization key of #FSentData
	static const NSHARE::CText KEY_SENTCOUNT;///<A serialization key of #FSentCount

	/** @brief default constructor
	 *
	 */
	diagnostic_io_t();

	/** Passes amount of received bytes
	 *
	 * @param aCount Amount of received bytes
	 */
	void MRecv(size_t aCount) const;

	/** Passes amount of sent bytes
	 *
	 * @param aCount Amount of received bytes
	 */
	void MSend(size_t aCount) const;


	/** Puts information about send state
	 *
	 *	@param aState a state info
	 */
	void MSend(sent_state_t const& aState) const;

	/** Puts information about error
	 *
	 *	@param aError A error info
	 */
	void operator+=(const sent_state_t &aError) const;


	/*! @brief Serialize object
	 *
	 * The key of serialized object is #NAME
	 *
	 * @return Serialized object.
	 */
	CConfig MSerialize() const;

	mutable atomic_t FRecvData;///< Amount of received data
	mutable atomic_t FRecvCount;///< Count received packets
	mutable atomic_t FSentData;///< Amount of sent data
	mutable atomic_t FSentCount;///< Count sent packets
	mutable sent_state_t FErrorInfo[sent_state_t::E_MAX_BITWISE_CODE];///< Info about errors

};
}
namespace std
{
inline std::ostream& operator <<(std::ostream& aStream,
		const NSHARE::sent_state_t::error_t& aVal)
{
	using namespace NSHARE;
	sent_state_t::error_t const& _val=aVal;

	bool _is=false;
	if (_val.MGetFlag(sent_state_t::E_INVALID_VALUE))
	{
		if(_is)
			aStream <<", ";
		_is=true;

		aStream << "No the specified receiver";
	}

	if (_val.MGetFlag(sent_state_t::E_NOT_OPENED))
	{
		if(_is)
			aStream <<", ";
		_is=true;

		aStream << "The socket is closed";
	}
	if (_val.MGetFlag(sent_state_t::E_TOO_LARGE))
	{
		if(_is)
			aStream <<", ";
		_is=true;

		aStream << "The data too large";
	}
	if (_val.MGetFlag(sent_state_t::E_AGAIN))
	{
		if(_is)
			aStream <<", ";
		_is=true;

		aStream << "The buffer of receiver is full";
	}
	if (_val.MGetFlag(sent_state_t::E_ERROR))
	{
		if(_is)
			aStream <<", ";
		_is=true;

		aStream << "Unknown error";
	}
	if(!_is)
		aStream << "No error";

	return aStream;
}
inline std::ostream& operator <<(std::ostream& aStream,
		const NSHARE::sent_state_t& aVal)
{
	aStream<<aVal.MGetError()<< " sent=" << aVal.MSentByte();
	return aStream;
}
inline std::ostream& operator <<(std::ostream& aStream,
		const NSHARE::diagnostic_io_t& aVal)
{
	using namespace NSHARE;

	aStream<<" Receive " << aVal.FRecvData<<" bytes,";
	aStream<<" packets # " << aVal.FRecvCount<<",";
	aStream<<" send " << aVal.FSentData<<" bytes,";
	aStream<<" packets # " << aVal.FSentCount;

	const unsigned _last_bits = sent_state_t::E_MAX_BITWISE_CODE;
	for (unsigned i = 0; i <= _last_bits; ++i)
	{
		if(!aVal.FErrorInfo[i].MIs())
			aStream<<aVal.FErrorInfo[i];
	}

	return aStream;
}
}
namespace NSHARE
{
inline sent_state_t::sent_state_t(eSendState eError, size_t aBytes) :
		FError(eError), //
		FBytes(aBytes) //
{

}
inline sent_state_t::sent_state_t() : //
		FError(E_ERROR),//
		FBytes(0) //
{
}
inline bool sent_state_t::MIs(eSendState const& aFlag) const
{
	if(aFlag== E_SENDED)
		return FError.MGetMask() == E_SENDED;
	return FError.MGetFlag(aFlag);
}
inline bool sent_state_t::MIs() const
{
	return MIs(E_SENDED);
}
inline sent_state_t::error_t const& sent_state_t::MGetError() const
{
	return FError;
}
inline size_t const& sent_state_t::MSentByte() const
{
	return FBytes;
}
inline void sent_state_t::operator+=(const size_t &aBytes)
{
	FBytes+=aBytes;
}
inline void sent_state_t::operator+=(const sent_state_t &aError)
{
	DLOG_IF(FATAL,MIs()!=aError.MIs())<<"You cannot sum error as in error exist (no exist) "<<aError<<
			" but  in error no exist (exist) "<<*this;

	FError.MSetFlag(aError.FError.MGetMask(),true);
	FBytes+=aError.FBytes;
}
inline void diagnostic_io_t::MRecv(size_t aCount) const
{
	++FRecvCount;
	FRecvData+=aCount;
}
inline void diagnostic_io_t::MSend(size_t aCount) const
{
	++FSentCount;
	FSentData+=aCount;
}
inline void diagnostic_io_t::diagnostic_io_t::MSend(sent_state_t const& aState) const
{
	using namespace std;

	if(aState.MIs())
		MSend(aState.MSentByte());
	else
	{
		sent_state_t::error_t::value_type const _mask=aState.MGetError().MGetMask();
		const unsigned _last_bits=sent_state_t::E_MAX_BITWISE_CODE;

		DCHECK_LE(_last_bits,sizeof(_mask)*8);

		for(unsigned i=0;i<=_last_bits;++i)
		{
			if(_mask & 0x1<<i)
				FErrorInfo[i]+=aState.MSentByte();
		}
	}
}
inline void diagnostic_io_t::operator+=(const sent_state_t &aError) const
{
	MSend(aError);
}
}


#endif /* DIAGNOSTIC_IO_T_H_ */
