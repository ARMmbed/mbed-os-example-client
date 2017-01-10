# NUCLEO_F401RE configuration on mbed OS

Here is explained how the NUCLEO_F401RE board is configured to use mbed-TLS and what optimization parameters are needed in order to fit it in the device.

The application:

* Connects to Thread connection.
* Registers with mbed Device Connector.
* Gives mbed Device Connector access to its resources (read and write).

## Required hardware

* [NUCLEO_F401RE](https://developer.mbed.org/platforms/ST-Nucleo-F401RE/) board.
* 1-2 micro-USB cables.
* mbed 6LoWPAN gateway router](https://firefly-iot.com/product/firefly-6lowpan-gateway-2-4ghz/) for 6LoWPAN ND and Thread.
* mbed 6LoWPAN shield (AT86RF212B/[AT86RF233](https://firefly-iot.com/product/firefly-arduino-shield-2-4ghz/)) for 6LoWPAN ND and Thread.
* Ethernet cable and connection to the internet.

## Supported Target hardware configurations

 This example has been tested in following configuration 
 * NUCLEO_F401RE + ATMEL AT233 15.4 shield

## Required software

* [ARM mbed account](https://developer.mbed.org/account/login/?next=/).
* [mbed-cli](https://github.com/ARMmbed/mbed-cli) - to build the example programs. To learn how to build mbed OS applications with mbed-cli, see [the user guide](https://github.com/ARMmbed/mbed-cli/blob/master/README.md).
* [Serial port monitor](https://developer.mbed.org/handbook/SerialPC#host-interface-and-terminal-applications).

## Application setup

### Configuration

The mbed_app_thread_NUCLEO_F401RE_ATMEL.json configuration file can be found under [configs](./configs) directory.

The file defines MBEDTLS_ENTROPY_HARDWARE_ALT flag, which means that we provide hardware entropy, but in this case it is faked. 
The mbedtls_mbed_client_config_nucleo.h file defines the mbed-TLS configuration. Note that this file is appended after the mbed-TLS config file, so it changes some mbed-TLS system configuration.

The entropy related flags:

* #undef MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES (Do not add default entropy sources. These are the platform specific, mbedtls_timing_hardclock and HAVEGE based poll functions.)
* #define MBEDTLS_NO_PLATFORM_ENTROPY (Do not use built-in platform entropy functions, e.g. /dev/urandom or Windows CryptoAPI.)
* #define MBEDTLS_ENTROPY_C (Enable the platform-specific entropy code)

The faked entropy function is implemented in the hw_test_entropy.c file.

### Client credentials

To register the application with the Connector service, you need to create and set the client side certificate.

1. Go to [mbed Device Connector](https://connector.mbed.com) and log in with your mbed account.
1. On mbed Device Connector, go to [My Devices > Security credentials](https://connector.mbed.com/#credentials) and click the **Get my device security credentials** to get new credentials for your device.
1. Replace the contents in the `security.h` file of this project's directory with the content copied above.

#### mbed gateway

To connect the example application in 6LoWPAN ND or Thread mode to Connector, you need to set up an mbed 6LoWPAN gateway router as follows:

1. Use an Ethernet cable to connect the mbed 6LoWPAN gateway router to the internet.
2. Use a micro-USB cable to connect the mbed 6LoWPAN gateway router to your computer. The computer will list the router as removable storage.
3. The firmware for the gateway is located in the `GW_Binary` folder in the root of this example. Select the binary matching your application bootstrap mode:

	* For the **6LoWPAN ND** bootstrap, use `gateway6LoWPANDynamic.bin`.
	* For the **Thread** bootstrap, use `gatewayThreadDynamic.bin`.

	The dynamic binaries use IPv6 autoconfiguration and enable the client to connect to the Connector service. The static binaries create a site-local IPv6 network and packets cannot be routed outside.

4. Copy the gateway binary file to the mbed 6LoWPAN gateway router to flash the device. The device reboots automatically after flashing. If that does not happen, press the **Reset** button on the board.

You can view debug traces from the gateway with a serial port monitor. The gateway uses baud rate 460800. The gateway IPv6 address is correctly configured when the following trace is visible: `Eth bootstrap ready, IP=XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX`.

#### Channel settings

The default 2.4GHz channel settings are already defined by the [mbed-mesh-api](https://github.com/ARMmbed/mbed-mesh-api) to match the mbed gateway settings. The application can override these settings by adding them to the `mbed_app.json` file in the main project directory. For example:

```json
    "target_overrides": {
        "*": {
            "mbed-mesh-api.6lowpan-nd-channel-page": 0,
            "mbed-mesh-api.6lowpan-nd-channel": 12,
            "mbed-mesh-api.thread-config-channel-page": 0,
            "mbed-mesh-api.thread-config-channel": 12
        }
    }
```

## Building the example

To build the example using mbed CLI:

1. Open a command line tool and navigate to the project’s directory.

2. Import this example:

    ```
    mbed import mbed-os-example-client
    ```

3. Check out the branch -> 

4. To build the application, select the hardware board and build the toolchain using the command:

    ```
    mbed compile -c -t ARM -m NUCLEO_F401RE --app-config configs/mbed_app_thread_NUCLEO_F401RE_ATMEL.json --profile mbed-os/tools/profiles/small.json
    ```

    mbed CLI builds a binary file under the project’s `BUILD/` directory.

5. Plug the Ethernet cable into the board if you are using Ethernet mode.

6. If you are using 6LoWPAN ND or Thread mode, connect and power on the gateway first.

7. Plug the micro-USB cable into the **OpenSDA** port. The board is listed as a mass-storage device.

8. Drag the binary `BUILD/K64F/GCC_ARM/mbed-os-example-client.bin` to the board to flash the application.

9. The board is automatically programmed with the new binary. A flashing LED on it indicates that it is still working. When the LED stops blinking, the board is ready to work.

10. Press the **Reset** button on the board to run the program.

11. For verification, continue to the [Monitoring the application](#monitoring-the-application) chapter.

**To build the example using the Online IDE:**

Import this repository in the Online IDE and continue from step 3 onwards.

## Monitoring the application

The application prints debug messages over the serial port, so you can monitor its activity with a serial port monitor. The application uses baud rate 115200.

<span class="notes">**Note:** Instructions to set this up are located [here](https://developer.mbed.org/handbook/SerialPC#host-interface-and-terminal-applications).</span>

After connecting, you should see messages about connecting to mbed Device Connector:

```
Starting mbed Client example...
Using <Network Interface>

Connected to Network successfully
IP address xxx.xxx.xxx.xxx

SOCKET_MODE : UDP
Connecting to coap://api.connector.mbed.com:5684

Registered

```

## Testing the application

1. Flash the application.
2. Verify that the registration succeeded. You should see `Registered object successfully!` printed to the serial port.
3. On mbed Device Connector, go to [My devices > Connected devices](https://connector.mbed.com/#endpoints). Your device should be listed here.
4. Press the **SW2** button on the device a number of times (make a note of how many times you did that).
5. Go to [Device Connector > API Console](https://connector.mbed.com/#console).
6. Click the **Endpoint directory lookups** drop down menu.
![](/docs/img/ep_lookup.PNG) 
7. In the menu, click **GET** next to **Endpoint's resource representation**. Select your _endpoint_ and _resource-path_. For example, the _endpoint_ is the identifier of your endpoint that can be found in the `security.h` file as `MBED_ENDPOINT_NAME`. Choose `3200/0/5501`as a resource path and click **TEST API**. 
8. The number of times you pressed **SW2** is shown.
9. Press the **SW3** button to unregister from mbed Device Connector. You should see `Unregistered Object Successfully` printed to the serial port and the LED starts blinking. This will also stop your application. Press the **Reset** button to run the program again.

<span class="notes">**Note:** On non-K64F boards, there is no unregistration functionality and button presses are simulated through timer ticks incrementing every 15 seconds.</span>

![SW2 pressed five times, as shown by the API Console](clicks.png)

<span class="tips">**Tip:** If you get an error, for example `Server Response: 410 (Gone)`, clear your browser's cache, log out, and log back in.</span>

<span class="notes">**Note:** Only GET methods can be executed through [Device Connector > API Console](https://connector.mbed.com/#console). For other methods, check the [mbed Device Connector Quick Start](https://github.com/ARMmbed/mbed-connector-api-node-quickstart).

