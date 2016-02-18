var soletta = require( 'bindings' )( 'soletta' ),
	_ = require( 'lodash' );

var spi_modes = {
	"mode0": soletta.sol_spi_mode.SOL_SPI_MODE_0,
	"mode1": soletta.sol_spi_mode.SOL_SPI_MODE_1,
	"mode2": soletta.sol_spi_mode.SOL_SPI_MODE_2,
	"mode3": soletta.sol_spi_mode.SOL_SPI_MODE_3,
}

exports.open = function( init ) {
	return new Promise( function( fulfill, reject ) {
		var bus = init.bus;
		var config = null;
		var spiMode = mode[init.mode];

		config = {
		    chip_select: init.chipSelect,
		    mode: spiMode,
		    bitsPerWord: init.bitsPerWord,
		    frequency: init.frequency,
		}

		fulfill( SPIBus( soletta.sol_spi_open( bus, config ) ) );
	});

}

var SPIBus = function( bus ) {
	if ( !this._isSPIBus )
		return new SPIBus( bus );
	this._bus = bus;
}

require( "util" ).inherits( SPIBus, require( "events" ).EventEmitter );

_.extend( SPIBus.prototype, {
	_isSPIBus: true,
	onchange: null,

    transfer: function(value) {
       return new Promise( _.bind( function( fulfill, reject ) {
           var buffer;
           if (Buffer.isBuffer(value))
               buffer = value;
           else
               buffer = new Buffer(value);

           soletta.sol_spi_transfer( this._bus, buffer, function( bus, inputData, returnStatus ) {
               // TODO: Convert inputData to SPIData
               fulfill( inputData );
           });
       }, this ) );
    },

	close: function() {
		return new Promise( _.bind( function( fulfill, reject ) {
			fulfill( soletta.sol_spi_close( this._bus) );
		}, this ) );
	},
});

exports.SPIBus = SPIBus;
