# Getting started on LWM2M Client Example

This document describes briefly the steps required to start use of LWM2M Client example application on mbed OS. LWM2M Client example application demonstrates how to register, unregister, read resource values and send resource observations onto mbed Device Server.

## Required hardware
* A [FRDM-K64F](http://developer.mbed.org/platforms/frdm-k64f/) board
* An ethernet connection to the internet
* An ethernet cable
* A micro-USB cable

## Required Software

* [Yotta](http://docs.yottabuild.org/#installing) - to build example programs
* [mbed Device Server (mDS)](#download-mbed-device-server-mds) - server example program will connect to.

## Optional Software
* [Wireshark](https://www.wireshark.org/) - for packet inspection / network debugging

## Setting up the environment
There are 3 main phases to this example:

- Download and run mDS server on computer
- Configure lwm2m-client-example program with server address, build with yotta, load onto board, plug board into ethernet
- Verify board talks to server

Note: You might need to open UDP port 5683 in your computer's firewall for mDS to communicate with this example application.

### IP address setup

This example uses IPV4 to talk to the mbed Device Server(mDS). The example program should automatically grab an IPV4 address from the router when connected via ethernet.

If your network does not have DHCP enabled you will have to manually assign a static IP to the board. We recommend having DHCP enabled to make everything run smoothly.

### Download mbed Device Server (mDS)

Example application will register to mbed Device Server. You should install mDS on your local computer.

1. Download the free developer version, which is used with this example, from [ARM silver](https://silver.arm.com/browse/SEN00).
2. Unzip the package on your local computer. You should see the following files:
```
Device Server.tar.gz
Device Server Clients.tar.gz
Device Server Tools.tar.gz
Ref Apps.tar.gz
```

### Starting the mbed Device Server (mDS)

Binary file will be created to /build/frdm-k64f-gcc/source/ - folder

### Flashing to target device

1. Connect the frdm-k64f to the internet using an ethernet cable
2. Connect the frdm-k64f to your computer with a micro-USB cable, being careful to use the micro-usb port labled "OpenSDA" on the bottom of the board.
3. Find the binary file named `lwm2m-client-example.bin` in the folder  `lwm2m-client-example/build/frdm-k64f-gcc/source/`. Drag and drop the file onto the "MBED" drive on your computer.

The board will be programmed when the LED stops flashing. Press the reset button to run the program.

## Testing

### Log network traffic (optional)

1. Start Wireshark on the computer where the mbed Device Server is running
2. Select your ethernet interface, usually "Local Area Connection"
3. Click **Start**
4. Select the "Filter" field in the toolbar and add a filter to correspond to your mbed Device Server. Press Enter after you have entered the expression to activate the filter.
  - For example, if your server's IP address is `123.123.123.123`, you would enter `ip.addr == 123.123.123.123` and press Enter.
5. Power up your mbed board and press the reset button.

You should see the endpoint after it has registered with the mbed Device Server.

### Testing lwm2m client example application with mbed Device Server

Ensure that mDS and the WebUI are running (see [Setting up the environment](#setting-up-the-environment)). Also, ensure that you have flashed the program to your mbed (see [Flashing to target device](#flashing-to-target-device)).

1. Open the WebUI by navigating to http://localhost:8082.
    - If you're working from a remote machine, you'll need to use the host machine's IP address instead of "localhost".
2. Enter `demo` as both the username and password.
3. Go to the **End-points** tab. After a short time your device should appear in the list (Refresh the page to update the list).

![Node registered](img/registered.jpg)

4. Click the endpoint name to view the registered resources. 

![Resource list](img/endpoint_resources.jpg)

5. You can make CoAP request to node resources by pressing "Read" - button.

![Read resources](img/read_resources.jpg)

6. In /Test - tab is resources demonstrating Dynamic and Static resource functionality. 

7. /Test/0/D represents dynamic resource which is observed by mbed Device Server. It is linked with SW2 button on the FRDM board. The value starts from zero and every time SW2 button is being pressed, node sends counter value to mbed Device server, and it is updated to UI without need to press "read".

![Dynamic Resource](img/dynamic_resource.jpg)

8. /Test/0/S represents static resource which is fixed value set in mbed Client. You can make CoAP request to node resources by pressing "Read" - button. This returns the fixed value of "Static value".

![Static Resource](img/static_resource.jpg)

Pressing button SW3 will cause endpoint to send unregister message to device server. After successful unregistration, led D12 starts blinking indicating that application has successfully completed and endpoint will disappear from endpoint list in web UI.
