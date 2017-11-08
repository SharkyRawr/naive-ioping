#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>

#include <sys/stat.h>
#include <fcntl.h>

#include "random_bytes.h"

static struct timespec tstart, tend;

int main(int argc, char** argv) {

	clock_gettime(CLOCK_MONOTONIC, &tstart);

	FILE* f = fopen("test.ping", "wb");
	if(f == NULL) {
		printf("Unable to open test.ping file!\n");
		return 1;
	}

	fwrite(___rand_bin, ___rand_bin_len, 1, f);
	fclose(f);
	clock_gettime(CLOCK_MONOTONIC, &tend);
	unlink("test.ping");

	double seconds = tend.tv_sec - tstart.tv_sec;
	double nanos = tend.tv_nsec - tstart.tv_nsec;
	printf("diff: %0.lf.%0.lf\n", seconds, nanos);

	return 0;
}