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

int main(int argc, char** argv) {

	clock_gettime(CLOCK_MONOTONIC, &tstart);

	int fd = open("test.ping", O_CREAT|O_WRONLY|O_DIRECT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if(fd == -1) {
		printf("Unable to open test.ping file!\n");
		printf("Errno: %d %s\n", errno, strerror(errno));
		return 1;
	}

	ssize_t byteswritten = write(fd, ___rand_bin, ___rand_bin_len);
	if(byteswritten == -1) {
		unlink("test.ping");
		printf("Unable to write bytes!\nerrno: %d %s\n", errno, strerror(errno));
		return 1;
	}
	close(fd);
	clock_gettime(CLOCK_MONOTONIC, &tend);
	unlink("test.ping");
	printf("wrote: %ld\n", byteswritten);

	double seconds = tend.tv_sec - tstart.tv_sec;
	double nanos = tend.tv_nsec - tstart.tv_nsec;
	printf("diff: %0.lf.%0.lf\n", seconds, nanos);

	return 0;
}