#include "common.h"

int main()
{
    void *ctx = zmq_ctx_new();
    void *responder = zmq_socket(ctx, ZMQ_REP);
    
    zmq_connect(responder, backend_addr.c_str());
    
    char message[16];
    
    while (1) 
    {
        memset(&message, 0, sizeof(message));

        zmq_recv(responder, &message, sizeof(message), ZMQ_DONTWAIT);
        
        std::cout << "received request " << std::string(message) << std::endl;
        
        sleep(1);
        
        zmq_send(responder, "world", 5, ZMQ_SNDMORE | ZMQ_DONTWAIT);
    }

    return 0;
}
