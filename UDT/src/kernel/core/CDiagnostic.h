/*
 * CDiagnostic.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 10.08.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CDIAGNOSTIC_H_
#define CDIAGNOSTIC_H_

namespace NUDT
{

class CDiagnostic:public NSHARE::CSingleton<CDiagnostic>
{
public:
	static const NSHARE::CText NAME;
	static const NSHARE::CText REFFRENCE_PREFIX;
	static const NSHARE::CText REFFRENCE_TO;
	static const NSHARE::CText REFFRENCE_NAME;

	CDiagnostic();
	~CDiagnostic();
	NSHARE::CConfig MSerialize() const;
	NSHARE::CConfig MSerialize(NSHARE::CText const& aName,bool aDeep=true) const;//with dependency
	void MAddState(IState*,NSHARE::CText const& aName);
	void MRemoveState(IState*);
private:
	typedef std::multimap<NSHARE::CText,IState*> states_t;
	typedef std::map<IState*,NSHARE::CText> states_by_pointer_t;
	void MAddStateTo( NSHARE::CConfig& _rval,
			IState* const _state) const;

	mutable NSHARE::CMutex FMutex;//todo erase mutex
	states_t FStates;
	states_by_pointer_t FStatesByPtr;
};

} /* namespace NUDT */
#endif /* CDIAGNOSTIC_H_ */
