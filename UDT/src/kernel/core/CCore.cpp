// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CDiagnostic.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 10.08.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <udt_share.h>
#include <CParserFactory.h>
#include "CCore.h"
#include "kernel_type.h"

template<>
NUDT::CCore::singleton_pnt_t NUDT::CCore::singleton_t::sFSingleton = NULL;
namespace NUDT
{
using namespace NSHARE;
const NSHARE::CText CCore::NAME="state";
const NSHARE::CText CCore::REFFRENCE_PREFIX="_ref";
const NSHARE::CText CCore::REFFRENCE_TO="_ref_to";
const NSHARE::CText CCore::REFFRENCE_NAME="_my_ref";
CCore::CCore()
{
}

CCore::~CCore()
{
}
NSHARE::CConfig CCore::MSerialize(NSHARE::CText const& aName,bool aDeep) const
{
	NSHARE::CConfig _rval(NAME);

	///1) Checking is request of program name
	if(aName==program_id_t::NAME)
	{
		program_id_t const& _info(get_my_id());
		_rval.MAdd(_info.MSerialize());
		return _rval;
	}

	///2) If request info about reference, deserialize pointer
	void* _pointer = NULL;
	CText::size_type _pos = aName.find(REFFRENCE_PREFIX);
	if (_pos == 0)
	{
		VLOG(2) << "It's prefix.";
		CText const _p_text(aName.substr(REFFRENCE_PREFIX.length()));
		bool const _is = from_string<void*>(_p_text, _pointer);
		LOG_IF(ERROR, !_is) << "Cannot convert " << _p_text << " to pointer";
		if (!_is)
			_pointer = NULL;
	}

	///3) Handling request
	{
		r_state_access _access = FState.MGetRAccess();
		state_object_t::states_by_pointer_t const& FStatesByPtr =
				_access->FStatesByPtr;
		state_object_t::states_t const& FStates = _access->FStates;

		if (_pointer)
		{
			VLOG(2) << "Search by pointer.";
			state_object_t::states_by_pointer_t::const_iterator _it =
					FStatesByPtr.find(
					(IState*) _pointer);
			LOG_IF(ERROR,_it==FStatesByPtr.end()) << "Cannot find " << aName
															<< " p="
															<< _pointer;
			if (_it != FStatesByPtr.end())
			{
				IState* const _state = (_it->first);
				MAddStateTo(_rval, _state);
			}
		}
		else
		{
			std::pair<state_object_t::states_t::const_iterator,
					state_object_t::states_t::const_iterator> _r =
					FStates.equal_range(aName);
			for (; _r.first != _r.second; ++_r.first)
			{
				IState* const _state = (_r.first->second);
				MAddStateTo(_rval, _state);
			}
		}
	}
	///4) Dereferencing reference to child if need
	if(aDeep)
	{
		ConfigSet& _childs=_rval.MChildren();
		ConfigSet::iterator _ch_it=_childs.begin(),_ch_it_end(_childs.end());
		for(;_ch_it!=_ch_it_end;++_ch_it)
		{
			const ConfigSet _set = (_ch_it)->MChildren(REFFRENCE_TO);
			if (!_set.empty())
			{
				ConfigSet::const_iterator _it = _set.begin(), _it_end(
						_set.end());
				for (; _it != _it_end; ++_it)
				{
					_rval.MAdd(MSerialize((_it)->MValue()));
				}
				_ch_it->MRemove(REFFRENCE_TO);
			}
		}
	}
	return _rval;
}

void CCore::MAddStateTo(NSHARE::CConfig& _rval,
		IState* const _state) const
{
	VLOG(2)<<"Add state ";
	NSHARE::CConfig const _ser(_state->MSerialize());
	CHECK(!_ser.MKey().empty());
	_rval.MAdd(_ser).MAdd(REFFRENCE_NAME, _state).MValue().insert(0,
			REFFRENCE_PREFIX);
}
NSHARE::CConfig CCore::MSerialize() const
{
	NSHARE::CConfig _rval(NAME);
	r_state_access _access = FState.MGetRAccess();
	state_object_t::states_by_pointer_t const& FStatesByPtr =
			_access->FStatesByPtr;
	state_object_t::states_t const& FStates = _access->FStates;

	state_object_t::states_by_pointer_t::const_iterator _it =
			FStatesByPtr.begin(), _it_end(FStatesByPtr.end());
	for (; _it != _it_end; ++_it)
	{
		IState* const _state = (_it->first);
		CHECK_NOTNULL(_state);

		MAddStateTo(_rval,  _state);
	}

	return _rval;
}
void CCore::MAddState(IState* aVal, NSHARE::CText const& aName)
{
	VLOG(2) << "Adding state to " << aName << " p=" << aVal;
	w_state_access _access = FState.MGetWAccess();
	_access->FStates.insert(state_object_t::states_t::value_type(aName, aVal));
	_access->FStatesByPtr.insert(
			state_object_t::states_by_pointer_t::value_type(aVal, aName));
}
void CCore::MRemoveState(IState* aVal)
{
	w_state_access _access = FState.MGetWAccess();
	state_object_t::states_by_pointer_t& FStatesByPtr = _access->FStatesByPtr;
	state_object_t::states_t& FStates = _access->FStates;

	VLOG(2) << "Removing state to  p=" << aVal;
	state_object_t::states_by_pointer_t::iterator _it = FStatesByPtr.find(aVal);
	if (_it != FStatesByPtr.end())
	{
		CText const _name(_it->second);
		VLOG(2)<<"Remove "<<_name;
		FStatesByPtr.erase(_it);
		std::pair<state_object_t::states_t::iterator,
				state_object_t::states_t::iterator> _r =
				FStates.equal_range(_name);
		for (; _r.first != _r.second && _r.first->second != aVal; ++_r.first)
			;
		CHECK(_r.first != _r.second);
		if (_r.first != _r.second)
			FStates.erase(_r.first);
	}

}
bool CCore::MStart()
{
	r_core_access _access = FCore.MGetRAccess();
	core_object_t::states_t const& FStatesByPtr = _access->FCores;
	
	core_object_t::states_t::const_iterator _it =
			FStatesByPtr.begin(), _it_end(FStatesByPtr.end());
	for (; _it != _it_end; ++_it)
	{
		ICore* const _state = (_it->first);
		CHECK_NOTNULL(_state);

		_state->MStart();
	}
	return true;
}
bool CCore::MAddState(ICore* aVal, NSHARE::CText const& aName)
{
	VLOG(2) << "Adding core to " << aName << " p=" << aVal;
	w_core_access _access = FCore.MGetWAccess();
	return _access->FCores.insert(
			core_object_t::states_t::value_type(aVal, aName)).second;
}
void CCore::MRemoveState(ICore* aVal)
{
	w_core_access _access = FCore.MGetWAccess();
	core_object_t::states_t& FCores = _access->FCores;

	VLOG(2) << "Removing core to p=" << aVal;
	core_object_t::states_t::iterator _it = FCores.find(aVal);
	if (_it != FCores.end())
	{
		CText const _name(_it->second);
		VLOG(2) << "Remove " << _name;
		FCores.erase(_it);
	}

}
ICore::ICore(NSHARE::CText const& aName) :
		IState(aName)
{
	bool const _is = CCore::sMGetInstance().MAddState(this, aName);
	DCHECK(_is);
}
ICore::~ICore()
{
	CCore::sMGetInstance().MRemoveState(this);
}


} /* namespace NUDT */
