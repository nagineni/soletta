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

#include <v8.h>
#include <node.h>
#include <nan.h>

#include <sol-i2c.h>

#include "../common.h"
#include "../data.h"
#include "../hijack.h"
#include "../structures/handles.h"

using namespace v8;

NAN_METHOD(bind_sol_i2c_open)
{
    VALIDATE_ARGUMENT_COUNT(info, 2);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 0, IsUint32);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 1, IsUint32);

    sol_i2c *i2c = NULL;
    sol_i2c_speed speed = (sol_i2c_speed) info[1]->Uint32Value();

    i2c = sol_i2c_open(info[0]->Uint32Value(), speed);

    if (i2c) {
        info.GetReturnValue().Set(js_sol_i2c(i2c));
    }
}

NAN_METHOD(bind_sol_i2c_open_raw)
{
    VALIDATE_ARGUMENT_COUNT(info, 2);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 0, IsUint32);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 1, IsUint32);

    sol_i2c *i2c = NULL;
    sol_i2c_speed speed = (sol_i2c_speed) info[1]->Uint32Value();
    i2c = sol_i2c_open_raw(info[0]->Uint32Value(), speed);

    if (i2c) {
        info.GetReturnValue().Set(js_sol_i2c(i2c));
    }
}

NAN_METHOD(bind_sol_i2c_set_slave_address)
{
    VALIDATE_ARGUMENT_COUNT(info, 2);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 1, IsUint32);

    sol_i2c *i2c = NULL;

    if (!c_sol_i2c(Local<Array>::Cast(info[0]), &i2c))
        return;

    bool returnValue = sol_i2c_set_slave_address(i2c, info[1]->Uint32Value());
    info.GetReturnValue().Set(Nan::New(returnValue));
}

NAN_METHOD(bind_sol_i2c_close)
{
    VALIDATE_ARGUMENT_COUNT(info, 1);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);

    sol_i2c *i2c = NULL;

    if (!c_sol_i2c(Local<Array>::Cast(info[0]), &i2c))
        return;

    sol_i2c_close(i2c);
}

NAN_METHOD(bind_sol_i2c_close_raw)
{
    VALIDATE_ARGUMENT_COUNT(info, 1);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);

    sol_i2c *i2c = NULL;

    if (!c_sol_i2c(Local<Array>::Cast(info[0]), &i2c))
        return;

    sol_i2c_close_raw(i2c);
}

NAN_METHOD(bind_sol_i2c_busy)
{
    VALIDATE_ARGUMENT_COUNT(info, 1);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);

    sol_i2c *i2c = NULL;

    if (!c_sol_i2c(Local<Array>::Cast(info[0]), &i2c))
        return;

    bool returnValue = sol_i2c_busy(i2c);

    info.GetReturnValue().Set(Nan::New(returnValue));
}

NAN_METHOD(bind_sol_i2c_pending_cancel)
{
    VALIDATE_ARGUMENT_COUNT(info, 2);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);
    VALIDATE_ARGUMENT_TYPE(info, 1, IsArray);

    sol_i2c *i2c = NULL;

    if (!c_sol_i2c(Local<Array>::Cast(info[0]), &i2c))
        return;

    sol_i2c_pending *i2c_pending = NULL;
    if (!c_sol_i2c_pending(Local<Array>::Cast(info[1]), &i2c_pending))
        return;

    sol_i2c_pending_cancel(i2c, i2c_pending);
    hijack_unref();
}

static void sol_i2c_write_cb(void *cb_data, struct sol_i2c *i2c,
                             uint8_t *data, ssize_t status)
{
    Nan::HandleScope scope;
    Nan::Callback *callback = (Nan::Callback *)cb_data;
    Local<Value> buffer;

    if (status >= 0) {
        Local <Object> bufObj;
        bufObj = Nan::NewBuffer((char *)data, status).ToLocalChecked();
        buffer = bufObj;
    } else {
        buffer = Nan::Null();
    }

    Local<Value> arguments[3] = {
        js_sol_i2c(i2c),
        buffer,
        Nan::New((int)status)
    };

    callback->Call(3, arguments);
    delete callback;
    free(data);
    hijack_unref();
}

