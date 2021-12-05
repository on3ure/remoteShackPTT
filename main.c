#include "httpd.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "gpiolib.h"

#define CHUNK_SIZE 1024 // read 1024 bytes at a time

int init_gpio()
{
  gpio_export(18);
  gpio_export(26);

  gpio_direction(18, 1);
  gpio_direction(26, 0);

  return 1;
}

int main(int c, char **v)
{
  init_gpio();

  char *port = c == 1 ? "8000" : v[1];
  serve_forever(port);
  return 0;
}

void route()
{
  ROUTE_START()

  GET("/")
  {
    HTTP_200;
    printf("RemoteShackPTT: ure@on3ure.be\n");
    printf("/in -> input state\n");
    printf("/out -> output state\n");
    printf("/out/on -> on state\n");
    printf("/out/off -> off state\n");
    printf("/out/toggle -> toggle output state\n");
    printf("/out/on/delayed?time -> toggle on delayed <time>\n");
    printf("/out/off/delayed?time -> toggle on delayed <time>\n");
  }

  GET("/in")
  {
    int state = gpio_read(26);

    HTTP_200;
    printf("{\"state\":%i}\n\n", state);
  }

  GET("/out")
  {
    int state = gpio_read(18);

    HTTP_200;
    printf("{\"state\":%i}\n\n", state);
  }

  GET("/out/toggle")
  {
    int toggle = 0;
    if (gpio_read(18) != 1)
    {
      gpio_write(18, 1);
      toggle = 1;
    }
    else
    {
      gpio_write(18, 0);
      toggle = 0;
    }

    HTTP_200;
    printf("{\"state\":%i}\n\n", toggle);
  }

  GET("/out/on/delayed")
  {
    HTTP_200;
    printf("{\"delay\":%s}\n", qs);
    printf("{\"state\":1}\n");
    gpio_write(18, 1);
    sleep(atoi(qs));
    gpio_write(18, 0);

    printf("{\"state\":0}\n\n");
  }

  GET("/out/off/delayed")
  {
    HTTP_200;
    printf("{\"delay\":%s}\n", qs);
    printf("{\"state\":0}\n");
    gpio_write(18, 0);
    sleep(atoi(qs));
    gpio_write(18, 1);

    printf("{\"state\":1}\n\n");
  }

  GET("/out/on")
  {
    gpio_write(18, 1);
    int state = gpio_read(18);

    HTTP_200;
    printf("{\"state\":%i}\n\n", state);
  }

  GET("/out/off")
  {
    gpio_write(18, 0);
    int state = gpio_read(18);

    HTTP_200;
    printf("{\"state\":%i}\n\n", state);
  }

  ROUTE_END()
}
