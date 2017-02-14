#!/bin/bash  
#
set -e
TOOL=GCC_ARM

echo Compiling with $TOOL
cp configs/eth_v4.json ./mbed_app.json
cp configs/eth-wifi-mbedignore ./.mbedignore
mbed compile -m K64F -t $TOOL
cp BUILD/K64F/GCC_ARM/mbed-os-example-client.bin k64f-$TOOL-eth-v4.bin
mbed compile -m NUCLEO_F429ZI -t $TOOL
cp ./BUILD/NUCLEO_F429ZI/GCC_ARM/mbed-os-example-client.bin f429zi-$TOOL-eth-v4.bin

cp configs/eth_v6.json ./mbed_app.json
cp configs/eth-wifi-mbedignore ./.mbedignore
mbed compile -m K64F -t $TOOL
cp BUILD/K64F/GCC_ARM/mbed-os-example-client.bin k64f-$TOOL-eth-v6.bin
mbed compile -m NUCLEO_F429ZI -t $TOOL
cp ./BUILD/NUCLEO_F429ZI/GCC_ARM/mbed-os-example-client.bin f429zi-$TOOL-eth-v4.bin

cp configs/wifi_v4.json ./mbed_app.json
cp configs/eth-wifi-mbedignore ./.mbedignore
mbed compile -m K64F -t $TOOL
cp BUILD/K64F/GCC_ARM/mbed-os-example-client.bin k64f-$TOOL-wifi-v4.bin
mbed compile -m UBLOX_EVK_ODIN_W2 -t $TOOL
cp ./BUILD/UBLOX_EVK_ODIN_W2/GCC_ARM/mbed-os-example-client.bin ublox-odin-$TOOL-wifi-v4.bin

cp configs/mesh_6lowpan.json ./mbed_app.json
cp configs/mesh-mbedignore ./.mbedignore
mbed compile -m K64F -t $TOOL
cp BUILD/K64F/GCC_ARM/mbed-os-example-client.bin k64f-$TOOL-6lowpan.bin
mbed compile -m NUCLEO_F429ZI -t $TOOL
cp ./BUILD/NUCLEO_F429ZI/GCC_ARM/mbed-os-example-client.bin f429zi-$TOOL-6lowpan.bin

cp configs/mesh_6lowpan_spirit1.json ./mbed_app.json
cp configs/mesh-mbedignore ./.mbedignore
mbed compile -m NUCLEO_F429ZI -t $TOOL
cp ./BUILD/NUCLEO_F429ZI/GCC_ARM/mbed-os-example-client.bin f429zi-$TOOL-6lowpan-spirit1.bin

cp configs/mesh_thread.json ./mbed_app.json
cp configs/mesh-mbedignore ./.mbedignore
mbed compile -m K64F -t $TOOL
cp BUILD/K64F/GCC_ARM/mbed-os-example-client.bin k64f-$TOOL-Thread.bin
mbed compile -m NUCLEO_F429ZI -t $TOOL
cp ./BUILD/NUCLEO_F429ZI/GCC_ARM/mbed-os-example-client.bin f429zi-$TOOL-Thread.bin