NAN_METHOD(bind_sol_i2c_write)
{
    VALIDATE_ARGUMENT_COUNT(info, 3);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);
    VALIDATE_ARGUMENT_TYPE(info, 1, IsObject);
    VALIDATE_ARGUMENT_TYPE(info, 2, IsFunction);

    uint8_t *outputBuffer = (uint8_t *) 0;
    sol_i2c *i2c = NULL;
    size_t count = 0;

    if (!c_sol_i2c(Local<Array>::Cast(info[0]), &i2c))
        return;

    if (!node::Buffer::HasInstance(info[1])) {
        Nan::ThrowTypeError("Argument 1 must be a node Buffer");
        return;
    }

    count = node::Buffer::Length(info[1]);

    outputBuffer = (uint8_t *) malloc(count * sizeof(uint8_t));
    if (!outputBuffer)
    {
        Nan::ThrowError("Failed to allocate memory for output buffer");
        return;
    }

    memcpy(outputBuffer, node::Buffer::Data(info[1]), count);

    Nan::Callback *callback =
            new Nan::Callback(Local<Function>::Cast(info[2]));
    sol_i2c_pending *i2c_pending =
            sol_i2c_write(i2c, outputBuffer, count, sol_i2c_write_cb, callback);

    if (!i2c_pending) {
        delete callback;
        return;
    } else {
        hijack_ref();
    }

    info.GetReturnValue().Set(js_sol_i2c_pending(i2c_pending));
}

static void sol_i2c_write_reg_cb(void *cb_data, struct sol_i2c *i2c,
                                 uint8_t reg, uint8_t *data, ssize_t status)
{
    Nan::HandleScope scope;
    Nan::Callback *callback = (Nan::Callback *)cb_data;
    Local<Value> buffer;

    if (status >= 0) {
        Local <Object> bufObj;
        bufObj = Nan::NewBuffer((char *)data, status).ToLocalChecked();
        buffer = bufObj;
    } else {
        buffer = Nan::Null();
    }

    Local<Value> arguments[4] = {
        js_sol_i2c(i2c),
        Nan::New(reg),
        buffer,
        Nan::New((int)status)
    };

    callback->Call(4, arguments);

    delete callback;
    free(data);
    hijack_unref();
}

NAN_METHOD(bind_sol_i2c_write_register)
{
    VALIDATE_ARGUMENT_COUNT(info, 4);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 1, IsUint32);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 2, IsObject);
    VALIDATE_ARGUMENT_TYPE(info, 3, IsFunction);

    uint8_t *outputBuffer = (uint8_t *) 0;
    sol_i2c *i2c = NULL;
    size_t count;

    if (!c_sol_i2c(Local<Array>::Cast(info[0]), &i2c))
        return;

    uint8_t reg = info[1]->Uint32Value();

    if (!node::Buffer::HasInstance(info[2])) {
        Nan::ThrowTypeError("Argument 2 must be a node Buffer");
        return;
    }

    count = node::Buffer::Length(info[2]);

    outputBuffer = (uint8_t *) malloc(count * sizeof(uint8_t));
    if (!outputBuffer) {
        Nan::ThrowError("Failed to allocate memory for output buffer");
        return;
    }

    memcpy(outputBuffer, node::Buffer::Data(info[2]), count);

    Nan::Callback *callback =
            new Nan::Callback(Local<Function>::Cast(info[3]));
    sol_i2c_pending *i2c_pending =
            sol_i2c_write_register(i2c, reg, outputBuffer, count,
                                   sol_i2c_write_reg_cb, callback);

    if (!i2c_pending) {
        delete callback;
        return;
    } else {
        hijack_ref();
    }

    info.GetReturnValue().Set(js_sol_i2c_pending(i2c_pending));
}

static void sol_i2c_write_quick_cb(void *cb_data, struct sol_i2c *i2c,
                                   ssize_t status)
{
    Nan::HandleScope scope;
    Nan::Callback *callback = (Nan::Callback *)cb_data;

    Local<Value> arguments[2] = {
        js_sol_i2c(i2c),
        Nan::New((int)status)
    };

    callback->Call(2, arguments);
    delete callback;
    hijack_unref();
}

NAN_METHOD(bind_sol_i2c_write_quick)
{
    VALIDATE_ARGUMENT_COUNT(info, 3);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);
    VALIDATE_ARGUMENT_TYPE(info, 1, IsBoolean);
    VALIDATE_ARGUMENT_TYPE(info, 2, IsFunction);

    sol_i2c *i2c = NULL;
    if (!c_sol_i2c(Local<Array>::Cast(info[0]), &i2c))
        return;

    bool rw = info[1]->BooleanValue();
    Nan::Callback *callback =
            new Nan::Callback(Local<Function>::Cast(info[2]));

    sol_i2c_pending *i2c_pending =
            sol_i2c_write_quick(i2c, rw, sol_i2c_write_quick_cb, callback);

    if (!i2c_pending) {
        delete callback;
        return;
    } else {
        hijack_ref();
    }

    info.GetReturnValue().Set(js_sol_i2c_pending(i2c_pending));
}

static void sol_i2c_read_cb(void *cb_data, struct sol_i2c *i2c, uint8_t *data,
                            ssize_t status)
{
    Nan::HandleScope scope;
    Nan::Callback *callback = (Nan::Callback *)cb_data;
    Local<Value> buffer;

    if (status >= 0) {
        Local <Object> bufObj;
        bufObj = Nan::NewBuffer((char *)data, status).ToLocalChecked();
        buffer = bufObj;
    } else {
        buffer = Nan::Null();
    }

    Local<Value> arguments[3] = {
        js_sol_i2c(i2c),
        buffer,
        Nan::New((int)status)
    };

    callback->Call(3, arguments);
    delete callback;
    free(data);
    hijack_unref();
}

