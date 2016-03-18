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

#ifndef __SOLETTA_JS_HANDLES_H__
#define __SOLETTA_JS_HANDLES_H__

#include <v8.h>
extern "C" {
#include <sol-gpio.h>
}

v8::Local<v8::Array> js_sol_gpio(sol_gpio *handle);
bool c_sol_gpio(v8::Local<v8::Array> handle,
    sol_gpio **p_cHandle);

#endif /* __SOLETTA_JS_HANDLES_H__ */
