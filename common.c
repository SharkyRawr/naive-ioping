#include "common.h"

static struct timespec tstart, tend;

timediff_t *do_ping(const void *aligned_buffer, const char* pingPath) {
	
	int fd = open(pingPath, O_CREAT|O_WRONLY|O_DIRECT|O_SYNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if(fd == -1) {
		fprintf(stderr, "Unable to open file for writing: %s!\n", pingPath);
		fprintf(stderr, "Errno: %d %s\n", errno, strerror(errno));
		exit(1);
	}

	clock_gettime(CLOCK_MONOTONIC, &tstart);
	ssize_t byteswritten = write(fd, aligned_buffer, ___rand_bin_len);
	if(byteswritten == -1) {
		unlink(pingPath);
		fprintf(stderr, "Unable to write bytes!\nerrno: %d %s\n", errno, strerror(errno));
		exit(1);
	}
	clock_gettime(CLOCK_MONOTONIC, &tend);
	close(fd);

	unlink(pingPath);
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