#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void load_module(char *buf, int size) {
  // insmod buster.ko with buf as parameter

  int status;
  char cmd[256];
  sprintf(cmd, "insmod buster.ko buf=%lu size=%lu", (unsigned long)buf,
          (4096 * (unsigned long)size));
  printf("cmd: %s\n", cmd);
  status = system(cmd);
  if (status < 0) {
    printf("system() failed\n");
  }
  return;
}

// filnames: dump-{timestamp}.bin
char *get_filename() {
  char *filename = malloc(256);
  char *dir = "/home/abdullah/iommu/dump/";
  time_t t = time(NULL);
  sprintf(filename, "%sdump-%ld.bin", dir, t);
  return filename;
}

int main(int argc, char *argv[]) {
  int fd;
  char *buf;
  int i;
  int ret;

  int size = atoi(argv[1]);

  if (size < 1) {
    printf("size must be > 0\n");
    return 1;
  }

  buf = malloc(4096 * size);
  if (!buf) {
    printf("malloc failed\n");
    goto err_buf;
  }

  load_module(buf, size);

  char *filename = get_filename();

  printf("filename: %s\n", filename);

  fd = open(filename, O_WRONLY | O_CREAT, 0644);
  if (fd < 0) {
    printf("open failed\n");
    goto err;
  }

  ret = write(fd, buf, 4096 * size);
  if (ret != 4096 * size) {
    printf("write failed\n");
    goto err;
  }

err:
  close(fd);
  free(filename);
err_buf:
  free(buf);

  return 0;
}
