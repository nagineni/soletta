var soletta = require( 'bindings' )( 'soletta' ),
	_ = require( 'lodash' );


exports.open = function( init ) {
	return new Promise( function( fulfill, reject ) {
		var precision = 12;

		if (init.precision)
			precision = init.precision;

		if (init.name && init.name != "") {
			fulfill( AIOPin( soletta.sol_aio_open_by_label( init.name, precision ) ) );
		} else if (init.raw) {
			fulfill( AIOPin( soletta.sol_aio_open_raw( init.device, init.pin, precision ) ) );
		} else {
			fulfill( AIOPin( soletta.sol_aio_open( init.device, init.pin, precision ) ) );
		}
	});

}

var AIOPin = function( pin ) {
	if ( !this._isAIOPin )
		return new AIOPin( pin );
	this._pin = pin;
}

_.extend( AIOPin.prototype, {
	_isAIOPin: true,

	read: function() {
		return new Promise( _.bind( function( fulfill, reject ) {
			soletta.sol_aio_get_value( this._pin, function( value ) {
				fulfill( value );
			});
		}, this ) );
	},

	close: function() {
		return new Promise( _.bind( function( fulfill, reject ) {
			fulfill( soletta.sol_aio_close( this._pin) );
		}, this ) );
	},
});

exports.AIOPin = AIOPin;