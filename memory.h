#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <cstring>
#include <iostream>
#include "datamessages.h"

// Memory class
class Memory {
private:
    block memory[4096] = {};
    bool op_ready = true;
    operation* memory_operation = nullptr;
    data_resp* data_response = nullptr;
    int cycle = 0;

    int invalidWriteResponse();
    int responseCreator();
    int operationUpdate();

public:
    Memory();
    ~Memory();
    int update();
    int readData(uint32_t address, int peid_src, int blocks, int QoS);
    int writeData(uint32_t address, int peid_src, block* data, int blocks, int QoS);
    uint32_t getWord(uint32_t address);
    bool get_op_ready();
    data_resp* get_data_response();
};

#endif // MEMORY_H