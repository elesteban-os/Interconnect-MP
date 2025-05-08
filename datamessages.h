#ifndef DATAMESSAGES_H
#define DATAMESSAGES_H

#include <cstdint>

// Enum for operation types
enum class operation_type { READ, WRITE, CACHE_INVALIDATE, CACHE_ACK, INVALIDATION_COMPLETE };

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
    block* data = nullptr;  // Aqui tambien es donde se puede guardar la direccion de cache a invalidar
    int blocks;
    uint8_t status = 0xFF;
};

enum class OPERATION_TYPE_PE { RESP_READ, RESP_WRITE, CACHE_INVALIDATE, INV_COMPLETE };

// Struct for data response
struct DATA_RESP_PE {
    uint8_t SRC;
    uint8_t CACHE_LINE;
    uint8_t DEST;
    uint8_t STATUS; // Para WRITE
    uint8_t* DATA;  // Para READ
    size_t DATA_SIZE;
    OPERATION_TYPE_PE OPERATION_TYPE; 
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