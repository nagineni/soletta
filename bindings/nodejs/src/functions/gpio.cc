extern "C" {
#include <sol-gpio.h>
}

#include <v8.h>
#include <node.h>
#include <nan.h>
#include <map>

#include "../common.h"
#include "../hijack.h"
#include "../structures/sol-js-gpio.h"
#include "../structures/handles.h"

using namespace v8;

// Associate the callback info with a gpio handle
static std::map<sol_gpio *, Nan::Callback *> annotation;

NAN_METHOD(bind_sol_gpio_open) {
	VALIDATE_ARGUMENT_COUNT(info, 2);
	VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 0, IsUint32);
	VALIDATE_ARGUMENT_TYPE(info, 1, IsObject);

	uint32_t pin;
	sol_gpio_config config;
	sol_gpio *gpio = NULL;

	pin = info[0]->Uint32Value();
	if (!c_sol_gpio_config(info[1]->ToObject(), &config)) {
	    Nan::ThrowError(("Unable to extract sol_gpio_config\n");
	    return;
	}

	Nan::Callback *callback = (Nan::Callback *)config.in.user_data;
	gpio = sol_gpio_open(pin, &config);
	if (gpio) {
	    info.GetReturnValue().Set(js_sol_gpio(gpio));
	    if (callback) {
	        annotation[gpio] = callback;
	        hijack_ref();
	    }
	} else if (callback) {
	    delete callback;
	}
}

NAN_METHOD(bind_sol_gpio_close) {
	VALIDATE_ARGUMENT_COUNT(info, 1);
	VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);

	sol_gpio *gpio = NULL;

 	if (!c_sol_gpio(Local<Array>::Cast(info[0]), &gpio)) {
    	return;
	}

	Nan::Callback *callback = annotation[gpio];
	sol_gpio_close(gpio);
	if (callback) {
	    annotation.erase(gpio);
	    delete callback;
	    hijack_unref();
	}
}

NAN_METHOD(bind_sol_gpio_write) {
	VALIDATE_ARGUMENT_COUNT(info, 2);
	VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);
	VALIDATE_ARGUMENT_TYPE(info, 1, IsBoolean);

	bool value;
	sol_gpio *gpio = NULL;

 	if (!c_sol_gpio(Local<Array>::Cast(info[0]), &gpio)) {
    	return;
	}

	value = info[1]->BooleanValue();

	info.GetReturnValue().Set(Nan::New(sol_gpio_write(gpio, value)));
}

NAN_METHOD(bind_sol_gpio_read) {
	VALIDATE_ARGUMENT_COUNT(info, 1);
	VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);

	sol_gpio *gpio = NULL;

 	if (!c_sol_gpio(Local<Array>::Cast(info[0]), &gpio)) {
    	return;
	}

	info.GetReturnValue().Set(Nan::New(sol_gpio_read(gpio)));
}
