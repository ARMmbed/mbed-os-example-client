/*
 * Copyright (c) 2016 ARM Limited. All rights reserved.
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
 
#ifdef MBED_MEM_TRACING_ENABLED

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

//#include "mbed.h"
#include "mbed_mem_trace.h"
#include "cmsis_os.h"

#include "mbed-trace/mbed_trace.h"


struct heap_stats_s
{
    int allocated_cell_count;
    size_t allocated_bytes;
    size_t allocated_bytes_peak;
};

static struct heap_stats_s heap_stats;

// 
typedef struct {
    uint8_t op;
    void *res;
    void *caller;
    union {
        struct {
            size_t arg_size;
        } malloc_info;
        struct {
            void *arg_ptr;
            size_t arg_size;
        } realloc_info;
        struct {
            size_t arg_nmemb;
            size_t arg_size;
        } calloc_info;
        struct {
            void *arg_ptr;
        } free_info;
    } data;
} mem_trace_data_t;

static void recv_thread (void const *argument);
static void mem_trace_cb_sender(uint8_t op, void *res, void *caller, ...);
static void mem_trace_cb_receiver(const mem_trace_data_t *trace_data);

// XXX: a fixed size output pool
osPoolDef(mpool, 64, mem_trace_data_t);                    // Define memory pool
static osPoolId  mpool;
osMessageQDef(MsgBox, 64, &mem_trace_data_t);              // Define message queue
static osMessageQId  MsgBox;

static osThreadId tid_recv_thread;
static osThreadDef(recv_thread, osPriorityAboveNormal, /*1,*/ 2048);

void mem_stat_init()
{
#ifdef MBED_MEM_TRACING_ENABLED

    mpool = osPoolCreate(osPool(mpool));                 // create memory pool
    MsgBox = osMessageCreate(osMessageQ(MsgBox), NULL);  // create msg queue

    tid_recv_thread = osThreadCreate(osThread(recv_thread), NULL);
    assert(tid_recv_thread != NULL);

    // reset everything
    memset(&heap_stats, 0, sizeof(heap_stats));

    mbed_mem_trace_set_callback(mem_trace_cb_sender);
#endif
}

static void mem_trace_cb_sender(uint8_t op, void *res, void *caller, ...)
{
    va_list va;

    va_start(va, caller);

    mem_trace_data_t *trace_data = osPoolAlloc(mpool);

    if (trace_data == NULL) {
        // XXX: what to do, when the output does not keep up? Waiting is not a
        // option as that may break timing.
        return;
    }

    trace_data->op = op;
    trace_data->res = res;
    trace_data->caller = caller;

    switch (op) {
        case MBED_MEM_TRACE_MALLOC:
            trace_data->data.malloc_info.arg_size = va_arg(va, size_t);
            // m:<res>;<caller>;<size>
            // tr_debug("SEND #m:%p;%p-%u", res, caller, trace_data->data.malloc_info.arg_size);
            // heap_stats.allocated_cell_count++;
            break;

        case MBED_MEM_TRACE_REALLOC:
            trace_data->data.realloc_info.arg_ptr = va_arg(va, void *);
            trace_data->data.realloc_info.arg_size = va_arg(va, size_t);
            // r:<res>;<caller>-<ptr>;<size>
            // tr_debug("SEND #r:%p;%p-%p;%u", res, caller, trace_data->data.realloc_info.arg_ptr, trace_data->data.malloc_info.arg_size);
            break;

        case MBED_MEM_TRACE_CALLOC:
            trace_data->data.calloc_info.arg_nmemb = va_arg(va, size_t);
            trace_data->data.calloc_info.arg_size = va_arg(va, size_t);
            // c:<res>;<caller>-<ptr>;<zize>
            // tr_debug("SEND #c:%p;%p-%u;%u", res, caller, trace_data->data.calloc_info.arg_nmemb, trace_data->data.malloc_info.arg_size);
            break;

        case MBED_MEM_TRACE_FREE:
            trace_data->data.free_info.arg_ptr = va_arg(va, void *);
            // f:<res>;<caller>-<ptr>
            // tr_debug("SEND #f:%p;%p-%p", res, caller, trace_data->data.realloc_info.arg_ptr);
            break;

        default:
            // tr_error("?");
            assert(false);
            break;
    }

    va_end(va);

    osMessagePut(MsgBox, (uint32_t)trace_data, osWaitForever);
}

static void recv_thread(void const *argument) {

    mem_trace_data_t *trace_data;
    osEvent  evt;

    for (;;) {
      evt = osMessageGet(MsgBox, osWaitForever);  // wait for message
      if (evt.status == osEventMessage) {
          trace_data = evt.value.p;

          mem_trace_cb_receiver(trace_data);

          osPoolFree(mpool, trace_data); // free memory allocated for message
      }
    }
}

static void mem_trace_cb_receiver(const mem_trace_data_t *trace_data)
{
    // XXX: don't use the %p as in example, as it will put 0x in front of data in ARMCC target, only in GCC
    switch (trace_data->op) {
        case MBED_MEM_TRACE_MALLOC:
            // m:<res>;<caller>;<size>
            printf("#m:0x%x;0x%x-%u\n", trace_data->res, trace_data->caller, trace_data->data.malloc_info.arg_size);
            break;

        case MBED_MEM_TRACE_REALLOC:
            // r:<res>;<caller>-<ptr>;<size>
            printf("#r:0x%x;0x%x-0x%x;%u\n", trace_data->res, trace_data->caller, trace_data->data.realloc_info.arg_ptr, trace_data->data.realloc_info.arg_size);
            break;

        case MBED_MEM_TRACE_CALLOC:
            // c:<res>;<caller>-<nmemb>;<size>
            printf("#c:0x%x;0x%x-%u;%u\n", trace_data->res, trace_data->caller, trace_data->data.calloc_info.arg_nmemb, trace_data->data.calloc_info.arg_size);
            break;

        case MBED_MEM_TRACE_FREE:
            // f:<res>;<caller>-<ptr>
            printf("#f:0x%x;0x%x-0x%x\n", trace_data->res, trace_data->caller, trace_data->data.free_info.arg_ptr);
            break;

        default:
            printf("error\n");
    }
}
#endif // MBED_MEM_TRACING_ENABLED
