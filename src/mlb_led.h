#ifndef MLB_LED_H_
#define MLB_LED_H_

#include "base/mlb_config.h"

#include "mlb_util.h"
#include "mlb_arduino_co.h"
#include "mlb_live_bool.h"

C_LINKAGE_BEGIN

/****************************************************************************************/

typedef struct MlbLed
{
  uint8_t i_pin;
  const bool *v;
} MlbLed;

#define MLB_LED_STATIC_INIT_NB(pin) { pin }
/* Initializes a steady led not monitoring any bool */

#define MLB_LED_STATIC_INIT(pin, b) { pin, b }

/****************************************************************************************/

static inline int mlb_led_attach_pin(MlbLed *led, uint8_t i_pin)
{
  assert(led != NULL);
  MLB_SWAP(uint8_t, led->i_pin, i_pin);
  pinMode(led->i_pin, OUTPUT);
  return i_pin;
}

static inline const bool *mlb_led_attach_bool(MlbLed *led, const bool *v)
{
  assert(led != NULL);
  MLB_SWAP(const bool *, led->v, v);
  return v;
}

static inline MlbLed *mlb_led_post(MlbLed *led)
{ /* Post-init routine (finalizer) to be applied to 'STATIC_INIT' initialized LEDs */
  assert(led != NULL);
  mlb_led_attach_pin(led, led->i_pin);
  return led;
}

static inline MlbLed *mlb_led_init(MlbLed *led, uint8_t i_pin, const bool *v)
{
  assert(led != NULL);
  *led = (MlbLed) { i_pin, v };
  return mlb_led_post(led);
}

static inline const bool *mlb_led_detach_bool(MlbLed *led)
{
  return mlb_led_attach_bool(led, NULL);
}

static inline const bool *mlb_led_attach_lb(MlbLed *led, const MlbLiveBool *lb)
{
  assert(led != NULL);
  const bool *old_v = led->v;
  led->v = lb != NULL ? &lb->v : NULL;
  return old_v;
}

static inline void mlb_led_set(MlbLed *led, bool on)
{ /* Can be used to directly set LED's pin state. This will persist until the next 
     update by 'mlb_leds_execute', if it will ever occur */
  digitalWrite(led->i_pin, mlb_bool_to_hl(on));
}

/****************************************************************************************/

/* #define MLB_CO_IS_SUSPENDED 0x1 */

typedef struct MlbLeds
{
  MlbLed *leds;
  unsigned n;
  uint8_t flags;
} MlbLeds;

#define MLB_LEDS_STATIC_INIT_NB(N) { (MlbLed [N]) { 0 }, N }

#define MLB_LEDS_STATIC_INIT(...)\
  { (MlbLed []) { __VA_ARGS__ }, MLB_ARRAY_N(((MlbLed []) { __VA_ARGS__ })) }

/****************************************************************************************/

static inline MlbLed *mlb_leds_at(const MlbLeds *mlb_leds, unsigned i)
{
  assert(mlb_leds != NULL && i < mlb_leds->n);
  return &mlb_leds->leds[i];
}

static inline MlbLeds *mlb_leds_post(MlbLeds *mlb_leds)
{ /* Post-init routine (finalizer) to be applied to 'STATIC_INIT' initialized LEDs */
  assert(mlb_leds != NULL);
  for (unsigned i = 0; i < mlb_leds->n; ++i)
    mlb_led_post(mlb_leds_at(mlb_leds, i));
  return mlb_leds;
}

static inline MlbLeds *mlb_leds_init(MlbLeds *mlb_leds, MlbLed *leds, unsigned n)
{ 
  assert(mlb_leds != NULL);
  *mlb_leds = (MlbLeds) { leds, n };
  /* Doesn't call 'mlb_leds_post' */
  return mlb_leds;
}

void mlb_leds_attach_pins(MlbLeds *mlb_leds, const uint8_t pins[]);
void mlb_leds_attach_pins_pgm(MlbLeds *mlb_leds, const uint8_t pins[]);

void mlb_leds_attach_pbools(MlbLeds *mlb_leds, /* const */ bool *const bools[]);
void mlb_leds_attach_bools(MlbLeds *mlb_leds, const bool bools[]);
void mlb_leds_attach_lbs(MlbLeds *mlb_leds, const MlbLiveBool lbs[]);
void mlb_leds_attach_mlb_lbs(MlbLeds *mlb_leds, const MlbLiveBools *mlb_lbs);

/****************************************************************************************/

CO_PROTOTYPE_DYNAMIC(mlb_leds_execute, MlbLeds *mlb_leds)
{
  unsigned i_led;
  MlbBool *states;
}
CO_PROTOTYPE_END

#define MLB_LEDS_DYN_SUSG__(n_) ((n_) * sizeof(MlbBool))
#define MLB_LEDS_SUSG(n_)\
  (MLB_STRICT_ALIGN_UP(COF_SUSG(mlb_leds_execute), alignof(MlbBool)) +\
   MLB_LBS_DYN_SUSG__(n_))

/****************************************************************************************/

C_LINKAGE_END

#endif /* MLB_LED_H_ */