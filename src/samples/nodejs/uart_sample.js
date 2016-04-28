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

/*
 * This sample code demonstrates the usage of UART JS bindings for
 * reading and writing data to a UART accessible device.
 *
 * Setup:
 * 1. Connect Edison to host using Serial (FTDI) to USB cable:
 *    - Plug FTDI cable to Edison pins(TX->FTDI RX, RX->FTDI TX, GND->FTDI GND)
      - Plug the other end of the FTDI cable in to a USB port on host.
 * 2. Launch serial connection on host against ttyX: screen /dev/ttyUSB0 115200
 * 3. Run the sample on Edison and start communicating to the device by sending
 *    some text from the host side.
 */
var uart = require( "soletta/uart" ),
    uartConnection = null;

// Setup UART
uart.open( {
    port: "ttyMFD1"
} ).then( function( connection ) {
    uartConnection = connection;

    // Setup 'onread' event handler to receive the data.
    connection.onread = function( event ) {
        console.log( "UART data: " + String.fromCharCode( event.data ) );
    };

    // Sending data ON
    connection.write( "UART test " ).then( function() {
        connection.write( [ 0x62, 0x75, 0x66, 0x66, 0x65, 0x72 ] ).then( () => {
            console.log("Data sent on UART");
        } );
    } ).catch( function( error ) {
        console.log( "Failed to write on UART: ", error );
        process.exit();
    } );
} ).catch( function( error ) {
    console.log( "UART error: ", error );
    process.exit();
} );

// Press Ctrl+C to exit the process
process.on( "SIGINT", function() {
    process.exit();
} );

process.on( "exit", function() {
    if ( uartConnection ) {
        uartConnection.close();
    }
} );
