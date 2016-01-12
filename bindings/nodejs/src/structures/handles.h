#ifndef __SOLETTA_JS_HANDLES_H__
#define __SOLETTA_JS_HANDLES_H__

#include <v8.h>
extern "C" {
#include <soletta/sol-aio.h>
#include <soletta/sol-gpio.h>
#include <soletta/sol-pwm.h>
}

v8::Local<v8::Array> js_sol_aio(sol_aio *handle);
bool c_sol_aio(v8::Local<v8::Array> handle,
                        sol_aio **p_cHandle);

v8::Local<v8::Array> js_sol_gpio(sol_gpio *handle);
bool c_sol_gpio(v8::Local<v8::Array> handle,
                        sol_gpio **p_cHandle);

v8::Local<v8::Array> js_sol_pwm(sol_pwm *handle);
bool c_sol_pwm(v8::Local<v8::Array> handle,
                        sol_pwm **p_cHandle);

#endif /* __SOLETTA_JS_HANDLES_H__ */
