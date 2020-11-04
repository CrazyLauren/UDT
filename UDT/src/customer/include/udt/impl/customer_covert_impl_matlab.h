/*
 * customer_covert_impl_matlab.h
 *
 *  Created on: 27.07.2020
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2020  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CUSTOMER_CONVER_IMPL_H_
#define CUSTOMER_CONVER_IMPL_H_
#include <deftype>
#include <customer.h>

namespace NSHARE
{
namespace matlab
{

#define SHARE_DESERIALIZE_FOR_MATLAB_KEY(aKey, aString)\
	if (!_is_found && !_is_##aKey##_value && strcmp(aString, fieldName) == 0)\
	{\
		_is_found = true;\
		_is_##aKey##_value = true;\
		if (!NSHARE::matlab::deserialize(&_rval.##aKey, fieldContent, 1, 0))\
		{\
			mexErrMsgIdAndTxt("ConvertingError",\
					"Error is occurred for %s",\
					fieldName);\
			_is = false;\
		}\
	}\
	/*END*/

#define SHARE_DESERIALIZE_FOR_MATLAB_ARRAY_KEY(aKey, aSize,aString)\
	if (!_is_found && !_is_##aKey##_value && strcmp(aString, fieldName) == 0)\
	{\
		_is_found = true;\
		_is_##aKey##_value = true;\
		if (!NSHARE::matlab::deserialize(_rval.##aKey, fieldContent, aSize, 0))\
		{\
			mexErrMsgIdAndTxt("ConvertingError",\
					"Error is occurred for %s",\
					fieldName);\
			_is = false;\
		}\
	}\
	/*END*/

#define SHARE_DESERIALIZE_DESERIALIZE_COMMON \
		bool _is_found = false;\
		\
		const char* fieldName = mxGetFieldNameByNumber(aObject, (int) i);\
		mxArray* fieldContent = mxGetFieldByNumber(aObject, aArrayIndex,\
				(int) i);\
				\
		if (!fieldName)\
		{\
			mexWarnMsgIdAndTxt("NoNameInField",\
					"No field name in structure ");\
			continue;\
		}\
		if (!fieldContent)\
		{\
			mexWarnMsgIdAndTxt("NoDataInField",\
					"No data in field %s", fieldName);\
			continue;\
		}\
		\
		size_t const _rows = static_cast<size_t>(mxGetN(fieldContent));\
		size_t const _cols = static_cast<size_t>(mxGetM(fieldContent));\
		size_t const _elems = static_cast<size_t>(mxGetNumberOfElements(\
				fieldContent));\
		size_t const _dims = static_cast<size_t>(mxGetNumberOfDimensions(\
				fieldContent));\
		mxClassID const _class = mxGetClassID(fieldContent);\
		/*END*/

#define SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(aKey)\
		bool _is_##aKey##_value = false;\
		/*END*/
//
template<>
inline mxArray* serialize<NSHARE::uuid_t>(NSHARE::uuid_t const& aObject)
{
	return NSHARE::matlab::serialize<uint64_t>(aObject.FVal);
}
template<>
bool deserialize<NSHARE::uuid_t>(NSHARE::uuid_t* aTo, mxArray const * aFrom,
		size_t aArraySize,
		size_t aArrayIndex)
{
	return deserialize<uint64_t>((uint64_t*) aTo, aFrom,
			aArraySize, aArrayIndex);
}
template<>
inline mxArray* allocate_memory<NSHARE::version_t>(size_t aArraySize)
{
	if (aArraySize == 0)
		return NULL;
	mwSize dims[2] =
			{ 1, aArraySize };

	const char* field_names[] =
			{
					"major",
					"minor",
					"revision"
			};
	int const _field_number = sizeof(field_names) / sizeof(*field_names);

	mxArray *var = mxCreateStructArray(2, dims, _field_number, field_names);
	return var;
}
template<>
inline mxArray* serialize<NSHARE::version_t>(NSHARE::version_t const* aObject,
		size_t aArraySize)
{
	mxArray *var = allocate_memory<NSHARE::version_t>(aArraySize);
	if (var != NULL)
		for (size_t i = 0; i < aArraySize; ++i)
		{
			NSHARE::version_t const& _object(aObject[i]);
			int _addr = 0;
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FMajor));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FMinor));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FRelease));
		}
	return var;
}
template<>
inline bool deserialize<NSHARE::version_t>(NSHARE::version_t* aTo,
		mxArray const* aObject, size_t aArraySize,
		size_t aArrayIndex)
{
	if (aTo == NULL)
		return false;

	bool _is = true;
	for (unsigned aArrayId = 0; aArrayId < aArraySize && _is; ++aArrayId)
	{
		NSHARE::version_t& _rval(aTo[aArrayId]);

		size_t const _field_num(mxGetNumberOfFields(aObject));

		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FMajor)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FMinor)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FRelease)

		for (size_t i = 0; i < _field_num; ++i)
		{
			SHARE_DESERIALIZE_DESERIALIZE_COMMON

			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FMajor, "major")
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FMinor, "minor")
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FRelease, "revision")

			if (!_is_found)
			{
				DCHECK(false);
				mexWarnMsgIdAndTxt("NoFieldInProtocol",
						"Field isn't exist in protocol %s", fieldName);
			}
		}
	}
	return _is;
}
//
//
//
template<>
inline mxArray* allocate_memory<NUDT::id_t>(size_t aArraySize)
{
	if (aArraySize == 0)
		return NULL;
	mwSize dims[2] =
			{ 1, aArraySize };

	const char* field_names[] =
			{
					(const char*) NSHARE::uuid_t::NAME.c_str(),
					(const char*) NUDT::id_t::KEY_NAME.c_str()
			};
	int const _field_number = sizeof(field_names) / sizeof(*field_names);

	mxArray *var = mxCreateStructArray(2, dims, _field_number, field_names);
	return var;
}
template<>
inline mxArray* serialize<NUDT::id_t>(NUDT::id_t const* aObject,
		size_t aArraySize)
{
	mxArray *var = allocate_memory<NUDT::id_t>(aArraySize);
	if (var != NULL)
		for (size_t i = 0; i < aArraySize; ++i)
		{
			NUDT::id_t const& _object(aObject[i]);
			int _addr = 0;
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FUuid));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FName));
		}
	return var;
}

