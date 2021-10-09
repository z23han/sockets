#ifndef EVENT_LOOP_HPP
#define EVENT_LOOP_HPP

#include "../common.h"

struct event_dispatcher;


struct event_loop
{
    int quit;
    const struct event_dispatcher *eventDispatcher;

    
};


struct event_dispatcher
{
    const char *name;

    void *(*init)(struct event_loop *eventLoop);

    int (*del)(struct event_loop *eventLoop, struct channel *channel);

    int (*update)(struct event_loop *eventLoop, struct channel *channel);

    int (*dispatch)(struct event_loop *eventLoop, struct timeval *);

    void (*clear)(struct event_loop *eventLoop);
};



#endif // EVENT_LOOP_HPP
