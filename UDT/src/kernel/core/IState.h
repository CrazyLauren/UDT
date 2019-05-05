/*
 * IState.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 10.08.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef ISTATE_H_
#define ISTATE_H_

namespace NUDT
{
/*! \brief This class provides API
 * for serializing object state.
 *
 */
class IState
{
public:
	/*! \brief The destructor autoremoves
	 * pointer from #NUDT::CCore object
	 *
	 */
	virtual ~IState();

	/*!\brief Serialize object
	 *
	 * The key of serialized object is #NAME
	 *
	 *\return Serialized object.
	 */
	virtual NSHARE::CConfig MSerialize() const=0;

	/*!\brief Serialize reference to object
	 *
	 * It's need to decrease the number of
	 * serialized subobjects of object.
	 *
	 *\return Serialized reference to object.
	 */
	NSHARE::CConfig MSerializeRef() const;
protected:

	/*! \brief The constructor is passed
	 * pointer to NUDT::CCore::MAddState method
	 *
	 *\param aName -A non-unique name
	 */
	IState(NSHARE::CText const& aName);
};
} /* namespace NUDT */
#endif /* ISTATE_H_ */
