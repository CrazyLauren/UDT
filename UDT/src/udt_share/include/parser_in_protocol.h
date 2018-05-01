/*
 * parser_in_protocol.h
 *
 *  Created on: 19.11.2015
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef PARSER_IN_PROTOCOL_H_
#define PARSER_IN_PROTOCOL_H_

namespace NUDT
{
//template<class T>

template<class T, class UserData = void*>
class CInParser
{
	enum e_error_t
	{
		Eok, ESizeDG, EBufferIsSmall, ENoDG
	};
	CInParser<T>& operator=(CInParser<T> const& aRht)
	{
		return *this;
	}
public:
	typedef T target_t;
	typedef UserData user_data_t;
	typedef CInParser<target_t, user_data_t> parser_t;
	typedef NSHARE::CBuffer data_t;
	typedef data_t::const_iterator it_t;

	struct state_t
	{
		state_t()
		{
			memset(this, 0, sizeof(*this));
		}
		std::size_t FInvalidVersion; //The number of packets which is not handled
								  //as its protocols vesrion is not support
		std::size_t FESize;								//invalid  sizeof header
		std::size_t FEBuffer;								//the buffer is small
		std::size_t FECrc;										//invalid crc
		std::size_t FDGCounter;			//the number of packets handled by user
		std::size_t FNumberOfData;							//count of usefully data
		std::size_t FReadBytes;			//the number of bytes handled by parser
		std::size_t FPackets;			//the naumber of packets handled by parser

		void MSerialize(NSHARE::CConfig& aTo) const
		{
			aTo.MAdd("ev", FInvalidVersion);
			aTo.MAdd("se", FESize);
			aTo.MAdd("sb", FEBuffer);
			aTo.MAdd("ce", FECrc);
			aTo.MAdd("hdg", FDGCounter);
			aTo.MAdd("dat", FNumberOfData);
			aTo.MAdd("re", FReadBytes);
			aTo.MAdd("ps", FPackets);
		}
	};
	CInParser() :
			FTarget(NULL)
	{

	}
	void MSetTarget(target_t* aVal)
	{
		VLOG(2) << "New target == " << aVal << " previous ==" << FTarget;
		FTarget = aVal;
	}
	CInParser(target_t* aTarget) :
			FTarget(aTarget)
	{
		;
	}
	CInParser(parser_t const& aRht) :
			FUserData(aRht.FUserData), FTarget(aRht.FTarget)
	{
		;
	}

	static bool sMIsValidProtocol(data_t::const_iterator aItBegin,
			data_t::const_iterator aItEnd)
	{
		using namespace NSHARE;
		ssize_t const _size = aItEnd - aItBegin;
		size_t const _head_size = sizeof(head_t);

		CHECK_GE(_size, 0);

		if (_size < (ssize_t) _head_size)
		{
			return false;
		}
		typedef head_t::crc_head_t _crc_t;
		_crc_t::type_t const _crc = _crc_t::sMCalcCRCofBuf(&(*aItBegin),
				&(*aItBegin) + _head_size);
		return _crc == _crc_t::sMCheckingConstant() ? true : false;
	}
	void MReceivedData(data_t::const_iterator aItBegin,
			data_t::const_iterator aItEnd)
	{
		VLOG(2) << "Put in Parser:" << (aItEnd - aItBegin) << ":" << this;
		if (aItEnd == aItBegin)
			return;
		FState.FReadBytes += aItEnd - aItBegin;
		++FState.FPackets;
		VLOG(2) << "Buffer Size:" << FBuf.size() << ":" << this;
		data_t const& _const_buf = FBuf;
		if (!FBuf.empty())
		{
			FBuf.insert(FBuf.end(), aItBegin, aItEnd);
			aItBegin = _const_buf.begin();
			aItEnd = _const_buf.end();
		}

		it_t _it = MAnalizeBuffer(aItBegin, aItEnd);

		if (_it != aItEnd)
		{
			VLOG(2) << "Buffer Size:" << FBuf.size() << ":" << this;
			if (FBuf.empty())
				FBuf.insert(FBuf.end(), _it, aItEnd);
			else
				FBuf.erase(FBuf.begin(),
						FBuf.begin() + (_it - _const_buf.begin())); //const to non-const
			VLOG(2) << "Buffer is small.remain:" << FBuf.size() << ":" << this;
		}
		else if (!FBuf.empty())
			MCleanBuffer();
	}
	void MCleanBuffer()
	{
		FBuf.clear();
		;
	}
	state_t const& MGetState() const
	{
		return FState;
	}
	user_data_t FUserData;

	//----------------------------------------

private:
	template<unsigned E>
	struct surroinding_endian_t
	{
		enum { type = E };
	};
	template<class E>
	struct surroinding_t
	{
		typedef E type;
	};

	inline it_t MAnalizeBuffer(it_t aItBegin, it_t aItEnd) 
	{
		VLOG(2) << "Analyze buffer distance " << (aItEnd - aItBegin) << ":"
				<< this;
		HANG_INIT;
		for (it_t _it; aItBegin != aItEnd; HANG_CHECK)
		{
			VLOG(2) << "Next iteration BufSize=" << (aItEnd - aItBegin);

			e_error_t _code = MLookingForHead(aItBegin, aItEnd, &_it);
			VLOG(2) << "Loking for code :" << (int) _code;
			if (_code == Eok)
			{
				VLOG(2) << "DG has been founded";
				_code = MLookingForDG(aItBegin, aItEnd, &_it);
			}
			LOG_IF(DFATAL, (_it > aItEnd || _it < aItBegin))
					<< "out of range iterator";
			switch (_code)
			{

			case Eok:
				if (_it != aItBegin)
					aItBegin = _it;
				else
					++aItBegin;
				VLOG(2) << "KD  is valid:" << this;
				break;
			case ESizeDG:
				++aItBegin;
				++FState.FESize;
				LOG(WARNING) << "KD size  is invalid:" << this;
				break;
			case EBufferIsSmall:
				++FState.FEBuffer;
				VLOG(2) << "Buffer  is small:" << this;
				return _it;
				break;
			case ENoDG:
				VLOG(2) << "No DG:" << this;
				//return ++_it;
				return _it;
				break;
			}
		}
		return aItBegin;
	}
	static e_error_t MLookingForHead(it_t aItBegin, it_t aItEnd, it_t* aLast)
	{
		using namespace NSHARE;
		ssize_t const _size = aItEnd - aItBegin;
		size_t const _head_size = sizeof(head_t);
		CHECK_GE(_size, 0);
		if (_size < (ssize_t) _head_size)
		{
			*aLast = aItBegin;
			return EBufferIsSmall;
		}
		typedef head_t::crc_head_t _crc_t;
		_crc_t::type_t _crc;
		aItEnd -= _head_size;

		//for optimization previous to call hang check
		//check for valid the crc
		_crc = _crc_t::sMCalcCRCofBuf(&(*aItBegin), &(*aItBegin) + _head_size);
		bool _is_found=_crc == _crc_t::sMCheckingConstant();
		if (!_is_found)
		{
			++aItBegin;
			for (HANG_INIT; aItBegin != aItEnd; HANG_CHECK, ++aItBegin)
			{
				_crc = _crc_t::sMCalcCRCofBuf(&(*aItBegin), &(*aItBegin) + _head_size);
				if (_crc == _crc_t::sMCheckingConstant())
				{
					//As The DG has not been found in the beginning,
					//its size is been checked for safety.
					if (reinterpret_cast<head_t const*>(&(*aItBegin))->FHeadSize
							== _head_size)
					{
						_is_found=true;
						break;
					}
				}
				//++FState.FECrc;
				VLOG(6) << "Invalid crc " << (unsigned) _crc << " Head Crc "
						<< (unsigned) (((head_t*) (&(*aItBegin)))->FCrcHead);
			}
		}
		*aLast = aItBegin;
		return _is_found?Eok:ENoDG;
	}



#define RECEIVE(Number,Type) \
			case Number: \
					VLOG(2) <<"Looked for DG Type:"<<Number;\
				return MCheckKD(aItBegin,aItEnd,aLast,surroinding_t<Type>());
	
	inline e_error_t MLookingForDG(it_t aItBegin, it_t aItEnd, it_t* aLast) 
	{
		LOG_IF(FATAL, (aItEnd - aItBegin) < (int) sizeof(head_t))
				<< "Invalid DG size.";
		head_t const* _head = reinterpret_cast<head_t const*>(&(*aItBegin));
		VLOG(4) << (*_head);
		if (_head->FHeadSize != sizeof(head_t))
		{
			LOG(ERROR) << "HeadSize=" << (unsigned) _head->FHeadSize
					<< ", HeadSize" << sizeof(head_t);
			return ESizeDG;
		}
		switch (_head->FType)
		{
#ifdef RECEIVES
		RECEIVES
		// см макрос RECEIVE выше
#endif
		default:
			LOG(WARNING) << "The DG Type '" << _head->FType
					<< "' is not handled";
			return MCheckKD(aItBegin, aItEnd, aLast, surroinding_t<head_t>());
			break;
		} //switch

		*aLast = aItBegin;
		return Eok;
	}
	template<class KD_T>
	inline e_error_t MCheckKD(it_t aPBegin, it_t aPEND, it_t* aLast,
			surroinding_t<KD_T>);
	inline e_error_t MCheckKD(it_t aPBegin, it_t aPEND, it_t* aLast,
			surroinding_t<head_t>);

	template<class DG_T>
	inline void MProcess(DG_T const* aP)
	{
		VLOG(2) << "Process DG:" << *aP << "; For " << FTarget << " :" << this;
		LOG_IF(DFATAL, !FTarget) << "The Parser target is null. Ignoring ...";
		if (FTarget)
		{
			++FState.FDGCounter;
			FTarget->MProcess(aP, this);
		}
	}

	state_t FState;
	data_t FBuf;
	target_t* FTarget;

};
template<class T, class UserData>
template<class KD_T>
inline typename CInParser<T, UserData>::e_error_t CInParser<T, UserData>::MCheckKD(
		it_t aPBegin, it_t aPEND, it_t* aLast, surroinding_t<KD_T>)
{
	VLOG(4) << "Check DG. Distance:" << (aPEND - aPBegin) << ". :" << this;
	head_t const* _head = reinterpret_cast<head_t const*>(&(*aPBegin));
	VLOG(4) << "Version of " << _head->MEndianCorrectValue(_head->FType) << " is " << _head->FVersion;
	VLOG(1) << (*_head);
	const NSHARE::version_t _kd_ver(KD_T::MAJOR,KD_T::MINOR);
	LOG_IF(WARNING,
			(_head->FVersion.FMajor == KD_T::MAJOR)
					&& (_head->FVersion.FMinor > KD_T::MINOR))
			<< "The Minor version  of received the " << _head->FType << " DG "
					" is greater than internal. (" << _head->FVersion.FMinor
			<< ">" << (unsigned)KD_T::MINOR << ")";
	bool _need_handled = true;
	if (!_kd_ver.MIsCompatibleWith(_head->FVersion))
	{
		_need_handled = false;
		LOG(DFATAL) << "Internal version of protocol is out-of-date.("
				<< _head->FVersion << "!=" << _kd_ver << ") for "
				<< _head->FType << " DG which will ignored.";
	}
	std::size_t const _data_size = _head->MGetDataSize();
	std::size_t const _size_kd = _head->FHeadSize + _data_size;
	if ((aPEND - aPBegin) < (int) _size_kd)
	{
		*aLast = aPBegin;
		return EBufferIsSmall;
	}
	unsigned const _min_crc_size = sizeof(head_t::crc_data_t::type_t);

	if (_data_size >= _min_crc_size)
	{
		VLOG_IF(1, !_head->MGetDataCRC()) << " Null data crc, Ignoring...";
		if (_head->MGetDataCRC()) //optmization by protocol
		{
			head_t::crc_data_t::type_t _crc16 =
					head_t::crc_data_t::sMCalcCRCofBuf(_head->MDataBegin(),
							_head->MDataBegin() + _data_size);

			if (_crc16 != _head->MGetDataCRC())
			{
				LOG(ERROR) << "Invalid DATA CRC:" << _crc16 << ", in DG "
						<< _head->FType << " DataCrc:" << _head->MGetDataCRC()
						<< ".It which will ignored.";
				_need_handled = false;
			}
		}
	}
	else
		LOG(WARNING) << "Incorrect data size. It is to have more than "
				<< _min_crc_size << " bytes";
	FState.FNumberOfData += _data_size;
	VLOG_IF(1, _need_handled) << "DG " << (unsigned)_head->FType << " is fully valid.";
	if (_need_handled)
		MProcess(reinterpret_cast<KD_T const*>(&(*aPBegin)));
	*aLast = aPBegin + _size_kd;
	return Eok;
}
template<class T, class UserData>
inline typename CInParser<T, UserData>::e_error_t CInParser<T, UserData>::MCheckKD(
		it_t aPBegin, it_t aPEND, it_t* aLast, surroinding_t<head_t>)
{
	VLOG(2) << "Check Unknown DG. Distance:" << (aPEND - aPBegin) << ". :"
			<< this;
	head_t const* _head = reinterpret_cast<head_t const*>(&(*aPBegin));
	VLOG(2) << "Version of " << _head->FType << " is " << _head->FVersion;
	VLOG(1) << (*_head);
	std::size_t const _data_size = _head->MGetDataSize();
	std::size_t const _size_kd = _head->FHeadSize + _data_size;

	if ((aPEND - aPBegin) < (int) _size_kd)
	{
		VLOG(2) << "Full size is invalid";
		*aLast = aPBegin;
		return EBufferIsSmall;
	}

	if ((!_head->FVersion.FMajor && !_head->FVersion.FMinor)
			|| !_head->MGetFromUUID() || !_head->MGetTime())
	{
		LOG(ERROR) << "There are not version " << _head->FVersion << " or UUID "
				<< _head->MGetFromUUID() << " or time " << _head->MGetTime();
		return Eok; //error as "aLast" is not changed
	}

	unsigned const _min_crc_size = sizeof(head_t::crc_data_t::type_t);

	if (_data_size >= _min_crc_size)
	{
		LOG_IF(WARNING, !_head->MGetDataCRC()) << " Null data crc, Ignoring...";
		if (_head->MGetDataCRC()) //optmization by protocol
		{
			head_t::crc_data_t::type_t _crc16 =
					head_t::crc_data_t::sMCalcCRCofBuf(_head->MDataBegin(),
							_head->MDataBegin() + _data_size);

			if (_crc16 != _head->MGetDataCRC())
			{
				VLOG(2) << "Invalid DATA CRC Real:" << _crc16;
				return Eok; //error
			}
		}
	}
	FState.FNumberOfData += _data_size;
	VLOG(1) << "DG " << _head->FType << " is fully valid.";
	*aLast = aPBegin + _size_kd;
	return Eok;
}
} //namespace NUDT

#endif /* PARSER_IN_PROTOCOL_H_ */
