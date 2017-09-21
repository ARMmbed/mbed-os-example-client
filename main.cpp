/*
 * Copyright (c) 2015, 2016 ARM Limited. All rights reserved.
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
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include "simpleclient.h"
#include <string>
#include <sstream>
#include <vector>
#include "mbed-trace/mbed_trace.h"
#include "mbedtls/entropy_poll.h"
#include "mbedtls_mbed_client_config.h"

#include "security.h"
#ifndef __linux__
#include "memory_tests.h"
#include "mbed.h"


// easy-connect compliancy, it has 2 sets of wifi pins we have only one
#define MBED_CONF_APP_ESP8266_TX MBED_CONF_APP_WIFI_TX
#define MBED_CONF_APP_ESP8266_RX MBED_CONF_APP_WIFI_RX
#include "easy-connect/easy-connect.h"


#ifdef TARGET_STM
#define RED_LED (LED3)
#define GREEN_LED (LED1)
#define BLUE_LED (LED2)
#define LED_ON (1)		     
#else // !TARGET_STM
#define RED_LED (LED1)
#define GREEN_LED (LED2)
#define BLUE_LED (LED3)			     
#define LED_ON (0) 
#endif // !TARGET_STM
#define LED_OFF (!LED_ON)

// Status indication
DigitalOut red_led(RED_LED);
DigitalOut green_led(GREEN_LED);
DigitalOut blue_led(BLUE_LED);

Ticker status_ticker;
void blinky() {
    green_led = !green_led;
}
#else
#include <stdio.h>
#include <unistd.h>
#define MBED_SERVER_ADDRESS "coap://api.connector.mbed.com:5684"
#endif

#define TRACE_GROUP "main"

// These are example resource values for the Device Object
struct MbedClientDevice device = {
    "Manufacturer_String",      // Manufacturer
    "Type_String",              // Type
    "ModelNumber_String",       // ModelNumber
    "SerialNumber_String"       // SerialNumber
};

// Instantiate the class which implements LWM2M Client API (from simpleclient.h)
MbedClient mbed_client(device);


#ifndef __linux__
// In case of K64F board , there is button resource available
// to change resource value and unregister
#ifdef TARGET_K64F
// Set up Hardware interrupt button.
InterruptIn obs_button(SW2);
InterruptIn unreg_button(SW3);
#else
//In non K64F boards , set up a timer to simulate updating resource,
// there is no functionality to unregister.
Ticker timer;
#endif

#ifdef MBED_MEM_TRACING_ENABLED
#include "mbed_mem_trace.h"
//#include "memory_statistics.h"
extern "C" void mem_stat_init();
#endif

#else

pthread_t blinky_thread;

static void *linux_blink(void* arg);
#endif//__linux__

#ifdef __cplusplus
    extern "C" {
#endif
void custom_memory_free(void *ptr) {
    M2MCoreMemory::memory_free(ptr);
}
#ifdef __cplusplus
    }
#endif

#ifdef __cplusplus
    extern "C" {
#endif
void *custom_memory_calloc(size_t num, size_t size) {

    void* ptr = M2MCoreMemory::memory_temp_alloc(num * size);
    memset(ptr, 0, num * size);
    return ptr;
}
#ifdef __cplusplus
    }
#endif


/*
 * Arguments for running "blink" in it's own thread.
 */
class BlinkArgs {
public:
    BlinkArgs() {
        clear();
    }
    void clear() {
        position = 0;
        blink_pattern.clear();
    }
    uint16_t position;
    std::vector<uint32_t> blink_pattern;
};


/*
 * The Led contains one property (pattern) and a function (blink).
 * When the function blink is executed, the pattern is read, and the LED
 * will blink based on the pattern.
 */
class LedResource {
public:
    LedResource():
    token(NULL),
    token_length(0){
    }

    bool allocate_resources() {
        // create ObjectID with metadata tag of '3201', which is 'digital output'
        led_object = M2MInterfaceFactory::create_object(3201);
        if(!led_object) {
            return false;
        }

        M2MObjectInstance* led_inst = led_object->create_object_instance();

        if(!led_inst) {
            delete led_object;
            return false;
        }

        // 5853 = Multi-state output

        M2MResource*  pattern_res = led_inst->create_dynamic_resource(5853,"Pattern",
            M2MResourceInstance::STRING, false);


        if(!pattern_res) {
            delete led_object;
            return false;
        }

        // read and write
        pattern_res->set_operation(M2MBase::GET_PUT_ALLOWED);
        // set initial pattern (toggle every 200ms. 7 toggles in total)
        pattern_res->set_value((const uint8_t*)"500:500:500:500:500:500:500", 27);

        // there's not really an execute LWM2M ID that matches... hmm...
        M2MResource*  led_res = led_inst->create_dynamic_resource(5850,"Blink",
            M2MResourceInstance::OPAQUE, false);

        if(!led_res) {
            delete led_object;
            return false;
        }

        // we allow executing a function here...
        led_res->set_operation(M2MBase::POST_ALLOWED);
        // when a POST comes in, we want to execute the led_execute_callback
        led_res->set_execute_function(execute_callback(this, &LedResource::blink));
        // Completion of execute function can take a time, that's why delayed response is used
        blink_args = new BlinkArgs();
        if(!blink_args) {
            delete led_object;
            return false;
        }
        return true;
    }

