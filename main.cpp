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
#include <string>
#include <sstream>
#include <vector>
#include "mbed-trace/mbed_trace.h"
#include "mbedtls/entropy_poll.h"

#include "security.h"

#include "mbed.h"

// easy-connect compliancy, it has 2 sets of wifi pins we have only one
#define MBED_CONF_APP_ESP8266_TX MBED_CONF_APP_WIFI_TX
#define MBED_CONF_APP_ESP8266_RX MBED_CONF_APP_WIFI_RX
#include "easy-connect/easy-connect.h"

// Should be defined after easy-connect.h
#include "simpleclient.h"

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

#define BLINK_SIGNAL 0x1

// Status indication
DigitalOut red_led(RED_LED);
DigitalOut green_led(GREEN_LED);
DigitalOut blue_led(BLUE_LED);

Ticker status_ticker;
void blinky() {
    green_led = !green_led;
}

// These are example resource values for the Device Object
struct MbedClientDevice device = {
    "Manufacturer_String",      // Manufacturer
    "Type_String",              // Type
    "ModelNumber_String",       // ModelNumber
    "SerialNumber_String"       // SerialNumber
};

// Instantiate the class which implements LWM2M Client API (from simpleclient.h)
MbedClient mbed_client(device);

// Set up a button interrupt for user interaction
#ifdef MBED_CONF_APP_BUTTON1
	InterruptIn counter_btn(MBED_CONF_APP_BUTTON1);
#endif


/**
 * User interaction handler / simulator. Sets up physical button handler and a ticker
 * for regular updates for the resources.
 *
 * MBED_CONF_APP_BUTTON1 is mapped to actual button pin the mbed_app.json file, where you need to
 * specify board-specific value or leave it undefined if the board does not have buttons.
 */
class InteractionProvider {

public:
	InteractionProvider(Semaphore& updates_sem) : updates(updates_sem) {

	    timer_ticked = false;
	    clicked = false;

		// Set up handler function for the interaction button, if available

#ifdef MBED_CONF_APP_BUTTON1
		counter_btn.fall(this, &InteractionProvider::counter_button_handler);
#endif

	    // Use the counter button handler to send an update of endpoint resource values
		// to connector every 15 seconds periodically.
	    timer.attach(this, &InteractionProvider::timer_handler, 15.0);
	}

	// flags for interaction, these are read from outside interrupt context
	volatile bool timer_ticked;
	volatile bool clicked;


private:

	void timer_handler() {
	    timer_ticked = true;
	    updates.release();
	}

	void counter_button_handler() {
	    clicked = true;
	    updates.release();
	}

	// time-based event source for regular resource updates
	Ticker timer;

	// Network interaction must be performed outside of interrupt context
	Semaphore& updates;

};

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
    LedResource() {
        // create ObjectID with metadata tag of '3201', which is 'digital output'
        blinky_thread.start(callback(this, &LedResource::do_blink));
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
        // Completion of execute function can take a time, that's why delayed response is used
        led_res->set_delayed_response(true);
        blink_args = new BlinkArgs();
    }

    ~LedResource() {
        delete blink_args;
    }

    M2MObject* get_object() {
        return led_object;
    }

    void blink(void *argument) {
        // read the value of 'Pattern'
        status_ticker.detach();
        green_led = LED_OFF;

        M2MObjectInstance* inst = led_object->object_instance();
        M2MResource* res = inst->resource("5853");
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
            String object_name = param->get_argument_object_name();
            uint16_t object_instance_id = param->get_argument_object_instance_id();
            String resource_name = param->get_argument_resource_name();
            int payload_length = param->get_argument_value_length();
            const uint8_t* payload = param->get_argument_value();
            printf("Resource: %s/%d/%s executed\n", object_name.c_str(), object_instance_id, resource_name.c_str());
            printf("Payload: %.*s\n", payload_length, payload);
        }
        // do_blink is called with the vector, and starting at -1
        blinky_thread.signal_set(BLINK_SIGNAL);
    }

