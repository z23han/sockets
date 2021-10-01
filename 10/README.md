request-reply model in zmq 

the broker contains 2 parts. One is called router, the other is called dealer. 
the model is constructed in a way, such that clients (requests) talk to a router, while servers (replies) talk to the dealer. 

We call router side as front-end, since it's talking to the clients 
We call dealer side as back-end, since it's talking to the servers 

We use ZeroMQ to wrap this model, since ZeroMQ has a nice and simple feature to handle socket programming. 
