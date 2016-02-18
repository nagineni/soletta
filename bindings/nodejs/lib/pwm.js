var soletta = require( 'bindings' )( 'soletta' ),
    _ = require( 'lodash' );

var alignments = {
    "left": soletta.sol_pwm_alignment.SOL_PWM_ALIGNMENT_LEFT,
    "right": soletta.sol_pwm_alignment.SOL_PWM_ALIGNMENT_RIGHT,
    "center": soletta.sol_pwm_alignment.SOL_PWM_ALIGNMENT_CENTER,
}

var polarities = {
    "normal": soletta.sol_pwm_polarity.SOL_PWM_POLARITY_NORMAL,
    "inverted": soletta.sol_pwm_polarity.SOL_PWM_POLARITY_INVERSED,
}

exports.open = function( init ) {
    return new Promise( function( fulfill, reject ) {
        var config = null;

        var alignmentVal = alignments[init.alignment];
        var polarityVal = polarities[init.polarity];

        config = {
            period_ns: init.period,
            duty_cycle_ns: init.dutyCycle,
            alignment: alignmentVal,
            polarity: polarityVal,
            enabled: init.enabled,
        }

        if (typeof init.name === 'string' && init.name !== "") {
            fulfill( PWMPin( soletta.sol_pwm_open_by_label( label, config ) ) );
        } else {
            var device = init.device;
            var channel = init.pin;

            fulfill( PWMPin( soletta.sol_pwm_open( device, channel, config ) ) );
        }

    });
}

var PWMPin = function( pin ) {
    if ( !this._isPWMPin )
        return new PWMPin( pin );
    this._pin = pin;
}

_.extend(PWMPin.prototype, {
    _isPWMPin: true,

    setEnabled: function() {
        return new Promise( _.bind( function( fulfill, reject ) {

            fulfill( soletta.sol_pwm_enabled( this._pin ) );
        }, this ) );
    },

    setDutyCycle: function( value ) {
        return new Promise( _.bind( function( fulfill, reject ) {
            fulfill( soletta.sol_pwm_set_duty_cycle( this._pin, value ) );
        }, this ) );
    },

    setPeriod: function( value ) {
        return new Promise( _.bind( function( fulfill, reject ) {
            fulfill( soletta.sol_pwm_set_period( this._pin, value ) );
        }, this ) );
    },

    close: function() {
        return new Promise( _.bind( function( fulfill, reject ) {
            _pins[ _pin ] = null;
            fulfill( soletta.sol_pwm_close( this._pin) );
        }, this ) );
    },
});

exports.PWMPin = PWMPin;
