#include <iostream>
#include <cstdint>
#include <cstring> 

// Declaracion de constantes del sistema
#define READ_CYCLES 8 // 50ns
#define WRITE_CYCLES 8 // 60ns

// Tipos de interaccion con memoria
enum operation_type {READ, WRITE};

// Para el write_response
#define OK 0x1
#define NOT_OK 0x0

// Definición de la estructura block
union block {
    uint8_t byte[4];  // Definición de un bloque de 8 bits (1 byte)
    uint32_t word; // Definición de un bloque de 32 bits (4 bytes)
};

// Respuesta de la operacion
struct data_resp {
    // Para ambos:
    int pe_ID;
    operation_type op_type;
    int QoS;

    // Para read
    block *data;
    int blocks; // Para especificar el tamanio del read (num de bloques)
    
    // Para write
    uint8_t status = 0xFF;
};

// Estructura de la operacion
struct operation {
    // Para ambos
    int pe_ID;
    operation_type op_type;
    int address;
    int blocks;
    int QoS;
    int start_cycle;

    // Para write
    block *write_data = nullptr;
    // - Usar blocks para especificar el tamanio

    // Para read
    // - Usar blocks para especificar cuantos bloques se quiere leer   
};



// Definición de la clase Memory
class Memory {
private:
    block memory[4096] = {}; // 4096 posiciones de memoria con 4 bytes cada una
    bool op_ready = true;
    operation* memory_operation = nullptr;
    data_resp* data_response = nullptr;
    int cycle = 0;

    // Cuando se hace una escritura invalida
    int invalidWriteResponse() {
        std::cout << "Function invalidWriteResponse() called." << std::endl;
        // Crear la respuesta
        this->data_response = new data_resp;
        this->data_response->pe_ID = this->memory_operation->pe_ID;
        this->data_response->op_type = this->memory_operation->op_type;
        this->data_response->QoS = this->memory_operation->QoS;
        this->data_response->status = NOT_OK;

        // Listo para una nueva operacion
        this->op_ready = true;
        std::cout << "Invalid write response created." << std::endl;
        return 1;
    }

    int responseCreator() {
        std::cout << "Function responseCreator() called." << std::endl;
        this->data_response = new data_resp;
        this->data_response->pe_ID = this->memory_operation->pe_ID;
        this->data_response->op_type = this->memory_operation->op_type;
        this->data_response->QoS = this->memory_operation->QoS;
        this->data_response->blocks = this->memory_operation->blocks;

        if (this->memory_operation->op_type == READ) {
            std::cout << "Creating READ response." << std::endl;
            this->data_response->data = new block[this->memory_operation->blocks];
            memcpy(this->data_response->data, &this->memory[this->memory_operation->address], this->memory_operation->blocks * sizeof(block));
            this->data_response->status = OK;
            return 1;
        } else { // WRITE
            std::cout << "Creating WRITE response." << std::endl;
            memcpy(&this->memory[this->memory_operation->address], this->memory_operation->write_data, this->memory_operation->blocks * sizeof(block));
            this->data_response->status = OK;
            return 1;
        }
    }

