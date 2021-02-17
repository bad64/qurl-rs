#ifndef RSNET_H
#define RSNET_H
#include "includes.h"

// This file only really exists for FFI compliance, nothing really prevents me from merging it with includes.h

extern "C" Request ffi_request(char* method, char* hostent, char* content, char* agent);

#endif // RSNET_H
