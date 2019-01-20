#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "random_bytes.h"

#define PING_PATH "/var/tmp/test.ping"

typedef struct {
	double diffsec;
	double diffnano;
} timediff_t;

timediff_t *do_ping();