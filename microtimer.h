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

#ifndef MICROTIMER_H
#define MICROTIMER_H

#ifndef MICROTIMER_MAX_EVENTS
#define MICROTIMER_MAX_EVENTS (12)
#endif

struct microtimer_event {
  unsigned char (*callback)(void *);
  void *callback_arg;
  unsigned long int trigger;
  unsigned long int period;
};

struct microtimer {
  unsigned long int last;
  struct microtimer_event events[MICROTIMER_MAX_EVENTS];
  struct microtimer_event *head;
};

int microtimer_add_event(struct microtimer *const timer,
                         unsigned char (*callback)(void *),
                         void *const callback_arg, const unsigned long period,
                         const unsigned long int start_tick);
struct microtimer_event *microtimer_get_event(struct microtimer *const timer,
                                              unsigned char (*callback)(void *),
                                              void *const callback_arg,
                                              const unsigned long int period);
int microtimer_remove_event(struct microtimer *const timer,
                            struct microtimer_event *event);
void microtimer_idle(struct microtimer *const timer, unsigned long int tick);
void microtimer_init(struct microtimer *const timer, unsigned long int tick);

#endif
