#include "common.h"

static struct timespec tstart, tend;

timediff_t *do_ping(const void *aligned_buffer, const size_t len, const char* pingPath ) {
	
	int fd = open(pingPath, O_CREAT|O_WRONLY|O_DIRECT|O_SYNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if(fd == -1) {
		fprintf(stderr, "Unable to open file for writing: %s!\n", pingPath);
		fprintf(stderr, "Errno: %d %s\n", errno, strerror(errno));
		exit(1);
	}

	clock_gettime(CLOCK_MONOTONIC, &tstart);
	ssize_t byteswritten = write(fd, aligned_buffer, len);
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

size_t get_random_data(void* buf, size_t len) {
	FILE *f = fopen("/dev/urandom", "rb");
	if(!f) {
		fprintf(stderr, "Unable to open /dev/urandom as a random data source!\n");
		exit(1);
	}

	size_t numread = fread(buf, 1, len, f);
	if(numread != len) {
		fprintf(stderr, "Expected %d bytes read, got %d! (/dev/urandom)\n", len, numread);
		exit(1);
	}

	fclose(f);
	return numread;
}