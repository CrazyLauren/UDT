/*
 * FloatCompare.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 16.06.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef FLOATCOMPARE_H_
#define FLOATCOMPARE_H_

#include <limits>
namespace NSHARE
{
template <class T>
inline bool approximatelyEqual(T a, T b, T epsilon=std::numeric_limits<T>::epsilon())
{
    return fabs(a - b) <= ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}
template <class T>
inline bool essentiallyEqual(T a, T b, T epsilon=std::numeric_limits<T>::epsilon())
{
    return fabs(a - b) <= ( (fabs(a) > fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}
template <class T>
inline bool definitelyGreaterThan(T a, T b, T epsilon=std::numeric_limits<T>::epsilon())
{
    return (a - b) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}
template <class T>
inline bool definitelyGreaterOrEqualThan(T a, T b, T epsilon=std::numeric_limits<T>::epsilon())
{
    return (a - b) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon)|| approximatelyEqual(a,b,epsilon);
}
template <class T>
inline bool definitelyLessThan(T a, T b, T epsilon=std::numeric_limits<T>::epsilon())
{
    return (b - a) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}
template <class T>
inline bool definitelyLessOrEqualThan(T a, T b, T epsilon=std::numeric_limits<T>::epsilon())
{
    return (b - a) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon) || approximatelyEqual(a,b,epsilon);
}
}
#endif /* FLOATCOMPARE_H_ */
