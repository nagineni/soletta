extern "C" {
#include <sol-aio.h>
}

#include <v8.h>
#include <node.h>
#include <nan.h>

#include "../common.h"
#include "../hijack.h"
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

NAN_METHOD(bind_sol_aio_open_by_label) {
    VALIDATE_ARGUMENT_COUNT(info, 2);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 0, IsString);
    VALIDATE_ARGUMENT_TYPE_OR_NULL(info, 1, IsUint32);

    uint32_t precision;
    sol_aio *aio = NULL;

    precision = info[1]->Uint32Value();

    aio = sol_aio_open_by_label((const char *)*String::Utf8Value(info[0]), precision);
    if ( aio ) {
        info.GetReturnValue().Set(js_sol_aio(aio));
    }
}

NAN_METHOD(bind_sol_aio_open_raw) {
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

    aio = sol_aio_open_raw(device, pin, precision);
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


static void sol_aio_read_cb(void *cb_data, struct sol_aio *aio,
                            int32_t ret)
{
    Nan::HandleScope scope;
    Nan::Callback *callback = (Nan::Callback *)cb_data;

    Local<Value> arguments[1] = {
        Nan::New(ret)
    };
    callback->Call(1, arguments);

    delete callback;
    hijack_unref();
}

NAN_METHOD(bind_sol_aio_get_value) {
	VALIDATE_ARGUMENT_COUNT(info, 2);
	VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);
    VALIDATE_ARGUMENT_TYPE(info, 1, IsFunction);

	sol_aio *aio = NULL;

 	if (!c_sol_aio(Local<Array>::Cast(info[0]), &aio)) {
    	return;
	}

    Nan::Callback *callback =
        new Nan::Callback(Local<Function>::Cast(info[1]));

    sol_aio_pending *aio_pending =
        sol_aio_get_value(aio, sol_aio_read_cb, callback);

    if (!aio_pending) {
        delete callback;
        return;
    } else {
        hijack_ref();
    }

    info.GetReturnValue().Set(js_sol_aio_pending(aio_pending));
}

NAN_METHOD(bind_sol_aio_pending_cancel)
{
    VALIDATE_ARGUMENT_COUNT(info, 2);
    VALIDATE_ARGUMENT_TYPE(info, 0, IsArray);
    VALIDATE_ARGUMENT_TYPE(info, 1, IsArray);

    sol_aio *aio = NULL;

    if (!c_sol_aio(Local<Array>::Cast(info[0]), &aio))
        return;

    sol_aio_pending *aio_pending = NULL;
    if (!c_sol_aio_pending(Local<Array>::Cast(info[1]), &aio_pending))
        return;

    sol_aio_pending_cancel(aio, aio_pending);
    hijack_unref();
}
