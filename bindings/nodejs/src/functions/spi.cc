/*
 * This file is part of the Soletta Project
 *
 * Copyright (C) 2015 Intel Corporation. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
    Local<Value> buffer;

    if (status >= 0) {
        Local <Object> bufObj;
        bufObj = Nan::NewBuffer((char *)rx, status).ToLocalChecked();
        buffer = bufObj;
    } else {
        buffer = Nan::Null();
    }

    Local<Value> arguments[3] = {
        js_sol_spi(spi),
        buffer,
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
