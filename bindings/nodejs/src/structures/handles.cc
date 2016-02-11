#include <nan.h>
#include "handles.h"
#include "../common.h"
#include "../data.h"

using namespace v8;

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

Local<Array> js_sol_aio_pending(sol_aio_pending *handle) {
  return jsArrayFromBytes(((unsigned char *)(&handle)),
                          sizeof(sol_aio_pending *));
}

bool c_sol_aio_pending(Local<Array> handle, sol_aio_pending **p_cHandle) {
  C_HANDLE(handle, sol_aio_pending *, p_cHandle);
}

Local<Array> js_sol_gpio(sol_gpio *handle) {
  return jsArrayFromBytes(((unsigned char *)(&handle)),
                          sizeof(sol_gpio *));
}

bool c_sol_gpio(Local<Array> handle, sol_gpio **p_cHandle) {
  C_HANDLE(handle, sol_gpio *, p_cHandle);
}


Local<Array> js_sol_i2c(struct sol_i2c *handle) {
  return jsArrayFromBytes(((unsigned char *)(&handle)),
                          sizeof(sol_i2c *));
}

bool c_sol_i2c(Local<Array> handle, sol_i2c **p_cHandle) {
  C_HANDLE(handle, sol_i2c *, p_cHandle);
}

Local<Array> js_sol_i2c_pending(struct sol_i2c_pending *handle) {
  return jsArrayFromBytes(((unsigned char *)(&handle)),
                          sizeof(sol_i2c_pending *));
}

bool c_sol_i2c_pending(Local<Array> handle, sol_i2c_pending **p_cHandle) {
  C_HANDLE(handle, sol_i2c_pending *, p_cHandle);
}

Local<Array> js_sol_pwm(sol_pwm *handle) {
  return jsArrayFromBytes(((unsigned char *)(&handle)),
                          sizeof(sol_pwm *));
}

bool c_sol_pwm(Local<Array> handle, sol_pwm **p_cHandle) {
  C_HANDLE(handle, sol_pwm *, p_cHandle);
}

Local<Array> js_sol_spi(struct sol_spi *handle) {
  return jsArrayFromBytes(((unsigned char *)(&handle)),
                          sizeof(sol_spi *));
}

bool c_sol_spi(Local<Array> handle, sol_spi **p_cHandle) {
  C_HANDLE(handle, sol_spi *, p_cHandle);
}

Local<Array> js_sol_uart(struct sol_uart *handle) {
  return jsArrayFromBytes(((unsigned char *)(&handle)),
                          sizeof(sol_uart *));
}

bool c_sol_uart(Local<Array> handle, sol_uart **p_cHandle) {
  C_HANDLE(handle, sol_uart *, p_cHandle);
}
