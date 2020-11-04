/*
 * matlab_serialize.h
 *
 * Copyright © 2020  https://github.com/CrazyLauren
 *
 *  Created on: 27.07.2020
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef MATLAB_SERIALIZE_H
#define MATLAB_SERIALIZE_H

#include <mex.h>
#include <matrix.h>
#include <stdint.h>

namespace NSHARE
{
namespace matlab
{
/** Serialize message to Matlab mxArray
 *
 * @param aObject - message type
 * @param aArraySize - array size
 *
 * @return pointer to allocated mxArray
 */
template<typename T>
inline mxArray* serialize(T const& aObject);
template<typename T>
mxArray* serialize(T const* aValue, size_t aArraySize = 1);

template<typename T>
inline mxArray* serialize(std::vector<T> const& aObject);

#ifdef SHARE_BUFFER_DEFINED
template<typename T>
inline mxArray* serialize(NSHARE::CPODBuffer<T> const& aObject);
template<typename T>
inline mxArray* serialize(NSHARE::CBuffer const& aObject);
#endif
/** Deserialize from Matlab mxArray
 *
 * @param aObject mxArray - object
 * @return new object or throw exception if invalid cannot deserialize
 */
template<class T>
inline bool deserialize(T* aTo, mxArray const* aFrom, size_t aArraySize = 1,
		size_t aArrayIndex = 0);
template<typename T>
inline bool deserialize(std::vector<T>* aTo, mxArray const* aObject,
		size_t aArraySize = 1,
		size_t aArrayIndex = 0);
#ifdef SHARE_BUFFER_DEFINED
template<typename T>
inline bool deserialize(NSHARE::CPODBuffer<T>* aTo, mxArray const* aObject, size_t aArraySize = 1,
		size_t aArrayIndex = 0);
template<typename T>
inline bool deserialize(NSHARE::CBuffer* aTo, mxArray const* aObject, size_t aArraySize = 1,
		size_t aArrayIndex = 0);

#endif
/** Creates array of matlab structure for type
 *
 * @param aArraySize - the array length
 * @return pointer to matlab structure
 */
template<typename T>
inline mxArray* allocate_memory(size_t aArraySize = 1);

template<typename T>
struct matlab_code_t
{
	enum
	{
		value = mxSTRUCT_CLASS
	};
	inline static const char* type()
	{
		return "structure";
	}
};
template<>
struct matlab_code_t<bool>
{
	enum
	{
		value = mxLOGICAL_CLASS
	};
	typedef mxLogical matlab_t;
	inline static const char* type()
	{
		return "Logical";
	}
};

template<>
struct matlab_code_t<int8_t>
{
	enum
	{
		value = mxINT8_CLASS
	};
	typedef mxInt8 matlab_t;
	inline static const char* type()
	{
		return "Int8";
	}
#if MX_HAS_INTERLEAVED_COMPLEX
	typedef mxComplexInt8 complex_t;

	inline static complex_t* sMGetComplex(const mxArray *pa)
	{
		return mxGetComplexInt8s(pa);
	}
	inline
	static int sMSetComplex(mxArray *pa, complex_t* dt)
	{
		return mxSetComplexInt8s(pa, dt);
	}
	inline static matlab_t* sMGet(const mxArray *pa)
	{
		return mxGetInt8s(pa);
	}
	inline
	static int sMSet(mxArray *pa, matlab_t* dt)
	{
		return mxSetInt8s(pa, dt);
	}
#endif
};
template<>
struct matlab_code_t<uint8_t>
{
	enum
	{
		value = mxUINT8_CLASS
	};
	typedef mxUint8 matlab_t;
	inline static const char* type()
	{
		return "Uint8";
	}
#if MX_HAS_INTERLEAVED_COMPLEX
	typedef mxComplexUint8 complex_t;

	inline static complex_t* sMGetComplex(const mxArray *pa)
	{
		return mxGetComplexUint8s(pa);
	}
	inline
	static int sMSetComplex(mxArray *pa, complex_t* dt)
	{
		return mxSetComplexUint8s(pa, dt);
	}
	inline static matlab_t* sMGet(const mxArray *pa)
	{
		return mxGetUint8s(pa);
	}
	inline
	static int sMSet(mxArray *pa, matlab_t* dt)
	{
		return mxSetUint8s(pa, dt);
	}
#endif
};
template<>
struct matlab_code_t<int16_t>
{
	enum
	{
		value = mxINT16_CLASS
	};
	typedef mxInt16 matlab_t;
	inline static const char* type()
	{
		return "Int16";
	}
#if MX_HAS_INTERLEAVED_COMPLEX
	typedef mxComplexInt16 complex_t;

