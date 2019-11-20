/*
 * Ruby SRT - Ruby binding for Secure, Reliable, Transport
 * Copyright (c) 2019 Klaas Speller, Recce.
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 */

/**
 * A Ruby wrapper for SRT (Secure Reliable Transport)
 * 
 * @author: Klaas Speller <klaas@recce.nl>
 */

#ifndef RBSRT_STATS_H
#define RBSRT_STATS_H

#include <ruby/ruby.h>

void RBSRT_stat_init(VALUE srt_module);

#endif /* RBSRT_STATS_H */