#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "gpiolib.h"
#include <getopt.h>
#include <curl/curl.h>

/* own a2i */
int a2i(const char *s)
{
  int sign = 1;
  if (*s == '-')
  {
    sign = -1;
    s++;
  }
  int num = 0;
  while (*s)
  {
    num = ((*s) - '0') + num * 10;
    s++;
  }
  return num * sign;
}

int main(int argc, char *argv[])
{

  int opt;
  char* url;
  int state;

  CURL *curl;
  CURLcode res;

  while (1)
  {
    int option_index = 0;
    static struct option long_options[] =
    {
      {"url",    required_argument, NULL,  'u'},
      {"state",  required_argument, NULL,  's'},
      {NULL,     0,                 NULL,    0}
    };

    int c = getopt_long(argc, argv, "-:a:d", long_options, &option_index);
    if (c == -1)
      break;

    switch (c)
    {
    /* url="http://whatever" opt */
    case 'u':
      url = optarg;
      break;

    /* state="0|1" ... default open/default close */
    case 's':
      state = a2i(optarg);
      break;

    case '?':
      printf("Unknown option %c\n", optopt);
      break;

    case ':':
      printf("Missing option for %c\n", optopt);
      break;

    default:
      printf("--url=\"http://whatever.com\"  --state=\"0|1\"\n");
      return 1;
    }
  }

  /* init gpio */
  int gpio_pin = 26;

  gpio_export(gpio_pin);
  gpio_direction(gpio_pin, 0);

  int curr_state = gpio_read(gpio_pin);

  if (curr_state == -1)
  {
    return -1;
  }

  while (1)
  {

    if (curr_state != state)
    {
      printf(">> GPIO %d ON\n", curr_state);
      curl = curl_easy_init();
      if (curl)
      {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        /* we tell libcurl to follow redirection */
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK)
          fprintf(stderr, "curl_easy_perform() failed: %s\n",
                  curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(curl);
      }
    }

    /* sleep 10ms */
    usleep(10000);

    /* update current state */
    curr_state = gpio_read(gpio_pin);
  }

  return 0;
}
