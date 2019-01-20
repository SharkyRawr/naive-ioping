#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

#include "common.h"

static void* aligned_buffer;
uint32_t count = 4, interval = 1;
uint64_t pinglen = 1024*1024*1; // @todo customizable
char *pingPath = "/var/tmp/ping.tmp";

void printHelp() {
    printf("naive-ioping - v1.0 by Hendrik 'Sharky' Schumann - sharky@sharky.pw\n"
    "usage:\n"
    "  ioping [-c --count count (default: 4)] [-i --interval interval (default: 1)]\n"
    "         [-p --path path (default: /var/tmp/ping.tmp)]\n");
}

int main(int argc, char** argv) {
    pinglen = ___rand_bin_len; // @todo customizable

	if(argc > 1) {
		for(int i = 1; i < argc; i++) {
            if(strcmp(argv[i], "--help") == 0 ||
                strcmp(argv[i], "-h") == 0) {
                printHelp();
                return 0;
            }

            if(strcmp(argv[i], "--count") == 0 ||
                strcmp(argv[i], "-c") == 0) {
                    if(argc <= i+1) {
                        continue;
                    }
                    count = atoi(argv[i+1]);
                    if(count <= 0) count = 1;
                    ++i;
                    continue;
                }

            if(strcmp(argv[i], "--interval") == 0 ||
                strcmp(argv[i], "-i") == 0) {
                    if(argc <= i+1) {
                        continue;
                    }
                    interval = atoi(argv[i+1]);
                    if(interval < 0) interval = 0;
                    ++i;
                    continue;
                }

            if(strcmp(argv[i], "--path") == 0 ||
                strcmp(argv[i], "-p") == 0) {
                    if(argc <= i+1) {
                        continue;
                    }
                    pingPath = argv[i+1];
                    ++i;
                    continue;
                }
        }
	}

    if(count == 1) {
        printf("Give me a ping, Vasili. One ping only, please.\n");
    } else {
        printf("%u ioping(s)\n", count);
    }

    printf("Path to file used for ioping: %s\n", pingPath);

    // check if there already is a file at pingPath, we don't want to overwrite anything important
    int accessTest = access(pingPath, F_OK);
    if(accessTest == 0) {
        printf("The file seems to exist! Aborting ...\n");
        exit(1);
    }
	
	if( posix_memalign(&aligned_buffer, ___rand_bin_len, ___rand_bin_len) != 0) {
		fprintf(stderr, "Unable to align buffer!\nerrno: %d %s\n", errno, strerror(errno));
		exit(1);
	}

	if( memcpy(aligned_buffer, ___rand_bin, ___rand_bin_len) == NULL) {
		fprintf(stderr, "Unable to memcpy to align buffer!\nerrno: %d %s\n", errno, strerror(errno));
		exit(1);
	}

	double total = 0, maxTime = -1, minTime = (unsigned int)~0;
	timediff_t* runs[count];

	for(int i = 0; i < count; ++i) {
		timediff_t *r = runs[i] = do_ping((const void*)aligned_buffer, pingPath);
		if(r == NULL) {
			fprintf(stderr, "ioping returned NULL?!\n");
			exit(1);
		}
        
        double val = (r->diffsec * 1e9) + r->diffnano;
        if(val > maxTime) maxTime = val;
        if(val < minTime) minTime = val;

        printf("%u bytes from %s, seq=%d, time=%lf ms\n", ___rand_bin_len, pingPath, i + 1, ((r->diffsec * 1e9) + r->diffnano)*1e-9);

		total += (r->diffsec * 1e9) + r->diffnano;
        if(count > 1) sleep(interval);
	}

    printf("\n--- ioping statistics ---\n");

	double averageTime = (total / (double)count);
	printf("%d iopings written, time %lf ms\n", count, total * 1e-9);
    printf("min/avg/max %lf/%lf/%lf ms\n", minTime * 1e-9, averageTime * 1e-9, maxTime * 1e-9);

	for(int i = 0; i < count; ++i) {
		free(runs[i]);
	}
	free(aligned_buffer);

	return 0;
}