	inline static complex_t* sMGetComplex(const mxArray *pa)
	{
		return mxGetComplexInt16s(pa);
	}
	inline
	static int sMSetComplex(mxArray *pa, complex_t* dt)
	{
		return mxSetComplexInt16s(pa, dt);
	}
	inline static matlab_t* sMGet(const mxArray *pa)
	{
		return mxGetInt16s(pa);
	}
	inline
	static int sMSet(mxArray *pa, matlab_t* dt)
	{
		return mxSetInt16s(pa, dt);
	}
#endif
};
template<>
struct matlab_code_t<uint16_t>
{
	enum
	{
		value = mxUINT16_CLASS
	};
	typedef mxUint16 matlab_t;
	inline static const char* type()
	{
		return "Uint16";
	}
#if MX_HAS_INTERLEAVED_COMPLEX
	typedef mxComplexUint16 complex_t;

	inline static complex_t* sMGetComplex(const mxArray *pa)
	{
		return mxGetComplexUint16s(pa);
	}
	inline
	static int sMSetComplex(mxArray *pa, complex_t* dt)
	{
		return mxSetComplexUint16s(pa, dt);
	}
	inline static matlab_t* sMGet(const mxArray *pa)
	{
		return mxGetUint16s(pa);
	}
	inline
	static int sMSet(mxArray *pa, matlab_t* dt)
	{
		return mxSetUint16s(pa, dt);
	}
#endif
};
template<>
struct matlab_code_t<int32_t>
{
	enum
	{
		value = mxINT32_CLASS
	};
	typedef mxInt32 matlab_t;
	inline static const char* type()
	{
		return "Int32";
	}
#if MX_HAS_INTERLEAVED_COMPLEX
	typedef mxComplexInt32 complex_t;

	inline static complex_t* sMGetComplex(const mxArray *pa)
	{
		return mxGetComplexInt32s(pa);
	}
	inline
	static int sMSetComplex(mxArray *pa, complex_t* dt)
	{
		return mxSetComplexInt32s(pa, dt);
	}
	inline static matlab_t* sMGet(const mxArray *pa)
	{
		return mxGetInt32s(pa);
	}
	inline
	static int sMSet(mxArray *pa, matlab_t* dt)
	{
		return mxSetInt32s(pa, dt);
	}
#endif
};
template<>
struct matlab_code_t<uint32_t>
{
	enum
	{
		value = mxUINT32_CLASS
	};
	typedef mxUint32 matlab_t;
	inline static const char* type()
	{
		return "Uint32";
	}
#if MX_HAS_INTERLEAVED_COMPLEX
	typedef mxComplexUint32 complex_t;

	inline static complex_t* sMGetComplex(const mxArray *pa)
	{
		return mxGetComplexUint32s(pa);
	}
	inline
	static int sMSetComplex(mxArray *pa, complex_t* dt)
	{
		return mxSetComplexUint32s(pa, dt);
	}
	inline static matlab_t* sMGet(const mxArray *pa)
	{
		return mxGetUint32s(pa);
	}
	inline
	static int sMSet(mxArray *pa, matlab_t* dt)
	{
		return mxSetUint32s(pa, dt);
	}
#endif
};
template<>
struct matlab_code_t<int64_t>
{
	enum
	{
		value = mxINT64_CLASS
	};
	typedef mxInt64 matlab_t;
	inline static const char* type()
	{
		return "Int64";
	}
#if MX_HAS_INTERLEAVED_COMPLEX
	typedef mxComplexInt64 complex_t;

	inline static complex_t* sMGetComplex(const mxArray *pa)
	{
		return mxGetComplexInt64s(pa);
	}
	inline
	static int sMSetComplex(mxArray *pa, complex_t* dt)
	{
		return mxSetComplexInt64s(pa, dt);
	}
	inline static matlab_t* sMGet(const mxArray *pa)
	{
		return mxGetInt64s(pa);
	}
	inline
	static int sMSet(mxArray *pa, matlab_t* dt)
	{
		return mxSetInt64s(pa, dt);
	}
#endif
};
template<>
struct matlab_code_t<uint64_t>
{
	enum
	{
		value = mxUINT64_CLASS
	};
	typedef mxUint64 matlab_t;
	inline static const char* type()
	{
		return "Uint64";
	}
#if MX_HAS_INTERLEAVED_COMPLEX
	typedef mxComplexUint64 complex_t;

