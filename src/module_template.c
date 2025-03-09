#include "module.h"
#include "util.h"
#include <param/param.h>
#include <param/param_client.h>
#include <param/param_string.h>
#include <csp/csp.h>

#include <csp/interfaces/csp_if_zmqhub.h>
#include "param_config.h"

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

csp_iface_t * add_interface(const char * device_name, uint16_t client_address)
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

/* START MODULE IMPLEMENTATION */
void module()
{
    /* Libparam */
    int VERBOSE = 2;
    int TIMEOUT = 1000; /* Timeout for remote access [ms] */
    int VERSION = 2; /* Current param interface version */

    /* Parameters for establishing connection to csp network*/
    const char * device_name = "0";
	csp_iface_t * default_iface;
    uint16_t client_address = 2; // Parameter through ippc?

    /* Init CSP */
    csp_init();

    /* Start router */
    router_start();

    /* Add interface */
    default_iface = add_interface(device_name, client_address);

    /* Ping server */
    csp_ping(NODE_GNSS, 1000, 100, CSP_O_NONE); // Why is ping needed? Maybe adds delay?

    /* Create queue */
    param_queue_t queue;
    uint8_t queue_buf[80];
    param_queue_init(&queue, queue_buf, sizeof(queue_buf), 0, PARAM_QUEUE_TYPE_GET, VERSION);
    param_queue_add(&queue, &gnss_lat, -1, NULL);
    param_queue_add(&queue, &gnss_lon, -1, NULL);
    param_queue_add(&queue, &gnss_date, -1, NULL);
    param_queue_add(&queue, &gnss_time, -1, NULL);
    param_queue_add(&queue, &gnss_speed, -1, NULL);
    param_queue_add(&queue, &gnss_alt, -1, NULL);
    param_queue_add(&queue, &gnss_course, -1, NULL);

    if (param_pull_queue(&queue, CSP_PRIO_HIGH, VERBOSE, NODE_GNSS, TIMEOUT) < 0)
    {
        printf("Retrieving multiple parameter values failed\n");
        // Error handling? 
    }

    /* Get number of images in input batch */
    int num_images = get_input_num_images();


    /* Add metadata */
    for (int i = 0; i < num_images; ++i)
    {
        Metadata *input_meta = get_metadata(i);

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

        // New metadata
        add_custom_metadata_int(&new_meta, gnss_lat.name, _gnss_lat);
        add_custom_metadata_int(&new_meta, gnss_date.name, _gnss_date);
        add_custom_metadata_int(&new_meta, gnss_time.name, _gnss_time);
        add_custom_metadata_float(&new_meta, gnss_lon.name, _gnss_lon);
        add_custom_metadata_float(&new_meta, gnss_speed.name, _gnss_speed);
        add_custom_metadata_float(&new_meta, gnss_alt.name, _gnss_alt);
        add_custom_metadata_float(&new_meta, gnss_course.name, _gnss_course);

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