template<>
inline bool deserialize<NUDT::id_t>(NUDT::id_t* aTo,
		mxArray const* aObject, size_t aArraySize,
		size_t aArrayIndex)
{
	if (aTo == NULL)
		return false;
	bool _is = true;
	for (unsigned aArrayId = 0; aArrayId < aArraySize && _is; ++aArrayId)
	{
		NUDT::id_t& _rval(aTo[aArrayId]);
		size_t const _field_num(mxGetNumberOfFields(aObject));

		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FUuid)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FName)

		for (size_t i = 0; i < _field_num; ++i)
		{
			SHARE_DESERIALIZE_DESERIALIZE_COMMON

			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FUuid, NSHARE::uuid_t::NAME.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FName, NUDT::id_t::KEY_NAME.c_str())

			if (!_is_found)
			{
				DCHECK(false);
				mexWarnMsgIdAndTxt("NoFieldInProtocol",
						"Field isn't exist in protocol %s", fieldName);
			}
		}
	}
	return _is;
}
//
//
//
template<>
inline mxArray* serialize<NSHARE::eEndian>(NSHARE::eEndian const& aObject)
{
	return NSHARE::matlab::serialize<unsigned>(aObject);
}
template<>
inline bool deserialize<NSHARE::eEndian>(NSHARE::eEndian* aTo,
		mxArray const* aObject, size_t aArraySize,
		size_t aArrayIndex)
{
	unsigned _to;
	bool const _is = NSHARE::matlab::deserialize<unsigned>(&_to, aObject,
			aArraySize, aArrayIndex);
	if (_is)
		*aTo = static_cast<NSHARE::eEndian>(_to);
	return _is;
}
template<>
inline mxArray* serialize<NUDT::eProgramType>(NUDT::eProgramType const& aObject)
{
	return NSHARE::matlab::serialize<unsigned>(aObject);
}
template<>
inline bool deserialize<NUDT::eProgramType>(NUDT::eProgramType* aTo,
		mxArray const* aObject, size_t aArraySize,
		size_t aArrayIndex)
{
	unsigned _to;
	bool const _is = NSHARE::matlab::deserialize<unsigned>(&_to, aObject,
			aArraySize, aArrayIndex);
	if (_is)
		*aTo = static_cast<NUDT::eProgramType>(_to);
	return _is;

}
//
//
//
template<>
inline mxArray* allocate_memory<NUDT::program_id_t>(size_t aArraySize)
{
	if (aArraySize == 0)
		return NULL;
	mwSize dims[2] =
			{ 1, aArraySize };

	const char* field_names[] =
			{
					(const char*) NUDT::id_t::NAME.c_str(),
					(const char*) NSHARE::version_t::NAME.c_str(),
					(const char*) NUDT::program_id_t::KEY_TIME.c_str(),
					(const char*) NUDT::program_id_t::KEY_PID.c_str(),
					(const char*) NUDT::program_id_t::KEY_PATH.c_str(),
					(const char*) NUDT::program_id_t::KEY_TYPE.c_str(),
					(const char*) NUDT::program_id_t::KEY_ENDIAN.c_str()
			};
	int const _field_number = sizeof(field_names) / sizeof(*field_names);

	mxArray *var = mxCreateStructArray(2, dims, _field_number, field_names);
	return var;
}

