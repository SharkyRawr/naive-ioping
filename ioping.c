#define NUM_IOPING_RUNS 3
#define SLEEP_BETWEEN_RUNS 1 // in seconds

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

static struct timespec tstart, tend;

typedef struct {
	double diffsec;
	double diffnano;
} timediff_t;

timediff_t *do_ping() {
	clock_gettime(CLOCK_MONOTONIC, &tstart);
	int fd = open("test.ping", O_CREAT|O_WRONLY|O_DIRECT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if(fd == -1) {
		fprintf(stderr, "Unable to open test.ping file!\n");
		fprintf(stderr, "Errno: %d %s\n", errno, strerror(errno));
		return NULL;
	}

	ssize_t byteswritten = write(fd, ___rand_bin, ___rand_bin_len);
	if(byteswritten == -1) {
		unlink("test.ping");
		fprintf(stderr, "Unable to write bytes!\nerrno: %d %s\n", errno, strerror(errno));
		return NULL;
	}
	close(fd);
	clock_gettime(CLOCK_MONOTONIC, &tend);

	unlink("test.ping");
	fprintf(stderr, "wrote: %ld\n", byteswritten);

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

	double total;
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
	printf("nanoseconds: %0.lf\n", total);

	for(int i = 0; i < NUM_IOPING_RUNS; ++i) {
		free(runs[i]);
	}

	return 0;
}