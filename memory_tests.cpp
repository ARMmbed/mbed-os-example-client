//----------------------------------------------------------------------------
// The confidential and proprietary information contained in this file may
// only be used by a person authorised under and to the extent permitted
// by a subsisting licensing agreement from ARM Limited or its affiliates.
//
// (C) COPYRIGHT 2016-2017 ARM Limited or its affiliates.
// ALL RIGHTS RESERVED
//
// This entire notice must be reproduced on all copies of this file
// and copies of this file may only be made by a person if such person is
// permitted to do so under the terms of a subsisting license agreement
// from ARM Limited or its affiliates.
//----------------------------------------------------------------------------

#ifndef  __linux__
#ifdef MBED_HEAP_STATS_ENABLED
// used by print_heap_stats only
#include "mbed_stats.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include "mbed-client/m2mbase.h"
#endif

// fixup the compilation on AMRCC for PRIu32
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "memory_tests.h"

#include "mbed-client/m2mblockmessage.h"
#include "mbed-client/m2mdevice.h"
#include "mbed-client/m2mfirmware.h"
#include "mbed-client/m2minterfacefactory.h"
#include "mbed-client/m2mobject.h"
#include "mbed-client/m2mserver.h"
#include "mbed-client/m2msecurity.h"
#include "source/include/m2mreporthandler.h"
#include "source/include/m2minterfaceimpl.h"
#include "source/include/m2mnsdlinterface.h"
#include "mbed-client-classic/m2mconnectionhandlerpimpl.h"
#include "mbed-client-mbedtls/m2mconnectionsecuritypimpl.h"
#include "mbed-client-c/nsdl-c/sn_nsdl_lib.h"

#include "mbed.h"
#include "mbed_stats.h"

#include <assert.h>

void heap_stats()
{
#ifdef MBED_HEAP_STATS_ENABLED
    mbed_stats_heap_t stats;
    mbed_stats_heap_get(&stats);
    printf("**** current_size: %" PRIu32 "\n", stats.current_size);
    printf("**** max_size    : %" PRIu32 "\n", stats.max_size);
#endif // MBED_HEAP_STATS_ENABLED
}

void m2mobject_test_set(M2MObjectList& object_list)
{
#ifdef MBED_HEAP_STATS_ENABLED
    printf("*************************************\n");

    mbed_stats_heap_t stats;
    mbed_stats_heap_get(&stats);

    uint32_t initial = stats.current_size;

    const int object_count = 1;
    const int object_id_range_start = 90;
    const int object_id_range_end = object_id_range_start + object_count;

    const int object_instance_count = 5;
    const int resource_count = 5;

    int total_object_count = 0;
    int total_object_instance_count = 0;
    int total_resource_count = 0;

    for (int object_id = object_id_range_start; object_id < object_id_range_end; object_id++) {

        M2MObject *obj = M2MInterfaceFactory::create_object(object_id);

        for (int object_instance_id = 0; object_instance_id < object_instance_count; object_instance_id++) {

            M2MObjectInstance* obj_inst = obj->create_object_instance(object_instance_id);

            assert(obj_inst != NULL);
            total_object_instance_count++;
            for (int resource_id = 0; resource_id < resource_count; resource_id++) {
                String res_type;

                res_type.append_int(resource_id);

                M2MResource* resource = obj_inst->create_dynamic_resource(resource_id, res_type.c_str(),
                                                                        M2MResourceInstance::INTEGER, true);

                assert(resource != NULL);

                resource->set_operation(M2MBase::GET_ALLOWED);
                resource->set_value(7);

                total_resource_count++;
            }
        }

        object_list.push_back(obj);
        total_object_count++;
    }

    printf("objects       : %d\n", total_object_count);
    printf("obj instances : %d\n", total_object_instance_count);
    printf("resources     : %d\n", total_resource_count);

    mbed_stats_heap_get(&stats);
    printf("heap used     : %" PRIu32 "\n", stats.current_size - initial);

    printf("*************************************\n");
#endif // MBED_HEAP_STATS_ENABLED
}

