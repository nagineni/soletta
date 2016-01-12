#ifndef __SOLETTA_JS_PWM_H__
#define __SOLETTA_JS_PWM_H__

#include <v8.h>
extern "C" {
#include <soletta/sol-pwm.h>
}

bool c_sol_pwm_config(v8::Local<v8::Object> pwmConfig, sol_pwm_config *config);

#endif /* __SOLETTA_JS_PWM_H__ */

