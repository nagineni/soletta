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

var edges = {
    "none": soletta.sol_gpio_edge.SOL_GPIO_EDGE_NONE,
    "rising": soletta.sol_gpio_edge.SOL_GPIO_EDGE_RISING,
    "falling": soletta.sol_gpio_edge.SOL_GPIO_EDGE_FALLING,
    "any": soletta.sol_gpio_edge.SOL_GPIO_EDGE_BOTH,
}

function pin_read_cb( pin, value ) {
    _pins[ pin ].dispatchEvent( "change", {
                        type: "change",
                        value: value
    } );
}

var _pins = new Array();

exports.open = function( init ) {
    return new Promise( function( fulfill, reject ) {
        var pin = init.pin;
        var dir = 0;
        var drive_mode = 0;
        var config = null;

        if ( init.pullup )
            drive_mode = soletta.sol_gpio_drive.SOL_GPIO_DRIVE_PULL_UP;
        else
            drive_mode = soletta.sol_gpio_drive.SOL_GPIO_DRIVE_PULL_DOWN;

        if ( init.direction == "in" ) {
            var edge = edges[init.edge];

            config = {
                dir: soletta.sol_gpio_direction.SOL_GPIO_DIR_IN,
                active_low: init.activeLow,
                poll_timeout: init.poll,
                drive_mode: drive_mode,
                trigger_mode: edge,
                callback: pin_read_cb,
            }

        } else {
            config = {
                dir: soletta.sol_gpio_direction.SOL_GPIO_DIR_OUT,
                active_low: init.activeLow,
                drive_mode: drive_mode,
            }
        }

        fulfill( GPIOPin( soletta.sol_gpio_open( pin, config ) ) );
    });

}

var GPIOPin = function( pin ) {
    if ( !this._isGPIOPin )
        return new GPIOPin( pin );
    this._pin = pin;
    _pins[ pin ] = this;
}

require( "util" ).inherits( GPIOPin, require( "events" ).EventEmitter );

_.extend( GPIOPin.prototype, {
    _isGPIOPin: true,
    onchange: null,

    read: function() {
        return new Promise( _.bind( function( fulfill, reject ) {

            fulfill( soletta.sol_gpio_read( this._pin ) );
        }, this ) );
    },

    write: function( value ) {
        return new Promise( _.bind( function( fulfill, reject ) {
            fulfill( soletta.sol_gpio_write( this._pin, value ) );
        }, this ) );
    },

    close: function() {
        return new Promise( _.bind( function( fulfill, reject ) {
            _pins[ _pin ] = null;
            fulfill( soletta.sol_gpio_close( this._pin) );
        }, this ) );
    },

    addEventListener: GPIOPin.prototype.addListener,

    removeEventListener: GPIOPin.prototype.removeListener,

    dispatchEvent: function( event, request ) {
        this.emit( event, request );
        if ( typeof this[ "on" + event ] === "function" ) {
            this[ "on" + event ]( request );
        }
    },

});

exports.GPIOPin = GPIOPin;
