/*
 * random_value.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 02.02.2019
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef RANDOM_VALUE_H_
#define RANDOM_VALUE_H_

namespace NSHARE
{
/**\brief Проверка доступности генератора случайных чисел
 *
 *\return true - ГСЧ доступен
 */
extern SHARE_EXPORT bool is_RNG_available();

/**\brief возвращает случайное число сгенерированное ГСЧ
 *
 *\return 0 - число
 */
extern SHARE_EXPORT unsigned int get_random_value_by_RNG();

/**\brief генерирует новый seed
 *
 *	При первом запуске генерирует новый seed,
 *	если  aForce true seed генерируется заново
 *
 *\param aForce - сгенерировать новый seed
 */
extern SHARE_EXPORT void generate_seed_if_need(bool aForce=false);
}

#endif /* RANDOM_VALUE_H_ */
