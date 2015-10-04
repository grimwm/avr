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

struct SerialOptions;

void print_usage(const char *prog);
struct SerialOptions parse_opts(int argc, char *argv[]);

#ifdef __cplusplus
} // extern "C"
#endif