    int operationUpdate() {
        std::cout << "Function operationUpdate() called. Current cycle: " << this->cycle << std::endl;
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

public:
    Memory() {
        // Constructor
    }

    int update() {
        std::cout << "Function update() called. Current cycle: " << this->cycle << std::endl;
        if (this->operationUpdate()) {
            std::cout << "Operation completed. Preparing response." << std::endl;
            responseCreator();
            if (this->memory_operation->op_type == WRITE) {
                delete[] this->memory_operation->write_data;
            }
            delete[] this->memory_operation;
            this->memory_operation = nullptr;
            this->op_ready = true;
            this->cycle++;
            return 1;
        }
        this->cycle++;
        return 0;
        
    }

    
    // Es el equivalente a Read_Mem
    int readData(uint32_t address, int peid_src, int blocks, int QoS) {
        std::cout << "Function readData() called. Address: " << address << ", PE ID: " << peid_src << ", Blocks: " << blocks << std::endl;
        if (this->op_ready) {
            if (address < 0x1000) { // Verifica que la dirección esté dentro del rango
                this->op_ready = false;
                // Hacer una nueva operacion
                this->memory_operation = new operation;
                this->memory_operation->address = address;
                this->memory_operation->pe_ID = peid_src;
                this->memory_operation->blocks = blocks;
                this->memory_operation->op_type = READ;
                this->memory_operation->start_cycle = this->cycle;
                this->memory_operation->QoS = QoS;
                std::cout << "Read operation created successfully." << std::endl;
                return 1; // Retorna 1 si el request se hizo
            } else {
                std::cout << "Error: Address out of range." << std::endl;
                return -1; // Retorna -1 si hubo un error
            }
        }
        // Se esta realizando una operacion entonces retorna 0
        else {
            std::cout << "Operation already in progress." << std::endl;
            return 0;
        }
    }

    // Es el equivalente a Write_Mem
    int writeData(uint32_t address, int peid_src, block *data, int blocks, int QoS) {
        std::cout << "Function writeData() called. Address: " << address << ", PE ID: " << peid_src << ", Blocks: " << blocks << std::endl;
        if (this->op_ready) {
            if ((address < 0x1000) || ((address + (blocks * 8)) > 0x1000)) { // Verifica que la dirección esté dentro del rango
                this->op_ready = false;
                // Hacer una nueva operacion
                this->memory_operation = new operation;
                this->memory_operation->address = address;
                this->memory_operation->pe_ID = peid_src;
                this->memory_operation->write_data = data;
                this->memory_operation->op_type = WRITE;
                this->memory_operation->blocks = blocks;
                this->memory_operation->start_cycle = this->cycle;
                this->memory_operation->QoS = QoS;
                std::cout << "Write operation created successfully." << std::endl;
                return 1; // Retorna 1 si el request se hizo
            } else {
                // Enviar respuesta de operacion invalida
                std::cout << "Error: Address or size out of range." << std::endl;
                return -1; // Retorna -1 si hubo un error
            }
        }
        // Se esta realizando una operacion entonces retorna 0
        else {
            std::cout << "Operation already in progress." << std::endl;
            return 0;
        }
    }

    uint32_t getWord(uint32_t address) {
        std::cout << "Function getWord() called. Address: " << address << std::endl;
        if (address < 0x1000) { // Verifica que la dirección esté dentro del rango
            std::cout << "Returning word from memory." << std::endl;
            return this->memory[address].word; // Retorna el bloque de 32 bits
        } else {
            std::cerr << "Error: Address out of range." << std::endl;
            return 0xFF;
        }
    }

    bool get_op_ready() {
        return this->op_ready;
    }

    data_resp* get_data_response() {
        return this->data_response;
    }
    
};


int main() {

    /*
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
    */

    // Prueba de memory
    Memory mem;
    mem.get_op_ready();

    block *wrtdata = new block[3];
    for(int i = 0; i < 3; i++) {
        wrtdata->word = i;
        std::cout << "Data: " << std::hex << static_cast<int>(wrtdata->word) << std::endl;
        wrtdata += 1;
    }
    wrtdata -= 3;

    mem.writeData(0x0008, 1, wrtdata, 3, 1);
    for(int i = 0; i <= 9; i++) {
        mem.update();
    }

    std::cout << "=== Data Response ===" << std::endl;
    std::cout << "Blocks: " << mem.get_data_response()->blocks << std::endl;

    std::cout << "Operation Type: " 
              << (mem.get_data_response()->op_type == READ ? "READ" : "WRITE") 
              << std::endl;
    std::cout << "PE ID: " << mem.get_data_response()->pe_ID << std::endl;
    std::cout << "QoS: " << mem.get_data_response()->QoS << std::endl;
    std::cout << "Status: " 
              << (mem.get_data_response()->status == OK ? "OK" : "NOT OK") 
              << std::endl;
    std::cout << "=====================" << std::endl;

    mem.readData(0x0008, 1, 3, 3);
    for(int i = 0; i <= 8; i++) {
        mem.update();
    }

    std::cout << "=== Data Response ===" << std::endl;
    std::cout << "Blocks: " << mem.get_data_response()->blocks << std::endl;
    std::cout << "Data Pointer: ";

    block *data = mem.get_data_response()->data;
    for (int i = 0; i < mem.get_data_response()->blocks; i++) {
        std::cout << data->word << ", ";
        data += 1;
    }
    std::cout << std::endl;

    std::cout << "Operation Type: " 
              << (mem.get_data_response()->op_type == READ ? "READ" : "WRITE") 
              << std::endl;
    std::cout << "PE ID: " << mem.get_data_response()->pe_ID << std::endl;
    std::cout << "QoS: " << mem.get_data_response()->QoS << std::endl;
    std::cout << "Status: " 
              << (mem.get_data_response()->status == OK ? "OK" : "NOT OK") 
              << std::endl;
    std::cout << "=====================" << std::endl;

    wrtdata = new block[2];
    for(int i = 0; i < 2; i++) {
        wrtdata->word = (i + 1) * 100;
        std::cout << "Data: " << std::hex << static_cast<int>(wrtdata->word) << std::endl;
        wrtdata += 1;
    }
    wrtdata -= 2;

    mem.writeData(0x0008, 1, wrtdata, 2, 1);
    for(int i = 0; i <= 9; i++) {
        mem.update();
    }
    mem.readData(0x0008, 1, 3, 3);
    for(int i = 0; i <= 8; i++) {
        mem.update();
    }

    std::cout << "=== Data Response ===" << std::endl;
    std::cout << "Blocks: " << mem.get_data_response()->blocks << std::endl;
    std::cout << "Data Pointer: ";

    data = mem.get_data_response()->data;
    for (int i = 0; i < mem.get_data_response()->blocks; i++) {
        std::cout << data->word << ", ";
        data += 1;
    }
    std::cout << std::endl;

    std::cout << "Operation Type: " 
              << (mem.get_data_response()->op_type == READ ? "READ" : "WRITE") 
              << std::endl;
    std::cout << "PE ID: " << mem.get_data_response()->pe_ID << std::endl;
    std::cout << "QoS: " << mem.get_data_response()->QoS << std::endl;
    std::cout << "Status: " 
              << (mem.get_data_response()->status == OK ? "OK" : "NOT OK") 
              << std::endl;
    std::cout << "=====================" << std::endl;
}