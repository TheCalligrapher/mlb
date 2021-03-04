#include "mlb_serial.h"
#include "impl/mlb_wrapper.h"

MlbSerial *mlb_serial(void)
{
  return (MlbSerial *) &Serial;
}

#define MLB_CPP_WRAPPER_GLOBAL Serial

  MLB_CPP_GLOBAL_WRAPPER_DEF(void, mlb_serial_begin, begin, unsigned long, baud)

#undef MLB_CPP_WRAPPER_GLOBAL
