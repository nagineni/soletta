var soletta = require( 'bindings' )( 'soletta' ),
	_ = require( 'lodash' );

var speed = {
	"10kbps": soletta.sol_i2c_speed.SOL_I2C_SPEED_10KBIT,
	"100kbps": soletta.sol_i2c_speed.SOL_I2C_SPEED_100KBIT,
	"400kbps": soletta.sol_i2c_speed.SOL_I2C_SPEED_400KBIT,
	"1000kbps": soletta.sol_i2c_speed.SOL_I2C_SPEED_1MBIT,
	"3400kbps": soletta.sol_i2c_speed.SOL_I2C_SPEED_3MBIT_400KBIT,	
}


exports.open = function( init ) {
	return new Promise( function( fulfill, reject ) {
		var bus = init.bus;
		var spd = speed[init.speed];
		var i2cbus = null;

		if (init.raw) {
			i2cbus = I2CBus( soletta.sol_i2c_open_raw( bus, spd ) );
		} else {
			i2cbus = I2CBus( soletta.sol_i2c_open( bus, spd ) );
		}

		//copy the properties
		_.extend(i2cbus, init);

		fulfill( i2cbus );
	});

}

var I2CBus = function( i2cbus ) {
	if ( !this._isI2CBus )
		return new I2CBus ( i2cbus );
	this._i2cbus = i2cbus;
}

_.extend( I2CBus.prototype, {
	_isI2CBus: true,
	_pending: null,
	busy: {
		get: function() {
			return soletta.sol_i2c_busy( this._i2cbus );
		}
	},

	read: function( device, size, register, repetitions ) {
		return new Promise( _.bind( function( fulfill, reject ) {
			if (!repetitions)
				repetitions = 1;

			soletta.sol_i2c_set_slave_address( this._i2cbus, device );
			if (register == null) {
				this._pending = soletta.sol_i2c_read( this._i2cbus, register, size, function( i2c, data, status ) {
					this._pending = null;
					fulfill( data.toString() );
				});
			} else if (repetitions > 1) {
				this._pending = soletta.sol_i2c_read_register_multiple( this._i2cbus, register, size, repetitions, function( i2c, register, data, status ) {
					this._pending = null;
					fulfill( data.toString() );
				});
			} else {
				this._pending = soletta.sol_i2c_read_register( this._i2cbus, register, size, function( i2c, register, data, status ) {
					this._pending = null;
					fulfill( data.toString() );
				});
			}
		}, this ) );
	},

	write: function( device, data, register ) {	
		return new Promise( _.bind( function( fulfill, reject ) {
			soletta.sol_i2c_set_slave_address( this._i2cbus, device );
			var buf;

			buf = new Buffer(data);
			if (!register) {
				this._pending = soletta.sol_i2c_write( this._i2cbus, buf, function( i2sbus, data, status ) {
					this._pending = null;
					fulfill();					
				} );
			} else {
				this._pending = soletta.sol_i2c_write_register( this._i2cbus, register, buf, function( i2sbus, register, data, status ) {
					this._pending = null;
					fulfill();
				} );
			}
		}, this ) );
	},

	writeBit: function( device, data ) {	
		return new Promise( _.bind( function( fulfill, reject ) {
			soletta.sol_i2c_set_slave_address( this._i2cbus, device );
			this._pending = soletta.sol_i2c_write_quick( this._i2cbus, data, function( i2sbus, status ) {
				this._pending = null;
				fulfill();
			} );
		}, this ) );
	},

	close: function() {
		return new Promise( _.bind( function( fulfill, reject ) {
			if (this.raw)
				fulfill( soletta.sol_i2c_close_raw( this._i2cbus) );
			else
				fulfill( soletta.sol_i2c_close( this._i2cbus) );
		}, this ) );
	},
});

exports.I2CBus = I2CBus;