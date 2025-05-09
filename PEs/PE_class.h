#ifndef PECLASS_H
#define PECLASS_H

#include "cache_class.cpp"
#include "interpreter.h"
#include <vector>
#include <variant>
#include "../units/datamessages.h"
#include "../units/messagemanagement.h"

// Definición de tipo de mensaje
using ProcessedMessage = std::variant<WRITE_MEM, READ_MEM, BROADCAST_INVALIDATE>;

class MessageManagementUnit;

class PE {
public:
    size_t nextCacheLine = 0; 
    int id;
    bool waiting;
    MessageManagementUnit* mmu;
    Cache cache;

    PE(int id);

    std::vector<Message> loadMessagesFromFile(const std::string& filename);
    std::vector<ProcessedMessage> processMessages(const std::vector<Message>& messages);

    WRITE_MEM writeMem(uint8_t SRC, uint16_t ADDR, uint8_t NUM_CACHE_LINES, uint8_t START_CACHE_LINE, uint8_t QoS);
    READ_MEM readMem(uint8_t SRC, uint16_t ADDR, uint16_t SIZE, uint8_t QoS);
    BROADCAST_INVALIDATE broadcastInvalidate(uint8_t SRC, uint8_t CACHE_LINE, uint8_t QoS);
    INV_ACK invAck(uint8_t SRC, uint8_t STATUS, uint8_t QoS, uint8_t DEST);

    void invalidateCacheLine(uint8_t CACHE_LINE, uint8_t SRC);
    void invComplete(uint8_t STATUS);
    void readResp(uint8_t* DATA, size_t DATA_SIZE);
    void writeResp(uint8_t STATUS);
    void getResponse(DATA_RESP_PE msg);
};

#endif