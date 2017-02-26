/*
 * counter_t.h
 *
 *  Created on: 24.02.2017
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *	
 *	Copyright © 2017 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef COUNTER_T_H_
#define COUNTER_T_H_

namespace NSHARE
{
SHARED_PACKED(struct SHARE_EXPORT counter_t
		{
		public:
			typedef  uint32_t value_type;

			counter_t(value_type const& aVal=value_type(1));

			void MAdd(int const& aVal) const;
			void MIncrement() const;
			value_type MDecrement() const;
			bool MIsOne() const;
			value_type MValue() const;
			void MWrite(value_type const& aVal) const;
		private:
			mutable volatile value_type FCount;
		});
}

#endif /* COUNTER_H_ */
