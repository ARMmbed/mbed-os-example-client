/*
 * Copyright (c) 2015 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "simpleclient.h"
#include <string>
#include <sstream>
#include <vector>
#include "mbed-trace/mbed_trace.h"

#include "security.h"

#include "mbed.h"
#include "rtos.h"

#define ETHERNET
#undef WIFI
#undef CELLULAR
#undef MESH_LOWPAN_ND
#undef MESH_THREAD

#if defined WIFI
#include "ESP8266Interface.h"
ESP8266Interface esp(D1, D0);
#elif defined (CELLULAR)
#include "C027Interface.h"
C027Interface c027;
#elif defined (ETHERNET)
#include "LWIPInterface.h"
LWIPInterface lwip;
#elif defined (MESH_LOWPAN_ND)
#define MESH
#include "NanostackInterface.h"
LoWPANNDInterface mesh;
#elif defined (MESH_THREAD)
#define MESH
#include "NanostackInterface.h"
ThreadInterface mesh;
#endif

#ifndef MESH
// This is address to mbed Device Connector
#define MBED_SERVER_ADDRESS "coap://api.connector.mbed.com:5684"
#else
// This is address to mbed Device Connector
#define MBED_SERVER_ADDRESS YOTTA_CFG_DEVICE_CONNECTOR_URI
#endif

Serial output(USBTX, USBRX);

// These are example resource values for the Device Object
struct MbedClientDevice device = {
    "Manufacturer_String",      // Manufacturer
    "Type_String",              // Type
    "ModelNumber_String",       // ModelNumber
    "SerialNumber_String"       // SerialNumber
};

// Instantiate the class which implements LWM2M Client API (from simpleclient.h)
MbedClient mbed_client(device);

// Set up Hardware interrupt button.
InterruptIn obs_button(SW2);
InterruptIn unreg_button(SW3);

// LED Output
DigitalOut led1(LED1);

/*
 * The Led contains one property (pattern) and a function (blink).
 * When the function blink is executed, the pattern is read, and the LED
 * will blink based on the pattern.
 */
class LedResource {
public:
    LedResource() {
        // create ObjectID with metadata tag of '3201', which is 'digital output'
        led_object = M2MInterfaceFactory::create_object("3201");
        M2MObjectInstance* led_inst = led_object->create_object_instance();

        // 5853 = Multi-state output
        M2MResource* pattern_res = led_inst->create_dynamic_resource("5853", "Pattern",
            M2MResourceInstance::STRING, false);
        // read and write
        pattern_res->set_operation(M2MBase::GET_PUT_ALLOWED);
        // set initial pattern (toggle every 200ms. 7 toggles in total)
        pattern_res->set_value((const uint8_t*)"500:500:500:500:500:500:500", 27);

        // there's not really an execute LWM2M ID that matches... hmm...
        M2MResource* led_res = led_inst->create_dynamic_resource("5850", "Blink",
            M2MResourceInstance::OPAQUE, false);
        // we allow executing a function here...
        led_res->set_operation(M2MBase::POST_ALLOWED);
        // when a POST comes in, we want to execute the led_execute_callback
        led_res->set_execute_function(execute_callback(this, &LedResource::blink));
    }

    M2MObject* get_object() {
        return led_object;
    }

    void blink(void *) {
        // read the value of 'Pattern'
        M2MObjectInstance* inst = led_object->object_instance();
        M2MResource* res = inst->resource("5853");

        // values in mbed Client are all buffers, and we need a vector of int's
        uint8_t* buffIn = NULL;
        uint32_t sizeIn;
        res->get_value(buffIn, sizeIn);

        // turn the buffer into a string, and initialize a vector<int> on the heap
        std::string s((char*)buffIn, sizeIn);
        std::vector<uint32_t>* v = new std::vector<uint32_t>;
        std::stringstream ss(s);
        std::string item;
        // our pattern is something like 500:200:500, so parse that
        while (std::getline(ss, item, ':')) {
            // then convert to integer, and push to the vector
            v->push_back(atoi((const char*)item.c_str()));
        }

        output.printf("led_execute_callback pattern=%s\r\n", s.c_str());

        // do_blink is called with the vector, and starting at -1
        do_blink(v, 0);
    }

private:
    M2MObject* led_object;

    void do_blink(std::vector<uint32_t>* pattern, uint16_t position) {
        // blink the LED
        led1 = !led1;

        // up the position, if we reached the end of the vector
        if (position >= pattern->size()) {
            // free memory, and exit this function
            delete pattern;
            return;
        }

        // how long do we need to wait before the next blink?
        uint32_t delay_ms = pattern->at(position);

        // Invoke same function after `delay_ms` (upping position)
        Thread::wait(delay_ms);
        do_blink(pattern, ++position);
    }
};

/*
 * The button contains one property (click count).
 * When `handle_button_click` is executed, the counter updates.
 */
