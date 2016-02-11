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
#include "sol-js-spi.h"
#include "../common.h"

using namespace v8;

bool c_sol_spi_config(v8::Local<v8::Object> jsUARTConfig,
                      sol_spi_config *p_config)
{
    sol_spi_config config = {
#ifdef SOL_SPI_CONFIG_API_VERSION
        SOL_SPI_CONFIG_API_VERSION,
#endif /* def SOL_SPI_CONFIG_API_VERSION */
        0, SOL_SPI_MODE_0, 0, 0
    };

    VALIDATE_AND_ASSIGN(config, chip_select, unsigned int, IsUint32,
                        "(Chip select)", false, jsUARTConfig, Uint32Value);

    VALIDATE_AND_ASSIGN(config, mode, sol_spi_mode, IsUint32,
                        "(SPI transfer mode)", false, jsUARTConfig,
                        Uint32Value);

    VALIDATE_AND_ASSIGN(config, frequency, uint32_t, IsUint32,
                        "(Frequency in Hz)", false, jsUARTConfig, Uint32Value);

    VALIDATE_AND_ASSIGN(config, bits_per_word, uint8_t, IsUint32,
                        "(Bits per word)", false, jsUARTConfig, Uint32Value);

    memcpy(p_config, &config, sizeof(sol_spi_config));

    return true;
}
