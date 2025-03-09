#ifndef PARAM_CONFIG_H
#define PARAM_CONFIG_H

#include "param/param.h"

/* GNSS parameters */
#define PARAMID_GNSS_LAT    24
#define PARAMID_GNSS_LON    25
#define PARAMID_GNSS_DATE   27
#define PARAMID_GNSS_TIME   28
#define PARAMID_GNSS_SPEED  29
#define PARAMID_GNSS_ALT    30
#define PARAMID_GNSS_COURSE 32

static uint8_t _gnss_lat;
static float _gnss_lon;
static uint32_t _gnss_date;
static uint32_t _gnss_time;
static float _gnss_speed;
static float _gnss_alt;
static float _gnss_course;

static const uint16_t NODE_GNSS = 4; // https://github.com/discosat/disco-ii-cortex-m7-scheduler/blob/master/src/can_iface.c#L202 line 43

extern param_t gnss_lat;
extern param_t gnss_lon;
extern param_t gnss_date;
extern param_t gnss_time;
extern param_t gnss_speed;
extern param_t gnss_alt;
extern param_t gnss_course;

#endif