	inline static complex_t* sMGetComplex(const mxArray *pa)
	{
		return mxGetComplexUint64s(pa);
	}
	inline
	static int sMSetComplex(mxArray *pa, complex_t* dt)
	{
		return mxSetComplexUint64s(pa, dt);
	}
	inline static matlab_t* sMGet(const mxArray *pa)
	{
		return mxGetUint64s(pa);
	}
	inline
	static int sMSet(mxArray *pa, matlab_t* dt)
	{
		return mxSetUint64s(pa, dt);
	}
#endif
};
template<>
struct matlab_code_t<float>
{
	enum
	{
		value = mxSINGLE_CLASS
	};
	typedef mxSingle matlab_t;
	inline static const char* type()
	{
		return "Single";
	}
#if MX_HAS_INTERLEAVED_COMPLEX
	typedef mxComplexSingle complex_t;

	inline static complex_t* sMGetComplex(const mxArray *pa)
	{
		return mxGetComplexSingles(pa);
	}
	inline
	static int sMSetComplex(mxArray *pa, complex_t* dt)
	{
		return mxSetComplexSingles(pa, dt);
	}
	inline static matlab_t* sMGet(const mxArray *pa)
	{
		return mxGetSingles(pa);
	}
	inline
	static int sMSet(mxArray *pa, matlab_t* dt)
	{
		return mxSetSingles(pa, dt);
	}
#endif
};
template<>
struct matlab_code_t<double>
{
	enum
	{
		value = mxDOUBLE_CLASS
	};
	typedef mxDouble matlab_t;
	inline static const char* type()
	{
		return "Double";
	}
#if MX_HAS_INTERLEAVED_COMPLEX
	typedef mxComplexDouble complex_t;

	inline static complex_t* sMGetComplex(const mxArray *pa)
	{
		return mxGetComplexDoubles(pa);
	}
	inline
	static int sMSetComplex(mxArray *pa, complex_t* dt)
	{
		return mxSetComplexDoubles(pa, dt);
	}
	inline static matlab_t* sMGet(const mxArray *pa)
	{
		return mxGetDoubles(pa);
	}
	inline
	static int sMSet(mxArray *pa, matlab_t* dt)
	{
		return mxSetDoubles(pa, dt);
	}
#endif
};
#ifdef SIZE_OF_LONG_DOUBLE

template<>
struct matlab_code_t<long double>:matlab_code_t<double>
{
};
#endif
template<>
struct matlab_code_t<mxChar>
{
	enum
	{
		value = mxCHAR_CLASS
	};
	typedef mxChar matlab_t;
	inline static const char* type()
	{
		return "Char";
	}
};

template<>
struct matlab_code_t<std::string::value_type> : matlab_code_t<mxChar>
{
};
template<>
struct matlab_code_t<std::string> : matlab_code_t<mxChar>
{
};
template<>
struct matlab_code_t<NSHARE::CText::value_type> : matlab_code_t<mxChar>
{
};
template<>
struct matlab_code_t<std::wstring::value_type> : matlab_code_t<mxChar>
{
};
inline const char* print_matlab_type(mxClassID aClass)
{
	switch (aClass)
	{
	case mxLOGICAL_CLASS:
		return matlab_code_t<bool>::type();
		break;
	case mxCHAR_CLASS:
		return matlab_code_t<mxChar>::type();
		break;
	case mxINT8_CLASS:
		return matlab_code_t<int8_t>::type();
		break;
	case mxUINT8_CLASS:
		return matlab_code_t<uint8_t>::type();
		break;
	case mxINT16_CLASS:
		return matlab_code_t<int16_t>::type();
		break;
	case mxUINT16_CLASS:
		return matlab_code_t<uint16_t>::type();
		break;
	case mxINT32_CLASS:
		return matlab_code_t<int32_t>::type();
		break;
	case mxUINT32_CLASS:
		return matlab_code_t<uint32_t>::type();
		break;
	case mxINT64_CLASS:
		return matlab_code_t<int64_t>::type();
		break;
	case mxUINT64_CLASS:
		return matlab_code_t<uint64_t>::type();
		break;
	case mxSINGLE_CLASS:
		return matlab_code_t<float>::type();
		break;
	case mxDOUBLE_CLASS:
		return matlab_code_t<double>::type();
		break;
	}
	return NULL;
}

