set -e

cp configs/eth_v4.json ./mbed_app.json
cp configs/eth-wifi-mbedignore ./.mbedignore
mbed compile -m K64F -t GCC_ARM -c
cp BUILD/K64F/GCC_ARM/mbed-os-example-client.bin k64f_eth_v4.bin

cp configs/eth_v6.json ./mbed_app.json
cp configs/eth-wifi-mbedignore ./.mbedignore
mbed compile -m K64F -t GCC_ARM -c
cp BUILD/K64F/GCC_ARM/mbed-os-example-client.bin k64f_eth_v6.bin

cp configs/wifi_v4.json ./mbed_app.json
cp configs/eth-wifi-mbedignore ./.mbedignore
mbed compile -m K64F -t GCC_ARM -c
cp BUILD/K64F/GCC_ARM/mbed-os-example-client.bin k64f_wifi_v4.bin
mbed compile -m UBLOX_EVK_ODIN_W2 -t GCC_ARM -c
cp ./BUILD/UBLOX_EVK_ODIN_W2/GCC_ARM/mbed-os-example-client.bin ublox_odin_wifi_v4.bin

cp configs/wifi_v6.json ./mbed_app.json
cp configs/eth-wifi-mbedignore ./.mbedignore
mbed compile -m K64F -t GCC_ARM -c
cp BUILD/K64F/GCC_ARM/mbed-os-example-client.bin k64f_wifi_v6.bin
mbed compile -m UBLOX_EVK_ODIN_W2 -t GCC_ARM -c
cp ./BUILD/UBLOX_EVK_ODIN_W2/GCC_ARM/mbed-os-example-client.bin ublox_odin_wifi_v6.bin

cp configs/mesh_6lowpan.json ./mbed_app.json
cp configs/mesh-mbedignore ./.mbedignore
mbed compile -m K64F -t GCC_ARM -c
cp BUILD/K64F/GCC_ARM/mbed-os-example-client.bin k64f_6lowpan.bin

cp configs/mesh_thread.json ./mbed_app.json
cp configs/mesh-mbedignore ./.mbedignore
mbed compile -m K64F -t GCC_ARM -c
cp BUILD/K64F/GCC_ARM/mbed-os-example-client.bin k64f_thread.bin




