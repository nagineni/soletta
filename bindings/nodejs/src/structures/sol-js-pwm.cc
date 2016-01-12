#include <nan.h>
#include "sol-js-pwm.h"
#include "../common.h"

using namespace v8;

extern "C" {
#include <string.h>
}


bool c_sol_pwm_config(v8::Local<v8::Object> jsPWMConfig, sol_pwm_config *p_config) {
	sol_pwm_config config = {0, 0, 0, SOL_PWM_ALIGNMENT_LEFT, 
                    SOL_PWM_POLARITY_NORMAL, false };

	VALIDATE_AND_ASSIGN(config, alignment,  sol_pwm_alignment, IsUint32,
	                	"(PWM Alignment)", false, jsPWMConfig,
	                	Uint32Value);

  VALIDATE_AND_ASSIGN(config, api_version,  uint16_t, IsUint32,
	                	"(PWM API Version)", false, jsPWMConfig,
	                	Uint32Value);

  VALIDATE_AND_ASSIGN(config, duty_cycle_ns,  uint32_t, IsUint32,
	                	"(PWM Duty Cycle)", false, jsPWMConfig,
	                	Uint32Value);

 VALIDATE_AND_ASSIGN(config, period_ns,  uint32_t, IsUint32,
	                	"(PWM Period)", false, jsPWMConfig,
	                	Uint32Value);


	VALIDATE_AND_ASSIGN(config, enabled, bool, IsBoolean,
	                	"(PWM enabled)", false, jsPWMConfig,
	                	BooleanValue);

	VALIDATE_AND_ASSIGN(config, polarity, sol_pwm_polarity, IsUint32,
	                	"(PWM polarity)", false, jsPWMConfig,
	                	Uint32Value);

	memcpy(p_config, &config, sizeof(sol_pwm_config));

	return true;
}
