#include "PE_class.h"
//#include "../units/messagemanagement.h"




// constructor de la clase PE
PE::PE(int id, MessageTimer* messageTime) : id(id), messageTimer(messageTime) {}


/* método para leer instrucciones desde un archivo
    * parámetros: nombre del archivo (filename)
    */
std::vector<Message> PE::loadMessagesFromFile(const std::string& filename) {
    MessageInterpreter mi;
    std::vector<Message> messages = mi.load_messages_from_file(filename); // cargar mensajes desde el archivo
    return messages; // devolver los mensajes
}

/* metodo para generar mensajes (structs) que se envian al MMU a partir de cargar las instrucciones desde un archivo
    * parámetros: mensajes (messages)
    */
std::vector<ProcessedMessage> PE::processMessages(const std::vector<Message>& messages) {
    std::vector<ProcessedMessage> result;

    for (const auto& msg : messages) {
        std::visit([this, &result](auto&& m) {
            using T = std::decay_t<decltype(m)>;
            if constexpr (std::is_same_v<T, WriteMem>) {
                result.push_back(
                    writeMem(m.src, m.addr, m.num_of_cache_lines, m.start_cache_line, m.qos)
                );
            } else if constexpr (std::is_same_v<T, ReadMem>) {
                result.push_back(
                    readMem(m.src, m.addr, m.size, m.qos)
                );
            } else if constexpr (std::is_same_v<T, BroadcastInvalidate>) {
                result.push_back(
                    broadcastInvalidate(m.src, m.cache_line, m.qos)
                );
            }
        }, msg);
    }

    return result;
}


/* ------ métodos de la clase (envío de mensajes al MMU) ------ */

/* método para escribir en la memoria principal datos desde la caché
    * parámetros: SRC ID del PE, dirección de memoria principal (ADDR), número de lineas de caché (NUM_CACHE_LINES),
    *             linea de caché inicial (START_CACHE_LINE), prioridad del mensaje (QoS)
    */  
WRITE_MEM PE::writeMem(uint8_t SRC, uint16_t ADDR, uint8_t NUM_CACHE_LINES, uint8_t START_CACHE_LINE, uint8_t QoS) {
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
READ_MEM PE::readMem(uint8_t SRC, uint16_t ADDR, uint16_t SIZE, uint8_t QoS) {
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
BROADCAST_INVALIDATE PE::broadcastInvalidate(uint8_t SRC, uint8_t CACHE_LINE, uint8_t QoS) {
    BROADCAST_INVALIDATE msg;
    msg.SRC = SRC; // id del PE que envía el mensaje
    msg.CACHE_LINE = CACHE_LINE; // línea de caché a invalidar
    msg.QoS = QoS; // prioridad del mensaje

    return msg; // devolver el mensaje
}

/* método para responder a una invalidación de caché
    * parámetros: ID del PE que envía el mensaje (SRC), prioridad del mensaje (QoS), estado de la línea de caché (CACHE_LINE)
    */
INV_ACK PE::invAck(uint8_t SRC, uint8_t STATUS, uint8_t QoS, uint8_t DEST) {
    INV_ACK msg;
    msg.SRC = SRC; // id del PE que envía el mensaje
    msg.STATUS = STATUS; // estado de la línea de caché (true = válida, false = inválida)
    msg.QoS = QoS; // prioridad del mensaje
    msg.DEST = DEST; // id del PE que envía el mensaje

    return msg; // devolver el mensaje
}

/* ------ métodos de la clase (recibimiento de mensajes del MMU) ------ */

/* método para recibir un mensaje de invalidación de caché (BROADCAST_INVALIDATE)
    * parámetros: índice de la línea de cache a invalidar (CACHE_LINE)
    */
void PE::invalidateCacheLine(uint8_t CACHE_LINE, uint8_t SRC) {
    cache.invalidate(CACHE_LINE); // invalidar la línea de caché

    // enviar un mensaje de respuesta al PE que envió la invalidación
    uint8_t STATUS = 1;
    uint8_t QoS = 0x0; // prioridad del mensaje por defecto 0
    INV_ACK msg = invAck(id, STATUS, QoS, SRC); // generar el mensaje de respuesta

    // invocar metodo ProcessMessage del MMU para enviarle el mensaje

    messageTimer->addMessage(1, [this, msg]() {
        this->mmu->processMessage(msg); // invocar al metodo que recibe los mensajes en el PE
    });

    //mmu->processMessage(msg); // enviar el mensaje al MMU
}

/* método para recibir mensaje si todas las invalidaciones fueron exitosas (INV_COMPLETE)
    * parámetros: flag de éxito (STATUS) 0x1 = OK, 0x0 = NOT_OK
    */
void PE::invComplete(uint8_t STATUS) {
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
void PE::readResp(uint8_t* DATA, size_t DATA_SIZE) {
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
void PE::writeResp(uint8_t STATUS) {
    // de momento se hace un print pero no se sabe si al final el PE tiene que hacer algo con la respuesta
    if (STATUS == 0x1) {
        std::cout << "Escritura en memoria principal exitosa.\n";
    } else {
        std::cout << "Error en la escritura en memoria principal.\n";
    }
}

/* ------ método de la clase (recibimiento de mensaje default del MMU) ------ */
/* método para recibir un mensaje el mensaje deafult de respuesta por parte del MMU
    * parámetros: struct con los datos del mensaje (DATA_RESP_PE struct)
    */
void PE::getResponse(DATA_RESP_PE msg) {
    /*
    std::cout << "entro al getResponse \n"; 
    // imprimir el mensaje recibido
    std::cout << "Mensaje recibido de PE: " << static_cast<int>(msg.SRC) << "\n";
    std::cout << "Tipo de operación: " << static_cast<int>(msg.OPERATION_TYPE) << "\n";
    std::cout << "Estado: " << static_cast<int>(msg.STATUS) << "\n";
    std::cout << "Tamaño de los datos: " << msg.DATA_SIZE << "\n";
    */

    if (msg.OPERATION_TYPE == OPERATION_TYPE_PE::RESP_READ) {
        // Guardar los datos en la caché
        readResp(msg.DATA, msg.DATA_SIZE); // Llamar a la función readResp para guardar los datos en la caché
        this->waiting = false;
        std::cout << "Waiting = " << this->waiting << "\n"; 
        // Imprimir los datos recibidos (opcional)
        /*
        std::cout << "Respuesta de lectura recibida. Datos: ";
        for (size_t i = 0; i < msg.DATA_SIZE; ++i) {
            std::cout << static_cast<int>(msg.DATA[i]) << " ";
        }
        std::cout << "\n"; */
    } else if (msg.OPERATION_TYPE == OPERATION_TYPE_PE::RESP_WRITE) {
        writeResp(msg.STATUS); // Llamar a la función writeResp para manejar el estado de la escritura
    } else if (msg.OPERATION_TYPE == OPERATION_TYPE_PE::CACHE_INVALIDATE) {
        invalidateCacheLine(msg.CACHE_LINE, msg.SRC);
    } else if (msg.OPERATION_TYPE == OPERATION_TYPE_PE::INV_COMPLETE) {
        invComplete(msg.STATUS);
    } else {
        std::cerr << "Error: Tipo de mensaje no reconocido.\n";
    }  
}

