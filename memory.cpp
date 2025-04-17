
#include <iostream>
#include <cstdint>

// Declaracion de constantes del sistema
#define READ_CYCLES 8 // 50ns
#define WRITE_CYCLES 8 // 60ns

// Tipos de interaccion con memoria
enum operation_type {READ, WRITE};

// Respuesta de la operacion
typedef struct data_resp {
    
};

// Estructura de la operacion
typedef struct operation {
    int pe_ID;
    operation_type op_type;
    int address;
    int blocks;
    block *write_data;
    int start_cycle;
};

// Definición de la estructura block
union block {
    uint8_t byte[4];  // Definición de un bloque de 8 bits (1 byte)
    uint32_t word; // Definición de un bloque de 32 bits (4 bytes)
};

// Definición de la clase Memory
class Memory {
private:
    block memory[4096]; // 4096 posiciones de memoria con 4 bytes cada una
    bool op_ready = true;
    operation* memory_operation = nullptr;
    data_resp* data_response = nullptr;
    int cycle;

public:
    Memory() {
        // Constructor
    }

    Memory() {
    }

    int update() {
        this->cycle++;
        if (this->operationUpdate()) {
            responseCreator();
        }

    }

    int responseCreator() {
        if (this->memory_operation->op_type == READ) {
            // Se hace la lectura de la informacion guardada en operation
        } else {
            // Se hace la escritura de la informacion guardada en operation
        }
    }

    // Esta funcion hace que no se tenga la informacion de lectura o escritura hasta 
    // que se pasen los ciclos
    int operationUpdate() {
        if (!this->op_ready) {
            if (this->cycle > this->cycle + this->memory_operation->start_cycle) {
                this->op_ready = true;
                return 1;
            } else {
                return 0;
            }
        } else {
            return -1;
        }
        
    }

    // Es el equivalente a Read_Mem
    int readData(uint32_t address, int peid_src, int blocks) {
        if (this->op_ready) {
            if (address < 0x1000) { // Verifica que la dirección esté dentro del rango
                this->op_ready = false;
                // Hacer una nueva operacion
                this->memory_operation = new operation;
                this->memory_operation->address = address;
                this->memory_operation->pe_ID = peid_src;
                this->memory_operation->blocks = blocks;
                this->memory_operation->op_type = READ;
                return 1; // Retorna 1 si el request se hizo
            } else {
                std::cout << "Error: Address out of range." << std::endl;
                return -1; // Retorna -1 si hubo un error
            }
        }
        // Se esta realizando una operacion entonces retorna 0
        else return 0;
    }

    // Es el equivalente a Write_Mem
    int writeData(uint32_t address, int peid_src, block *data) {
        if (this->op_ready) {
            if (address < 0x1000) { // Verifica que la dirección esté dentro del rango
                this->op_ready = false;
                // Hacer una nueva operacion
                this->memory_operation = new operation;
                this->memory_operation->address = address;
                this->memory_operation->pe_ID = peid_src;
                this->memory_operation->write_data = data;
                this->memory_operation->op_type = WRITE;
                return 1; // Retorna 1 si el request se hizo
            } else {
                std::cout << "Error: Address out of range." << std::endl;
                return -1; // Retorna -1 si hubo un error
            }
        }
        // Se esta realizando una operacion entonces retorna 0
        else return 0;
    }

    
    uint32_t getWord(uint32_t address) {
        if (address < 0x1000) { // Verifica que la dirección esté dentro del rango
            return this->memory[address].word; // Retorna el bloque de 32 bits
        } else {
            std::cerr << "Error: Address out of range." << std::endl;
            return 0xFF;
        }
    }

    

};


int main() {

    // Ejemplo de uso de block
    block data[2];

    data[0].word = 0x12345678; 
    data[1].word = 0x87654321; 

    std::cout << "Data: " << std::hex << data[0].word << std::endl; // Imprimir el valor en hexadecimal
    std::cout << "Data: " << std::hex << data[1].word << std::endl; // Imprimir el valor en hexadecimal

    std::cout << "Data: " << std::hex << static_cast<int>(data[0].byte[0]) << std::endl; // Imprimir el valor en hexadecimal
    std::cout << "Data: " << std::hex << static_cast<int>(data[0].byte[1]) << std::endl; // Imprimir el valor en hexadecimal
    std::cout << "Data: " << std::hex << static_cast<int>(data[0].byte[2]) << std::endl; // Imprimir el valor en hexadecimal
    std::cout << "Data: " << std::hex << static_cast<int>(data[0].byte[3]) << std::endl; // Imprimir el valor en hexadecimal

}