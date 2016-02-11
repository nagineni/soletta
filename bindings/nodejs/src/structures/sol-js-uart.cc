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
#include "../common.h"
#include "../hijack.h"

#include "sol-js-uart.h"

using namespace v8;

static void sol_uart_rx_callback(void *user_data, struct sol_uart *uart,
                                 unsigned char byte_read)
{
    Nan::HandleScope scope;
    Nan::Callback *callback = (Nan::Callback *)user_data;

    Local<Value> arguments[1] =	{
        Nan::New(byte_read)
    };

    callback->Call(1, arguments);
}

bool c_sol_uart_config(v8::Local<v8::Object> jsUARTConfig,
                       sol_uart_config *p_config)
{
    sol_uart_config config = {
#ifdef SOL_UART_CONFIG_API_VERSION
        SOL_UART_CONFIG_API_VERSION,
#endif /* def SOL_UART_CONFIG_API_VERSION */
        SOL_UART_BAUD_RATE_9600,
        SOL_UART_DATA_BITS_8,
        SOL_UART_PARITY_NONE,
        SOL_UART_STOP_BITS_ONE,
        NULL, NULL, false
    };

    VALIDATE_AND_ASSIGN(config, baud_rate, sol_uart_baud_rate, IsUint32,
                        "(Baud rate)", false, jsUARTConfig, Uint32Value);

    VALIDATE_AND_ASSIGN(config, data_bits, sol_uart_data_bits, IsUint32,
                        "(Amount of data bits)", false, jsUARTConfig,
                        Uint32Value);

    VALIDATE_AND_ASSIGN(config, parity, sol_uart_parity, IsUint32,
                        "(Parity characteristic)", false, jsUARTConfig,
                        Uint32Value);

    VALIDATE_AND_ASSIGN(config, stop_bits, sol_uart_stop_bits, IsUint32,
                        "(Amount of stop bits)", false, jsUARTConfig,
                        Uint32Value);

    Local<Value> read_cb =
            Nan::Get(jsUARTConfig,
                     Nan::New("rx").ToLocalChecked()).ToLocalChecked();

    if (read_cb->IsFunction()) {
        Nan::Callback *rx_cb =
                new Nan::Callback(Local < Function >::Cast(read_cb));

        config.rx_cb = sol_uart_rx_callback;
        config.rx_cb_user_data = rx_cb;
    }

    VALIDATE_AND_ASSIGN(config, flow_control, bool, IsBoolean,
                        "(Enable software flow control)", false, jsUARTConfig,
                        BooleanValue);

    memcpy(p_config, &config, sizeof(sol_uart_config));

    return true;
}
