/*
 * CBarrier.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 27.04.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CBARRIER_H_
#define CBARRIER_H_

namespace NSHARE
{
/**\brief This class provides an object-oriented thread barrier interface
 *
 */
class SHARE_EXPORT CBarrier: CDenyCopying
{
public:
	CBarrier(int aNum=0);
	~CBarrier();

	/**\brief Block the caller until all count threads have called wait
	 *
	 */
	bool MWait(unsigned  aNum=0);

    /**\brief Aborting the wait of all waiting threads
     *
     */
	bool MShutDown ();

	/**\brief Reset the barrier to it's original state.
	*
	*/
	void MReset();

	static bool sMUnitTest();
private:
	struct CImpl;
	CImpl* FPImpl;
};

} /* namespace NSHARE */

#endif /* CBARRIER_H_ */
