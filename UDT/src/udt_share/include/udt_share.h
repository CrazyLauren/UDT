/*
 * udt_share.h
 *
 *  Created on: 30.04.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef UDT_SHARE_H_
#define UDT_SHARE_H_
#include <udt_share_macros.h>
#include <shared_types.h>

namespace NUDT
{
struct user_data_t;
struct user_data_dg_t;
struct user_data_info_t;

extern UDT_SHARE_EXPORT program_id_t const& get_my_id();
extern UDT_SHARE_EXPORT bool is_id_initialized();
extern UDT_SHARE_EXPORT int init_id(char const *aName,eType aType,NSHARE::version_t const& aVer);

extern UDT_SHARE_EXPORT bool fill_dg_head(void* aWhat,size_t aFullSize,const program_id_t& aFrom,bool aIsNeedCrc =false);
extern UDT_SHARE_EXPORT bool deserialize(user_data_t& aTo,
		const user_data_dg_t*, NSHARE::IAllocater*);
extern UDT_SHARE_EXPORT std::pair<size_t, size_t> deserialize_dg_head(
		user_data_info_t& aTo, NSHARE::CBuffer::const_pointer aFrom);

extern UDT_SHARE_EXPORT bool serialize(NSHARE::CBuffer* aTo,const user_data_info_t& aWhat,size_t aSize);
extern UDT_SHARE_EXPORT size_t serialize(NSHARE::CBuffer* aTo,const user_data_t& aWhat,bool aIsNeedCrc =false);
extern  size_t UDT_SHARE_EXPORT get_full_size(user_data_info_t const& aData);
extern  size_t UDT_SHARE_EXPORT get_full_size(user_data_t const& aData);
extern  size_t UDT_SHARE_EXPORT fill_header(NSHARE::CBuffer::pointer  aTo,user_data_info_t const& aData,NSHARE::CBuffer::offset_pointer_t aOffset);

template<class aKdTypeY,class T>
inline size_t serialize(NSHARE::CBuffer* _buf,const T& aWhat, const routing_t& aRoute,error_info_t const&aError)
{
	NSHARE::CConfig _conf(aWhat.MSerialize());
	if (!aRoute.empty())
	{
		VLOG(4) << "Add route " << aRoute;
		DCHECK(aRoute.MIsValid());
		_conf.MAdd(aRoute.MSerialize());
	}
	if (aError.MIsValid())
	{
		VLOG(4)<<"Add error "<<aError;
		_conf.MAdd(/*"error_",*/aError.MSerialize());
	}

	NSHARE::CText _text;
	_conf.MToJSON(_text);


	const size_t _str_size=_text.length_code();
	const size_t full_size = sizeof(aKdTypeY) + _str_size+1;
	size_t const _begin = _buf->size();

	_buf->resize(_begin + full_size);
	CHECK_LE(full_size, _buf->size());

	NSHARE::CBuffer::value_type* const _p_begin = (NSHARE::CBuffer::value_type*) _buf->ptr()
			+ _begin;
	NSHARE::CBuffer::value_type* _p = _p_begin;

	aKdTypeY* _kd=new (_p) aKdTypeY;

	if(!aRoute.empty())
		_kd->MSet(aKdTypeY::E_ROUTE_OF_MSG,true);
	if(aError.MIsValid())
		_kd->MSet(aKdTypeY::E_ERROR_OF_MSG,true);

	_kd->MSetStrSize (static_cast<uint16_t>(_str_size+1));
	_p+=sizeof(aKdTypeY);
	memcpy(_p,_text.c_str(),_str_size);
	_p+=_str_size;
	*_p++ = '\0';

	fill_dg_head(_p_begin, full_size,get_my_id());
	CHECK_EQ(full_size,
			reinterpret_cast<aKdTypeY*>(_p_begin)->FHeadSize
					+ reinterpret_cast<aKdTypeY*>(_p_begin)->MGetDataSize());
	VLOG(2) << "Serialized DG  "
						<< *reinterpret_cast<aKdTypeY*>(_p_begin);
	return full_size;
}
inline void deserialize_route_impl(NSHARE::CConfig  const& aConf,routing_t* aRoute)
{
	if (aRoute)
		{
			*aRoute = routing_t(aConf.MChild(routing_t::NAME));
			DCHECK(aRoute->MIsValid());
		}
}
inline void deserialize_error_impl(NSHARE::CConfig  const& aConf,error_info_t* aError)
{
	if (aError)
	{
		VLOG(2) <<"Deserialize error";
		*aError = error_info_t(aConf.MChild(error_info_t::NAME));
		DCHECK(aError->MIsValid());
	}
}
template<class Tto>
inline Tto deserialize_impl(NSHARE::CConfig  const& aConf,routing_t* aRoute,error_info_t *aError)
{
	NSHARE::CConfig const& _conf=aConf.MChild(Tto::NAME);

	deserialize_route_impl(_conf, aRoute);
	deserialize_error_impl(_conf, aError);

	return Tto(_conf);
}
template<>
inline program_id_t deserialize_impl<program_id_t>(NSHARE::CConfig  const& aConf,routing_t* aRoute,error_info_t *aError)
{
	NSHARE::CConfig const& _conf=aConf.MChild(program_id_t::NAME);
	deserialize_route_impl(_conf, aRoute);
	deserialize_error_impl(_conf, aError);

	return program_id_t(_conf);
}

template<class aKdTypeY,class Tto>
inline Tto deserialize(aKdTypeY const* aP,routing_t* aRoute,error_info_t *aError)
{
	VLOG(2) << "Deserializing DG  "<<aKdTypeY::MSG_TYPE;

	NSHARE::utf8 const* _begin = (NSHARE::utf8 const*) aP->MStrBegin();
	size_t const _str_size=aP->MGetStrSize();
	NSHARE::CText _text(_begin,_str_size-1);//-1 - '\0'
	CHECK(!_text.empty());
	CHECK_EQ(_text.length_code(),_str_size-1);
	NSHARE::CConfig _conf;
	_conf.MFromJSON(_text);
	CHECK(!aP->MIs(aKdTypeY::E_ROUTE_OF_MSG) || (aRoute != NULL));
	CHECK(!aP->MIs(aKdTypeY::E_ERROR_OF_MSG) || (aError != NULL));

	return deserialize_impl<Tto>(_conf,aP->MIs(aKdTypeY::E_ROUTE_OF_MSG)?aRoute:NULL,aP->MIs(aKdTypeY::E_ERROR_OF_MSG)?aError:NULL);
}

}
#endif /* UDT_SHARE_H_ */
