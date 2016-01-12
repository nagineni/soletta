#include <nan.h>
#include "handles.h"
#include "../common.h"

using namespace v8;

static Local<Array> jsArrayFromBytes(unsigned char *bytes, size_t length) {
  size_t index;
  Local<Array> returnValue = Nan::New<Array>(length);

  for (index = 0; index < length; index++) {
    Nan::Set(returnValue, index, Nan::New(bytes[index]));
  }
  return returnValue;
}

static bool fillCArrayFromJSArray(unsigned char *bytes, size_t length,
                                  Local<Array> array) {
  size_t index, arrayLength;

  arrayLength = array->Length();
  if (arrayLength != length) {
    Nan::ThrowError("byte array has the wrong length");
    return false;
  }

  for (index = 0; index < length; index++) {
    Local<Value> byte = Nan::Get(array, index).ToLocalChecked();
    VALIDATE_VALUE_TYPE(byte, IsUint32, "byte array item", false);
    bytes[index] = (unsigned char)(byte->Uint32Value());
  }

  return true;
}

#define C_HANDLE(jsHandle, cType, destination)                         \
  cType local;                                                         \
  if (!fillCArrayFromJSArray(((unsigned char *)&local), sizeof(cType), \
                             jsHandle)) {                              \
    return false;                                                      \
  }                                                                    \
  *destination = local;                                                \
  return true;

Local<Array> js_sol_aio(sol_aio *handle) {
  return jsArrayFromBytes(((unsigned char *)(&handle)),
                          sizeof(sol_aio *));
}

bool c_sol_aio(Local<Array> handle, sol_aio **p_cHandle) {
  C_HANDLE(handle, sol_aio *, p_cHandle);
}

Local<Array> js_sol_gpio(sol_gpio *handle) {
  return jsArrayFromBytes(((unsigned char *)(&handle)),
                          sizeof(sol_gpio *));
}

bool c_sol_gpio(Local<Array> handle, sol_gpio **p_cHandle) {
  C_HANDLE(handle, sol_gpio *, p_cHandle);
}

Local<Array> js_sol_pwm(sol_pwm *handle) {
  return jsArrayFromBytes(((unsigned char *)(&handle)),
                          sizeof(sol_pwm *));
}

bool c_sol_pwm(Local<Array> handle, sol_pwm **p_cHandle) {
  C_HANDLE(handle, sol_pwm *, p_cHandle);
}
