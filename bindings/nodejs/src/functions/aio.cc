extern "C" {
#include <sol-aio.h>
}

#include <v8.h>
#include <node.h>
#include <nan.h>

#include "../common.h"
#include "../structures/handles.h"

using namespace v8;

NAN_METHOD(bind_sol_aio_open) {
	VALIDATE_ARGUMENT_COUNT(info, 3);
	VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 0, IsInt32);
	VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 1, IsInt32);
	VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 2, IsUint32);

	int32_t device;
	int32_t pin;
	uint32_t precision;
	sol_aio *aio = NULL;

	device = info[0]->Int32Value();
	pin = info[1]->Int32Value();
	precision = info[2]->Uint32Value();

	aio = sol_aio_open(device, pin, precision);
	if ( aio ) {
		info.GetReturnValue().Set(js_sol_aio(aio));
	}
}

NAN_METHOD(bind_sol_aio_close) {
	VALIDATE_ARGUMENT_COUNT(info, 1);
	VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);

	sol_aio *aio = NULL;

 	if (!c_sol_aio(Local<Array>::Cast(info[0]), &aio)) {
    	return;
	}

	sol_aio_close(aio);
}

NAN_METHOD(bind_sol_gpio_get_value) {
	VALIDATE_ARGUMENT_COUNT(info, 1);
	VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);

	sol_aio *aio = NULL;

 	if (!c_sol_aio(Local<Array>::Cast(info[0]), &aio)) {
    	return;
	}

	info.GetReturnValue().Set(Nan::New(sol_aio_get_value(aio)));
}
