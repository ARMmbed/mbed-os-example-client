# Getting started on mbed Client Example

This document describes briefly the steps required to start using the mbed Client example application on mbed OS. The mbed Client example application demonstrates how to register and read resource values to mbed Device Connector and deregister from it.

## Required hardware

* An [FRDM-K64F](http://developer.mbed.org/platforms/frdm-k64f/) board.
* An Ethernet connection to the internet.
* An Ethernet cable.
* A micro-USB cable.

## Required software

* [yotta](http://docs.yottabuild.org/#installing) - to build the example programs.

## Setting up the environment

To set up the environment, you need to do the following:

1. Go to [mbed Device Connector website](https://connector.mbed.com) and log in with your mbed.org account.
2. Clone this example into your local computer.
3. Configure the mbed Client example program with desired parameters. See [mbed Build instructions](#mbed-build-instructions) chapter for more information.
4. Set yotta's target for this project, for example `yotta target frdm-k64f-gcc`.
5. Build the application with yotta.
6. Plug the Ethernet cable to the board.
7. Load the application to the FRDM-K64F board.

### IP address setup

This example uses IPV4 to communicate with the [mbed Device Connector Server](https://api.connector.mbed.com). The example program should automatically get an IPV4 address from the router when connected via Ethernet.

If your network does not have DHCP enabled, you have to manually assign a static IP address to the board. We recommend having DHCP enabled to make everything run smoothly.

## mbed Build instructions		
		
### Building
This example uses the Certificate mode.

#### Setting socket type		
		
You can also connect in different socket mode. To select the binding mode for the socket, change `SOCKET_MODE` between `M2MInterface::UDP` and `M2MInterface::TCP`. The instructions further in this document remain same irrespective of the socket mode you select.

#### General 

1. Connect the FRDM-K64F board to the computer with the micro-USB cable. Make sure that you are using the micro-USB port labeled **OpenSDA**.
2. Install yotta. See instructions [here](http://docs.yottabuild.org/#installing).
3. Install the necessary toolchains (`arm-none-eabi-gcc`). Refer to the yotta installation instructions (in step 3) to learn how to install the toolchains.
4. In the command prompt, go to the **mbed-client-examples** directory.
5. Select and set the certificate as instructed [below](#setting-certificate-for-the-application).
6. Set up the target device, `yotta target frdm-k64f-gcc`.
7. In the command prompt, type `yotta build`. The binary file `mbed-client-examples.bin` will be created in the `/build/frdm-k64f-gcc/source/` folder.

#### Setting Certificate for the application

1. Go to  [mbed Device Connector website](https://connector.mbed.com).
2. Navigate to **Security credentials** under **My devices**.
3. Click **GET MY DEVICE SECURITY CREDENTIALS**. You will get the needed certificate information as well as the endpoint name and domain.
4. Copy the created security credentials to `source/security.h`.

### Flashing to target device

1. Connect the FRDM-K64F board to the internet using an Ethernet cable.
2. Connect the FRDM-K64F board to your computer using a micro-USB cable. Make sure that you plug into the micro-USB port labeled **OpenSDA**, on the bottom of the board.
3. Find the binary file named `mbed-client-examples.bin` in the folder `mbed-client-examples/build/frdm-k64f-gcc/source/`. Drag and drop the file onto the `MBED` drive on your computer.
4. The board will be programmed when the LED stops flashing. Press the **RESET** button to run the program.

## Testing

### Testing the mbed Client example application with the mbed Device Connector

Ensure that you have flashed the program to your mbed device (see [Flashing to target device](#flashing-to-target-device)).

**Step 1**: Go to [mbed Device Connector website](https://connector.mbed.com).

**Step 2**: Log in using your mbed account.

**Step 3**: Click the **Connected devices** link under **My devices** to see your registered mbed Client example device.

**Step 4**: You can send requests to mbed Client device with mbed Device Connector API. To do that, click **API Console** under **mbed Device Connector**. Click the URL to create a request. For example: `https://api.connector.mbed.com/endpoints/<Your-endpoint-name>/Test/0/S` creates a GET request to the static **/Test/0/S** resource.

The **/Test/0/S** represents the static resource that is a fixed value set in the mbed Client. 

The **/Test/0/D** represents the dynamic resource that can be read by the mbed Device Server. It is linked with the **SW2** button on the FRDM board. The value starts from zero and every time you press the **SW2** button the node increases the counter value by 1. You can make a CoAP request to the node resources to get the latest value. To do that, click **API Console** under **mbed Device Connector**. Click the URL to create a request. For example: `https://api.connector.mbed.com/endpoints/<Your-endpoint-name>/Test/0/D` creates a GET request to the **/Test/0/D** resource. This returns the latest value of **/Test/0/D**. 

**NOTE:** In case you are getting an error, for example `Server Response : 410(Gone)` or other such error, try clearing the cache of your browser, log out and log in again and then try.

For more information on the mbed Device Connector REST API, see [help pages](https://connector.mbed.com/#help-rest-api).

**Step 5**: If you press the **SW3** button, the endpoint sends a deregister message to the mbed Device Connector. After a successful deregistration, LED **D12** starts blinking indicating that the application has successfully completed the task.