template<>
inline mxArray* serialize<NUDT::program_id_t>(NUDT::program_id_t const* aObject,
		size_t aArraySize)
{
	mxArray *var = allocate_memory<NUDT::program_id_t>(aArraySize);
	if (var != NULL)
		for (size_t i = 0; i < aArraySize; ++i)
		{
			NUDT::program_id_t const& _object(aObject[i]);

			int _addr = 0;
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FId));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FVersion));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FTime));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FPid));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FPath));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FType));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FEndian));
		}
	return var;
}
template<>
inline mxArray* serialize<std::set<NUDT::program_id_t> >(
		std::set<NUDT::program_id_t> const& aObject)
{
	mxArray *var = allocate_memory<NUDT::program_id_t>(aObject.size());
	if (var != NULL)
	{
		std::set<NUDT::program_id_t>::const_iterator _it(aObject.begin()),
				_it_end(aObject.end());
		for (size_t i = 0; _it != _it_end; ++i, ++_it)
		{
			int _addr = 0;
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_it->FId));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_it->FVersion));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_it->FTime));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_it->FPid));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_it->FPath));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_it->FType));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_it->FEndian));
		}
	}
	return var;
}
template<>
inline bool deserialize<NUDT::program_id_t>(NUDT::program_id_t* aTo,
		mxArray const* aObject, size_t aArraySize,
		size_t aArrayIndex)
{
	if (aTo == NULL)
		return false;
	bool _is = true;
	for (unsigned aArrayId = 0; aArrayId < aArraySize && _is; ++aArrayId)
	{
		NUDT::program_id_t& _rval(aTo[aArrayId]);

		size_t const _field_num(mxGetNumberOfFields(aObject));

		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FId)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FVersion)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FTime)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FPid)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FPath)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FType)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FEndian)

		for (size_t i = 0; i < _field_num; ++i)
		{
			SHARE_DESERIALIZE_DESERIALIZE_COMMON

			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FId, NUDT::id_t::NAME.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FVersion, NSHARE::version_t::NAME.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FTime, NUDT::program_id_t::KEY_TIME.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FPid, NUDT::program_id_t::KEY_PID.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FPath, NUDT::program_id_t::KEY_PATH.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FType, NUDT::program_id_t::KEY_TYPE.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FEndian, NUDT::program_id_t::KEY_ENDIAN.c_str())

			if (!_is_found)
			{
				DCHECK(false);
				mexWarnMsgIdAndTxt("NoFieldInProtocol",
						"Field isn't exist in protocol %s", fieldName);
			}
		}
	}
	return _is;
}
template<>
inline bool deserialize<std::set<NUDT::program_id_t> >(
		std::set<NUDT::program_id_t>* aTo,
		mxArray const* aObject, size_t aArraySize,
		size_t aArrayIndex)
{
	if (aTo == NULL)
		return false;
	bool _is = true;
	for (unsigned aArrayId = 0; aArrayId < aArraySize && _is; ++aArrayId)
	{
		NUDT::program_id_t _rval;

		size_t const _field_num(mxGetNumberOfFields(aObject));

		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FId)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FVersion)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FTime)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FPid)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FPath)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FType)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FEndian)

		for (size_t i = 0; i < _field_num; ++i)
		{
			SHARE_DESERIALIZE_DESERIALIZE_COMMON

			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FId, NUDT::id_t::NAME.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FVersion, NSHARE::version_t::NAME.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FTime, NUDT::program_id_t::KEY_TIME.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FPid, NUDT::program_id_t::KEY_PID.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FPath, NUDT::program_id_t::KEY_PATH.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FType, NUDT::program_id_t::KEY_TYPE.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FEndian, NUDT::program_id_t::KEY_ENDIAN.c_str())

			if (!_is_found)
			{
				DCHECK(false);
				mexWarnMsgIdAndTxt("NoFieldInProtocol",
						"Field isn't exist in protocol %s", fieldName);
			}
		}
		aTo->insert(_rval);
	}
	return _is;
}
//
//
//
template<>
inline mxArray* allocate_memory<NUDT::required_header_t>(size_t aArraySize)
{
	if (aArraySize == 0)
		return NULL;
	mwSize dims[2] =
			{ 1, aArraySize };

	const char* field_names[] =
			{
					(const char*) NSHARE::version_t::NAME.c_str(),
					(const char*) NUDT::required_header_t::KEY_HEADER.c_str()
			};
	int const _field_number = sizeof(field_names) / sizeof(*field_names);

	mxArray *var = mxCreateStructArray(2, dims, _field_number, field_names);
	return var;
}

