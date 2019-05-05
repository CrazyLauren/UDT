/*
 * CCore.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 10.08.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CDIAGNOSTIC_H_
#define CDIAGNOSTIC_H_

#include "ICore.h"
namespace NUDT
{

class CCore:public NSHARE::CSingleton<CCore>
{
public:
	static const NSHARE::CText NAME;
	static const NSHARE::CText REFFRENCE_PREFIX;
	static const NSHARE::CText REFFRENCE_TO;
	static const NSHARE::CText REFFRENCE_NAME;

	/*! default constructor
	 *
	 */
	CCore();

	/*! default destructor
	 *
	 */
	~CCore();

	/*!\brief Serialize all objects
	 *
	 * The key of serialized object is #NAME
	 *
	 *\return Serialized object.
	 *\warning low-productive process
	 */
	NSHARE::CConfig MSerialize() const;

	/*!\brief Serialize the object by its name
	 *
	 *\param aName non-unique name of object
	 *\param aDeep Serialize subobject too, otherwise
	 * insert the reference to it
	 *
	 *\return Serialized object.
	 */
	NSHARE::CConfig MSerialize(NSHARE::CText const& aName,
			bool aDeep = true) const;

	/*!\brief Save information about core
	 *
	 *\param aName An unique name of object
	 *\param aCore A pointer to object
	 *
	 *\return true if successfully
	 */
	bool MAddState(ICore* aCore, NSHARE::CText const& aName);

	/*!\brief Remove information about core
	 *
	 *\param aCore A pointer to object
	 *
	 */
	void MRemoveState(ICore* aCore);

	/*!\brief Save information about object
	 * that have state
	 *
	 *\param aName A non-unique name of object
	 *\param aCore A pointer to object
	 *
	 */
	void MAddState(IState*, NSHARE::CText const& aName);

	/*!\brief Remove information about object
	 *
	 *\param aState A pointer to object
	 *
	 */
	void MRemoveState(IState* aState);

	/*!\brief Start all cores
	 *
	 *\return true if  successfully
	 */
	bool MStart();
private:


	/*! Information about pointer to object
	 * which realized NUDT::IState interface
	 *
	 *
	 */
	struct state_object_t
	{
		typedef std::multimap<NSHARE::CText, IState*> states_t;
		typedef std::map<IState*, NSHARE::CText> states_by_pointer_t;

		states_t FStates; ///< info for convertion name -> pointer
		states_by_pointer_t FStatesByPtr; ///< info for convertion pointer -> name
	};
	typedef NSHARE::CSafeData<state_object_t> state_data_t; ///< RWLock for #state_object_t
	typedef state_data_t::RAccess<> const r_state_access; ///< Read only access to #state_object_t
	typedef state_data_t::WAccess<> w_state_access; ///< Read write access to #state_object_t

	/*! Information about pointer to cores
	 * which realized #NUDT::ICore interface
	 *
	 */
	struct core_object_t
	{
		typedef std::map<ICore*, NSHARE::CText> states_t;

		states_t FCores; ///< info for convertion name -> pointer
	};
	typedef NSHARE::CSafeData<core_object_t> cores_data_t; ///< RWLock for #core_object_t
	typedef cores_data_t::RAccess<> const r_core_access; ///< Read only access to #core_object_t
	typedef cores_data_t::WAccess<> w_core_access; ///< Read write access to #core_object_t


	void MAddStateTo( NSHARE::CConfig& _rval,
			IState* const _state) const;

	state_data_t FState; ///< Inforamtion about state
	cores_data_t FCore; ///< Inforamtion about core

};

} /* namespace NUDT */
#endif /* CDIAGNOSTIC_H_ */
