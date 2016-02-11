#ifndef __SOLETTA_JS_HANDLES_H__
#define __SOLETTA_JS_HANDLES_H__

#include <v8.h>
extern "C" {
#include <sol-aio.h>
#include <sol-gpio.h>
#include <sol-pwm.h>
#include <sol-uart.h>
#include <sol-spi.h>
}

v8::Local<v8::Array> js_sol_aio(sol_aio *handle);
bool c_sol_aio(v8::Local<v8::Array> handle,
                        sol_aio **p_cHandle);

v8::Local<v8::Array> js_sol_aio_pending(sol_aio_pending *handle);
bool c_sol_aio_pending(v8::Local<v8::Array> handle,
                        sol_aio_pending **p_cHandle);

v8::Local<v8::Array> js_sol_gpio(sol_gpio *handle);
bool c_sol_gpio(v8::Local<v8::Array> handle,
                        sol_gpio **p_cHandle);

v8::Local<v8::Array> js_sol_pwm(sol_pwm *handle);
bool c_sol_pwm(v8::Local<v8::Array> handle,
                        sol_pwm **p_cHandle);

v8::Local<v8::Array> js_sol_spi(struct sol_spi *handle);
bool c_sol_spi(v8::Local<v8::Array> handle,
                        sol_spi **p_cHandle);

v8::Local<v8::Array> js_sol_uart(struct sol_uart *handle);
bool c_sol_uart(v8::Local<v8::Array> handle,
                        sol_uart **p_cHandle);

#endif /* __SOLETTA_JS_HANDLES_H__ */