NAN_METHOD(bind_sol_i2c_read)
{
    VALIDATE_ARGUMENT_COUNT(info, 3);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 1, IsUint32);
    VALIDATE_ARGUMENT_TYPE(info, 2, IsFunction);

    uint8_t *outputBuffer = (uint8_t *) 0;
    sol_i2c *i2c = NULL;

    if (!c_sol_i2c(Local<Array>::Cast(info[0]), &i2c))
        return;

    size_t count = info[1]->Uint32Value();
    outputBuffer = (uint8_t *) malloc(count * sizeof(uint8_t));

    if (!outputBuffer) {
        Nan::ThrowError("Failed to allocate memory for output buffer");
        return;
    }

    Nan::Callback *callback =
            new Nan::Callback(Local<Function>::Cast(info[2]));

    sol_i2c_pending *i2c_pending =
            sol_i2c_read(i2c, outputBuffer, count, sol_i2c_read_cb, callback);

    if (!i2c_pending) {
        delete callback;
        return;
    } else {
        hijack_ref();
    }

    info.GetReturnValue().Set(js_sol_i2c_pending(i2c_pending));
}

static void sol_i2c_read_reg_cb(void *cb_data, struct sol_i2c *i2c,
                                uint8_t reg, uint8_t *data, ssize_t status)
{
    Nan::HandleScope scope;
    Nan::Callback *callback = (Nan::Callback *)cb_data;
    Local<Value> buffer;

    if (status >= 0) {
        Local <Object> bufObj;
        bufObj = Nan::NewBuffer((char *)data, status).ToLocalChecked();
        buffer = bufObj;
    } else {
        buffer = Nan::Null();
    }

    Local<Value> arguments[4] = {
        js_sol_i2c(i2c),
        Nan::New(reg),
        buffer,
        Nan::New((int)status)
    };

    callback->Call(4, arguments);
    delete callback;
    free(data);
    hijack_unref();
}

NAN_METHOD(bind_sol_i2c_read_register)
{
    VALIDATE_ARGUMENT_COUNT(info, 4);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 1, IsUint32);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 2, IsUint32);
    VALIDATE_ARGUMENT_TYPE(info, 3, IsFunction);

    uint8_t *outputBuffer = (uint8_t *) 0;
    sol_i2c *i2c = NULL;

    if (!c_sol_i2c(Local<Array>::Cast(info[0]), &i2c))
        return;

    uint8_t reg = info[1]->Uint32Value();
    size_t count = info[2]->Uint32Value();

    outputBuffer = (uint8_t *) malloc(count * sizeof(uint8_t));
    if (!outputBuffer) {
        Nan::ThrowError("Failed to allocate memory for output buffer");
        return;
    }

    Nan::Callback *callback =
            new Nan::Callback(Local<Function>::Cast(info[3]));

    sol_i2c_pending *i2c_pending =
            sol_i2c_read_register(i2c, reg, outputBuffer, count,
                                  sol_i2c_read_reg_cb, callback);

    if (!i2c_pending) {
        delete callback;
        return;
    } else {
        hijack_ref();
    }

    info.GetReturnValue().Set(js_sol_i2c_pending(i2c_pending));
}

NAN_METHOD(bind_sol_i2c_read_register_multiple)
{
    VALIDATE_ARGUMENT_COUNT(info, 5);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 1, IsUint32);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 2, IsUint32);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 3, IsUint32);
    VALIDATE_ARGUMENT_TYPE(info, 4, IsFunction);

    uint8_t *outputBuffer = (uint8_t *) 0;
    sol_i2c *i2c = NULL;

    if (!c_sol_i2c(Local<Array>::Cast(info[0]), &i2c))
        return;

    uint8_t reg = info[1]->Uint32Value();
    size_t count = info[2]->Uint32Value();
    uint8_t times = info[3]->Uint32Value();

    outputBuffer = (uint8_t *) malloc(times * count * sizeof(uint8_t));
    if (!outputBuffer) {
        Nan::ThrowError("Failed to allocate memory for output buffer");
        return;
    }

    Nan::Callback *callback =
            new Nan::Callback(Local<Function>::Cast(info[4]));

    sol_i2c_pending *i2c_pending =
            sol_i2c_read_register_multiple(i2c, reg, outputBuffer, count, times,
                                           sol_i2c_read_reg_cb, callback);

    if (!i2c_pending) {
        delete callback;
        return;
    } else {
        hijack_ref();
    }

    info.GetReturnValue().Set(js_sol_i2c_pending(i2c_pending));
}
