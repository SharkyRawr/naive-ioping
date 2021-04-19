[![ko-fi](https://www.ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/W7W52AOXC)

# naive-IOping #

This is a simple tool (and munin plugin) to measure HDD latency with high accuracy.

Features:

  - Direct I/O to bypass any caching (only on Linux (?))
  - clock_gettime(CLOCK_MONOTONIC, ...) for nano-second accuracy (depending on platform)
  - munin compatible output
  - no external dependencies

## How it works: ##

Pretty simple stuff:

- The tool sets up and aligns a hardcoded 1MB buffer
- measures the current timestamp as described by `clock_gettime(CLOCK_MONOTONIC, ...)`
  - creates & opens a file for direct I/O
  - writes the buffer & closes the file
- creates a second timestamp and displays the difference between the two (the elapsed time)

## Configuring: ##

`ioping.c` offers minimal configuration using the #define's at the beginning of the file.

```c
#define NUM_IOPING_RUNS 3 // number of times ioping writes to disk, results will be averaged
#define SLEEP_BETWEEN_RUNS 1 // in seconds
#define PING_PATH "/var/tmp/test.ping" // file path to write to
```

Using `/tmp` for `PING_PATH` is discouraged since some servers mount a ramdisk/tmpfs on that path.

## Compiling: ##

I personally use CMake but you are free to use your own build system or directly invoke gcc:

```bash
mkdir build
cmake ..
make
# run with:
./ioping
```

## Munin support: ##

Simply drop the binary in your plugins directory and restart munin-node:

```bash
cp -v ioping /etc/munin/plugins/
systemctl restart munin-node
```

#### Tips: ####

[![ko-fi](https://www.ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/W7W52AOXC)

