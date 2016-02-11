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
#include "handles.h"
#include "../common.h"
#include "../data.h"

using namespace v8;

#define C_HANDLE(jsHandle, cType, destination)  \
    cType local;    \
    if (!fillCArrayFromJSArray(((unsigned char *)&local), sizeof(cType),    \
                             jsHandle)) {   \
        return false;   \
    }   \
    *destination = local;   \
    return true;

Local<Array> js_sol_i2c(struct sol_i2c *handle) {
  return jsArrayFromBytes(((unsigned char *)(&handle)),
                          sizeof(sol_i2c *));
}

bool c_sol_i2c(Local<Array> handle, sol_i2c **p_cHandle) {
  C_HANDLE(handle, sol_i2c *, p_cHandle);
}

Local<Array> js_sol_i2c_pending(struct sol_i2c_pending *handle) {
  return jsArrayFromBytes(((unsigned char *)(&handle)),
                          sizeof(sol_i2c_pending *));
}

bool c_sol_i2c_pending(Local<Array> handle, sol_i2c_pending **p_cHandle) {
  C_HANDLE(handle, sol_i2c_pending *, p_cHandle);
}