    ~LedResource() {
        if(token){
            free(token);
        }
        delete blink_args;
        delete led_object;
    }

    M2MObject* get_object() {
        return led_object;
    }

    void blink(void *argument) {
        // read the value of 'Pattern'
#ifndef __linux__
        status_ticker.detach();
        green_led = LED_OFF;
#endif
        M2MObjectInstance* inst = led_object->object_instance();
        M2MResource* res = inst->resource(5853);
        // Clear previous blink data
        blink_args->clear();

        // values in mbed Client are all buffers, and we need a vector of int's
        uint8_t* buffIn = NULL;
        uint32_t sizeIn;
        res->get_value(buffIn, sizeIn);

        // turn the buffer into a string, and initialize a vector<int> on the heap
        std::string s((char*)buffIn, sizeIn);
        free(buffIn);
        printf("led_execute_callback pattern=%s\n", s.c_str());

        // our pattern is something like 500:200:500, so parse that
        std::size_t found = s.find_first_of(":");
        while (found!=std::string::npos) {
            blink_args->blink_pattern.push_back(atoi((const char*)s.substr(0,found).c_str()));
            s = s.substr(found+1);
            found=s.find_first_of(":");
            if(found == std::string::npos) {
                blink_args->blink_pattern.push_back(atoi((const char*)s.c_str()));
            }
        }
        // check if POST contains payload
        if (argument) {
            M2MResource::M2MExecuteParameter* param = (M2MResource::M2MExecuteParameter*)argument;
            uint16_t object_id = param->get_argument_object_id();
            uint16_t object_instance_id = param->get_argument_object_instance_id();
            uint16_t resource_id = param->get_argument_resource_id();
            int payload_length = param->get_argument_value_length();
            const uint8_t* payload = param->get_argument_value();
            printf("Resource: %d/%d/%d executed\n", object_id, object_instance_id, resource_id);
            if(payload) {
                printf("Payload: %.*s\n", payload_length, payload);
            }
            if(token) {
                free(token);
                token = NULL;
            }
            token = (uint8_t*)malloc(param->get_token_length());
            memcpy(token, param->get_token(), param->get_token_length());
            token_length = param->get_token_length();
            printf("Token Length %d\n", token_length);
            tr_debug("Token (%s)", tr_array(token, token_length));

        }
#ifndef __linux__
        // do_blink is called with the vector, and starting at -1
        blinky_thread.start(callback(this, &LedResource::do_blink));
#else
        pthread_create(&blinky_thread, NULL, &linux_blink, this);
#endif
    }

private:
    M2MObject* led_object;
#ifndef __linux__
    Thread blinky_thread;
#endif
    BlinkArgs *blink_args;
    uint8_t *token;
    uint8_t token_length;

    void do_blink() {
        for (;;) {
#ifndef __linux__
            // blink the LED
            red_led = !red_led;
#endif
            // up the position, if we reached the end of the vector
            if (blink_args->position >= blink_args->blink_pattern.size()) {
                // send delayed response after blink is done
                M2MObjectInstance* inst = led_object->object_instance();
                M2MResource* led_res = inst->resource(5850);
#ifdef __linux__
                usleep(1000);
#endif
            led_res->send_post_response(token, token_length);
#ifndef __linux__
                red_led = LED_OFF;
                status_ticker.attach_us(blinky, 250000);
#endif
                return;
            }
#ifndef __linux
            // Wait requested time, then continue prosessing the blink pattern from next position.
            Thread::wait(blink_args->blink_pattern.at(blink_args->position));
#endif
            blink_args->position++;
        }

    }
};

#ifdef __linux__

static void *linux_blink(void* arg) {
    LedResource * led_res = (LedResource*)arg;
    M2MObject *led_object = led_res->get_object();
    for (;;) {
        // send delayed response
        M2MObjectInstance* inst = led_object->object_instance();
        M2MResource* led_res = inst->resource(5850);
        usleep(3000);
        led_res->send_post_response(led_res->token, led_res->token_length);
        return NULL;
    }
}
#endif

