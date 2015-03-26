# Getting started on LWM2M Client Example

This document describes briefly the steps required to start use of LWM2M Client example application on mbed OS. LWM2M Client example application demonstrates how to perform OMA LWM2M bootstrap with bootstrap server and how to register and unregister to mbed Device Server.

## Required hardware
* A [FRDM-K64F](http://developer.mbed.org/platforms/frdm-k64f/) board
* An ethernet connection to the internet
* An ethernet cable
* A micro-USB cable

## Required Software

* [Yotta](http://docs.yottabuild.org/#installing) - to build example programs
* [mbed Device Server](#user-content-mbed-device-server-mds) - server example program will connect to.

## Optional Software
* [Wireshark](https://www.wireshark.org/) - for packet inspection / network debugging

## Setting up the environment
There are 3 main phases to this example:
- Download and run mDS server on computer
- Configure example mbed program with server address, build with yotta, load onto board, plug board into ethernet
- Verify board talks to server

### IP address setup

This example uses IPV4 to talk to the mbed Device Server(mDS). The example program should automatically grab an IPV4 address from the router when connected via ethernet.

If your network does not have DHCP enabled you will have to manually assign a static IP to the board. We recommend having DHCP enabled to make everything run smoothly.

### Download Bootstrap Server and mbed Device Server (mDS)

Demo application will register to mbed Device Server using OMA LWM2M Bootstrap Server. You should install mDS test version with bootstrap server on your local computer. Refer to mbed Device Server documentation for installing instructions.

If you have licensed the commercial version of mbed Device Server, this can be downloaded from [ARM Connect](http://connect.arm.com/).
Otherwise, a free developer version can also be used with this example and can be downloaded [here](https://silver.arm.com/browse/SEN00).
Go to downloads/Evaluation product/SEN00 – Sensinode/Development tools/Nanoservice platform. Download MBED device server 2.0 or newer.

### Setting up the Bootstrap Server

#### Starting the Bootstrap Server

1. In the bootstrap-server package, go to the `conf` folder.
2. Open the file `bootstrapserver.properties`.
3. Change `coap.port = 5683` to `coap.port = 5693`.
4. Change `admin.secure = true` to `admin.secure = false`
5. Save the file.
6. Go to the `bin` folder.
7. Run the start script:
    - If you are using Linux OS, run the `runBootstrapServer.sh` script in a new shell.
    - If you are using Windows, run the `runBootstrapServer.bat` batch file in a new command prompt.

This will start the bootstrap server on your system.

#### Configuring the Bootstrap Server

1. Navigate to http://localhost:8090.
    - If you're working from a remote machine, you'll need to use the host machine's IP address instead of "localhost".
2. Enter `admin` as both the username and password.
3. Go to the **OMA Servers** tab.
4. Add a new mbed Device Server configuration as shown below:
![Adding an mbed Device Server instance to the Bootstrap Server](/img/addmbedDeviceServer.png)

5. Name the server "test".
6. Configure the IP address to match the mbed Device Server's IP address. For instance, if the IP address of your mDS machine is `192.168.0.1`, you would enter `coap://192.168.0.1:5683`. Click **Save** when finished.
![Configuring an mbed Device Server instance on the Bootstrap Server](/img/addmbedDeviceServerDialog.png)

7. Go to the **OMA Clients** tab.
8. Add your endpoint client as shown below:
![Adding a client instance to the Bootstrap Server](/img/addClient.png)

9. Set the "Client name" to `lwm2m-endpoint`. This should match the mbed example application's name in the main.cpp file. Click **Submit** when finished.
![Adding a client instance to the Bootstrap Server](/img/addClientDialog.png)

The bootstrap server is now configured!

**NOTE:** If you ever stop the bootstrap server's process, you will have to complete these configuration steps again.

### Starting the mbed Device Server (mDS)

1. Go to the `bin` folder in the device server package that you downloaded.
2. Run the start script:
    - If you are using Linux OS, run the `runDS.sh` in a new shell.
    - If you are using Windows, run the `runDS.bat` in a new command prompt.

This will start the mbed Device Server on your system.

### Starting the WebUI ("Lighting" reference app)
1. Go to the `bin` folder in the lighting reference app that you downloaded.
2. Run the start script:
    - If you are using Linux OS, run the `runLighting.sh` in a new shell.
    - If you are using Windows, run the `runLighting.bat` in a new command prompt.

This will start the WebUI on your system.

## mbed Build instructions

### Building
1. Connect the frdm-k64f to the internet using the ethernet cable
2. Connect the frdm-k64f to the computer with the micro-USB cable, being careful to use the micro-usb port labled "OpenSDA"
3. Install Yotta. See instructions from http://docs.yottabuild.org/#installing
4. Install needed toolchains (arm-none-eabi-gcc). Refer to the yotta installation page (in step 1 above) for instructions on how do install the toolchains.
5. Clone **lwm2m-client-example** from https://github.com/ARMmbed/lwm2m-client-example
6. `cd ` **lwm2m-client-example**
7. Open file main.cpp, edit your mbed Device Server's Ipv4 address and port number in place of  `<xxx.xxx.xxx.xxx>` and `<port>` in `coap://<xxx.xxx.xxx.xxx>:<port>` so that it looks something like this `coap://192.168.0.1:5693`
8. Set up target device, `yotta target frdm-k64f-gcc`
9. Type `yotta build`

Binary file will be created to /build/frdm-k64f-gcc/source/ - folder

### Flashing to target device

You need to plug in the USB cable on J26 port on the K64F borad and other end into  USB port of your computer.
Supported mbed board have drag&drop flashing capability. All you need to do is to copy the binary file to
board's usb mass storage device and it will be automatically flashed to target MCU after reset.
You can find the binary file from `cd lwm2m-client-example/build/frdm-k64f-gcc/test/helloworld-lwm2mclient/` with following name `lwm2m-client-example-test-helloworld-lwm2mclient.bin`
Press the reset button to run the program.

## Testing

### Log network traffic (optional)

1. Start Wireshark on the computer where the mbed Device Server is running
2. Select your ethernet interface, usually "Local Area Connection"
3. Click start
4. Select "filter" field on top and add a filter to correspond your bootstrap server and mbed Device Servers ports
5. Power up your mbed board

You should see the endpoint comeplete the OMA LWM2M bootstrap process with the bootstrap server, and after that register with the mbed Device Server.

### Testing OMA lwm2m device with mbed Device Server

Ensure that mDS, the bootstrap server, and the WebUI are all running. These services must be started and configured before the mbed is powered up. See [Setting up the environment](/#setting-up-the-environment) to set up these services.

Power up your mbed. Ensure that you have flashed the program ([Flashing to target device](/#flashing-to-target-device)). Press the reset button to start the program.

1. Open the WebUI by navigating to http://localhost:8083.
    - If you're working from a remote machine, you'll need to use the host machine's IP address instead of "localhost".
2. Enter `demo` as both the username and password.
3. Go to the **End-points** tab. After a short time your device should appear in the list (Refresh the page to update the list).
![Node registered](/img/registered.jpg)

4. Click the endpoint name to view the registered resources. In this release, making requests to resources is not currently implemented.
![Resource list](/img/endpoint_resources.jpg)

Pressing button SW2 will cause the endpoint to send an unregister message to the device server. After successful unregistration, led D12 will start blinking indicating that the application has successfully completed and the endpoint will disappear from the endpoint list in the WebUI.
