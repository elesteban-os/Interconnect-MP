// Parte de los mensajes que vienen de la clase de los PE.

#include <cstdint>
#include <vector>

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
};