template<>
inline mxArray* serialize<NUDT::required_header_t>(
		NUDT::required_header_t const* aObject,
		size_t aArraySize)
{
	mxArray *var = allocate_memory<NUDT::required_header_t>(aArraySize);
	if (var != NULL)
		for (size_t i = 0; i < aArraySize; ++i)
		{
			NUDT::required_header_t const& _object(aObject[i]);
			int _addr = 0;
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FVersion));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FMessageHeader,
					UDT_MAX_MESSAGE_HEADER_SIZE));

		}
	return var;
}

template<>
inline bool deserialize<NUDT::required_header_t>(NUDT::required_header_t* aTo,
		mxArray const* aObject, size_t aArraySize,
		size_t aArrayIndex)
{
	if (aTo == NULL)
		return false;
	bool _is = true;
	for (unsigned aArrayId = 0; aArrayId < aArraySize && _is; ++aArrayId)
	{
		NUDT::required_header_t& _rval(aTo[aArrayId]);

		size_t const _field_num(mxGetNumberOfFields(aObject));

		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FVersion)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FMessageHeader)

		for (size_t i = 0; i < _field_num; ++i)
		{
			SHARE_DESERIALIZE_DESERIALIZE_COMMON

			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FVersion, NSHARE::version_t::NAME.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_ARRAY_KEY(FMessageHeader,
					UDT_MAX_MESSAGE_HEADER_SIZE,
					NUDT::required_header_t::KEY_HEADER.c_str())

			if (!_is_found)
			{
				DCHECK(false);
				mexWarnMsgIdAndTxt("NoFieldInProtocol",
						"Field isn't exist in protocol %s", fieldName);
			}
		}
	}
	return _is;
}

//
//
//
template<>
inline mxArray* allocate_memory<NUDT::requirement_msg_info_t>(size_t aArraySize)
{
	if (aArraySize == 0)
		return NULL;
	mwSize dims[2] =
			{ 1, aArraySize };

	const char* field_names[] =
			{
					(const char*) NUDT::requirement_msg_info_t::KEY_PROTOCOL_NAME.c_str(),
					(const char*) NUDT::required_header_t::NAME.c_str(),
					(const char*) NUDT::requirement_msg_info_t::KEY_FLAGS.c_str(),
					(const char*) NUDT::requirement_msg_info_t::KEY_FROM.c_str()
			};
	int const _field_number = sizeof(field_names) / sizeof(*field_names);

	mxArray *var = mxCreateStructArray(2, dims, _field_number, field_names);
	return var;
}

template<>
inline mxArray* serialize<NUDT::requirement_msg_info_t>(
		NUDT::requirement_msg_info_t const* aObject,
		size_t aArraySize)
{
	mxArray *var = allocate_memory<NUDT::requirement_msg_info_t>(aArraySize);
	if (var != NULL)
		for (size_t i = 0; i < aArraySize; ++i)
		{
			int _addr = 0;
			NUDT::requirement_msg_info_t const& _object(aObject[i]);
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FProtocolName));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FRequired));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FFlags));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FFrom));
		}
	return var;
}

