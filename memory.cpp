#include <iostream>
#include <cstdint>
#include <cstring>
#include "memory.h"

// Declaracion de constantes del sistema
#define READ_CYCLES 8
#define WRITE_CYCLES 8

// Para el write_response
#define OK 0x1
#define NOT_OK 0x0


// Constructor
Memory::Memory() {
    // Initialize memory or other resources if needed
}

// Destructor
Memory::~Memory() {
    delete memory_operation;
    delete data_response;
}

// Handle invalid write response
int Memory::invalidWriteResponse() {
    this->data_response = new data_resp;
    this->data_response->pe_ID = this->memory_operation->pe_ID;
    this->data_response->op_type = this->memory_operation->op_type;
    this->data_response->QoS = this->memory_operation->QoS;
    this->data_response->status = NOT_OK;

    this->op_ready = true;
    return 1;
}

// Create a response for the current operation
int Memory::responseCreator() {
    this->data_response = new data_resp;
    this->data_response->pe_ID = this->memory_operation->pe_ID;
    this->data_response->op_type = this->memory_operation->op_type;
    this->data_response->QoS = this->memory_operation->QoS;
    this->data_response->blocks = this->memory_operation->blocks;

    if (this->memory_operation->op_type == operation_type::READ) {
        this->data_response->data = new block[this->memory_operation->blocks];
        memcpy(this->data_response->data, &this->memory[this->memory_operation->address],
               this->memory_operation->blocks * sizeof(block));
        this->data_response->status = OK;
        return 1;
    } else {  // operation_type::WRITE
        memcpy(&this->memory[this->memory_operation->address], this->memory_operation->write_data,
               this->memory_operation->blocks * sizeof(block));
        this->data_response->status = OK;
        return 1;
    }
}

// Update the memory state
int Memory::operationUpdate() {
    if (!this->op_ready) {
        if (this->cycle >= this->memory_operation->start_cycle + READ_CYCLES) {
            std::cout << "Operation is now ready." << std::endl;
            return 1;
        } else {
            std::cout << "Operation not ready yet." << std::endl;
            return 0;
        }
    } else {
        std::cout << "No operation in progress." << std::endl;
        return 0;
    }
}

// Update memory state and process operations
int Memory::update() {
    if (this->operationUpdate()) {
        responseCreator();
        if (this->memory_operation->op_type == operation_type::WRITE) {
            delete[] this->memory_operation->write_data;
        }
        delete this->memory_operation;
        this->memory_operation = nullptr;
        this->op_ready = true;
        this->cycle++;
        return 1;
    }
    this->cycle++;
    return 0;
}

// Read data from memory
int Memory::readData(uint32_t address, int peid_src, int blocks, int QoS) {
    std::cout << "Function readData() called. Address: " << address << ", PE ID: " << peid_src
              << ", Blocks: " << blocks << std::endl;
    if (this->op_ready) {
        if ((address + (blocks * 4)) < 0x4000 && (address % 4) == 0) {
            this->op_ready = false;
            this->memory_operation = new operation;
            this->memory_operation->address = address / 4;
            this->memory_operation->pe_ID = peid_src;
            this->memory_operation->blocks = blocks;
            this->memory_operation->op_type = operation_type::READ;
            this->memory_operation->start_cycle = this->cycle;
            this->memory_operation->QoS = QoS;
            return 1;
        } else {
            std::cout << "Error: Invalid address." << std::endl;
            return -1;
        }
    } else {
        std::cout << "Operation already in progress." << std::endl;
        return 0;
    }
}

// Write data to memory
int Memory::writeData(uint32_t address, int peid_src, block* data, int blocks, int QoS) {
    std::cout << "Function writeData() called. Address: " << address << ", PE ID: " << peid_src
              << ", Blocks: " << blocks << std::endl;
    if (this->op_ready) {
        if ((address + (blocks * 4)) < 0x4000 && (address % 4) == 0) {
            this->op_ready = false;
            this->memory_operation = new operation;
            this->memory_operation->address = address / 4;
            this->memory_operation->pe_ID = peid_src;
            this->memory_operation->write_data = data;
            this->memory_operation->op_type = operation_type::WRITE;
            this->memory_operation->blocks = blocks;
            this->memory_operation->start_cycle = this->cycle;
            this->memory_operation->QoS = QoS;
            return 1;
        } else {
            std::cout << "Error: Invalid address." << std::endl;
            return -1;
        }
    } else {
        std::cout << "Operation already in progress." << std::endl;
        return 0;
    }
}

// Get a word from memory
uint32_t Memory::getWord(uint32_t address) {
    if (address < 0x4000) {
        return this->memory[address].word;
    } else {
        return 0xFF;
    }
}

// Check if the operation is ready
bool Memory::get_op_ready() {
    return this->op_ready;
}

// Get the data response
data_resp* Memory::get_data_response() {
    return this->data_response;
}

int mainmem() {
    Memory mem;

    operation op1 = {1, operation_type::READ, 0x1000, 4, 5, 0};
    operation op2 = {2, operation_type::WRITE, 0x2000, 8, 3, 1};

    mem.readData(op1.address, op1.pe_ID, op1.blocks, op1.QoS);
    mem.update();

    mem.writeData(op2.address, op2.pe_ID, op2.write_data, op2.blocks, op2.QoS);
    mem.update();

    return 0;
}