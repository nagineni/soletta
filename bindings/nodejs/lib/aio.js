/*
 * This file is part of the Soletta Project
 *
 * Copyright (C) 2015 Intel Corporation. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
