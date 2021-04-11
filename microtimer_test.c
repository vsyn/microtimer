/*
The MIT License

Copyright (c) 2014 Julian Ingram.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 */

#define MICROTIMER_MAX_EVENTS 3

#include "microtimer.h"

#if _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

#include <stdio.h>

static unsigned char done = 0;

static unsigned char callback_1(void *arg) {
  (void)arg;
  printf("1s cb\r\n");
  return 0;
}

static unsigned char callback_2(void *arg) {
  (void)arg;
  printf("    5s cb\r\n");
  return 0;
}

static unsigned char callback_3(void *arg) {
  struct microtimer *timer = (struct microtimer *)arg;
  static unsigned char state = 0;
  printf("        10s cb ");
  switch (state) {
  case 0: {
    struct microtimer_event *event =
        microtimer_get_event(timer, &callback_1, timer, 1000);
    if (event != 0) {
      printf("removing 1s cb\r\n");
      microtimer_remove_event(timer, event);
    }
    state = 1;
    break;
  }
  case 1:
    printf("adding 1s cb\r\n");
    microtimer_add_event(timer, &callback_1, timer, 1000, timer->last + 1000);
    state = 2;
    break;
  case 2:
    printf("done\r\n");
    // done = 1;
    state = 0;
    break;
  }
  return 0;
}

static unsigned long long int get_tick(void) {
  unsigned long long int tick;
#if _WIN32
  tick = GetTickCount();
#else
  struct timespec time;
  clock_gettime(CLOCK_REALTIME, &time);
  tick = (time.tv_sec * 1000) + ((time.tv_nsec + 500000) / 1000000);
#endif
  return tick;
}

int main() {
  struct microtimer timer;
  const unsigned long long int start = get_tick();

  microtimer_init(&timer, start);

  microtimer_add_event(&timer, &callback_1, &timer, 1000, start + 1000);
  microtimer_add_event(&timer, &callback_2, &timer, 5000, start + 5000);
  microtimer_add_event(&timer, &callback_3, &timer, 10000, start + 10000);

  while (done == 0) {
    microtimer_idle(&timer, get_tick());
  }
  return 0;
}