template<>
inline bool deserialize<NUDT::requirement_msg_info_t>(
		NUDT::requirement_msg_info_t* aTo,
		mxArray const* aObject, size_t aArraySize,
		size_t aArrayIndex)
{
	if (aTo == NULL)
		return false;
	bool _is = true;
	for (unsigned aArrayId = 0; aArrayId < aArraySize && _is; ++aArrayId)
	{
		NUDT::requirement_msg_info_t& _rval(aTo[aArrayId]);
		size_t const _field_num(mxGetNumberOfFields(aObject));

		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FProtocolName)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FRequired)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FFlags)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FFrom)

		for (size_t i = 0; i < _field_num; ++i)
		{
			SHARE_DESERIALIZE_DESERIALIZE_COMMON

			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FProtocolName,
					NUDT::requirement_msg_info_t::KEY_PROTOCOL_NAME.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FRequired, NUDT::required_header_t::NAME.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FFlags,
					NUDT::requirement_msg_info_t::KEY_FLAGS.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FFrom,
					NUDT::requirement_msg_info_t::KEY_FROM.c_str())

			if (!_is_found)
			{
				DCHECK(false);
				mexWarnMsgIdAndTxt("NoFieldInProtocol",
						"Field isn't exist in protocol %s", fieldName);
			}
		}
	}
	return _is;
}
//
//
//
template<>
inline mxArray* allocate_memory<NUDT::subcribe_receiver_args_t::what_t>(
		size_t aArraySize)
{
	if (aArraySize == 0)
		return NULL;
	mwSize dims[2] =
			{ 1, aArraySize };

	const char* field_names[] =
			{
					(const char*) NUDT::requirement_msg_info_t::NAME.c_str(),
					(const char*) NUDT::subcribe_receiver_args_t::what_t::KEY_WHO.c_str()
			};
	int const _field_number = sizeof(field_names) / sizeof(*field_names);

	mxArray *var = mxCreateStructArray(2, dims, _field_number, field_names);
	return var;
}

template<>
inline mxArray* serialize<NUDT::subcribe_receiver_args_t::what_t>
(NUDT::subcribe_receiver_args_t::what_t const* aObject,
		size_t aArraySize)
{
	mxArray *var = allocate_memory<NUDT::subcribe_receiver_args_t::what_t>(
			aArraySize);
	if (var != NULL)
		for (size_t i = 0; i < aArraySize; ++i)
		{
			int _addr = 0;
			NUDT::subcribe_receiver_args_t::what_t const& _object(aObject[i]);
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FWhat));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FWho));
		}
	return var;
}

template<>
inline bool deserialize<NUDT::subcribe_receiver_args_t::what_t>
(NUDT::subcribe_receiver_args_t::what_t* aTo,
		mxArray const* aObject, size_t aArraySize,
		size_t aArrayIndex)
{
	if (aTo == NULL)
		return false;

	bool _is = true;
	for (unsigned aArrayId = 0; aArrayId < aArraySize && _is; ++aArrayId)
	{
		NUDT::subcribe_receiver_args_t::what_t& _rval(aTo[aArrayId]);

		size_t const _field_num(mxGetNumberOfFields(aObject));

		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FWhat)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FWho)

		for (size_t i = 0; i < _field_num; ++i)
		{
			SHARE_DESERIALIZE_DESERIALIZE_COMMON

			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FWhat, NUDT::requirement_msg_info_t::NAME.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FWho,
					NUDT::subcribe_receiver_args_t::what_t::KEY_WHO.c_str())

			if (!_is_found)
			{
				DCHECK(false);
				mexWarnMsgIdAndTxt("NoFieldInProtocol",
						"Field isn't exist in protocol %s", fieldName);
			}
		}
	}
	return _is;
}
//
//
//
template<>
inline mxArray* allocate_memory<NUDT::subcribe_receiver_args_t>(
		size_t aArraySize)
{
	if (aArraySize == 0)
		return NULL;
	mwSize dims[2] =
			{ 1, aArraySize };

	const char* field_names[] =
			{
					(const char*) NUDT::subcribe_receiver_args_t::KEY_RECEIVERS.c_str(),
			};
	int const _field_number = sizeof(field_names) / sizeof(*field_names);

	mxArray *var = mxCreateStructArray(2, dims, _field_number, field_names);
	return var;
}

