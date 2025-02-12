#include "module.h"
#include "util.h"
#include <param/param.h>
#include <param/param_client.h>
#include <param/param_string.h>
#include <csp/csp.h>

#include <csp/interfaces/csp_if_zmqhub.h>
#include <time.h>

#include <csp/csp_debug.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <pthread.h>


static int csp_pthread_create(void * (*routine)(void *)) {

	pthread_attr_t attributes;
	pthread_t handle;
	int ret;

	if (pthread_attr_init(&attributes) != 0) {
		return CSP_ERR_NOMEM;
	}
	/* no need to join with thread to free its resources */
	pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED);

	ret = pthread_create(&handle, &attributes, routine, NULL);
	pthread_attr_destroy(&attributes);

	if (ret != 0) {
		return ret;
	}

	return CSP_ERR_NONE;
}

static void * task_router(void * param) {

	/* Here there be routing */
	while (1) {
		csp_route_work();
	}

	return NULL;
}

int router_start(void) {
	return csp_pthread_create(task_router);
}

/* Define custom error codes */
enum ERROR_CODE {
    MALLOC_ERR = 1,
    PLACEHOLDER = 2,
};

static uint8_t server_address = 2;
static uint8_t client_address = 5;

csp_iface_t * add_interface(const char * device_name)
{
    csp_iface_t * default_iface = NULL;

	if (CSP_HAVE_LIBZMQ) {
        int error = csp_zmqhub_init(client_address, device_name, 0, &default_iface);
        if (error != CSP_ERR_NONE) {
            csp_print("failed to add ZMQ interface [%s], error: %d\n", device_name, error);
            exit(1);
        }
        default_iface->is_default = 1;
    }

	return default_iface;
}

/* Libparam */
int INDEX_ALL = -1; /* Pull/push all indices */
int VERBOSE = 2;
int TIMEOUT = 1000; /* Timeout for remote access [ms] */
int VERSION = 2; /* Current param interface version */

/*
    PARAMID_STATE:
        Needs to be mapped to existing parameter id
        Only maps to ONE parameter
    
    _state:
        Same datatype as the parameter
*/
uint8_t _state;
const uint16_t NODE = 3;
const uint16_t PARAMID_STATE = 51;
PARAM_DEFINE_REMOTE(state, NODE, PARAMID_STATE, PARAM_TYPE_UINT8, \
sizeof(_state), sizeof(_state), PM_TELEM, &_state, NULL);


/* START MODULE IMPLEMENTATION */
void module()
{
    /* Parameters for establishing connection to csp network*/
    
    
    const char * device_name = "localhost";
	csp_iface_t * default_iface;
	struct timespec start_time;

    /* Init CSP */
    csp_print("Initialising CSP\n");
    csp_init();

    /* Start router */
    router_start();

    /* Add interface */
    default_iface = add_interface(device_name);

    /* Network parameters */
    csp_print("Connection table\r\n");
    csp_conn_print_table();

    csp_print("Interfaces\r\n");
    csp_iflist_print();

    /* Start client work */
	csp_print("Client started\n");
	
    /* Ping server */
    int result = csp_ping(server_address, 1000, 100, CSP_O_NONE);
    csp_print("Ping address: %u, result %d [mS]\n", NODE, result);

    if (param_pull_single(&state, -1, CSP_PRIO_HIGH, VERBOSE, state.node, TIMEOUT, 2) < 0)
    {
        printf("Retrieving parameter value failed\n");
    }
    param_get_uint8(&state);
    printf("%u\n", _state);
    printf("%u\n", param_get_uint8(&state));

    /* Get number of images in input batch */
    int num_images = get_input_num_images();

    /* Retrieve module parameters by name if any (defined in config.yaml) */
    // int param_1 = get_param_bool("param_name_1");
    // int param_2 = get_param_int("param_name_2");
    // float param_3 = get_param_float("param_name_3");
    // char *param_4 = get_param_string("param_name_4");

    /* Example code for iterating a pixel value at a time */
    for (int i = 0; i < num_images; ++i)
    {
        Metadata *input_meta = get_metadata(i);

        /* Get custom metadata values */
        // int example_bool = get_custom_metadata_bool(input_meta, "example_bool");
        // int int_example = get_custom_metadata_int(input_meta, "example_int");
        // float example_float = get_custom_metadata_float(input_meta, "example_float");
        // char *example_string = get_custom_metadata_string(input_meta, "example_string");
        
        unsigned char *image_data;
        size_t size = get_image_data(i, &image_data);

        Metadata new_meta = METADATA__INIT;
        new_meta.size = input_meta->size;
        new_meta.width = input_meta->width;
        new_meta.height = input_meta->height;
        new_meta.channels = input_meta->channels;
        new_meta.timestamp = input_meta->timestamp;
        new_meta.bits_pixel = input_meta->bits_pixel;
        new_meta.camera = input_meta->camera;

        /* Add custom metadata key-value, if any new meta data is to be added */
        // add_custom_metadata_bool(&new_meta, "example_bool", 1);
        // add_custom_metadata_int(&new_meta, "example_int", 20);
        // add_custom_metadata_float(&new_meta, "example_float", 20.5);
        // add_custom_metadata_string(&new_meta, "example_string", "TEST");

        /* Append the image to the result batch */
        append_result_image(image_data, size, &new_meta);

        /* Remember to free any allocated memory */
        free(image_data);
    }
}
/* END MODULE IMPLEMENTATION */

/* Main function of module (NO NEED TO MODIFY) */
ImageBatch run(ImageBatch *input_batch, ModuleParameterList *module_parameter_list, int *ipc_error_pipe)
{
    ImageBatch result_batch;
    result = &result_batch;
    input = input_batch;
    config = module_parameter_list;
    error_pipe = ipc_error_pipe;
    initialize();

    module();

    finalize();

    return result_batch;
}
