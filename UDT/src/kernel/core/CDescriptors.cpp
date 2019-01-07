/*
 * CDescriptors.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 18.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <share_socket.h>
#include <vector>
#include "IState.h"
#include "CDescriptors.h"
#include "CDataObject.h"

using namespace NSHARE;
template<>
NUDT::CDescriptors::singleton_pnt_t NUDT::CDescriptors::singleton_t::sFSingleton =
		NULL;

namespace NUDT
{
const NSHARE::CText CDescriptors::NAME="desc";
const NSHARE::CText CDescriptors::DESCRIPTOR_NAME="kerd";

//const descriptor_t CDescriptors::MAX = 255;
const descriptor_t CDescriptors::INVALID = -1;
CDescriptors::CDescriptors() :
		IState(NAME),FLast(1) //1 - minimal  value of descriptor
{
	//MIncreaseNumberOfFree();
}
//void CDescriptors::MIncreaseNumberOfFree()
//{
//	descriptor_t _max = FLastSize + MAX;
//	for (descriptor_t i = FLastSize; i < _max; ++i)
//	{
//		FFree.insert(i);
//	}
//	FLastSize = _max;
//}
int CDescriptors::MCreate()
{
	VLOG(2) << "Open descriptor";
	CRAII<CMutex> _block(FBLock);
	descriptor_t _new = MGetFree();
	VLOG(2) << "New descriptor is  " << _new;
	FDescriptors[_new] = smart_info_t();

	create_descriptor _val(_new);
	CDataObject::sMGetInstance().MPush(_val);

	return _new;
}

void CDescriptors::MOpenInfo(d_list_t::iterator _it,
		const descriptor_info_t& aInfo, descriptor_t aVal)
{	
	_it->second = aInfo;
	FByUUIDs[aInfo.FProgramm.FId.FUuid] = _it;
	open_descriptor const _val(aVal, aInfo);
	CDataObject::sMGetInstance().MPush(_val);
}

bool CDescriptors::MOpen(descriptor_t aVal, const descriptor_info_t& aInfo)
{
	CRAII<CMutex> _block(FBLock);
	d_list_t::iterator _it = FDescriptors.find(aVal);
	if (_it == FDescriptors.end())
	{
		LOG(ERROR)<<"The "<<aVal<<" descriptor is not exist.";
		return false;
	}
	MOpenInfo(_it, aInfo, aVal);
	return true;
}
int CDescriptors::MGetFree()
{
//	if (FFree.empty())
//	{
//		MIncreaseNumberOfFree();
//	}
//	int _new = *FFree.begin();
//	FFree.erase(_new);
//	return _new;
	return ++FLast;
}

void CDescriptors::MCloseInfo(const descriptor_info_t& _info, descriptor_t aVal)
{
	FByUUIDs.erase(_info.FProgramm.FId.FUuid);
	close_descriptor _val(aVal, _info);
	CDataObject::sMGetInstance().MPush(_val);
}

void CDescriptors::MClose(descriptor_t aVal)
{
	VLOG(2) << "Close descriptor " << aVal;
	CRAII<CMutex> _block(FBLock);
	d_list_t::iterator _it = FDescriptors.find(aVal);
	if (_it == FDescriptors.end())
	{
		LOG(ERROR)<<"The "<<aVal<<" descriptor is not exist.";
		return;
	}
	smart_info_t const _info = _it->second;
	FDescriptors.erase(_it);
	//FFree.insert(aVal);

	if (_info.MIs())
	{
		MCloseInfo(_info.MGetConst(), aVal);
	}

	destroy_descriptor _val(aVal);
	CDataObject::sMGetInstance().MPush(_val);
}
bool CDescriptors::MIs(descriptor_t aVal) const
{
	VLOG(2) << "Is descriptor " << aVal;
	CRAII<CMutex> _block(FBLock);
	return FDescriptors.find(aVal) != FDescriptors.end();
}
bool CDescriptors::MIs(const NSHARE::uuid_t& aVal) const
{
	VLOG(2) << "Is descriptor " << aVal;
	CRAII<CMutex> _block(FBLock);
	return FByUUIDs.find(aVal) != FByUUIDs.end();
}

descriptor_t CDescriptors::MGet(const NSHARE::uuid_t& aVal) const
{
	CRAII<CMutex> _block(FBLock);
	VLOG(2) << "Get by " << aVal;
	uuid_list_t::const_iterator _it = FByUUIDs.find(aVal);
	if (_it != FByUUIDs.end())
		return _it->second->first;
	else
	{
		VLOG(2) << "There is not descriptor for " << aVal;
		return INVALID;
	}
}
descriptor_t CDescriptors::MGet(const descriptor_info_t& aVal) const
{
	CRAII<CMutex> _block(FBLock);
	d_list_t::const_iterator _it = FDescriptors.begin();
	for (; _it != FDescriptors.end(); ++_it)
	{
		if (!(aVal == _it->second))
			return _it->first;
	}
	return INVALID;
}
bool CDescriptors::MIsInfo(descriptor_t aVal) const
{
	CRAII<CMutex> _block(FBLock);
	d_list_t::const_iterator const _it = FDescriptors.find(aVal);
	if (_it != FDescriptors.end() && _it->second.MIs())
	{
		return true;
	}
	return false;
}
std::pair<descriptor_info_t, bool>  CDescriptors::MGet(
		descriptor_t const& aVal) const
{
	CRAII<CMutex> _block(FBLock);
	std::pair<descriptor_info_t, bool> _rval;
	_rval.second = false;
	d_list_t::const_iterator _it = FDescriptors.find(aVal);
	if (_it != FDescriptors.end() && _it->second.MIs())
	{
		_rval.first = _it->second;
		_rval.second = true;
	}
	return _rval;
}
void CDescriptors::MGetAll(d_list_t& aTo) const
{
	CRAII<CMutex> _block(FBLock);
	aTo = FDescriptors;
}
bool CDescriptors::MIsCustomer(descriptor_t const& aVal) const
{
	CRAII<CMutex> _block(FBLock);
	d_list_t::const_iterator _it = FDescriptors.find(aVal);
	if (_it != FDescriptors.end() && _it->second.MIs())
		return _it->second.MGetConst().FProgramm.FType == E_CONSUMER;
	return false;
}
CDescriptors::d_list_t CDescriptors::MGetAll(eType aType) const
{
	CRAII<CMutex> _block(FBLock);
	d_list_t _list;
	d_list_t::const_iterator _it = FDescriptors.begin();

	for (; _it != FDescriptors.end(); ++_it)
		if (_it->second.MIs() && _it->second.MGetConst().FProgramm.FType == aType)
			_list.insert(*_it);
	return _list;
}

bool CDescriptors::sMIsValid(descriptor_t aVal)
{
	return aVal >= 0;
}
NSHARE::CConfig CDescriptors::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	//_conf.MAdd("Max", MAX);
	_conf.MAdd("last", FLast);

	CRAII<CMutex> _block(FBLock);
	d_list_t::const_iterator _it = FDescriptors.begin(), _it_end(
			FDescriptors.end());

	for (; _it != _it_end; ++_it)
	{
		NSHARE::CConfig _c_id(DESCRIPTOR_NAME);
		_c_id.MAdd("numd",_it->first);
		if(_it->second.MIs())
		{
			//_c_id.MAdd("info",_it->second.MGetConst().MSerialize());
			_c_id.MAdd(_it->second.MGetConst().MSerialize());
		}
		_conf.MAdd(_c_id);
	}
	return _conf;
}
}
