/**
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.  If a copy of the MPL
 * was not distributed with this file, you can obtain one at
 * https://mozilla.org/MPL/2.0/.
 *
 * Copyright William Grim, 2015
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define MHZ 1000000U
#if F_CPU < MHZ
#  error F_CPU must be >= 1000000
#endif

/**
 * Converts the number of microseconds given to the nearest number of
 * clock cycles necessary to reach it, rounded down to the nearest clock.
 */
#define us_clocks(us, cs) ((us) / ((F_CPU / MHZ) >> us_clocks_ ## cs))
#define us_clocks_Prescaled_1 0
#define us_clocks_Prescaled_8 3
#define us_clocks_Prescaled_64 5
#define us_clocks_Prescaled_256 8
#define us_clocks_Prescaled_1024 10

#ifdef __cplusplus
} // extern "C"
#endif
