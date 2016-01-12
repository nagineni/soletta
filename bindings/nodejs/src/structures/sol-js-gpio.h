#ifndef __SOLETTA_JS_GPIO_H__
#define __SOLETTA_JS_GPIO_H__

#include <v8.h>
extern "C" {
#include <sol-gpio.h>
}

bool c_sol_gpio_config(v8::Local<v8::Object> gpioConfig, sol_gpio_config *config);

#endif /* __SOLETTA_JS_GPIO_H__ */