// Note: the mbed-os needs to be compiled with MBED_HEAP_STATS_ENABLED to get
// functional heap stats, or the mbed_stats_heap_get() will return just zeroes.
void m2mobject_stats()
{
#ifdef MBED_HEAP_STATS_ENABLED
    printf("\n*** M2M object sizes in bytes ***\n");
    printf("M2MBase: %" PRIu32 "\n", sizeof(M2MBase));
    printf("M2MObject: %" PRIu32 "\n", sizeof(M2MObject));
    printf("M2MObjectInstance: %" PRIu32 "\n", sizeof(M2MObjectInstance));
    printf("M2MResource: %" PRIu32 "\n", sizeof(M2MResource));
    printf("M2MResourceInstance: %" PRIu32 "\n", sizeof(M2MResourceInstance));
    printf("M2MDevice: %" PRIu32 "\n", sizeof(M2MDevice));
    printf("M2MFirmware: %" PRIu32 "\n", sizeof(M2MFirmware));
    printf("M2MServer: %" PRIu32 "\n", sizeof(M2MServer));
    printf("M2MSecurity: %" PRIu32 "\n", sizeof(M2MSecurity));
    printf("M2MBlockMessage: %" PRIu32 "\n", sizeof(M2MBlockMessage));
    printf("M2MInterfaceImpl: %" PRIu32 "\n", sizeof(M2MInterfaceImpl));
    printf("M2MNsdlInterface: %" PRIu32 "\n", sizeof(M2MNsdlInterface));
    printf("M2MConnectionSecurityPimpl: %" PRIu32 "\n", sizeof(M2MConnectionSecurityPimpl));
    printf("M2MConnectionHandlerPimpl: %" PRIu32 "\n", sizeof(M2MConnectionHandlerPimpl));
    printf("M2MTimer: %" PRIu32 "\n", sizeof(M2MTimer));
    printf("sn_nsdl_dynamic_resource_parameters_s: %" PRIu32 "\n", sizeof(sn_nsdl_dynamic_resource_parameters_s));
    printf("M2MReportHandler: %" PRIu32 "\n", sizeof(M2MReportHandler));

    printf("*************************************\n\n");

    mbed_stats_heap_t stats;
    mbed_stats_heap_get(&stats);

    printf("*** M2M heap stats in bytes***\n");
    uint32_t initial = stats.current_size;

    // M2MDevice
    M2MDevice *device_object = M2MInterfaceFactory::create_device();
    assert(device_object);
    mbed_stats_heap_get(&stats);
    printf("M2MDevice heap size: %" PRIu32 "\n", stats.current_size - initial);
    M2MDevice::delete_instance();
    mbed_stats_heap_get(&stats);
    if (initial != stats.current_size) {
        printf("M2MDevice leaked: %" PRIu32 "bytes\n", stats.current_size - initial);
    }

    // M2MServer
    initial = stats.current_size;
    M2MServer *server = M2MInterfaceFactory::create_server();
    mbed_stats_heap_get(&stats);
    printf("M2MServer heap size: %" PRIu32 "\n", stats.current_size - initial);
    delete server;
    mbed_stats_heap_get(&stats);
    if (initial != stats.current_size) {
        printf("M2MServer leaked: %" PRIu32 "bytes\n", stats.current_size - initial);
    }

    // M2MSecurity
    initial = stats.current_size;
    M2MSecurity *security = M2MInterfaceFactory::create_security(M2MSecurity::M2MServer);
    mbed_stats_heap_get(&stats);
    printf("M2MSecurity heap size: %" PRIu32 "\n", stats.current_size - initial);
    delete security;
    mbed_stats_heap_get(&stats);
    if (initial != stats.current_size) {
        printf("M2MSecurity leaked: %" PRIu32 "bytes\n", stats.current_size - initial);
    }

    // M2MFirmware
    initial = stats.current_size;
    M2MFirmware *firmware = M2MInterfaceFactory::create_firmware();
    assert(firmware);
    mbed_stats_heap_get(&stats);
    printf("M2MFirmware heap size: %" PRIu32 "\n", stats.current_size - initial);
    M2MFirmware::delete_instance();
    mbed_stats_heap_get(&stats);
    if (initial != stats.current_size) {
        printf("M2MFirmware leaked: %" PRIu32 "bytes\n", stats.current_size - initial);
    }

    // Basic object creation
    initial = stats.current_size;
    uint32_t before_object = initial;
    M2MObject *obj = M2MInterfaceFactory::create_object(1);
    mbed_stats_heap_get(&stats);
    printf("M2MObject heap size: %" PRIu32 "\n", stats.current_size - initial);
    initial = stats.current_size;

    M2MObjectInstance* obj_inst = obj->create_object_instance();
    mbed_stats_heap_get(&stats);
    printf("M2MObjectInstance heap size: %" PRIu32 "\n", stats.current_size - initial);

    initial = stats.current_size;
    M2MResource* res = obj_inst->create_dynamic_resource(1, "1", M2MResourceInstance::STRING, false);
    assert(res);
    mbed_stats_heap_get(&stats);
    printf("M2MResource heap size: %" PRIu32 "\n", stats.current_size - initial);

    delete obj;
    mbed_stats_heap_get(&stats);
    if (before_object != stats.current_size) {
        printf("Resource leaked: %" PRIu32 "bytes\n", stats.current_size - before_object);
    }
    printf("*************************************\n\n");
#endif // MBED_HEAP_STATS_ENABLED
}
#endif
