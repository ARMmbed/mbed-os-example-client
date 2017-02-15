#!/bin/bash  
#
set -e
TOOL=GCC_ARM

echo Compiling with $TOOL
echo Ethernet v4
cp configs/eth_v4.json ./mbed_app.json
cp configs/eth-wifi-mbedignore ./.mbedignore
mbed compile -m K64F -t $TOOL
cp BUILD/K64F/$TOOL/mbed-os-example-client.bin k64f-$TOOL-eth-v4.bin
mbed compile -m NUCLEO_F429ZI -t $TOOL
cp ./BUILD/NUCLEO_F429ZI/$TOOL/mbed-os-example-client.bin f429zi-$TOOL-eth-v4.bin
mbed compile -m UBLOX_EVK_ODIN_W2 -t $TOOL
cp ./BUILD/UBLOX_EVK_ODIN_W2/$TOOL/mbed-os-example-client.bin ublox-odin-$TOOL-eth-v4.bin

echo Ethernet v6
cp configs/eth_v6.json ./mbed_app.json
cp configs/eth-wifi-mbedignore ./.mbedignore
mbed compile -m K64F -t $TOOL
cp BUILD/K64F/$TOOL/mbed-os-example-client.bin k64f-$TOOL-eth-v6.bin
mbed compile -m NUCLEO_F429ZI -t $TOOL
cp ./BUILD/NUCLEO_F429ZI/$TOOL/mbed-os-example-client.bin f429zi-$TOOL-eth-v4.bin
mbed compile -m UBLOX_EVK_ODIN_W2 -t $TOOL
cp ./BUILD/UBLOX_EVK_ODIN_W2/$TOOL/mbed-os-example-client.bin ublox-odin-$TOOL-eth-v6.bin

echo WIFI - ESP8266 
cp configs/wifi_esp8266_v4.json ./mbed_app.json
cp configs/eth-wifi-mbedignore ./.mbedignore
mbed compile -m K64F -t $TOOL
cp BUILD/K64F/$TOOL/mbed-os-example-client.bin k64f-$TOOL-esp-wifi-v4.bin
mbed compile -m NUCLEO_F429ZI -t $TOOL
cp ./BUILD/NUCLEO_F429ZI/$TOOL/mbed-os-example-client.bin f429zi-$TOOL-esp-wifi-v4.bin

echo WIFI - ODIN for UBLOX_EVK_ODIN_W2
cp configs/wifi_odin_v4.json ./mbed_app.json
cp configs/eth-wifi-mbedignore ./.mbedignore
mbed compile -m UBLOX_EVK_ODIN_W2 -t $TOOL
cp ./BUILD/UBLOX_EVK_ODIN_W2/$TOOL/mbed-os-example-client.bin ublox-odin-$TOOL-wifi-v4.bin

echo 6-Lowpan builds
cp configs/mesh_6lowpan.json ./mbed_app.json
cp configs/mesh-mbedignore ./.mbedignore
mbed compile -m K64F -t $TOOL
cp BUILD/K64F/$TOOL/mbed-os-example-client.bin k64f-$TOOL-6lowpan.bin
mbed compile -m NUCLEO_F429ZI -t $TOOL
cp ./BUILD/NUCLEO_F429ZI/$TOOL/mbed-os-example-client.bin f429zi-$TOOL-6lowpan.bin

echo 6-Lowpan Sub-1 GHz builds
cp configs/mesh_6lowpan_subg.json ./mbed_app.json
cp configs/mesh-mbedignore ./.mbedignore
mbed compile -m NUCLEO_F429ZI -t $TOOL
cp ./BUILD/NUCLEO_F429ZI/$TOOL/mbed-os-example-client.bin f429zi-$TOOL-6lowpan-subg.bin

echo Thread builds
cp configs/mesh_thread.json ./mbed_app.json
cp configs/mesh-mbedignore ./.mbedignore
mbed compile -m K64F -t $TOOL
cp BUILD/K64F/$TOOL/mbed-os-example-client.bin k64f-$TOOL-Thread.bin
mbed compile -m NUCLEO_F429ZI -t $TOOL
cp ./BUILD/NUCLEO_F429ZI/$TOOL/mbed-os-example-client.bin f429zi-$TOOL-Thread.bin