template<>
inline bool deserialize<std::string>(std::string* aTo, mxArray const* aObject,
		size_t aArraySize,
		size_t aArrayIndex)
{
	if (aObject == NULL)
		return false;

	size_t const total_num_of_elements =
			static_cast<size_t>( mxGetNumberOfElements(aObject));
	aTo->resize(total_num_of_elements);
	return aTo->empty()
			|| deserialize(&aTo->front(), aObject, total_num_of_elements);
}
template<>
inline bool deserialize<NSHARE::CText>(NSHARE::CText* aTo,
		mxArray const* aObject, size_t aArraySize,
		size_t aArrayIndex)
{
	if (aObject == NULL)
		return false;
	size_t const total_num_of_elements =
			static_cast<size_t>( mxGetNumberOfElements(aObject));
	aTo->resize(total_num_of_elements);
	return aTo->empty()
			|| deserialize(&aTo->at(0), aObject, total_num_of_elements);
}
template<typename T>
inline bool deserialize(std::vector<T>* aTo, mxArray const* aObject,
		size_t aArraySize,
		size_t aArrayIndex)
{
	if (aObject == NULL)
		return false;
	size_t const total_num_of_elements =
			static_cast<size_t>( mxGetNumberOfElements(aObject));
	aTo->resize(total_num_of_elements);
	return aTo->empty()
			|| deserialize(&aTo->front(), aObject, total_num_of_elements);
}
#ifdef SHARE_BUFFER_DEFINED
template<typename T>
inline bool deserialize(NSHARE::CPODBuffer<T>* aTo, mxArray const* aObject, size_t aArraySize,
		size_t aArrayIndex )
{
	size_t const total_num_of_elements = static_cast<size_t>( mxGetNumberOfElements(aObject));
	aTo->resize(total_num_of_elements);
	return aTo->empty() || deserialize(&aTo -> front(), aObject, total_num_of_elements);
}
template<typename T>
inline bool deserialize(NSHARE::CBuffer* aTo, mxArray const* aObject, size_t aArraySize,
		size_t aArrayIndex )
{
	size_t const total_num_of_elements = static_cast<size_t>( mxGetNumberOfElements(aObject));
	aTo->resize(total_num_of_elements);
	return aTo->empty() || deserialize((unit8_t*)&aTo -> front(), aObject, total_num_of_elements);
}
#endif

template<typename T>
inline mxArray* allocate_memory(size_t aArraySize)
{
	const mwSize dims[] =
			{ 1, aArraySize };
	mxArray *var = mxCreateNumericArray(2, dims,
			static_cast<mxClassID>(matlab_code_t<T>::value), mxREAL);
	return var;
}

template<typename T>
mxArray* serialize(T const* aValue, size_t aArraySize)
{
	if (aArraySize == 0)
		return NULL;
	mxArray *var = allocate_memory<T>(aArraySize);
	unsigned char *start_of_pr = (unsigned char *) mxGetData(var);

	const size_t _element_size = mxGetElementSize(var);
	DCHECK_EQ(_element_size, sizeof(T));
	if (_element_size != sizeof(T))
	{
		//@ todo type trace
		mexWarnMsgIdAndTxt("invalidDataType", "Sizeof data isn't equal ");
		return NULL;
	}
	memcpy(start_of_pr, aValue, sizeof(T) * aArraySize);
	return var;
}
template<>
mxArray* serialize<char>(char const* aValue, size_t aArraySize)
{
	typedef char T;
	if (aArraySize == 0)
		return NULL;

	const mwSize dims[] =
			{ 1, aArraySize };
	mxArray *var = mxCreateCharArray(2, dims);
	mxChar* start_of_pr = (mxChar *) mxGetData(var);
	for (size_t i = 0; i < aArraySize; ++i)
		*start_of_pr++ = *aValue++;

	return var;
}
template<typename T>
mxArray* serialize(T const& aValue)
{
	return serialize<T>(&aValue);
}
template<typename T>
inline mxArray* serialize(std::vector<T> const& aObject)
{
	return serialize(aObject.empty() ?
	NULL :
										&aObject.at(0), aObject.size());
}

