#ifndef DATAMESSAGES_H
#define DATAMESSAGES_H

#include <cstdint>

// Enum for operation types
enum class operation_type { READ, WRITE };

// Union block
union block {
    uint8_t byte[4];
    uint32_t word;
};

// Struct for data response
struct data_resp {
    int pe_ID = -1;
    operation_type op_type;
    int QoS;
    block* data = nullptr;
    int blocks;
    uint8_t status = 0xFF;
};

// Struct for operation
struct operation {
    int pe_ID = -1;
    operation_type op_type;
    int address;
    int blocks;
    int QoS;
    
    int start_cycle;
    block* write_data = nullptr;
};

#endif // DATAMESSAGES_H