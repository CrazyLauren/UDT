/*
 * kernel_trype.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 10.02.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <Socket.h>
#include "kernel_type.h"
namespace NUDT
{
const NSHARE::CText descriptor_info_t::NAME = "DescriptorInfo";
const NSHARE::CText create_descriptor::NAME = "CreateDescriptor";
const NSHARE::CText destroy_descriptor::NAME = "DestroyDescriptor";

const NSHARE::CText open_descriptor::NAME = "OpenDescriptor";
const NSHARE::CText close_descriptor::NAME = "CloseDescriptor";
//const NSHARE::CText user_data_t::NAME = "UserData";

template<>
const NSHARE::CText new_id_t::NAME = "NewId";
template<>
const NSHARE::CText user_data_id_t::NAME = "UserDataId";

template<>
const NSHARE::CText demands_id_t::NAME = "DemandsId";
template<>
const NSHARE::CText kernel_infos_array_id_t::NAME = kernel_infos_array_t::NAME+ "Id";
template<>
const NSHARE::CText demand_dgs_for_by_id_t::NAME = demand_dgs_for_t::NAME+ "Id";

template<>
const NSHARE::CText fail_send_by_id_t::NAME = "FailById";





//bool descriptor_info_t::operator<(NSHARE::net_address const& aRht) const
//{
//	if (FTypeLink != E_TCP)
//		return false;
//	return FAddr < aRht;
//}

bool descriptor_info_t::operator<(descriptor_info_t const& aRht) const
{
	return FProgramm < aRht.FProgramm;
}
bool descriptor_info_t::operator==(descriptor_info_t const& aRht) const
{
	return FProgramm == aRht.FProgramm;
}
NSHARE::CConfig descriptor_info_t::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	_conf.MAdd("po", FProgramm.MSerialize());
	_conf.MAdd("info",FInfo);

	return _conf;
}
const NSHARE::CText split_info::NAME = "mtu";
const NSHARE::CText split_info::TYPE = "t";
const NSHARE::CText split_info::SIZE = "s";
split_info::split_info() :
		FMaxSize(0), //
		FCbData(NULL), //
		pMCalculate(NULL)
{
	;
}
split_info::split_info(NSHARE::CConfig const& aConf):
				FMaxSize(0), //
				FCbData(NULL), //
				pMCalculate(NULL)
{
	aConf.MGetIfSet(SIZE, FMaxSize);
	unsigned _val= FType.MGetMask();
	if(aConf.MGetIfSet(TYPE, _val))
		FType.MSetFlag(_val,true);//todo serialize
}
NSHARE::CConfig split_info::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	_conf.MSet(TYPE, FType.MGetMask());
	_conf.MSet(SIZE, FMaxSize);
	return _conf;
}
bool split_info::MIsValid() const
{
	return true;
}

size_t split_info::MDataSize(user_data_t const& aData) const
{
	if (pMCalculate)
		return /*aData.FDataId.FSplit.MGetAlignmentSize(*/
				(*pMCalculate)(aData, FCbData)/*)*/;
	return /*aData.FDataId.FSplit.MGetAlignmentSize(*/aData.FData.size()/*)*/;
}
}