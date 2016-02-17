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


#include <map>
#include <node.h>
#include <nan.h>

#include <sol-uart.h>

#include "../common.h"
#include "../data.h"
#include "../hijack.h"
#include "../structures/sol-js-uart.h"
#include "../structures/handles.h"

using namespace v8;

// Associate the read callback info with a UART handle
static std::map<sol_uart *, Nan::Callback *> annotation;

NAN_METHOD(bind_sol_uart_open)
{
    VALIDATE_ARGUMENT_COUNT(info, 2);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 0, IsString);
    VALIDATE_ARGUMENT_TYPE(info, 1, IsObject);

    sol_uart_config config;
    sol_uart *uart = NULL;

    if (!c_sol_uart_config(info[1]->ToObject(), &config)) {
        Nan::ThrowError("Unable to extract sol_uart_config\n");
        return;
    }

    Nan::Callback *rx_callback = (Nan::Callback *)config.rx_cb_user_data;
    uart = sol_uart_open((const char *)*String::Utf8Value(info[0]), &config);

    if (!uart) {
        delete rx_callback;
    } else {
        annotation[uart] = rx_callback;
        hijack_ref();

        info.GetReturnValue().Set(js_sol_uart(uart));
    }
}

NAN_METHOD(bind_sol_uart_close)
{
    VALIDATE_ARGUMENT_COUNT(info, 1);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);

    sol_uart *uart = NULL;

    if (!c_sol_uart(Local<Array>::Cast(info[0]), &uart))
        return;

    Nan::Callback *callback = annotation[uart];
    sol_uart_close(uart);
    if (callback) {
        annotation.erase(uart);
        delete callback;
        hijack_unref();
    }
}

static void sol_uart_write_callback(void *data, struct sol_uart *uart,
                                    unsigned char *tx, int status)
{
    Nan::HandleScope scope;
    Nan::Callback *callback = (Nan::Callback *) data;
    Local<Value> buffer;

    if (status >= 0) {
        Local <Object> bufObj;
        bufObj = Nan::NewBuffer((char *)tx, status).ToLocalChecked();
        buffer = bufObj;
    } else {
        buffer = Nan::Null();
    }

    Local<Value> arguments[3] = {
        js_sol_uart(uart),
        buffer,
        Nan::New(status)
    };
    callback->Call(3, arguments);

    delete callback;
    free(tx);
    hijack_unref();
}

NAN_METHOD(bind_sol_uart_write)
{
    VALIDATE_ARGUMENT_COUNT(info, 3);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 1, IsObject);
    VALIDATE_ARGUMENT_TYPE(info, 2, IsFunction);

    unsigned char *outputBuffer = (unsigned char *) 0;
    sol_uart *uart = NULL;
    size_t length = 0;

    if (!c_sol_uart(Local<Array>::Cast(info[0]), &uart))
        return;

    if (!node::Buffer::HasInstance(info[1])) {
        Nan::ThrowTypeError("Argument 1 must be a node Buffer");
        return;
    }

    length = node::Buffer::Length(info[1]);

    outputBuffer = (unsigned char *) malloc(length * sizeof(unsigned char));
    if (!outputBuffer) {
        Nan::ThrowError("Failed to allocate memory for output buffer");
        return;
    }

    memcpy(outputBuffer, node::Buffer::Data(info[1]), length);

    Nan::Callback *callback =
            new Nan::Callback(Local<Function>::Cast(info[2]));
    bool returnValue =
            sol_uart_write(uart, outputBuffer, length, sol_uart_write_callback,
                           callback);

    annotation[uart] = callback;

    if (!returnValue) {
        delete callback;
    } else {
        hijack_ref();
    }

    info.GetReturnValue().Set(Nan::New(returnValue));
}
