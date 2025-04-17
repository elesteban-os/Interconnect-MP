
#include <iostream>
#include <cstdint>

// Declaracion de constantes del sistema
#define READ_BYTE_CRITIC_TIME 50 // 50ns
#define WRITE_BYTE_CRITIC_TIME 60 // 60ns

// Respuesta de la lectura
typedef struct data_resp {
    
};

// Respuesta de la lectura
typedef struct operation {
    // tipo de operacion, dato leido/escrito, tamano
    enum type {READ, WRITE};
    type op_type;
    int address;
    uint32_t data;
    // time, cycle, peid??
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
    operation memory_operation = NULL;

public:
    Memory() {
        // Constructor
    }

    Memory() {
    }

    int update() {

    }

    // Es el equivalente a Read_Mem. Se debe hacer con
    int requestData(uint32_t address, int src, int blocks) {
        if (this->op_ready) {
            if (this->address < 0x1000) { // Verifica que la dirección esté dentro del rango
                this->op_ready = false
                return 0; // Retorna 0 si la operación fue exitosa
            } else {
                std::cerr << "Error: Address out of range." << std::endl;
                return -1; // Retorna -1 si hubo un error
            }
        }
        // Se esta realizando una operacion
        else return -1;
    }

    
    uint32_t getWord(uint32_t address) {
        if (this->address < 0x1000) { // Verifica que la dirección esté dentro del rango
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