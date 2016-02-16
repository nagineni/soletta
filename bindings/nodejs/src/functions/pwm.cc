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

#include <sol-pwm.h>

#include <v8.h>
#include <node.h>
#include <nan.h>

#include "../common.h"
#include "../structures/handles.h"
#include "../structures/sol-js-pwm.h"

using namespace v8;

NAN_METHOD(bind_sol_pwm_open) {
    VALIDATE_ARGUMENT_COUNT(info, 3);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 0, IsInt32);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 1, IsInt32);
    VALIDATE_ARGUMENT_TYPE(info, 2, IsObject);

    int device;
    int channel;
    sol_pwm_config config;
    sol_pwm *pwm = NULL;

    device = info[0]->Int32Value();
    channel = info[1]->Int32Value();

    if (!c_sol_pwm_config(info[2]->ToObject(), &config)) {
        Nan::ThrowError("Unable to extract sol_pwm_config\n");
        return;
    }

    pwm = sol_pwm_open(device, channel, &config);
    if ( pwm ) {
        info.GetReturnValue().Set(js_sol_pwm(pwm));
    }
}

NAN_METHOD(bind_sol_pwm_open_raw) {
    VALIDATE_ARGUMENT_COUNT(info, 3);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 0, IsInt32);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 1, IsInt32);
    VALIDATE_ARGUMENT_TYPE(info, 2, IsObject);

    int device;
    int channel;
    sol_pwm_config config;
    sol_pwm *pwm = NULL;

    device = info[0]->Int32Value();
    channel = info[1]->Int32Value();

    if (!c_sol_pwm_config(info[2]->ToObject(), &config)) {
        Nan::ThrowError("Unable to extract sol_pwm_config\n");
        return;
    }

    pwm = sol_pwm_open_raw(device, channel, &config);
    if ( pwm ) {
        info.GetReturnValue().Set(js_sol_pwm(pwm));
    }
}

NAN_METHOD(bind_sol_pwm_open_by_label) {
    VALIDATE_ARGUMENT_COUNT(info, 2);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 0, IsString);
    VALIDATE_ARGUMENT_TYPE(info, 1, IsObject);

    sol_pwm_config config;
    sol_pwm *pwm = NULL;

    if (!c_sol_pwm_config(info[1]->ToObject(), &config)) {
        Nan::ThrowError("Unable to extract sol_pwm_config\n");
        return;
    }

    pwm = sol_pwm_open_by_label((const char *)*String::Utf8Value(info[0]),
                                &config);

    if ( pwm ) {
        info.GetReturnValue().Set(js_sol_pwm(pwm));
    }
}

NAN_METHOD(bind_sol_pwm_close) {
    VALIDATE_ARGUMENT_COUNT(info, 1);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);

    sol_pwm *pwm = NULL;

    if (!c_sol_pwm(Local<Array>::Cast(info[0]), &pwm)) {
    	return;
    }

    sol_pwm_close(pwm);
}

NAN_METHOD(bind_sol_pwm_set_enabled) {
    VALIDATE_ARGUMENT_COUNT(info, 2);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);
    VALIDATE_ARGUMENT_TYPE(info, 1, IsBoolean);

    bool value;
    sol_pwm *pwm = NULL;

    if (!c_sol_pwm(Local<Array>::Cast(info[0]), &pwm)) {
    	return;
    }

    value = info[1]->BooleanValue();

    info.GetReturnValue().Set(Nan::New(sol_pwm_set_enabled(pwm, value)));
}

NAN_METHOD(bind_sol_pwm_set_period) {
    VALIDATE_ARGUMENT_COUNT(info, 2);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);
    VALIDATE_ARGUMENT_TYPE(info, 1, IsUint32);

    uint32_t value;
    sol_pwm *pwm = NULL;

    if (!c_sol_pwm(Local<Array>::Cast(info[0]), &pwm)) {
    	return;
    }

    value = info[1]->Uint32Value();

    info.GetReturnValue().Set(Nan::New(sol_pwm_set_period(pwm, value)));
}

NAN_METHOD(bind_sol_pwm_set_duty_cycle) {
    VALIDATE_ARGUMENT_COUNT(info, 2);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);
    VALIDATE_ARGUMENT_TYPE(info, 1, IsUint32);

    uint32_t value;
    sol_pwm *pwm = NULL;

    if (!c_sol_pwm(Local<Array>::Cast(info[0]), &pwm)) {
    	return;
    }

    value = info[1]->Uint32Value();

    info.GetReturnValue().Set(Nan::New(sol_pwm_set_duty_cycle(pwm, value)));
}


NAN_METHOD(bind_sol_pwm_get_enabled) {
    VALIDATE_ARGUMENT_COUNT(info, 1);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);

    sol_pwm *pwm = NULL;

    if (!c_sol_pwm(Local<Array>::Cast(info[0]), &pwm)) {
    	return;
    }

    info.GetReturnValue().Set(Nan::New(sol_pwm_get_enabled(pwm)));
}

NAN_METHOD(bind_sol_pwm_get_period) {
    VALIDATE_ARGUMENT_COUNT(info, 1);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);

    sol_pwm *pwm = NULL;

    if (!c_sol_pwm(Local<Array>::Cast(info[0]), &pwm)) {
    	return;
    }

    info.GetReturnValue().Set(Nan::New(sol_pwm_get_period(pwm)));
}

NAN_METHOD(bind_sol_pwm_get_duty_cycle) {
    VALIDATE_ARGUMENT_COUNT(info, 1);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);

    sol_pwm *pwm = NULL;

    if (!c_sol_pwm(Local<Array>::Cast(info[0]), &pwm)) {
    	return;
    }

    info.GetReturnValue().Set(Nan::New(sol_pwm_get_duty_cycle(pwm)));
}
