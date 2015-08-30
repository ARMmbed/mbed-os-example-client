# Getting started on mbed Client Example

**DISCLAIMER**: This example application connects to [Device Connector server](http://api.connector.mbed.com) which will be hosted by ARM. You can test this example application through [Device Connector Service](http://connector.mbed.com) Web UI , also hosted by ARM. However, the service may not go-live by the time you are testing this application. In that case, please look out for < Link in mbed.com for announcement>  Device Connector release announcements.

This document describes briefly the steps required to start using the mbed Client example application on mbed OS. The mbed Client example application demonstrates how to register and read resource values to mbed Device Connector and deregister from it.

## Required hardware
* An [FRDM-K64F](http://developer.mbed.org/platforms/frdm-k64f/) board
* An Ethernet connection to the internet
* An Ethernet cable
* A micro-USB cable

## Required software

* [yotta](http://docs.yottabuild.org/#installing) - to build the example programs.

## Setting up the environment
To set up the environment, you need to do the following:

1. Go to [Device Connector website](http://connector.mbed.com) and log in with your mbed.org account.
2. Configure the mbed Client example program with desired parameters. See [mbed Build instructions](#mbed-build-instructions) chapter for more information.
3. Build the application with yotta.
4. Plug the Ethernet cable to the board.
5. Load the application to the FRDM-K64F board.

### IP address setup

This example uses IPV4 to communicate with the mbed Device Connector. The example program should automatically get an IPV4 address from the router when connected via Ethernet.

If your network does not have DHCP enabled, you have to manually assign a static IP address to the board. We recommend having DHCP enabled to make everything run smoothly.

## mbed Build instructions		
		
### Building
You can use this example in the following connection modes:

- Non-secure mode
- Certificate mode

The general instructions for both modes are the same. The only difference comes in step 5, when selecting the mode.

#### General 
1. Connect the FRDM-K64F board to the computer with the micro-USB cable. Make sure that you are using the micro-USB port labeled **OpenSDA**.
2. Install yotta. See instructions [here](http://docs.yottabuild.org/#installing).
3. Install the necessary toolchains (`arm-none-eabi-gcc`). Refer to the yotta installation instructions (in step 3) to learn how to install the toolchains.
4. In the command prompt, go to **mbed-client-examples** directory.
5. Select and set the connection mode as instructed below.
6. Set up the target device, `yotta target frdm-k64f-gcc`.
7. In the command prompt, type `yotta build`. The binary file `mbed-client-examples.bin` will be created in the `/build/frdm-k64f-gcc/source/` folder.

#### Setting Non-secure mode
1. Set the `CONN_MODE` value to `M2MSecurity::NoSecurity`.
2. Set `MBED_SERVER_PORT` to `5683`.
3. Open `sources/security.h` with your code editor to set the registration domain. You **must** use your **mbed developer account username** as a domain name. Enter your **mbed developer account username** as domain in `MBED_DOMAIN`.
4. The endpoint registration name is defined as `MBED_ENDPOINT_NAME` in `sources/security.h`. You can change it by modifying it with your code editor.

#### Setting Certificate mode
1. Set the `CONN_MODE` value to `M2MSecurity::Certificate`.
2. Set `MBED_SERVER_PORT` to `5684`.
3. Go to  [Device Connector website](http://connector-test.dev.mbed.com).
4. Navigate to **Security credentials** under **My devices**.
5. Click **GET MY DEVICE SECURITY CREDENTIALS**. You will get the needed certificate information as well as the endpoint name and domain.
6. Copy the created security credentials to `sources/security.h`

### Flashing to target device

1. Connect the FRDM-K64F board to the internet using an Ethernet cable
2. Connect the FRDM-K64F board to your computer using a micro-USB cable. Make sure that you plug into the micro-USB port labeled **OpenSDA**, on the bottom of the board.
3. Find the binary file named `mbed-client-examples.bin` in the folder `mbed-client-examples/build/frdm-k64f-gcc/source/`. Drag and drop the file onto the `MBED` drive on your computer.
4. The board will be programmed when the LED stops flashing. Press the **RESET** button to run the program.

## Testing

### Testing the mbed Client example application with the mbed Device Connector

Ensure that you have flashed the program to your mbed device (see [Flashing to target device](#flashing-to-target-device)).

**Step 1**: Go to [Device Connector website](http://connector.mbed.com).

**Step 2**: Log in using your mbed account.

**Step 3**: Click the **Connected devices** link under **My devices** to see your registered mbed Client example device.

**Step 4**: You can send requests to mbed Client device with Device Connector API. To do that, click **API Console** under **Device Connector**. Click the URL to create a request. For example: `http://api.connector.mbed.com/v1/endpoints/lwm2m-endpoint/Test/0/S` creates a GET request to the static **/Test/0/S** resource.

The **/Test/0/S** represents the static resource that is a fixed value set in the mbed Client. 

The **/Test/0/D** represents the dynamic resource that can be read by the mbed Device Server. It is linked with the **SW2** button on the FRDM board. The value starts from zero and every time you press the **SW2** button the node increases the counter value by 1. You can make a CoAP request to the node resources to get the latest value. To do that, click **API Console** under **Device Connector**. Click the URL to create a request. For example: `http://api.connector.mbed.com/v1/endpoints/lwm2m-endpoint/Test/0/D` creates a GET request to the **/Test/0/D** resource.This returns the latest value of **/Test/0/D**. 

NOTE: Remember to select the `sync=true` option while making the REST URL query for the resource value else the API will return error.

For more information on the mbed Device Connector REST API, see [help pages](http://connector.mbed.com/#help-rest-api).

**Step 5**: If you press the **SW3** button, the endpoint sends a deregister message to the mbed Device Connector. After a successful deregistration, LED **D12** starts blinking indicating that the application has successfully completed the task.
