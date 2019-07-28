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

	static const NSHARE::CText KEY_INVALID_VALUE;///<A serialization key of #E_INVALID_VALUE
	static const NSHARE::CText KEY_NOT_OPENED;///<A serialization key of #E_NOT_OPENED
	static const NSHARE::CText KEY_TOO_LARGE;///<A serialization key of #E_TOO_LARGE
	static const NSHARE::CText KEY_AGAIN;///<A serialization key of #E_AGAIN
	static const NSHARE::CText KEY_UNKNOWN_ERROR;///<A serialization key of #E_ERROR
	static const NSHARE::CText KEY_SOCKET_CLOSED;///<A serialization key of #E_SOCKET_CLOSED


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
		E_SOCKET_CLOSED	=0x1<<5, ///< Socket is closed
		E_MAX_BITWISE_CODE = 6,///< Technology value
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

	/*! @brief Deserialize object
	 *
	 * To check the result of deserialization,
	 * used the MIsValid().
	 * @param aConf Serialized object
	 */
	explicit sent_state_t(NSHARE::CConfig const& aConf);

	/*! @brief Serialize object
	 *
	 * The key of serialized object is #NAME
	 *
	 * @return Serialized object.
	 */
	CConfig MSerialize() const;

	/*! @brief Checks object for valid
	 *
	 * Usually It's used after deserializing object
	 * @return true if it's valid.
	 */
	bool MIsValid() const;

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
	void MRecv(size_t aCount);

	/** Passes amount of sent bytes
	 *
	 * @param aCount Amount of received bytes
	 */
	void MSend(size_t aCount);


	/** Puts information about send state
	 *
	 *	@param aState a state info
	 */
	void MSend(sent_state_t const& aState);

	/** Puts information about error
	 *
	 *	@param aError A error info
	 */
	void operator+=(const sent_state_t &aError);

	/** Composition information about IO
	 *
	 *	@param aInfo The other info
	 */
	void operator+=(const diagnostic_io_t &aInfo);

	/*! @brief Deserialize object
	 *
	 * To check the result of deserialization,
	 * used the MIsValid().
	 * @param aConf Serialized object
	 */
	explicit diagnostic_io_t(NSHARE::CConfig const& aConf);


	/*! @brief Serialize object
	 *
	 * The key of serialized object is #NAME
	 *
	 * @return Serialized object.
	 */
	CConfig MSerialize() const;

	/*! @brief Checks object for valid
	 *
	 * Usually It's used after deserializing object
	 * @return true if it's valid.
	 */
	bool MIsValid() const;

	/** Returns info about specified error
	 *
	 * @param aError An error type
	 * @return info about error
	 */
	sent_state_t const& MGetState(sent_state_t::eSendState const& aError) const;

	/** Returns info about amount of received data
	 *
	 * @return  The number of bytes
	 */
	size_t MGetRecvDataInfo() const;

	/** Returns info about the number of received packets
	 *
	 * @return  The number of packets
	 */
	size_t MGetRecvCountInfo() const;

	/** Returns info about amount of sent data
	 *
	 * @return  The number of bytes
	 */
	size_t MGetSentDataInfo() const;

	/** Returns info about the number of sent packets
	 *
	 * @return  The number of packets
	 */
	size_t MGetSentCountInfo() const;

private:
	atomic_t FRecvData;///< Amount of received data
	atomic_t FRecvCount;///< Count received packets
	atomic_t FSentData;///< Amount of sent data
	atomic_t FSentCount;///< Count sent packets
	sent_state_t FErrorInfo[sent_state_t::E_MAX_BITWISE_CODE];///< Info about errors

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
//		if(_is)
//			aStream <<", ";
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

	aStream<<" Receive " << aVal.MGetRecvDataInfo()<<" bytes,";
	aStream<<" packets # " << aVal.MGetRecvCountInfo()<<",";
	aStream<<" send " << aVal.MGetSentDataInfo()<<" bytes,";
	aStream<<" packets # " << aVal.MGetSentCountInfo();

	const unsigned _last_bits = sent_state_t::E_MAX_BITWISE_CODE;
	for (unsigned i = 0; i < _last_bits; ++i)
	{
		sent_state_t::eSendState const _error((sent_state_t::eSendState)(0x1<<i));
		if(!aVal.MGetState(_error).MIs())
			aStream<<aVal.MGetState(_error);
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
		FError(0),//
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
inline void diagnostic_io_t::MRecv(size_t aCount)
{
	++FRecvCount;
	FRecvData+=aCount;
}
inline void diagnostic_io_t::MSend(size_t aCount)
{
	++FSentCount;
	FSentData+=aCount;
}
inline void diagnostic_io_t::diagnostic_io_t::MSend(sent_state_t const& aState)
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
			if(_mask & (0x1<<i))
				FErrorInfo[i]+=aState.MSentByte();
		}
	}
}
inline void diagnostic_io_t::operator+=(const sent_state_t &aError)
{
	MSend(aError);
}
inline void diagnostic_io_t::operator+=(const diagnostic_io_t &aInfo)
{
	FRecvData+=aInfo.FRecvData;
	FRecvCount+=aInfo.FRecvCount;
	FSentData+=aInfo.FSentData;
	FSentCount+=aInfo.FSentCount;
	for(unsigned i=0;i<sent_state_t::E_MAX_BITWISE_CODE;++i)
		FErrorInfo[i]+=aInfo.FErrorInfo[i];
}
inline size_t diagnostic_io_t::MGetRecvDataInfo() const
{
	return FRecvData;
}
inline size_t diagnostic_io_t::MGetRecvCountInfo() const
{
	return FRecvCount;
}
inline size_t diagnostic_io_t::MGetSentDataInfo() const
{
	return FSentData;
}
inline size_t diagnostic_io_t::MGetSentCountInfo() const
{
	return FSentCount;
}

}


#endif /* DIAGNOSTIC_IO_T_H_ */
