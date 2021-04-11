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

#include "microtimer.h"

#include <string.h>

static inline unsigned char
microtimer_triggers_lt(const unsigned long int tick,
                       const unsigned long int trig1,
                       const unsigned long int trig2) {
  return ((trig1 - tick) < (trig2 - tick)) ? 1 : 0;
}

static void microtimer_sort_head(struct microtimer *const timer) {
  struct microtimer_event *swap_events[2] = {timer->head - 2, timer->head - 1};

  while ((swap_events[1] > timer->events) &&
         (microtimer_triggers_lt(timer->last, swap_events[0]->trigger,
                                 swap_events[1]->trigger) != 0)) {
    /* swap events */
    const struct microtimer_event tmp_event = *swap_events[0];
    *swap_events[0] = *swap_events[1];
    *swap_events[1] = tmp_event;

    /* decrement the events */
    --swap_events[0];
    --swap_events[1];
  }
}

int microtimer_add_event(struct microtimer *const timer,
                         unsigned char (*callback)(void *),
                         void *const callback_arg,
                         const unsigned long int period,
                         const unsigned long int start_tick) {
  if (timer->head >= &timer->events[MICROTIMER_MAX_EVENTS]) {
    return -1;
  }

  /* push the event at position head */
  struct microtimer_event *const event = timer->head;
  event->callback = callback;
  event->callback_arg = callback_arg;
  event->period = period;
  event->trigger = start_tick;

  ++timer->head;

  microtimer_sort_head(timer);
  return 0;
}

struct microtimer_event *microtimer_get_event(struct microtimer *const timer,
                                              unsigned char (*callback)(void *),
                                              void *const callback_arg,
                                              const unsigned long int period) {
  struct microtimer_event *event = timer->events;
  while ((event < timer->head) &&
         ((event->callback != callback) ||
          (event->callback_arg != callback_arg) || (event->period != period))) {
    ++event;
  }
  return (event >= timer->head) ? 0 : event;
}

int microtimer_remove_event(struct microtimer *const timer,
                            struct microtimer_event *event) {
  if ((event < timer->events) ||
      (event >= &timer->events[MICROTIMER_MAX_EVENTS])) {
    return -1;
  }
  while (event != (timer->head - 1)) {
    struct microtimer_event *prev_event = event + 1;
    /* set the value of event to the prev event */
    *event = *prev_event;
    /* decrement the event */
    event = prev_event;
  }
  --timer->head;
  return 0;
}

void microtimer_idle(struct microtimer *const timer,
                     const unsigned long int tick) {
  struct microtimer_event *event = timer->head - 1;
  while ((timer->head != timer->events) &&
         (microtimer_triggers_lt(timer->last, tick, event->trigger) == 0)) {
    /* there is at least one active event and the event is triggered */
    timer->last = event->trigger;
    if (event->callback(event->callback_arg) == 0) /* call the callback */
    { /* update the trigger and re-sort the list */
      event = timer->head - 1;
      event->trigger += event->period;
      microtimer_sort_head(timer);
    } else {
      --timer->head;
    }
    event = timer->head - 1;
  }
}

void microtimer_init(struct microtimer *const timer,
                     const unsigned long int tick) {
  timer->last = tick;
  timer->head = timer->events;
}