template<>
inline mxArray* serialize<NUDT::subcribe_receiver_args_t>
(NUDT::subcribe_receiver_args_t const* aObject,
		size_t aArraySize)
{
	mxArray *var = allocate_memory<NUDT::subcribe_receiver_args_t>(aArraySize);
	if (var != NULL)
		for (size_t i = 0; i < aArraySize; ++i)
		{
			int _addr = 0;
			NUDT::subcribe_receiver_args_t const& _object(aObject[i]);
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(
							_object.FReceivers.empty() ?
									NULL :
									&_object.FReceivers.at(0)
											,
							_object.FReceivers.size()));
		}
	return var;
}

template<>
inline bool deserialize<NUDT::subcribe_receiver_args_t>
(NUDT::subcribe_receiver_args_t* aTo,
		mxArray const* aObject, size_t aArraySize,
		size_t aArrayIndex)
{
	if (aTo == NULL)
		return false;

	bool _is = true;
	for (unsigned aArrayId = 0; aArrayId < aArraySize && _is; ++aArrayId)
	{
		NUDT::subcribe_receiver_args_t& _rval(aTo[aArrayId]);

		size_t const _field_num(mxGetNumberOfFields(aObject));

		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FReceivers)

		for (size_t i = 0; i < _field_num; ++i)
		{
			SHARE_DESERIALIZE_DESERIALIZE_COMMON

			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FReceivers,
					NUDT::subcribe_receiver_args_t::KEY_RECEIVERS.c_str())

			if (!_is_found)
			{
				DCHECK(false);
				mexWarnMsgIdAndTxt("NoFieldInProtocol",
						"Field isn't exist in protocol %s", fieldName);
			}
		}
	}
	return _is;
}
//
//
//
template<>
inline mxArray* allocate_memory<NUDT::customers_updated_args_t>(
		size_t aArraySize)
{
	if (aArraySize == 0)
		return NULL;
	mwSize dims[2] =
			{ 1, aArraySize };

	const char* field_names[] =
			{
					(const char*) NUDT::customers_updated_args_t::KEY_DISCONNECTED.c_str(),
					(const char*) NUDT::customers_updated_args_t::KEY_CONNECTED.c_str(),
			};
	int const _field_number = sizeof(field_names) / sizeof(*field_names);

	mxArray *var = mxCreateStructArray(2, dims, _field_number, field_names);
	return var;
}

template<>
inline mxArray* serialize<NUDT::customers_updated_args_t>
(NUDT::customers_updated_args_t const* aObject,
		size_t aArraySize)
{
	mxArray *var = allocate_memory<NUDT::customers_updated_args_t>(aArraySize);
	if (var != NULL)
		for (size_t i = 0; i < aArraySize; ++i)
		{
			int _addr = 0;
			NUDT::customers_updated_args_t const& _object(aObject[i]);
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(
							_object.FDisconnected));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(
							_object.FConnected));
		}
	return var;
}

template<>
inline bool deserialize<NUDT::customers_updated_args_t>
(NUDT::customers_updated_args_t* aTo,
		mxArray const* aObject, size_t aArraySize,
		size_t aArrayIndex)
{
	if (aTo == NULL)
		return false;

	bool _is = true;
	for (unsigned aArrayId = 0; aArrayId < aArraySize && _is; ++aArrayId)
	{
		NUDT::customers_updated_args_t& _rval(aTo[aArrayId]);

		size_t const _field_num(mxGetNumberOfFields(aObject));

		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FDisconnected)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FConnected)

		for (size_t i = 0; i < _field_num; ++i)
		{
			SHARE_DESERIALIZE_DESERIALIZE_COMMON

			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FDisconnected,
					NUDT::customers_updated_args_t::KEY_DISCONNECTED.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FConnected,
					NUDT::customers_updated_args_t::KEY_CONNECTED.c_str())

			if (!_is_found)
			{
				DCHECK(false);
				mexWarnMsgIdAndTxt("NoFieldInProtocol",
						"Field isn't exist in protocol %s", fieldName);
			}
		}
	}
	return _is;
}

