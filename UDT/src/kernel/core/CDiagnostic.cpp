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
#include "IState.h"
#include "CDiagnostic.h"
#include "kernel_type.h"


template<>
NUDT::CDiagnostic::singleton_pnt_t NUDT::CDiagnostic::singleton_t::sFSingleton = NULL;
namespace NUDT
{
using namespace NSHARE;
const NSHARE::CText CDiagnostic::NAME="state";
const NSHARE::CText CDiagnostic::REFFRENCE_PREFIX="_ref";
const NSHARE::CText CDiagnostic::REFFRENCE_TO="_ref_to";
const NSHARE::CText CDiagnostic::REFFRENCE_NAME="_my_ref";
CDiagnostic::CDiagnostic()
{
}

CDiagnostic::~CDiagnostic()
{
}
NSHARE::CConfig CDiagnostic::MSerialize(NSHARE::CText const& aName,bool aDeep) const
{
	NSHARE::CConfig _rval(NAME);
	if(aName==program_id_t::NAME)
	{
		program_id_t const& _info(get_my_id());
		_rval.MAdd(_info.MSerialize());
		return _rval;
	}else if(aName==CParserFactory::NAME)
	{
		_rval.MAdd(static_cast<CParserFactory*>(CParserFactory::sMGetInstancePtr())->MSerialize());
		return _rval;
	}




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
	NSHARE::CRAII<NSHARE::CMutex> _lock(FMutex);
	if (_pointer)
	{
		VLOG(2) << "Search by pointer.";
		states_by_pointer_t::const_iterator _it = FStatesByPtr.find((IState*)_pointer);
		LOG_IF(ERROR,_it==FStatesByPtr.end()) << "Cannot find " << aName << " p="
													<< _pointer;
		if (_it != FStatesByPtr.end())
		{
			IState* const _state = (_it->first);
			MAddStateTo(_rval, _state);
		}
	}
	else
	{
		std::pair<states_t::const_iterator, states_t::const_iterator> _r =
				FStates.equal_range(aName);
		for (; _r.first != _r.second; ++_r.first)
		{
			IState* const _state = (_r.first->second);
			MAddStateTo(_rval, _state);
		}
	}
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

void CDiagnostic::MAddStateTo(NSHARE::CConfig& _rval,
		 IState* const _state) const
{
	VLOG(2)<<"Add state ";
	NSHARE::CConfig const _ser(_state->MSerialize());
	CHECK(!_ser.MKey().empty());
	_rval.MAdd(_ser).MAdd(REFFRENCE_NAME, _state).MValue().insert(0,
			REFFRENCE_PREFIX);
}

NSHARE::CConfig CDiagnostic::MSerialize() const
{
	NSHARE::CConfig _rval(NAME);
	NSHARE::CRAII<NSHARE::CMutex> _lock(FMutex);
	states_by_pointer_t::const_iterator _it = FStatesByPtr.begin(), _it_end(FStatesByPtr.end());
	for (; _it != _it_end; ++_it)
	{
		IState* const _state=(_it->first);
		CHECK_NOTNULL(_state);

		MAddStateTo(_rval,  _state);
	}

	return _rval;
}
void CDiagnostic::MAddState(IState* aVal,NSHARE::CText const& aName)
{
	NSHARE::CRAII<NSHARE::CMutex> _lock(FMutex);
	FStates.insert(states_t::value_type(aName, aVal));
	FStatesByPtr.insert(states_by_pointer_t::value_type(aVal, aName));
}
void CDiagnostic::MRemoveState(IState* aVal)
{
	NSHARE::CRAII<NSHARE::CMutex> _lock(FMutex);
	states_by_pointer_t::iterator _it=FStatesByPtr.find(aVal);
	if (_it != FStatesByPtr.end())
	{
		CText const _name(_it->second);
		VLOG(2)<<"Remove "<<_name;
		FStatesByPtr.erase(_it);
		std::pair<states_t::iterator, states_t::iterator> _r =
				FStates.equal_range(_name);
		for (; _r.first != _r.second && _r.first->second != aVal; ++_r.first)
			;
		CHECK(_r.first != _r.second);
		if (_r.first != _r.second)
			FStates.erase(_r.first);
	}

}

} /* namespace NUDT */
