#define NUM_IOPING_RUNS 3
#define SLEEP_BETWEEN_RUNS 1 // in seconds
#define PING_PATH "/var/tmp/test.ping"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "random_bytes.h"

typedef struct {
	double diffsec;
	double diffnano;
} timediff_t;

static struct timespec tstart, tend;
static void* aligned_buffer;

timediff_t *do_ping() {
	clock_gettime(CLOCK_MONOTONIC, &tstart);
	int fd = open(PING_PATH, O_CREAT|O_WRONLY|O_DIRECT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if(fd == -1) {
		fprintf(stderr, "Unable to open test.ping file!\n");
		fprintf(stderr, "Errno: %d %s\n", errno, strerror(errno));
		exit(1);
	}

	ssize_t byteswritten = write(fd, aligned_buffer, ___rand_bin_len);
	if(byteswritten == -1) {
		unlink(PING_PATH);
		fprintf(stderr, "Unable to write bytes!\nerrno: %d %s\n", errno, strerror(errno));
		exit(1);
	}
	close(fd);
	clock_gettime(CLOCK_MONOTONIC, &tend);

	unlink(PING_PATH);
	//fprintf(stderr, "wrote: %ld\n", byteswritten);

	timediff_t *delta = (timediff_t*)malloc(sizeof(timediff_t));
	if(delta == NULL) {
		fprintf(stderr, "Failed to allocate memory!\n");
		exit(1);
	}

	delta->diffsec = tend.tv_sec - tstart.tv_sec;
	delta->diffnano = tend.tv_nsec - tstart.tv_nsec;

	return delta;
}

int main(int argc, char** argv) {
	if( posix_memalign(&aligned_buffer, ___rand_bin_len, ___rand_bin_len) != 0) {
		fprintf(stderr, "Unable to align buffer!\nerrno: %d %s\n", errno, strerror(errno));
		exit(1);
	}

	if( memcpy(aligned_buffer, ___rand_bin, ___rand_bin_len) == NULL) {
		fprintf(stderr, "Unable to memcpy to align buffer!\nerrno: %d %s\n", errno, strerror(errno));
		exit(1);
	}

	if(argc > 1) {
		if(strcmp(argv[1], "config") == 0) {
			printf("graph_title Naive IO latency\n"
					"graph_args --base 1000 -l 0\n"
					"graph_scale no\n"
					"graph_category disk\n"
					"graph_info Runs a naive IO latency test and shows the results in nanoseconds, source code available at https://github.com/vmp32k/naive-ioping\n"
					"graph_vlabel nanoseconds\n"
					"nano.label nanoseconds\n");
			exit(0);
		}
		return 0;
	}

	double total = 0;
	timediff_t* runs[NUM_IOPING_RUNS];

	for(int i = 0; i < NUM_IOPING_RUNS; ++i) {
		timediff_t *r = runs[i] = do_ping();
		if(r == NULL) {
			fprintf(stderr, "ioping returned NULL?!\n");
			exit(1);
		}

		total += (r->diffsec * 1e9) + r->diffnano;
		sleep(SLEEP_BETWEEN_RUNS);
	}

	total = total / NUM_IOPING_RUNS;	
	printf("nano.value %0.lf\n", total);

	for(int i = 0; i < NUM_IOPING_RUNS; ++i) {
		free(runs[i]);
	}
	free(aligned_buffer);

	return 0;
}