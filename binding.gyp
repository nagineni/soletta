{
	"variables": {
		"BUILD_SOLETTA": '<!(sh bindings/nodejs/establish-flags.sh BUILD_SOLETTA)',
		"SOLETTA_CFLAGS": [ '<!@(sh bindings/nodejs/establish-flags.sh SOLETTA_CFLAGS)' ],
		"SOLETTA_LIBS": [ '<!@(sh bindings/nodejs/establish-flags.sh SOLETTA_LIBS)' ]
	},
	"conditions": [
		[ "'<(BUILD_SOLETTA)'=='true'", {
			"targets": [
				{
					"target_name": "csdk",
					"type": "none",
					"actions": [ {
						"action_name": "build-csdk",
						"message": "Building C SDK",
						"outputs": [ "build/soletta_sysroot" ],
						"inputs": [ "" ],
						"action": [ "sh", "bindings/nodejs/build-for-npm.sh" ]
					} ]
				}
			]
		}, {
			"targets": [
				{
					"target_name": "collectbindings",
					"type": "none",
					"actions": [ {
						"action_name": "collectbindings",
						"message": "Collecting bindings",
						"outputs": [ "bindings/nodejs/generated/main.cc" ],
						"inputs": [
							"bindings/nodejs/generated/main.cc.prologue",
							"bindings/nodejs/generated/main.cc.epilogue",
						],
						"action": [
							"sh",
							"-c",
							'./bindings/nodejs/generate-main.sh <(SOLETTA_CFLAGS)'
						]
					} ]
				},
				{
					"target_name": "soletta",
					"sources": [
						"bindings/nodejs/generated/main.cc",
						"bindings/nodejs/src/async-bridge.cc",
						"bindings/nodejs/src/data.cc",
						"bindings/nodejs/src/hijack.c",
						"bindings/nodejs/src/functions/aio.cc",
						"bindings/nodejs/src/functions/gpio.cc",
						"bindings/nodejs/src/functions/pwm.cc",
						"bindings/nodejs/src/functions/spi.cc",
						"bindings/nodejs/src/functions/uart.cc",
						"bindings/nodejs/src/functions/simple.cc",
						"bindings/nodejs/src/structures/handles.cc",
						"bindings/nodejs/src/structures/sol-js-gpio.cc",
						"bindings/nodejs/src/structures/sol-js-pwm.cc",
						"bindings/nodejs/src/structures/sol-js-spi.cc",
						"bindings/nodejs/src/structures/sol-js-uart.cc",
					],
					"include_dirs": [
						"<!(node -e \"require('nan')\")"
					],
					"cflags": [ '<(SOLETTA_CFLAGS)' ],
					"xcode_settings": {
						"OTHER_CFLAGS": [ '<(SOLETTA_CFLAGS)' ]
					},
					"libraries": [ '<(SOLETTA_LIBS)' ],
					"dependencies": [ "collectbindings" ]
				}
			]
		} ]
	]
}