private:
    M2MObject* led_object;
    Thread blinky_thread;
    BlinkArgs *blink_args;
    void do_blink() {
        for(;;) {
            blinky_thread.signal_wait(BLINK_SIGNAL);
	        for (;;) {
	            // blink the LED
	            red_led = !red_led;
	            // up the position, if we reached the end of the vector
	            if (blink_args->position >= blink_args->blink_pattern.size()) {
	                // send delayed response after blink is done
	                M2MObjectInstance* inst = led_object->object_instance();
	                M2MResource* led_res = inst->resource("5850");
	                led_res->send_delayed_post_response();
	                red_led = LED_OFF;
	                status_ticker.attach_us(blinky, 250000);
	                break;
	            }
	            // Wait requested time, then continue prosessing the blink pattern from next position.
	            Thread::wait(blink_args->blink_pattern.at(blink_args->position));
	            blink_args->position++;
	        }
        }
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
        if (mbed_client.register_successful()) {
            M2MObjectInstance* inst = btn_object->object_instance();
            M2MResource* res = inst->resource("5501");

            // up counter
            counter++;
            printf("handle_button_click, new value of counter is %d\n", counter);
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

/*
 * The timer contains one property: counter.
 * When `handle_timer_tick` is executed, the counter updates.
 */
class TimerResource {
public:
    TimerResource(): counter(0) {
        // create ObjectID with metadata tag of '3200', which is 'digital input'
        btn_object = M2MInterfaceFactory::create_object("3200");
        M2MObjectInstance* btn_inst = btn_object->create_object_instance();
        // create resource with ID '5502', which is digital input counter
        M2MResource* btn_res = btn_inst->create_dynamic_resource("5502", "Timer",
            M2MResourceInstance::INTEGER, true /* observable */);
        // we can read this value
        btn_res->set_operation(M2MBase::GET_ALLOWED);
        // set initial value (all values in mbed Client are buffers)
        // to be able to read this data easily in the Connector console, we'll use a string
        btn_res->set_value((uint8_t*)"0", 1);
    }

    ~TimerResource() {
    }

    M2MObject* get_object() {
        return btn_object;
    }

    /*
     * When the timer ticks, we read the current value of the click counter
     * from mbed Device Connector, then up the value with one.l
     */
    void handle_timer_tick() {
        if (mbed_client.register_successful()) {
            M2MObjectInstance* inst = btn_object->object_instance();
            M2MResource* res = inst->resource("5502");

            // up counter
            counter++;
            printf("handle_timer_click, new value of counter is %d\n", counter);
            // serialize the value of counter as a string, and tell connector
            char buffer[20];
            int size = sprintf(buffer,"%d",counter);
            res->set_value((uint8_t*)buffer, size);
        } else {
            printf("handle_timer_tick, device not registered\n");
        }
    }

private:
    M2MObject* btn_object;
    uint16_t counter;
};



class BigPayloadResource {
public:
    BigPayloadResource() {
        big_payload = M2MInterfaceFactory::create_object("1000");
        M2MObjectInstance* payload_inst = big_payload->create_object_instance();
        M2MResource* payload_res = payload_inst->create_dynamic_resource("1", "BigData",
            M2MResourceInstance::STRING, true /* observable */);
        payload_res->set_operation(M2MBase::GET_PUT_ALLOWED);
        payload_res->set_value((uint8_t*)"0", 1);
        payload_res->set_incoming_block_message_callback(
                    incoming_block_message_callback(this, &BigPayloadResource::block_message_received));
        payload_res->set_outgoing_block_message_callback(
                    outgoing_block_message_callback(this, &BigPayloadResource::block_message_requested));
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




// debug printf function
void trace_printer(const char* str) {
    printf("%s\r\n", str);
}

// Entry point to the program
int main() {

    unsigned int seed;
    size_t len;

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

    srand(seed);
    red_led = LED_OFF;
    blue_led = LED_OFF;

    status_ticker.attach_us(blinky, 250000);
    // Keep track of the main thread
    osThreadId mainThread = osThreadGetId();

    printf("\nStarting mbed Client example\n");

    mbed_trace_init();
    mbed_trace_print_function_set(trace_printer);
    mbed_trace_config_set(TRACE_MODE_COLOR | TRACE_ACTIVE_LEVEL_INFO | TRACE_CARRIAGE_RETURN);

    NetworkInterface* network = easy_connect(true);
    if(network == NULL) {
        printf("\nConnection to Network Failed - exiting application...\n");
        return -1;
    }

    // we create our button, timer and LED resources
    ButtonResource button_resource;
    LedResource led_resource;
    BigPayloadResource big_payload_resource;
    TimerResource timer_resource;

    // Network interaction must be performed outside of interrupt context
    Semaphore updates(0);

    InteractionProvider interaction_provider(updates);


    // Create endpoint interface to manage register and unregister
    mbed_client.create_interface(MBED_SERVER_ADDRESS, network);

    // Create Objects of varying types, see simpleclient.h for more details on implementation.
    M2MSecurity* register_object = mbed_client.create_register_object(); // server object specifying connector info
    M2MDevice*   device_object   = mbed_client.create_device_object();   // device resources object

    // Create list of Objects to register
    M2MObjectList object_list;

    // Add objects to list
    object_list.push_back(device_object);
    object_list.push_back(button_resource.get_object());
    object_list.push_back(led_resource.get_object());
    object_list.push_back(big_payload_resource.get_object());
    object_list.push_back(timer_resource.get_object());

    // Set endpoint registration object
    mbed_client.set_register_object(register_object);

    // Register with mbed Device Connector
    mbed_client.test_register(register_object, object_list);
    volatile bool registered = true;

    while (true) {
        updates.wait(25000);
        if(registered) {
            if(!interaction_provider.clicked) {
                mbed_client.test_update_register();
            }
        }else {
            break;
        }
        if(interaction_provider.clicked) {
            interaction_provider.clicked = false;
            button_resource.handle_button_click();
        }
        if(interaction_provider.timer_ticked) {
            interaction_provider.timer_ticked = false;
            timer_resource.handle_timer_tick();
        }
    }

    mbed_client.test_unregister();
    status_ticker.detach();
}