/*
 * The button contains one property (click count).
 * When `handle_button_click` is executed, the counter updates.
 */
class ButtonResource {
public:
    ButtonResource(): counter(0) {        
    }

    ~ButtonResource() {
        delete btn_object;
    }

    bool allocate_resources() {
        // create ObjectID with metadata tag of '3200', which is 'digital input'
        btn_object = M2MInterfaceFactory::create_object(3200);
        if(!btn_object) {
            return false;
        }
        M2MObjectInstance* btn_inst = btn_object->create_object_instance();

        if(!btn_object) {
            delete btn_object;
            return false;
        }
        // create resource with ID '5501', which is digital input counter

        M2MResource*  btn_res = btn_inst->create_dynamic_resource(5501,"Button",
            M2MResourceInstance::INTEGER, true);

        if(!btn_res) {
            delete btn_object;
            return false;
        }
        // we can read this value
        btn_res->set_operation(M2MBase::GET_ALLOWED);
        // set initial value (all values in mbed Client are buffers)
        // to be able to read this data easily in the Connector console, we'll use a string
        btn_res->set_value((uint8_t*)"0", 1);
        return true;
    }

    M2MObject* get_object() {
        return btn_object;
    }

    /*
     * When you press the button, we read the current value of the click counter
     * from mbed Device Connector, then up the value with one.
     */
    void handle_button_click() {
        if (mbed_client.register_successful()) {
            M2MObjectInstance* inst = btn_object->object_instance();
            M2MResource* res = inst->resource(5501);

            // up counter
            counter++;
#ifndef __linux__
    #ifdef TARGET_K64F
            printf("handle_button_click, new value of counter is %d\n", counter);
    #else
            printf("simulate button_click, new value of counter is %d\n", counter);
    #endif
#else
            printf("simulate button_click, new value of counter is %d\n", counter);
#endif
            // serialize the value of counter as a string, and tell connector
            char buffer[20];
            int size = sprintf(buffer,"%d",counter);
            res->set_value((uint8_t*)buffer, size);
        } else {
            printf("simulate button_click, device not registered\n");
        }
    }

private:
    M2MObject* btn_object;
    uint16_t counter;
};

class BigPayloadResource {
public:
    BigPayloadResource() {
    }

    ~BigPayloadResource() {
        delete big_payload;
    }

    bool allocate_resources() {
        big_payload = M2MInterfaceFactory::create_object(1000);
        if(!big_payload) {
            return false;
        }
        M2MObjectInstance* payload_inst = big_payload->create_object_instance();
        if(!payload_inst) {
            delete big_payload;
            return false;
        }

        M2MResource*  payload_res = payload_inst->create_dynamic_resource(1,"BigData",
            M2MResourceInstance::INTEGER, true);

        if(!payload_res) {
            delete big_payload;
            return false;
        }
        payload_res->set_operation(M2MBase::GET_PUT_ALLOWED);
        payload_res->set_value((uint8_t*)"0", 1);
#ifndef DISABLE_BLOCK_MESSAGE
        payload_res->set_incoming_block_message_callback(
                    incoming_block_message_callback(this, &BigPayloadResource::block_message_received));
        payload_res->set_outgoing_block_message_callback(
                    outgoing_block_message_callback(this, &BigPayloadResource::block_message_requested));
#endif
        return true;

    }

    M2MObject* get_object() {
        return big_payload;
    }

    void block_message_received(M2MBlockMessage *argument) {
        if (argument) {
            if (M2MBlockMessage::ErrorNone == argument->error_code()) {
                if (argument->is_last_block()) {
                    printf("Last block received\n");
                }
                printf("Block number: %d\n", argument->block_number());
                // First block received
                if (argument->block_number() == 0) {
                    // Store block
                // More blocks coming
                } else {
                    // Store blocks
                }
            } else {
                printf("Error when receiving block message!  - EntityTooLarge\n");
            }
            printf("Total message size: %" PRIu32 "\n", argument->total_message_size());
        }
    }

    void block_message_requested(const String& resource, uint8_t *&/*data*/, uint32_t &/*len*/) {
        printf("GET request received for resource: %s\n", resource.c_str());
        // Copy data and length to coap response
    }

private:
    M2MObject*  big_payload;
};

#ifndef __linux__
// Network interaction must be performed outside of interrupt context
Semaphore updates(0);
osThreadId mainThread;
#endif

volatile bool registered = false;
volatile bool clicked = false;

