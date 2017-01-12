#ifndef RESOURCE_DATA
#define RESOURCE_DATA

#ifdef MBED_HEAP_STATS_ENABLED
#include "mbed_stats.h"
#endif

#ifdef MBED_MEM_TRACING_ENABLED
#include "mbed_mem_trace.h"
#endif

//#define ENABLE_MBED_CLIENT_MBED_TLS_DEBUGS

// 1
const static sn_nsdl_static_resource_parameters_s my_params_static = {
    (char*)"",            // resource_type_ptr
    (char*)"",            // interface_description_ptr
    (uint8_t*)"9/0/0",    // path
    (uint8_t*)"",           // resource
    5,                      // strlen("x/x/x")
    0,                      // resourcelen
    false,                  // external_memory_block
    SN_GRS_DYNAMIC,         // mode
    false                   // free_on_delete
};

static sn_nsdl_dynamic_resource_parameters_s firmware_package_params_dynamic = {
    __nsdl_c_callback,
    &my_params_static,
    {NULL, NULL},                     // link
    COAP_CONTENT_OMA_PLAIN_TEXT_TYPE, // coap_content_type
    M2MBase::GET_PUT_ALLOWED,   // access
    0,                      // registered
    true,                  // publish_uri
    false,                  // free_on_delete
    false                    // observable
};

const static M2MBase::lwm2m_parameters my_update_params = {
    0, // max_age
    0, // instance_id
    0, // name_id
    (char*)"0", // name
    &firmware_package_params_dynamic,
    M2MBase::Resource, // base_type
    false // free_on_delete
};

/*
// 2
const static sn_nsdl_static_resource_parameters_s my_params_static_2 = {
    (char*)"",            // resource_type_ptr
    (char*)"",            // interface_description_ptr
    (uint8_t*)"9/0/1",    // path
    (uint8_t*)"",           // resource
    5,                      // strlen("x/x/x")
    0,                      // resourcelen
    false,                  // external_memory_block
    SN_GRS_DYNAMIC,         // mode
    false                   // free_on_delete
};

static sn_nsdl_dynamic_resource_parameters_s firmware_package_params_dynamic_2 = {
    __nsdl_c_callback,
    &my_params_static_2,
    {NULL, NULL},                     // link
    COAP_CONTENT_OMA_PLAIN_TEXT_TYPE, // coap_content_type
    M2MBase::GET_PUT_ALLOWED,   // access
    0,                      // registered
    true,                  // publish_uri
    false,                  // free_on_delete
    false                    // observable
};

const static M2MBase::lwm2m_parameters my_update_params_2 = {
    0, // max_age
    0, // instance_id
    0, // name_id
    (char*)"1", // name
    &firmware_package_params_dynamic_2,
    M2MBase::Resource, // base_type
    false // free_on_delete
};
*/

// 3
const static sn_nsdl_static_resource_parameters_s my_params_static_3 = {
    (char*)"",            // resource_type_ptr
    (char*)"",            // interface_description_ptr
    (uint8_t*)"9/0/2",    // path
    (uint8_t*)"foo",           // resource
    5,                      // strlen("x/x/x")
    3,                      // resourcelen
    false,                  // external_memory_block
    SN_GRS_STATIC,         // mode
    false                   // free_on_delete
};

static sn_nsdl_dynamic_resource_parameters_s firmware_package_params_dynamic_3 = {
    __nsdl_c_callback,
    &my_params_static_3,
    {NULL, NULL},                     // link
    COAP_CONTENT_OMA_PLAIN_TEXT_TYPE, // coap_content_type
    M2MBase::GET_ALLOWED,   // access
    0,                      // registered
    true,                  // publish_uri
    false,                  // free_on_delete
    false                    // observable
};

const static M2MBase::lwm2m_parameters my_update_params_3 = {
    0, // max_age
    0, // instance_id
    0, // name_id
    (char*)"2", // name
    &firmware_package_params_dynamic_3,
    M2MBase::Resource, // base_type
    false // free_on_delete
};

volatile uint32_t last_alloc=0;
M2MObject* my_obj;

uint32_t get_alloc_size() 
{
	uint32_t ret=0;	
	mbed_stats_heap_t stats;
    mbed_stats_heap_get(&stats);
	ret = stats.current_size-last_alloc;
	last_alloc = stats.current_size;	    
	
	return ret;
}

void const_data_test() {

        #ifdef MBED_MEM_TRACING_ENABLED        
        mbed_mem_trace_set_callback(mbed_mem_trace_callback);
        #endif
        output.printf("very_simple_test\n\r");
        output.printf("Simple test \n\r");
        output.printf("M2MResource size %d\n", sizeof(M2MResource));
        output.printf("M2MDevice size %d\n", sizeof(M2MDevice));
        output.printf("M2MObject size %d\n", sizeof(M2MObject));
        output.printf("M2MSecurity size %d\n", sizeof(M2MSecurity));
        output.printf("M2MObjectInstance size %d\n", sizeof(M2MObjectInstance));
        output.printf("M2MResourceInstance size %d\n", sizeof(M2MResourceInstance));
        output.printf("M2MBase size %d\n", sizeof(M2MBase));
        output.printf("String size %d\n", sizeof(String));               
        
        M2MResource* dyn_res=NULL;
        M2MResource* dyn_res_2=NULL;
        M2MResource* stat_res=NULL;
        uint32_t start = last_alloc;
                
        my_obj = M2MInterfaceFactory::create_object("9");
        M2MObjectInstance* obj_instance = my_obj->create_object_instance();
        if(obj_instance) {
            obj_instance->set_operation(M2MBase::GET_PUT_ALLOWED);        
        }
        get_alloc_size();
        dyn_res = obj_instance->create_dynamic_resource(&my_update_params, M2MResourceInstance::INTEGER, false);                                                            
        dyn_res->set_value(77);
        dyn_res_2 = obj_instance->create_dynamic_resource(&my_update_params_2, M2MResourceInstance::INTEGER, false);        
        dyn_res_2->set_value(777);
        stat_res = obj_instance->create_static_resource(&my_update_params_3, M2MResourceInstance::STRING);
        output.printf("RESOURCE SIZE %d\n", get_alloc_size());                                                                                                      
        
}

#endif
