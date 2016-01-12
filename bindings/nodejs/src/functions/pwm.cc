extern "C" {
#include <sol-pwm.h>
}

#include <v8.h>
#include <node.h>
#include <nan.h>

#include "../common.h"
#include "../structures/sol-js-pwm.h"
#include "../structures/handles.h"

using namespace v8;

NAN_METHOD(bind_sol_pwm_open) {
	VALIDATE_ARGUMENT_COUNT(info, 3);
	VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 0, IsInt32);
 	VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 1, IsInt32);
	VALIDATE_ARGUMENT_TYPE(info, 2, IsObject);

	int device;
  int channel;
	sol_pwm_config config;
	sol_pwm *pwm = NULL;

	device = info[0]->Int32Value();
  channel = info[1]->Int32Value();

	if (!c_sol_pwm_config(info[2]->ToObject(), &config)) {
		printf("Unable to extract sol_pwm_config\n");
    	return;
	}

	pwm = sol_pwm_open(device, channel, &config);
	if ( pwm ) {
		info.GetReturnValue().Set(js_sol_pwm(pwm));
	}
}

NAN_METHOD(bind_sol_pwm_close) {
	VALIDATE_ARGUMENT_COUNT(info, 1);
	VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);

	sol_pwm *pwm = NULL;

 	if (!c_sol_pwm(Local<Array>::Cast(info[0]), &pwm)) {
    	return;
	}

	sol_pwm_close(pwm);
}

NAN_METHOD(bind_sol_pwm_set_enabled) {
	VALIDATE_ARGUMENT_COUNT(info, 2);
	VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);
	VALIDATE_ARGUMENT_TYPE(info, 0, IsBoolean);

	bool value;
	sol_pwm *pwm = NULL;

 	if (!c_sol_pwm(Local<Array>::Cast(info[0]), &pwm)) {
    	return;
	}

	value = info[1]->BooleanValue();

	info.GetReturnValue().Set(Nan::New(sol_pwm_set_enabled(pwm, value)));
}

NAN_METHOD(bind_sol_pwm_set_period) {
	VALIDATE_ARGUMENT_COUNT(info, 2);
	VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);
	VALIDATE_ARGUMENT_TYPE(info, 0, IsUint32);

	uint32_t value;
	sol_pwm *pwm = NULL;

 	if (!c_sol_pwm(Local<Array>::Cast(info[0]), &pwm)) {
    	return;
	}

	value = info[1]->Uint32Value();

	info.GetReturnValue().Set(Nan::New(sol_pwm_set_period(pwm, value)));
}

NAN_METHOD(bind_sol_pwm_set_duty_cycle) {
	VALIDATE_ARGUMENT_COUNT(info, 2);
	VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);
	VALIDATE_ARGUMENT_TYPE(info, 0, IsUint32);

	uint32_t value;
	sol_pwm *pwm = NULL;

 	if (!c_sol_pwm(Local<Array>::Cast(info[0]), &pwm)) {
    	return;
	}

	value = info[1]->Uint32Value();

	info.GetReturnValue().Set(Nan::New(sol_pwm_set_duty_cycle(pwm, value)));
}


NAN_METHOD(bind_sol_pwm_get_enabled) {
	VALIDATE_ARGUMENT_COUNT(info, 1);
	VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);

	sol_pwm *pwm = NULL;

 	if (!c_sol_pwm(Local<Array>::Cast(info[0]), &pwm)) {
    	return;
	}

	info.GetReturnValue().Set(Nan::New(sol_pwm_get_enabled(pwm)));
}

NAN_METHOD(bind_sol_pwm_get_period) {
	VALIDATE_ARGUMENT_COUNT(info, 1);
	VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);

	sol_pwm *pwm = NULL;

 	if (!c_sol_pwm(Local<Array>::Cast(info[0]), &pwm)) {
    	return;
	}

	info.GetReturnValue().Set(Nan::New(sol_pwm_get_period(pwm)));
}

NAN_METHOD(bind_sol_pwm_get_duty_cycle) {
	VALIDATE_ARGUMENT_COUNT(info, 1);
	VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);

	sol_pwm *pwm = NULL;

 	if (!c_sol_pwm(Local<Array>::Cast(info[0]), &pwm)) {
    	return;
	}

	info.GetReturnValue().Set(Nan::New(sol_pwm_get_duty_cycle(pwm)));
}
