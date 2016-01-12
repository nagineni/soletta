#include <nan.h>
#include "sol-js-gpio.h"
#include "../common.h"

using namespace v8;

extern "C" {
#include <string.h>
}


bool c_sol_gpio_config(v8::Local<v8::Object> jsGPIOConfig, sol_gpio_config *p_config) {
	sol_gpio_config config = {
                SOL_GPIO_CONFIG_API_VERSION,
                SOL_GPIO_DIR_OUT,
                false, SOL_GPIO_DRIVE_NONE,
                {SOL_GPIO_EDGE_BOTH, NULL, NULL, 0} };

	VALIDATE_AND_ASSIGN(config, dir, sol_gpio_direction, IsUint32,
	                	"(GPIO direction)", false, jsGPIOConfig,
	                	Uint32Value);

	VALIDATE_AND_ASSIGN(config, drive_mode, sol_gpio_drive, IsUint32,
	                	"(GPIO pull-up/pull-down resistor)", false, jsGPIOConfig,
	                	Uint32Value);

	VALIDATE_AND_ASSIGN(config, active_low, bool, IsBoolean,
	                	"(GPIO active_low state)", false, jsGPIOConfig,
	                	BooleanValue);

	if (config.dir == SOL_GPIO_DIR_IN) {
		Local<Value> poll_timeout =
			Nan::Get(jsGPIOConfig, Nan::New("poll_timeout").ToLocalChecked())
				.ToLocalChecked();
		VALIDATE_VALUE_TYPE(poll_timeout, IsUint32, "GPIO in poll_timeout",
			false);
		config.in.poll_timeout = (uint32_t)poll_timeout->Uint32Value();

		Local<Value> trigger_mode =
			Nan::Get(jsGPIOConfig, Nan::New("trigger_mode").ToLocalChecked())
				.ToLocalChecked();
		VALIDATE_VALUE_TYPE(poll_timeout, IsUint32, "GPIO in trigger_mode",
			false);
		config.in.trigger_mode = (sol_gpio_edge)trigger_mode->Uint32Value();		
	}

	memcpy(p_config, &config, sizeof(sol_gpio_config));

	return true;
}
