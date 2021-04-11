# Microtimer #

A library for scheduling events on a microcontroller.

Calls event callbacks from inside an idle function at specific times using a tick function called at a fixed interval as its timing source. Functions are provided to add and remove events.

## Functionality ##

`MICROTIMER_MAX_EVENTS` controls the amount of memory allocated for events. The value should specify the maximum number of events your program will ever be waiting on at any time.

`int microtimer_add_event(struct microtimer* const timer,
                          unsigned char(*callback)(void*),
                          void* const callback_arg, const unsigned long period,
                          const unsigned long int start_tick);`

Adds an event, will call `callback` with argument `callback_arg` inside the next `microtimer_idle`(below) call after `start_tick`. The the callback will then be called periodically with period `period` while it returns 0.

Returns 0 on success, -1 on failure.

`struct microtimer_event* microtimer_get_event(struct microtimer* const timer,
                                              unsigned char(*callback)(void*),
                                              void* const callback_arg,
                                              const unsigned long int period);`

Returns a pointer to an event structure which can then be used to remove the event. All the events can move around when one is triggered, added or removed so be sure that this does not happen in between your calls to `microtimer_get_event` and `microtimer_remove_event`.

`int microtimer_remove_event(struct microtimer* const timer,
                             struct microtimer_event* event);`

Removes an event from the queue using its pointer, use `microtimer_get_event` to retrieve an events pointer.

Returns 0 on success, -1 on failure.

`void microtimer_idle(struct microtimer* const timer, unsigned long int tick);`

Calls the callbacks when required.

`void microtimer_init(struct microtimer* const timer, unsigned long int tick);`

Initialises the `struct microtimer` for use with the above functions.

*Examples of all the above can be found in `microtimer_test.c`*
