/*
 * This file is part of the Soletta Project
 *
 * Copyright (C) 2015 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <nan.h>
#include "sol-js-pwm.h"
#include "../common.h"

using namespace v8;

bool c_sol_pwm_config(v8::Local<v8::Object> jsPWMConfig, sol_pwm_config *p_config) {
    sol_pwm_config config = {
#ifdef SOL_PWM_CONFIG_API_VERSION
        SOL_PWM_CONFIG_API_VERSION,
#endif /* def SOL_PWM_CONFIG_API_VERSION */
        0, 0, SOL_PWM_ALIGNMENT_LEFT,
        SOL_PWM_POLARITY_NORMAL, false };

    VALIDATE_AND_ASSIGN(config, alignment,  sol_pwm_alignment, IsUint32,
                        "(PWM Alignment)", false, jsPWMConfig,
                        Uint32Value);

    VALIDATE_AND_ASSIGN(config, duty_cycle_ns,  uint32_t, IsUint32,
                        "(PWM Duty Cycle)", false, jsPWMConfig,
                        Uint32Value);

    VALIDATE_AND_ASSIGN(config, period_ns,  uint32_t, IsUint32,
                        "(PWM Period)", false, jsPWMConfig,
                        Uint32Value);

    VALIDATE_AND_ASSIGN(config, enabled, bool, IsBoolean,
                        "(PWM enabled)", false, jsPWMConfig,
                        BooleanValue);

    VALIDATE_AND_ASSIGN(config, polarity, sol_pwm_polarity, IsUint32,
                        "(PWM polarity)", false, jsPWMConfig,
                        Uint32Value);

    memcpy(p_config, &config, sizeof(sol_pwm_config));

    return true;
}
