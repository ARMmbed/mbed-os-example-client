# Getting started on mbed Client Example

This document describes briefly the steps required to start using the mbed Client example application on mbed OS. The mbed Client example application demonstrates how to register, unregister and read resource values to mbed Device Connector.

## Required hardware
* An [FRDM-K64F](http://developer.mbed.org/platforms/frdm-k64f/) board
* An ethernet connection to the internet
* An ethernet cable
* A micro-USB cable

## Required software

* [Yotta](http://docs.yottabuild.org/#installing) - to build the example programs.

## Setting up the environment
To set up the environment, you will need to do the following:

1. Go to [Device Connector website](connector-test.dev.mbed.com) and log in with your mbed.org account.
2. Configure the mbed Client example program with wanted parameters. See [mbed Build instructions](#mbed-Build-instructions) chapter for more information.
3. Build the application with Yotta.
4. Plug the ethernet cable to the board.
5. Load the application to the FRDM-K64F board.

### IP address setup

This example uses IPV4 to communicate with the mbed Device Connector. The example program should automatically get an IPV4 address from the router when connected via ethernet.

If your network does not have DHCP enabled, you have to manually assign a static IP address to the board. We recommend having DHCP enabled to make everything run smoothly.

## mbed Build instructions		
		
### Building
You can use this example in the following connection modes:

- Non-secure mode
- Certificate mode

The general instructions for all modes are the same. The only difference comes in step 6, when selecting the mode.

#### General 
1. Connect the FRDM-K64F board to the computer with the micro-USB cable. Make sure that you are using the micro-USB port labled **OpenSDA**.
2. Install Yotta. See instructions [here](http://docs.yottabuild.org/#installing).
3. Install the necessary toolchains (`arm-none-eabi-gcc`). Refer to the yotta installation instructions (in step 3) to learn how to install the toolchains.
4. In the command prompt, go to **mbed-client-examples** directory.
5. Select connection mode from below
7. Open main.cpp with your code editor to set registration domain, you **must** use your **mbed developer account username** as a domain name. Domain is set as a parameter to M2MInterfaceFactory::create_interface object. Enter your **mbed developer account username** as domain in `MBED_USER_NAME_DOMAIN`.
7. Set up the target device, `yotta target frdm-k64f-gcc`.
8. In the command prompt, type `yotta build`. The binary file `mbed-client-examples.bin` will be created in the `/build/frdm-k64f-gcc/source/` folder.

#### Setting Non-secure mode
1. Set the `CONN_MODE` value to `M2MSecurity::NoSecurity`.
2. Set `MBED_SERVER_PORT` to `5683`.

#### Setting Certificate mode
1. Set the `CONN_MODE` value to `M2MSecurity::Certificate`.
2. Set `MBED_SERVER_PORT` to `5684`.
3. Go to  [Device Connector website](connector-test.dev.mbed.com)
4. Navigate to *Security credentials* under *My devices*.
5. Click *GET MY DEVICE SECURITY CREDENTIALS*
6. Copy created security credentials to `sources/security.h`

### Flashing to target device

1. Connect the FRDM-K64F board to the internet using an ethernet cable
2. Connect the FRDM-K64F board to your computer using a micro-USB cable. Make sure that you plug into the micro-USB port labeled **OpenSDA**, on the bottom of the board.
3. Find the binary file named `mbed-client-examples.bin` in the folder `mbed-client-examples/build/frdm-k64f-gcc/source/`. Drag and drop the file onto the `MBED` drive on your computer.
4. The board will be programmed when the LED stops flashing. Press the **RESET** button to run the program.

## Testing

### Testing the mbed Client example application with the mbed Device Connector

Ensure that you have flashed the program to your mbed device (see [Flashing to target device](#flashing-to-target-device)).

**Step 1**: Go to [Device Connector website](connector-test.dev.mbed.com).

**Step 2**: Log in using your mbed account.

**Step 3**: Click the *Connected devices* link under *My devices* to see your registered mbed Client example device.

**Step 4**: You can send requests to mbed Client device with Device Connector API. To do that, click *API Console* under *Device Connector*. Click URL to create request. For example: `http://ds-test.dev.mbed.com/v1/endpoints/lwm2m-endpoint/Test/0/S`creates GET request to static **/Test/0/S** resource.

The **/Test/0/S** represents the static resource that is a fixed value set in the mbed Client. 

The **/Test/0/D** represents the dynamic resource which can be read by the mbed Device Server. It is linked with the **SW2** button on the FRDM board. The value starts from zero and every time you press the **SW2** button the node increases counter value by 1. To make a CoAP request to the node resources to get latest value, click **Read**. This returns the latest value of **/Test/0/D**. 

For more information about mbed Device Connector rest API, see [help pages](http://connector-test.dev.mbed.com/#help-rest-api).

**Step 5**: If you press the **SW3** button, the endpoint sends an unregister message to mbed Device Connector. After a successful unregistration, LED **D12** starts blinking indicating that the application has successfully completed the task.
