#include "common.h"


void process_client_event(void *frontend, void *backend, char *message, size_t len)
{
    memset(message, 0, len);

    int rt = zmq_recv(frontend, message, len, ZMQ_DONTWAIT);

    std::cout << "client recv " << std::to_string(rt) << std::endl;

    if (rt != -1)
        zmq_send(backend, message, len, ZMQ_SNDMORE | ZMQ_DONTWAIT);
}


void process_worker_event(void *frontend, void *backend, char *message, size_t len)
{
    memset(message, 0, len);

    int rt = zmq_recv(backend, message, len, ZMQ_DONTWAIT);

    std::cout << "worker recv " << std::to_string(rt) << std::endl;

    if (rt != -1)
        zmq_send(frontend, message, len, ZMQ_SNDMORE | ZMQ_DONTWAIT);
}


int main()
{
    void *ctx = zmq_ctx_new();
    void *frontend = zmq_socket(ctx, ZMQ_ROUTER);
    void *backend = zmq_socket(ctx, ZMQ_DEALER);
    
    zmq_bind(frontend, frontend_addr.c_str());
    zmq_bind(backend, backend_addr.c_str());
    
    zmq::pollitem_t items[] = {
        { frontend, 0, ZMQ_POLLIN, 0 }, 
        { backend, 0, ZMQ_POLLIN, 0 }
    };

    char message[128];

    while (1) 
    {
        zmq_poll(&items[0], 2, -1);
        
        if (items[0].revents & ZMQ_POLLIN)
        {
            process_client_event(frontend, backend, &message[0], sizeof(message));
        }
        if (items[1].revents & ZMQ_POLLIN)
        {
            process_worker_event(frontend, backend, &message[0], sizeof(message));
        }
    }

    return 0;
}


