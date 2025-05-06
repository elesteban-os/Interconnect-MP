
#include "memory.h"
#include "executeunit.cpp"
#include "messagemanagement.cpp"

// Usando calendarizador, memoria y unidad de ejecucion
int main() {
    // Crear un scheduler
    Scheduler<data_resp> schedulerResp(ScheduleType::PRIORITY);
    Scheduler<operation> schedulerOperations(ScheduleType::PRIORITY);
    
    // Crear una memoria
    Memory mem;

    // Crear una unidad de ejecucion
    ExecuteUnit executeUnit(&schedulerOperations, &schedulerResp, &mem);

    // Crear un vector de operaciones
    std::vector<operation> operations;

    // Crear operaciones de ejemplo
    operation op1;
    op1.pe_ID = 1;
    op1.op_type = operation_type::READ;
    op1.address = 0;
    op1.blocks = 4;
    op1.QoS = 10;
    operations.push_back(op1);  

    operation op2;
    op2.pe_ID = 2;
    op2.op_type = operation_type::WRITE;
    op2.address = 4;
    op2.blocks = 4;
    op2.QoS = 0;
    op2.write_data = new block[4];
    for (int i = 0; i < 4; ++i) {
        op2.write_data->word = 0x1000 + i; // Example data
        op2.write_data++;
    }
    op2.write_data -= 4; // Reset pointer to the start of the data
    operations.push_back(op2);

    operation op3;
    op3.pe_ID = 1;
    op3.op_type = operation_type::READ;
    op3.address = 1;
    op3.blocks = 4;
    op3.QoS = 3;
    operations.push_back(op3);  

    // Agregar operaciones al scheduler
    for (const auto& op : operations) {
        schedulerOperations.addOperation(op);
    }

        // Simular la ejecución de las operaciones
    for (int cycle = 0; cycle < 40; ++cycle) {
        std::cout << "Cycle: " << cycle << std::endl;
        mem.update();
        executeUnit.update();
        
        // Wait for user input to proceed to the next cycle
        //std::cout << "Press Enter to continue to the next cycle..." << std::endl;
        //std::cin.get(); // Waits for the user to press Enter
    }
    
    // Imprimir la memoria
    std::cout << "Memory contents:" << std::endl;
    for (int i = 0; i < 20; ++i) {
        std::cout << "Address " << 4 * i << ": " << std::hex << mem.getWord(i) << std::endl;
    }

    // Ver contenido del calendarizador de respuestas
    std::cout << "Response Scheduler contents:" << std::endl;
    while (schedulerResp.getNextOperation().pe_ID != -1) {
        // Get the next response from the scheduler
        data_resp resp = schedulerResp.getNextOperation();

        // Ver todos los datos posibles de la respuesta
        std::cout << "Response PE ID: " << resp.pe_ID << std::endl;
        std::cout << "Response QoS: " << resp.QoS << std::endl;
        std::cout << "Response blocks: " << resp.blocks << std::endl;
        std::cout << "Response status: " << (int)resp.status << std::endl;
        std::cout << "Response data: ";
        if (resp.data == nullptr) {
            std::cout << "No data" << std::endl;
        } else {
            for (int i = 0; i < resp.blocks; ++i) {
                std::cout << std::hex << resp.data[i].word << " ";
            }
            std::cout << std::endl;
        }

        // Pop the response from the scheduler
        schedulerResp.popQueue();
    }
    

    return 0;

}