var soletta = require( 'bindings' )( 'soletta' ),
	_ = require( 'lodash' );

var baud_rates = {
	"9600": soletta.sol_uart_baud_rate.SOL_UART_BAUD_RATE_9600,
	"19200": soletta.sol_uart_baud_rate.SOL_UART_BAUD_RATE_19200,
	"38400": soletta.sol_uart_baud_rate.SOL_UART_BAUD_RATE_38400,
	"57600": soletta.sol_uart_baud_rate.SOL_UART_BAUD_RATE_57600,
	"115200": soletta.sol_uart_baud_rate.SOL_UART_BAUD_RATE_115200,
}

var data_bits = {
	"5": soletta.sol_uart_data_bits.SOL_UART_DATA_BITS_5,
	"6": soletta.sol_uart_data_bits.SOL_UART_DATA_BITS_6,
	"7": soletta.sol_uart_data_bits.SOL_UART_DATA_BITS_7,
	"8": soletta.sol_uart_data_bits.SOL_UART_DATA_BITS_8,
}

var parities = {
	"none": soletta.sol_uart_parity.SOL_UART_PARITY_NONE,
	"even": soletta.sol_uart_parity.SOL_UART_PARITY_EVEN,
	"odd": soletta.sol_uart_parity.SOL_UART_PARITY_ODD,
}

var stop_bits = {
	"1": soletta.sol_uart_stop_bits.SOL_UART_STOP_BITS_ONE,
	"1": soletta.sol_uart_stop_bits.SOL_UART_STOP_BITS_TWO,
}

function uart_rx_cb( connection, value ) {
	_connections[ connection ].dispatchEvent( "change", {
						type: "change",
						value: value
	} );
}

var _connections = new Array();

exports.open = function( init ) {
	return new Promise( function( fulfill, reject ) {
		var config = null;
		var baudRate = baud_rates[init.baud];
		var dataBit = data_bits[init.dataBits];
		var parityValue = parities[init.parity];
		var stopBit = stop_bits[init.stopBits];

		config = {
		    baud_rate: baudRate,
		    data_bits: dataBit,
		    parity: parityValue,
		    stop_bits: stopBit,
		    rx: uart_rx_cb,
		    flow_control: init.flowControl,
	    }

		fulfill( UARTConnection( soletta.sol_uart_open( init.port, config ) ) );
	});

}

var UARTConnection = function( connection ) {
	if ( !this._isUARTConnection )
		return new UARTConnection( connection );
	this._connection = connection;
	_connections[ connection ] = this;
}

require( "util" ).inherits( UARTConnection, require( "events" ).EventEmitter );


_.extend( UARTConnection.prototype, {
	_isUARTConnection: true,
	onchange: null,

	write: function( value ) {
		return new Promise( _.bind( function( fulfill, reject ) {
            var buffer;
            if (Buffer.isBuffer(value))
                buffer = value;
            else
                buffer = new Buffer(value);

            soletta.sol_uart_write( this._connection, buffer, function( connection, data, returnStatus ) {
                // TODO: Convert inputData to SPIData
               fulfill( returnStatus );
           });
		}, this ) );
	},

	close: function() {
		return new Promise( _.bind( function( fulfill, reject ) {
			_pins[ _pin ] = null;
			fulfill( soletta.sol_uart_close( this._connection) );
		}, this ) );
	},

	addEventListener: UARTConnection.prototype.addListener,

	removeEventListener: UARTConnection.prototype.removeListener,

	dispatchEvent: function( event, request ) {
		this.emit( event, request );
		if ( typeof this[ "on" + event ] === "function" ) {
			this[ "on" + event ]( request );
		}
	},

});

exports.UARTConnection = UARTConnection;