class ButtonResource {
public:
    ButtonResource(): counter(0) {
        // create ObjectID with metadata tag of '3200', which is 'digital input'
        btn_object = M2MInterfaceFactory::create_object("3200");
        M2MObjectInstance* btn_inst = btn_object->create_object_instance();
        // create resource with ID '5501', which is digital input counter
        M2MResource* btn_res = btn_inst->create_dynamic_resource("5501", "Button",
            M2MResourceInstance::INTEGER, true /* observable */);
        // we can read this value
        btn_res->set_operation(M2MBase::GET_ALLOWED);
        // set initial value (all values in mbed Client are buffers)
        // to be able to read this data easily in the Connector console, we'll use a string
        btn_res->set_value((uint8_t*)"0", 1);        
    }

    ~ButtonResource() {
    }

    M2MObject* get_object() {
        return btn_object;
    }

    /*
     * When you press the button, we read the current value of the click counter
     * from mbed Device Connector, then up the value with one.
     */
    void handle_button_click() {
        M2MObjectInstance* inst = btn_object->object_instance();
        M2MResource* res = inst->resource("5501");

        // up counter
        counter++;

        printf("handle_button_click, new value of counter is %d\r\n", counter);

        // serialize the value of counter as a string, and tell connector
        stringstream ss;
        ss << counter;
        std::string stringified = ss.str();
        res->set_value((uint8_t*)stringified.c_str(), stringified.length());
    }

private:
    M2MObject* btn_object;
    uint16_t counter;
};

// Network interaction must be performed outside of interrupt context
Semaphore updates(0);
volatile bool registered = false;
volatile bool clicked = false;
osThreadId mainThread;

void unregister() {
    registered = false;
    updates.release();
}

void button_clicked() {
    clicked = true;
    updates.release();
}

// debug printf function
void trace_printer(const char* str) {
    printf("%s\r\n", str);
}

// Status indication
Ticker status_ticker;
DigitalOut status_led(LED1);
void blinky() { status_led = !status_led; }


// Entry point to the program
int main() {
    status_ticker.attach_us(blinky, 250000);

    // Keep track of the main thread
    mainThread = osThreadGetId();

    // Sets the console baud-rate
    output.baud(115200);

    output.printf("Starting mbed Client example...\r\n");

    mbed_trace_init();
    mbed_trace_print_function_set(trace_printer);

    NetworkStack *network_stack = 0;
#if defined WIFI
    output.printf("\n\rUsing WiFi \r\n");
    output.printf("\n\rConnecting to WiFi..\r\n");
    #error "Remember to provide your WiFi credentials and provide in esp.connect("ssid","password");"
    esp.connect("ssid", "password");
    output.printf("\n\rConnected to WiFi\r\n");
    network_stack = &esp;
#elif defined ETHERNET
    lwip.connect();
    output.printf("Using Ethernet LWIP\r\n");
    network_stack = &lwip;
#elif defined (CELLULAR)
    c027.connect();
    output.printf("Using Cellular C027\r\n");
    network_stack = &c027;
#elif defined MESH
    mesh.connect();
    network_stack = &mesh;
#endif
    const char *ip_addr = network_stack->get_ip_address();
    if (ip_addr) {
        output.printf("IP address %s\r\n", ip_addr);
    } else {
        output.printf("No IP address\r\n");
    }

    // we create our button and LED resources
    ButtonResource button_resource;
    LedResource led_resource;

    // On press of SW3 button on K64F board, example application
    // will call unregister API towards mbed Device Connector
    //unreg_button.fall(&mbed_client,&MbedClient::test_unregister);
    unreg_button.fall(&unregister);

    // Observation Button (SW2) press will send update of endpoint resource values to connector
    obs_button.fall(&button_clicked);

    // Create endpoint interface to manage register and unregister
    mbed_client.create_interface(MBED_SERVER_ADDRESS, network_stack);

    // Create Objects of varying types, see simpleclient.h for more details on implementation.
    M2MSecurity* register_object = mbed_client.create_register_object(); // server object specifying connector info
    M2MDevice*   device_object   = mbed_client.create_device_object();   // device resources object

    // Create list of Objects to register
    M2MObjectList object_list;

    // Add objects to list
    object_list.push_back(device_object);
    object_list.push_back(button_resource.get_object());
    object_list.push_back(led_resource.get_object());

    // Set endpoint registration object
    mbed_client.set_register_object(register_object);

    // Register with mbed Device Connector
    mbed_client.test_register(register_object, object_list);
    registered = true;

    while (true) {
        updates.wait(25000);
        if(registered) {
            if(!clicked) {
                mbed_client.test_update_register();
            }
        }else {
            break;
        }
        if(clicked) {
           clicked = false;
            button_resource.handle_button_click();
        }
    }

    mbed_client.test_unregister();
    status_ticker.detach();
}
