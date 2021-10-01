#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <zmq.hpp>
#include <iostream>
#include <unistd.h>
#include <cassert>

const std::string frontend_addr = "tcp://127.0.0.1:5559";
const std::string backend_addr = "tcp://127.0.0.1:5560";


#endif // COMMON_H
