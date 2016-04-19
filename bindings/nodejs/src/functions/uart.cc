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

#include <nan.h>
#include <node.h>
#include <sol-buffer.h>
#include <sol-str-slice.h>
#include <sol-uart.h>

#include "../common.h"
#include "../hijack.h"
#include "../structures/sol-js-uart.h"
#include "../structures/js-handle.h"

using namespace v8;

class SolUART : public JSHandle<SolUART> {
public:
    static const char *jsClassName() { return "SolUART"; }
};

static void sol_uart_read_callback(void *user_data, struct sol_uart *uart) {
    Nan::HandleScope scope;
    sol_uart_data *uart_data = (sol_uart_data *)user_data;
    if (!uart_data)
        return;

    Nan::Callback *callback = uart_data->rx_cb;
    if (!callback)
        return;
    callback->Call(0, NULL);
}

static void sol_uart_write_callback(void *data, struct sol_uart *uart,
    struct sol_blob *blob, int status) {
    Nan::HandleScope scope;
    Local<Value> buffer;
    sol_uart_data *uart_data = (sol_uart_data *)data;
    Nan::Callback *callback = uart_data->tx_cb;
    if (!callback)
        return;

    if (status < 0) {
        buffer = Nan::Null();
    } else {
        struct sol_str_slice slice;
        slice = sol_str_slice_from_blob(blob);
        char *tx = sol_str_slice_to_string(slice);
        if (tx) {
            buffer = Nan::NewBuffer(tx, slice.len).ToLocalChecked();
        } else {
            buffer = Nan::Null();
        }
    }

    Local<Value> arguments[2] = {
        buffer,
        Nan::New(status)
    };
    callback->Call(2, arguments);
}

NAN_METHOD(bind_sol_uart_open) {
    VALIDATE_ARGUMENT_COUNT(info, 2);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 0, IsString);
    VALIDATE_ARGUMENT_TYPE(info, 1, IsObject);
    sol_uart_config config;
    sol_uart *uart = NULL;

    if (!hijack_ref())
        return;

    sol_uart_data *uart_data = new sol_uart_data;
    uart_data->rx_cb = NULL;
    uart_data->tx_cb = NULL;
    if (!c_sol_uart_config(info[1]->ToObject(), uart_data, &config)) {
        delete uart_data;
        Nan::ThrowError("Unable to extract sol_uart_config\n");
        return;
    }

    Nan::Callback *rx_callback = uart_data->rx_cb;
    Nan::Callback *tx_callback = uart_data->tx_cb;
    config.rx_cb = sol_uart_read_callback;
    config.tx_cb = sol_uart_write_callback;

    uart = sol_uart_open((const char *)*String::Utf8Value(info[0]), &config);
    if (!uart) {
        if (rx_callback)
            delete rx_callback;
        if (tx_callback)
            delete tx_callback;
        delete uart_data;
        hijack_unref();
        return;
    }

    uart_data->uart = uart;
    info.GetReturnValue().Set(SolUART::New(uart_data));
}

NAN_METHOD(bind_sol_uart_close) {
    VALIDATE_ARGUMENT_COUNT(info, 1);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 0, IsObject);
    Local<Object> jsUART = Nan::To<Object>(info[0]).ToLocalChecked();
    sol_uart_data *uart_data = (sol_uart_data *)SolUART::Resolve(jsUART);
    if (!uart_data)
        return;

    sol_uart *uart = uart_data->uart;
    Nan::Callback *rx_callback = uart_data->rx_cb;
    Nan::Callback *tx_callback = uart_data->tx_cb;
    sol_uart_close(uart);

    if (rx_callback)
        delete rx_callback;

    if (tx_callback)
        delete tx_callback;

     hijack_unref();
     delete uart_data;
     Nan::SetInternalFieldPointer(jsUART, 0, 0);
}

NAN_METHOD(bind_sol_uart_read) {
    VALIDATE_ARGUMENT_COUNT(info, 1);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 0, IsObject);

    Local<Object> jsUART = Nan::To<Object>(info[0]).ToLocalChecked();
    sol_uart_data *uart_data = (sol_uart_data *)SolUART::Resolve(jsUART);
    if (!uart_data)
        return;
    sol_uart *uart = uart_data->uart;

    struct sol_buffer rx_buffer;
    sol_buffer_flags flags = (sol_buffer_flags) (SOL_BUFFER_FLAGS_DEFAULT |
        SOL_BUFFER_FLAGS_NO_NUL_BYTE);

    sol_buffer_init_flags(&rx_buffer, NULL, 0, flags);
    int result = sol_uart_read(uart, &rx_buffer);

    if (result < 0) {
        Nan::ThrowError("Could not read data from UART");
        return ;
    }

    struct sol_str_slice slice;
    slice = sol_buffer_get_slice(&rx_buffer);
    char *rx = sol_str_slice_to_string(slice);
    if (rx)
        info.GetReturnValue().Set(Nan::NewBuffer(rx, slice.len).ToLocalChecked());
}