#ifdef SHARE_BUFFER_DEFINED
template<typename T>
inline mxArray* serialize(NSHARE::CPODBuffer<T> const& aObject)
{
	return serialize(aObject.empty()?
			NULL:
			&aObject.at(0), aObject.size());
}
template<typename T>
inline mxArray* serialize(NSHARE::CBuffer const& aObject)
{
	return serialize(aObject.empty()?
			NULL:
			&aObject.at(0), aObject.size());
}
#endif

template<>
inline mxArray* serialize<std::string>(std::string const& aObject)
{
	typedef char T;

	if (aObject.empty())
		return NULL;

	const mwSize dims[] =
			{ 1, aObject.length() };

	mxArray *var = mxCreateCharArray(2, dims);
	mxChar* start_of_pr = (mxChar *) mxGetData(var);

	std::string::const_iterator _it(aObject.begin()),
			_it_end(aObject.end());

	for (; _it != _it_end; ++_it, ++start_of_pr)
		*start_of_pr = static_cast<mxChar>(*_it);

	return var;
}

template<>
inline mxArray* serialize<NSHARE::CText>(NSHARE::CText const& aObject)
{
	typedef char T;
	if (aObject.empty())
		return NULL;

	const mwSize dims[] =
			{ 1, aObject.length() };

	mxArray *var = mxCreateCharArray(2, dims);
	mxChar* start_of_pr = (mxChar *) mxGetData(var);

	NSHARE::CText::const_iterator _it(aObject.begin()),
			_it_end(aObject.end());

	for (; _it != _it_end; ++_it, ++start_of_pr)
		*start_of_pr = static_cast<mxChar>(*_it);

	return var;
}

template<typename T>
bool deserialize(T* aTo, mxArray const * aFrom, size_t aArraySize,
		size_t aArrayIndex)
{
	using namespace std;

	if(aFrom == NULL)
		return false;

	typedef matlab_code_t<T> matlab_api_t;

	typedef typename matlab_api_t::matlab_t matlab_t;
	bool const _is = sizeof(T) == sizeof(matlab_t);

	mxClassID const category = mxGetClassID(aFrom);
	bool const _is_valid = category == matlab_api_t::value;

	DCHECK(_is_valid) << (unsigned) category << " != "
			<< (unsigned) matlab_api_t::value;

	if (!_is_valid)
	{
		mexErrMsgIdAndTxt("InvalidDataType", "Invalid type of data. Need %s",
				matlab_api_t::type());
		return false;
	}

	size_t const total_num_of_elements =
			static_cast<size_t>( mxGetNumberOfElements(aFrom));
	size_t const _size = min(total_num_of_elements, aArraySize);
	DCHECK_EQ(total_num_of_elements, aArraySize);

	if (aArraySize != total_num_of_elements)
		mexWarnMsgIdAndTxt("ArrayIsSmall",
				"Invalid array len. Require %d ",
				aArraySize);

#if MX_HAS_INTERLEAVED_COMPLEX

	typedef typename matlab_api_t::complex_t complex_t;
	if (mxIsComplex(aFrom))
	{
		mexWarnMsgIdAndTxt("ComplexDataIsUsed",
				"Complex data type is used");

		complex_t const * pc = matlab_api_t::sMGetComplex(aFrom);

		for (size_t index = 0; index < _size;
				++index,
						++aTo,
						++pc
				)
			*aTo = pc->real;
	}
	else
	{
		matlab_t const* p = matlab_api_t::sMGet(aFrom);
		if (_is)
			memcpy(aTo, p, sizeof(T) * _size);
		else
			for (size_t index = 0; index < _size;
					++index,
							++aTo,
							++p
					)
				*aTo = *p;
	}
#else
	if(_is)
	memcpy(aTo, mxGetData(aFrom), sizeof(T) * _size);
	else
	{
		matlab_t const* p= static_cast<matlab_t const*> (mxGetData(aFrom));
		for (size_t index=0; index< _size;
				++index,
				++aTo,
				++p
		)
		*aTo = *p;
	}
#endif
	return true;
}
} //namespace matlab
} //namespace NSHARE
namespace std
{
template<typename T>
inline std::ostream& operator<<(std::ostream & aStream,
		NSHARE::matlab::matlab_code_t<T> const& aVal)
{
	aStream << aVal.type();
	return aStream;
}
}
#endif /* MATLAB_SERIALIZE_H */
