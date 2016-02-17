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

#include <node.h>
#include <nan.h>

#include <sol-spi.h>

#include "../common.h"
#include "../data.h"
#include "../hijack.h"
#include "../structures/sol-js-spi.h"
#include "../structures/handles.h"

using namespace v8;

NAN_METHOD(bind_sol_spi_open)
{
    VALIDATE_ARGUMENT_COUNT(info, 2);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsInt32);
    VALIDATE_ARGUMENT_TYPE(info, 1, IsObject);

    sol_spi_config config;
    sol_spi *spi = NULL;

    if (!c_sol_spi_config(info[1]->ToObject(), &config)) {
        Nan::ThrowError("Unable to extract sol_spi_config\n");
        return;
    }

    spi = sol_spi_open(info[0]->Int32Value(), &config);
    if (spi) {
        info.GetReturnValue().Set(js_sol_spi(spi));
    }
}

NAN_METHOD(bind_sol_spi_close)
{
    VALIDATE_ARGUMENT_COUNT(info, 1);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);

    sol_spi *spi = NULL;

    if (!c_sol_spi(Local<Array>::Cast(info[0]), &spi)) {
        return;
    }
    sol_spi_close(spi);
}

static void sol_spi_transfer_cb(void *cb_data, struct sol_spi *spi,
                                const uint8_t *tx, uint8_t *rx,
                                ssize_t status)
{
    Nan::HandleScope scope;
    Nan::Callback *callback = (Nan::Callback *)cb_data;
    if (!callback)
        return;

    Local <Object> buf;

    if (status >= 0)
        buf = Nan::NewBuffer((char *)rx, status).ToLocalChecked();

    Local<Value> arguments[3] = {
        js_sol_spi(spi),
        buf,
        Nan::New((int)status)
    };

    callback->Call(3, arguments);
    delete callback;
    free((uint8_t *)tx);
    free(rx);
    hijack_unref();
}

NAN_METHOD(bind_sol_spi_transfer)
{
    VALIDATE_ARGUMENT_COUNT(info, 3);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 1, IsObject);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 2, IsFunction);

    uint8_t *outputBuffer = (uint8_t *) 0;
    sol_spi *spi = NULL;
    size_t length;

    if (!c_sol_spi(Local<Array>::Cast(info[0]), &spi))
        return;

    if (!node::Buffer::HasInstance(info[1])) {
        Nan::ThrowTypeError("Argument 1 must be a node Buffer");
        return;
    }

    length = node::Buffer::Length(info[1]);

    outputBuffer = (uint8_t *) malloc(length * sizeof(uint8_t));
    if (!outputBuffer) {
        Nan::ThrowError("Failed to allocate memory for output buffer");
        return;
    }

    memcpy(outputBuffer, node::Buffer::Data(info[1]), length);

    uint8_t *inputBuffer = (uint8_t *) malloc(length * sizeof(uint8_t));
    Nan::Callback *callback =
            new Nan::Callback(Local<Function>::Cast(info[2]));
    bool returnValue =
            sol_spi_transfer(spi, outputBuffer, inputBuffer, length,
                             sol_spi_transfer_cb, callback);

    if (!returnValue) {
        delete callback;
    } else {
        hijack_ref();
    }

    info.GetReturnValue().Set(Nan::New(returnValue));
}