//
//
//
template<>
inline mxArray* allocate_memory<NUDT::received_message_info_t>(
		size_t aArraySize)
{
	if (aArraySize == 0)
		return NULL;
	mwSize dims[2] =
			{ 1, aArraySize };

	const char* field_names[] =
			{
					(const char*) NUDT::received_message_info_t::KEY_FROM.c_str(),
					(const char*) NUDT::received_message_info_t::KEY_PROTOCOL_NAME.c_str(),
					(const char*) NUDT::received_message_info_t::KEY_PACKET_NUMBER.c_str(),
					(const char*) NUDT::required_header_t::NAME.c_str(),
					(const char*) NUDT::received_message_info_t::KEY_TO.c_str(),
					(const char*) NUDT::received_message_info_t::KEY_OCCUR_USER_ERROR.c_str(),
					(const char*) NUDT::received_message_info_t::KEY_ENDIAN.c_str(),
					(const char*) NUDT::received_message_info_t::KEY_REMAIN_CALLBACKS.c_str(),
					(const char*) NUDT::received_message_info_t::KEY_CBS.c_str(),
					(const char*) NUDT::received_message_info_t::KEY_FLAGS.c_str()
			};
	int const _field_number = sizeof(field_names) / sizeof(*field_names);

	mxArray *var = mxCreateStructArray(2, dims, _field_number, field_names);
	return var;
}

template<>
inline mxArray* serialize<NUDT::received_message_info_t>(
		NUDT::received_message_info_t const* aObject,
		size_t aArraySize)
{
	mxArray *var = allocate_memory<NUDT::received_message_info_t>(aArraySize);
	if (var != NULL)
		for (size_t i = 0; i < aArraySize; ++i)
		{
			int _addr = 0;
			NUDT::received_message_info_t const& _object(aObject[i]);
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FFrom));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FProtocolName));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FPacketNumber));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FHeader));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FTo));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FOccurUserError));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FEndian));

			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FRemainCallbacks));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FCbs));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FFlags));

		}
	return var;
}

template<>
inline bool deserialize<NUDT::received_message_info_t>(
		NUDT::received_message_info_t* aTo,
		mxArray const* aObject, size_t aArraySize,
		size_t aArrayIndex)
{
	if (aTo == NULL)
		return false;
	bool _is = true;
	for (unsigned aArrayId = 0; aArrayId < aArraySize && _is; ++aArrayId)
	{
		NUDT::received_message_info_t& _rval(aTo[aArrayId]);

		size_t const _field_num(mxGetNumberOfFields(aObject));

		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FFrom)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FProtocolName)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FPacketNumber)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FHeader)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FTo)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FOccurUserError)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FEndian)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FRemainCallbacks)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FCbs)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FFlags)

		for (size_t i = 0; i < _field_num; ++i)
		{
			SHARE_DESERIALIZE_DESERIALIZE_COMMON

			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FFrom,
					NUDT::received_message_info_t::KEY_FROM.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FProtocolName,
					NUDT::received_message_info_t::KEY_PROTOCOL_NAME.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FPacketNumber,
					NUDT::received_message_info_t::KEY_PACKET_NUMBER.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FHeader, NUDT::required_header_t::NAME.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FTo, NUDT::received_message_info_t::KEY_TO.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FOccurUserError,
					NUDT::received_message_info_t::KEY_OCCUR_USER_ERROR.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FEndian,
					NUDT::received_message_info_t::KEY_ENDIAN.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FRemainCallbacks,
					NUDT::received_message_info_t::KEY_REMAIN_CALLBACKS.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FCbs,
					NUDT::received_message_info_t::KEY_CBS.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FFlags,
					NUDT::received_message_info_t::KEY_FLAGS.c_str())

			if (!_is_found)
			{
				DCHECK(false);
				mexWarnMsgIdAndTxt("NoFieldInProtocol",
						"Field isn't exist in protocol %s", fieldName);
			}
		}
	}
	return _is;
}
//
//
//
template<>
inline mxArray* allocate_memory<NUDT::received_message_args_t>(
		size_t aArraySize)
{
	if (aArraySize == 0)
		return NULL;
	mwSize dims[2] =
			{ 1, aArraySize };

	const char* field_names[] =
			{
					(const char*) NUDT::received_message_info_t::NAME.c_str(),
					(const char*) NUDT::received_data_t::NAME.c_str()
			};
	int const _field_number = sizeof(field_names) / sizeof(*field_names);

	mxArray *var = mxCreateStructArray(2, dims, _field_number, field_names);
	return var;
}