NAN_METHOD(bind_sol_uart_write) {
    VALIDATE_ARGUMENT_COUNT(info, 2);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 0, IsObject);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 1, IsObject);

    Local<Object> jsUART = Nan::To<Object>(info[0]).ToLocalChecked();
    sol_uart_data *uart_data = (sol_uart_data *)SolUART::Resolve(jsUART);
    if (!uart_data)
        return;

    sol_uart *uart = uart_data->uart;
    if (!node::Buffer::HasInstance(info[1])) {
        Nan::ThrowTypeError("Argument 1 must be a Buffer");
        return;
    }

    struct sol_blob *blob;
    size_t length = node::Buffer::Length(info[1]);
    void *mem =  malloc(length);
    if (!mem) {
        Nan::ThrowError("Failed to allocate memory");
        return;
    }

    memcpy(mem, node::Buffer::Data(info[1]), length);
    blob = sol_blob_new(SOL_BLOB_TYPE_DEFAULT, NULL, mem, length);
    if (!blob) {
        free(mem);
    }

    int returnValue = sol_uart_write(uart, blob);
    sol_blob_unref(blob);

    info.GetReturnValue().Set(Nan::New(returnValue));
}

NAN_METHOD(bind_sol_uart_baud_rate_from_str) {
    VALIDATE_ARGUMENT_COUNT(info, 1);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsString);

    sol_uart_baud_rate baud_rate = sol_uart_baud_rate_from_str(
        (const char *)*String::Utf8Value(info[0]));
    info.GetReturnValue().Set(Nan::New(baud_rate));
}

NAN_METHOD(bind_sol_uart_baud_rate_to_str) {
    VALIDATE_ARGUMENT_COUNT(info, 1);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsInt32);

    const char *idString = sol_uart_baud_rate_to_str(
        (sol_uart_baud_rate)info[0]->Uint32Value());

    if (idString) {
        info.GetReturnValue().Set(Nan::New(idString).ToLocalChecked());
    } else {
       info.GetReturnValue().Set(Nan::Null());
    }
}

NAN_METHOD(bind_sol_uart_get_pending_write_bytes) {
    VALIDATE_ARGUMENT_COUNT(info, 1);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 0, IsObject);
    Local<Object> jsUART = Nan::To<Object>(info[0]).ToLocalChecked();
    sol_uart_data *uart_data = (sol_uart_data *)SolUART::Resolve(jsUART);
    if (!uart_data)
        return;

    sol_uart *uart = uart_data->uart;
    size_t pending;
    int result = sol_uart_get_pending_write_bytes(uart, &pending);
    if (result < 0)
        return;

    info.GetReturnValue().Set(Nan::New((int)pending));
}

NAN_METHOD(bind_sol_uart_data_bits_from_str) {
    VALIDATE_ARGUMENT_COUNT(info, 1);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsString);

    sol_uart_data_bits data_bits = sol_uart_data_bits_from_str(
        (const char *)*String::Utf8Value(info[0]));
    info.GetReturnValue().Set(Nan::New(data_bits));
}

NAN_METHOD(bind_sol_uart_data_bits_to_str) {
    VALIDATE_ARGUMENT_COUNT(info, 1);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsInt32);

    const char *idString = sol_uart_data_bits_to_str(
        (sol_uart_data_bits)info[0]->Int32Value());

    if (idString) {
        info.GetReturnValue().Set(Nan::New(idString).ToLocalChecked());
    } else {
        info.GetReturnValue().Set(Nan::Null());
    }
}

NAN_METHOD(bind_sol_uart_stop_bits_from_str) {
    VALIDATE_ARGUMENT_COUNT(info, 1);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsString);

    sol_uart_stop_bits stop_bits = sol_uart_stop_bits_from_str(
        (const char *)*String::Utf8Value(info[0]));
    info.GetReturnValue().Set(Nan::New(stop_bits));
}

NAN_METHOD(bind_sol_uart_stop_bits_to_str) {
    VALIDATE_ARGUMENT_COUNT(info, 1);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsInt32);

    const char *idString = sol_uart_stop_bits_to_str(
        (sol_uart_stop_bits)info[0]->Int32Value());

    if (idString) {
        info.GetReturnValue().Set(Nan::New(idString).ToLocalChecked());
    } else {
        info.GetReturnValue().Set(Nan::Null());
    }
}

NAN_METHOD(bind_sol_uart_parity_from_str) {
    VALIDATE_ARGUMENT_COUNT(info, 1);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsString);

    sol_uart_parity parity = sol_uart_parity_from_str(
        (const char *)*String::Utf8Value(info[0]));
    info.GetReturnValue().Set(Nan::New(parity));
}

NAN_METHOD(bind_sol_uart_parity_to_str) {
    VALIDATE_ARGUMENT_COUNT(info, 1);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsInt32);

    const char *idString = sol_uart_parity_to_str(
        (sol_uart_parity)info[0]->Int32Value());

    if (idString) {
        info.GetReturnValue().Set(Nan::New(idString).ToLocalChecked());
    } else {
        info.GetReturnValue().Set(Nan::Null());
    }
}
