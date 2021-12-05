#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "gpiolib.h"
#include <getopt.h>
#include <curl/curl.h>

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
      {"url",     required_argument, NULL,  'u'},
      {"state",  required_argument, NULL,  's'},
      {NULL,      0,                 NULL,    0}
    };

    int c = getopt_long(argc, argv, "-:a:d", long_options, &option_index);
    if (c == -1)
      break;

    switch (c)
    {
    case 'u':
      url = optarg;
      //printf("option a with value '%s'\n", optarg);
      break;

    case 's':
      state = a2i(optarg);
      //printf("option s with value '%i'\n", a2i(optarg));
      break;

    case '?':
      printf("Unknown option %c\n", optopt);
      break;

    case ':':
      printf("Missing option for %c\n", optopt);
      break;

    default:
      printf("?? getopt returned character code 0%o ??\n", c);
      return 1;
    }
  }

  int gpio_pin = 26;

  gpio_export(gpio_pin);
  gpio_direction(gpio_pin, 0);

  int curr_state = gpio_read(26);

  if (curr_state == -1)
  {
    return -1;
  }

  while (1)
  {

    printf(">> GPIO %d ON\n", curr_state);
    if (curr_state != state)
    {
      curl = curl_easy_init();
      if (curl)
      {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        /* example.com is redirected, so we tell libcurl to follow redirection */
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

    usleep(1000);
    curr_state = gpio_read(26);
  }

  return 0;
}
