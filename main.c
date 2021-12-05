#include "httpd.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "gpiolib.h"

#define CHUNK_SIZE 1024 // read 1024 bytes at a time

// Public directory settings
#define PUBLIC_DIR "./public"
#define NOT_FOUND_HTML "/404.html"

int init_gpio() {
  gpio_export(18);
  gpio_export(26);

  gpio_direction(18,1);
  gpio_direction(26,0);

  gpio_write(18,1);

  sleep(2);
  
  gpio_write(18,0);

  return 1;
}

int main(int c, char **v) {
  init_gpio();

  char *port = c == 1 ? "8000" : v[1];
  serve_forever(port);
  return 0;
}

int file_exists(const char *file_name) {
  struct stat buffer;
  int exists;

  exists = (stat(file_name, &buffer) == 0);

  return exists;
}

int read_file(const char *file_name) {
  char buf[CHUNK_SIZE];
  FILE *file;
  size_t nread;
  int err = 1;

  file = fopen(file_name, "r");

  if (file) {
    while ((nread = fread(buf, 1, sizeof buf, file)) > 0)
      fwrite(buf, 1, nread, stdout);

    err = ferror(file);
    fclose(file);
  }
  return err;
}

void route() {
  ROUTE_START()

  GET("/") {
    HTTP_200;
      printf("Hello! You are using %s\n\n", request_header("User-Agent"));
  }

  GET("/out") {
    HTTP_200;
    printf("List of request headers:\n\n");

    header_t *h = request_headers();

    while (h->name) {
      printf("%s: %s\n", h->name, h->value);
      h++;
    }
  }

  ROUTE_END()
}
