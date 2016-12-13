#!/bin/bash  
# Copyright (c) 2016 ARM Ltd.
#
set -e
TOOL=GCC_ARM

echo Compiling with $TOOL
cp configs/eth_v4.json ./mbed_app.json
cp configs/eth-wifi-mbedignore ./.mbedignore
mbed compile -m K64F -t $TOOL
cp BUILD/K64F/GCC_ARM/mbed-os-example-client.bin k64f_$TOOL_eth_v4.bin
mbed compile -m NUCLEO_F439ZI -t $TOOL
cp ./BUILD/NUCLEO_F439ZI/GCC_ARM/mbed-os-example-client.bin f439zi-$TOOL-eth-v4.bin

cp configs/eth_v6.json ./mbed_app.json
cp configs/eth-wifi-mbedignore ./.mbedignore
mbed compile -m K64F -t $TOOL
cp BUILD/K64F/GCC_ARM/mbed-os-example-client.bin k64f_$TOOL_eth_v6.bin
mbed compile -m NUCLEO_F439ZI -t $TOOL
cp ./BUILD/NUCLEO_F439ZI/GCC_ARM/mbed-os-example-client.bin f439zi-$TOOL-eth-v4.bin

cp configs/wifi_v4.json ./mbed_app.json
cp configs/eth-wifi-mbedignore ./.mbedignore
mbed compile -m K64F -t $TOOL
cp BUILD/K64F/GCC_ARM/mbed-os-example-client.bin k64f_$TOOL_wifi_v4.bin
mbed compile -m UBLOX_EVK_ODIN_W2 -t $TOOL
cp ./BUILD/UBLOX_EVK_ODIN_W2/GCC_ARM/mbed-os-example-client.bin ublox_odin_$TOOL_wifi_v4.bin

cp configs/wifi_v6.json ./mbed_app.json
cp configs/eth-wifi-mbedignore ./.mbedignore
mbed compile -m K64F -t $TOOL
cp BUILD/K64F/GCC_ARM/mbed-os-example-client.bin k64f_$TOOL_wifi_v6.bin
mbed compile -m UBLOX_EVK_ODIN_W2 -t $TOOL
cp ./BUILD/UBLOX_EVK_ODIN_W2/GCC_ARM/mbed-os-example-client.bin ublox_odin_$TOOL_wifi_v6.bin

cp configs/mesh_6lowpan.json ./mbed_app.json
cp configs/mesh-mbedignore ./.mbedignore
mbed compile -m K64F -t $TOOL
cp BUILD/K64F/GCC_ARM/mbed-os-example-client.bin k64f_$TOOL_6lowpan.bin
mbed compile -m NUCLEO_F439ZI -t $toolchain
cp ./BUILD/NUCLEO_F439ZI/GCC_ARM/mbed-os-example-client.bin f439zi-$TOOL-6lowpan-v4.bin

cp configs/mesh_thread.json ./mbed_app.json
cp configs/mesh-mbedignore ./.mbedignore
mbed compile -m K64F -t $TOOL
cp BUILD/K64F/GCC_ARM/mbed-os-example-client.bin k64f_$TOOL_thread.bin
mbed compile -m NUCLEO_F439ZI -t $toolchain
cp ./BUILD/NUCLEO_F439ZI/GCC_ARM/mbed-os-example-client.bin f439zi-$TOOL-thread-v4.bin
