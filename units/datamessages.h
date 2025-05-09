#ifndef DATAMESSAGES_H
#define DATAMESSAGES_H

#include <cstdint>
#include <vector>
#include <condition_variable>
#include <mutex>

//#include "../PEs/PE_class.h" // For PE
//#include "messagemanagement.h" // For MessageManagementUnit

// Declaraciones adelantadas
class PE;
class MessageManagementUnit;
class MessageTimer;

// Enum for operation types
enum class operation_type { READ, WRITE, CACHE_INVALIDATE, CACHE_ACK, INVALIDATION_COMPLETE, NO_OP };

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

/* struct de WRITE_MEM */
struct WRITE_MEM {
    uint8_t SRC; //id del PE que envía el mensaje
    uint16_t ADDR; // dirección de memoria principal
    uint8_t NUM_CACHE_LINES; // número de lineas de caché
    uint8_t START_CACHE_LINE; // linea de caché inicial
    uint8_t QoS; // prioridad del mensaje
    std::vector<uint8_t> data; // datos a escribir

    // Constructor para inicializar el vector con el tamaño adecuado
    WRITE_MEM(uint8_t numCacheLines, uint8_t blockSize)
        : NUM_CACHE_LINES(numCacheLines), data(numCacheLines * blockSize) {}
};

/* struct de READ_MEM */
struct READ_MEM {
    uint8_t SRC; // id del PE que envía el mensaje
    uint16_t ADDR; // dirección de memoria principal
    uint16_t SIZE; // cantidad de bytes a leer de memoria principal
    uint8_t QoS; // prioridad del mensaje
};

/* struct de BROADCAST_INVALIDATE */
struct BROADCAST_INVALIDATE {
    uint8_t SRC; // id del PE que envía el mensaje
    uint8_t CACHE_LINE; // linea de caché a invalidar
    uint8_t QoS; // prioridad del mensaje
};

/* struct de INV_ACK */
struct INV_ACK {
    uint8_t SRC; // id del PE que envía el mensaje
    uint8_t STATUS; // estado de la línea de caché (true = válida, false = inválida)
    uint8_t QoS; // prioridad del mensaje
    uint8_t DEST; // id del PE que recibe el mensaje
};


// estructura para pasar datos a los hilos
struct PEThreadData {
    PE* pe;
    int id;
    MessageManagementUnit* mmu;
    MessageTimer* messageTimer;
    std::condition_variable cv; // Condición para esperar la respuesta
    std::mutex mtx;             // Mutex para sincronización
    bool responseReady = false; // Bandera para indicar si la respuesta está lista
    
    // Explicitly define copy assignment operator
    PEThreadData& operator=(const PEThreadData& other) {
        if (this != &other) {
            pe = other.pe;
            id = other.id;
            mmu = other.mmu;
            messageTimer = other.messageTimer;
        }
        return *this;
    }
};

#endif // DATAMESSAGES_H