void unregister() {
    registered = false;
#ifndef __linux__
    updates.release();
#endif
}

void button_clicked() {
    clicked = true;
#ifndef __linux__
    updates.release();
#endif
}

// Entry point to the program
int main() {
#ifdef MBED_MEM_TRACING_ENABLED
    mem_stat_init();
#endif

    m2mobject_stats();
    unsigned int seed;
    size_t len;

#ifndef __linux__
#ifdef MBEDTLS_ENTROPY_HARDWARE_ALT
    // Used to randomize source port
    mbedtls_hardware_poll(NULL, (unsigned char *) &seed, sizeof seed, &len);

#elif defined MBEDTLS_TEST_NULL_ENTROPY

#warning "mbedTLS security feature is disabled. Connection will not be secure !! Implement proper hardware entropy for your selected hardware."
    // Used to randomize source port
    mbedtls_null_entropy_poll( NULL,(unsigned char *) &seed, sizeof seed, &len);

#else

#error "This hardware does not have entropy, endpoint will not register to Connector.\
You need to enable NULL ENTROPY for your application, but if this configuration change is made then no security is offered by mbed TLS.\
Add MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES and MBEDTLS_TEST_NULL_ENTROPY in mbed_app.json macros to register your endpoint."

#endif

#endif //#ifndef __linux__

    srand(seed);
#ifndef __linux__
    red_led = LED_OFF;
    blue_led = LED_OFF;

    status_ticker.attach_us(blinky, 250000);
    // Keep track of the main thread
    mainThread = osThreadGetId();

    printf("\nStarting mbed Client example\n");
    heap_stats();
#endif

    mbed_trace_init();


#ifndef __linux__
    NetworkInterface* network = easy_connect(true);
    if(network == NULL) {
        printf("\nConnection to Network Failed - exiting application...\n");
        return -1;
    }
    heap_stats();
#else
    void *network = NULL;
    unsigned int net=0xFFFFFFFF;
    network = &net;
#endif

    // Create endpoint interface to manage register and unregister
    if(!mbed_client.create_interface(MBED_SERVER_ADDRESS, network)) {
        printf("\nCannot create mbedClient interface - exiting application...\n");
        return -1;
    }
#ifndef __linux__

    printf("\nAfter creating mbedClient Interface\n");
    heap_stats();
#endif

    // we create our button and LED resources
    ButtonResource button_resource;
    LedResource led_resource;
    BigPayloadResource big_payload_resource;

    if(!button_resource.allocate_resources() ||
       !led_resource.allocate_resources() ||
       !big_payload_resource.allocate_resources()) {
        printf("\nResource Allocation failed - exiting application...\n");
        return -1;
    }
#ifndef __linux__

    printf("\nAfter creating mbedClient Resources\n");
    heap_stats();

#ifdef TARGET_K64F
    // On press of SW3 button on K64F board, example application
    // will call unregister API towards mbed Device Connector
    //unreg_button.fall(&mbed_client,&MbedClient::test_unregister);
    unreg_button.fall(&unregister);

    // Observation Button (SW2) press will send update of endpoint resource values to connector
    obs_button.fall(&button_clicked);
#else
    // Send update of endpoint resource values to connector every 15 seconds periodically
    timer.attach(&button_clicked, 15.0);
#endif

#endif // #ifndef __linux__

    // Create Objects of varying types, see simpleclient.h for more details on implementation.
    M2MSecurity* register_object = mbed_client.create_register_object(); // server object specifying connector info
    M2MDevice*   device_object   = mbed_client.create_device_object();   // device resources object
#ifndef __linux__
    printf("\nAfter creating mbedClient Device and Register object\n");
    heap_stats();
#endif

    if(!register_object || !device_object) {
         printf("\nCannot create Regsiter or Device Object- exiting application...\n");
         return -1;
    }
    // Create list of Objects to register
    M2MObjectList object_list;

    // Add objects to list
    object_list.push_back(device_object);
    object_list.push_back(button_resource.get_object());
    object_list.push_back(led_resource.get_object());
    object_list.push_back(big_payload_resource.get_object());

    // Set endpoint registration object
    mbed_client.set_register_object(register_object);

    // Register with mbed Device Connector
    mbed_client.test_register(register_object, object_list);
#ifndef __linux__

    printf("\nAfter calling mbedClient register\n");
    heap_stats();
#endif

    registered = true;

#ifndef __linux__
    while (true) {
        updates.wait(10000);
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
#else
    while(!mbed_client.unregister_successful()) {
        sleep(1);
    }
#endif

#ifndef __linux__
    mbed_client.test_unregister();
    status_ticker.detach();
#endif
}
