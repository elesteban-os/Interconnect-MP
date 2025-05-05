#include "cache_class.cpp"
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

// Clase PE (Processing Element)
class PE {
public:
    /* atributos de la clase */
    size_t nextCacheLine = 0; 
    int id;
    Cache cache;

    PE(int id) : id(id) {} // constructor

    /* ------ métodos de la clase (envío de mensajes al MMU) ------ */

    /* método para escribir en la memoria principal datos desde la caché
     * parámetros: SRC ID del PE, dirección de memoria principal (ADDR), número de lineas de caché (NUM_CACHE_LINES),
     *             linea de caché inicial (START_CACHE_LINE), prioridad del mensaje (QoS)
     */  
    WRITE_MEM writeMem(uint8_t SRC, uint16_t ADDR, uint8_t NUM_CACHE_LINES, uint8_t START_CACHE_LINE, uint8_t QoS) {
        uint8_t data[NUM_CACHE_LINES * Cache::BLOCK_SIZE]; // buffer para guardar los datos leidos
        cache.readCacheLines(START_CACHE_LINE, data, NUM_CACHE_LINES); // leer de la caché
        // generar struct (mensaje) con datos leidos
        WRITE_MEM msg(NUM_CACHE_LINES, Cache::BLOCK_SIZE);
        msg.SRC = SRC; // id del PE que envía el mensaje
        msg.ADDR = ADDR; // dirección de memoria principal
        msg.NUM_CACHE_LINES = NUM_CACHE_LINES; // número de lineas de caché
        msg.START_CACHE_LINE = START_CACHE_LINE; // linea de caché inicial
        msg.QoS = QoS; // prioridad del mensaje
        msg.data.assign(data, data + NUM_CACHE_LINES * Cache::BLOCK_SIZE); // copiar datos leidos al mensaje
        
        return msg; // devolver el mensaje
    }

    /* metodo para solicitar la lectura de datos de memoria principal
     * parámetros: SRC ID del PE, dirección de memoria principal (ADDR), cantidad de bytes a leer de memoria principal (SIZE), 
     *             prioridad del mensaje (QoS)
     */
    READ_MEM readMem(uint8_t SRC, uint16_t ADDR, uint16_t SIZE, uint8_t QoS) {
        READ_MEM msg;
        msg.SRC = SRC; // id del PE que envía el mensaje
        msg.ADDR = ADDR; // dirección de memoria principal
        msg.SIZE = SIZE; // cantidad de bytes a leer de memoria principal
        msg.QoS = QoS; // prioridad del mensaje

        return msg; // devolver el mensaje
    }

    /* método para invalidar una línea de caché de los demás PEs
     * parámetros: índice del bloque a invalidar (blockIndex)
     */
    BROADCAST_INVALIDATE broadcastInvalidate(uint8_t SRC, uint8_t CACHE_LINE, uint8_t QoS) {
        BROADCAST_INVALIDATE msg;
        msg.SRC = id; // id del PE que envía el mensaje
        msg.CACHE_LINE = CACHE_LINE; // línea de caché a invalidar
        msg.QoS = QoS; // prioridad del mensaje

        return msg; // devolver el mensaje
    }

    /* método para responder a una invalidación de caché
     * parámetros: ID del PE que envía el mensaje (SRC), prioridad del mensaje (QoS), estado de la línea de caché (CACHE_LINE)
     */
    INV_ACK invAck(uint8_t SRC, uint8_t STATUS, uint8_t QoS) {
        INV_ACK msg;
        msg.SRC = SRC; // id del PE que envía el mensaje
        msg.STATUS = STATUS; // estado de la línea de caché (true = válida, false = inválida)
        msg.QoS = QoS; // prioridad del mensaje

        return msg; // devolver el mensaje
    }

    /* ------ métodos de la clase (recibimiento de mensajes del MMU) ------ */

    /* método para recibir un mensaje de invalidación de caché (BROADCAST_INVALIDATE)
     * parámetros: índice de la línea de cache a invalidar (CACHE_LINE)
     */
    void invalidateCacheLine(uint8_t CACHE_LINE) {
        cache.invalidate(CACHE_LINE); // invalidar la línea de caché

        // creo que aquí debería invocarse a invAck() y cache.isValid() para enviar un mensaje de respuesta al MMU automáticamente
    }

    /* método para recibir mensaje si todas las invalidaciones fueron exitosas (INV_COMPLETE)
     * parámetros: flag de éxito (STATUS) 0x1 = OK, 0x0 = NOT_OK
     */
    void invComplete(uint8_t STATUS) {
        // de momento se hace un print pero no se sabe si al final el PE tiene que hacer algo con la respuesta
        if (STATUS == 0x1) {
            std::cout << "Invalidación de caché exitosa.\n";
        } else {
            std::cout << "Error en la invalidación de caché.\n";
        }
    }

    /* metodo para recibir un mensaje de respuesta de un read (READ_RESP) 
     * parámetros: datos a guardar en la caché (data), tamaño de los datos (dataSize) sizeof(data)
     */
    void readResp(uint8_t* DATA, size_t DATA_SIZE) {
        size_t blockSize = Cache::BLOCK_SIZE; // Tamaño de una línea de caché
        size_t blocksNeeded = (DATA_SIZE + blockSize - 1) / blockSize; // Número de bloques necesarios
    
        for (size_t i = 0; i < blocksNeeded; ++i) {
            size_t offset = i * blockSize; // Desplazamiento en los datos
            size_t bytesToWrite = std::min(blockSize, DATA_SIZE - offset); // Número de bytes a escribir en este bloque
    
            // Escribir en la línea de caché correspondiente
            cache.write(nextCacheLine, DATA + offset, bytesToWrite);
    
            // Avanzar al siguiente índice en forma circular
            nextCacheLine = (nextCacheLine + 1) % Cache::BLOCKS;
        }
    }

    /* método para recibir un mensaje de respuesta de un write (WRITE_RESP)
     * parámetros: estado de la escritura a memoria principal (STATUS) 0x1 = OK, 0x0 = NOT_OK 
     */
    void writeResp(uint8_t STATUS) {
        // de momento se hace un print pero no se sabe si al final el PE tiene que hacer algo con la respuesta
        if (STATUS == 0x1) {
            std::cout << "Escritura en memoria principal exitosa.\n";
        } else {
            std::cout << "Error en la escritura en memoria principal.\n";
        }
    }

};