template<>
inline mxArray* serialize<NUDT::received_message_args_t>(
		NUDT::received_message_args_t const* aObject,
		size_t aArraySize)
{
	mxArray *var = allocate_memory<NUDT::received_message_args_t>(aArraySize);
	if (var != NULL)
		for (size_t i = 0; i < aArraySize; ++i)
		{
			int _addr = 0;
			NUDT::received_message_args_t const& _object(aObject[i]);

			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize<NUDT::received_message_info_t>(_object));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FMessage.FBuffer));

		}
	return var;
}

//
//
//
template<>
inline mxArray* allocate_memory<NUDT::fail_sent_args_t>(
		size_t aArraySize)
{
	if (aArraySize == 0)
		return NULL;
	mwSize dims[2] =
			{ 1, aArraySize };

	const char* field_names[] =
			{
					(const char*) NUDT::fail_sent_args_t::KEY_FROM.c_str(),
					(const char*) NUDT::fail_sent_args_t::KEY_PROTOCOL_NAME.c_str(),
					(const char*) NUDT::fail_sent_args_t::KEY_PACKET_NUMBER.c_str(),
					(const char*) NUDT::required_header_t::NAME.c_str(),
					(const char*) NUDT::fail_sent_args_t::KEY_ERROR_CODE.c_str(),
					(const char*) NUDT::fail_sent_args_t::KEY_USER_ERROR.c_str(),
					(const char*) NUDT::fail_sent_args_t::KEY_SENT_TO.c_str(),
					(const char*) NUDT::fail_sent_args_t::KEY_FAILS.c_str()
			};
	int const _field_number = sizeof(field_names) / sizeof(*field_names);

	mxArray *var = mxCreateStructArray(2, dims, _field_number, field_names);
	return var;
}

template<>
inline mxArray* serialize<NUDT::fail_sent_args_t>(
		NUDT::fail_sent_args_t const* aObject,
		size_t aArraySize)
{
	mxArray *var = allocate_memory<NUDT::fail_sent_args_t>(aArraySize);
	if (var != NULL)
		for (size_t i = 0; i < aArraySize; ++i)
		{
			int _addr = 0;
			NUDT::fail_sent_args_t const& _object(aObject[i]);
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FFrom));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FProtocolName));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FPacketNumber));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FHeader));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FErrorCode));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FUserCode));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FSentTo));
			mxSetFieldByNumber(var, i, (int) _addr++,
					NSHARE::matlab::serialize(_object.FFails));

		}
	return var;
}

template<>
inline bool deserialize<NUDT::fail_sent_args_t>(
		NUDT::fail_sent_args_t* aTo,
		mxArray const* aObject, size_t aArraySize,
		size_t aArrayIndex)
{
	if (aTo == NULL)
		return false;
	bool _is = true;
	for (unsigned aArrayId = 0; aArrayId < aArraySize && _is; ++aArrayId)
	{
		NUDT::fail_sent_args_t& _rval(aTo[aArrayId]);

		size_t const _field_num(mxGetNumberOfFields(aObject));

		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FFrom)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FProtocolName)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FPacketNumber)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FHeader)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FErrorCode)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FUserCode)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FSentTo)
		SHARE_DESERIALIZE_DEFINE_MATLAB_KEY(FFails)

		for (size_t i = 0; i < _field_num; ++i)
		{
			SHARE_DESERIALIZE_DESERIALIZE_COMMON

			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FFrom,
					NUDT::fail_sent_args_t::KEY_FROM.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FProtocolName,
					NUDT::fail_sent_args_t::KEY_PROTOCOL_NAME.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FPacketNumber,
					NUDT::fail_sent_args_t::KEY_PACKET_NUMBER.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FHeader, NUDT::required_header_t::NAME.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FErrorCode, NUDT::fail_sent_args_t::KEY_ERROR_CODE.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FUserCode,
					NUDT::fail_sent_args_t::KEY_USER_ERROR.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FSentTo,
					NUDT::fail_sent_args_t::KEY_SENT_TO.c_str())
			SHARE_DESERIALIZE_FOR_MATLAB_KEY(FFails,
					NUDT::fail_sent_args_t::KEY_FAILS.c_str())

			if (!_is_found)
			{
				DCHECK(false);
				mexWarnMsgIdAndTxt("NoFieldInProtocol",
						"Field isn't exist in protocol %s", fieldName);
			}
		}
	}
	return _is;
}
}
}
#endif /* CUSTOMER_CONVER_IMPL_H_ */
