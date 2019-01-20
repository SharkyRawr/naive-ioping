#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "common.h"
#include "random_bytes.h"

static void* aligned_buffer;

#define NUM_IOPING_RUNS 3
#define SLEEP_BETWEEN_RUNS 1 // in seconds

int main(int argc, char** argv) {
	if(argc > 1) {
		if(strcmp(argv[1], "config") == 0) {
			printf("graph_title Naive IO latency\n"
					"graph_args --base 1000 -l 0\n"
					"graph_scale yes\n"
					"graph_category disk\n"
					"graph_info Runs a naive IO latency test with nanosecond accuracy (usually only as accurate as the platform allows), source code available at https://gitlab.com/Sharky/naive-ioping\n"
					"graph_vlabel latency in seconds\n"
					"sec.label seconds\n");
			exit(0);
		}
		return 0;
	}
	
	if( posix_memalign(&aligned_buffer, ___rand_bin_len, ___rand_bin_len) != 0) {
		fprintf(stderr, "Unable to align buffer!\nerrno: %d %s\n", errno, strerror(errno));
		exit(1);
	}

	if( memcpy(aligned_buffer, ___rand_bin, ___rand_bin_len) == NULL) {
		fprintf(stderr, "Unable to memcpy to align buffer!\nerrno: %d %s\n", errno, strerror(errno));
		exit(1);
	}

	double total = 0;
	timediff_t* runs[NUM_IOPING_RUNS];

	for(int i = 0; i < NUM_IOPING_RUNS; ++i) {
		timediff_t *r = runs[i] = do_ping((const void*)aligned_buffer, ___rand_bin_len, PING_PATH);
		if(r == NULL) {
			fprintf(stderr, "ioping returned NULL?!\n");
			exit(1);
		}

		total += (r->diffsec * 1e9) + r->diffnano;
		sleep(SLEEP_BETWEEN_RUNS);
	}

	total = (total / (double)NUM_IOPING_RUNS) * 1e-9;
	printf("sec.value %lf\n", total);

	for(int i = 0; i < NUM_IOPING_RUNS; ++i) {
		free(runs[i]);
	}
	free(aligned_buffer);

	return 0;
}
