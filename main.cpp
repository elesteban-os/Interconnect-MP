#include "SystemSimulator.h"

int main() {
    SystemSimulator sim;
    sim.start();
    /*
    for (int i = 0; i < 10000; ++i) {
        sim.step();
        std::cout << "Presione enter para continuar..." << std::endl;
        std::cin.get();
    }
    */
    sim.waitForThreads();
    return 0;
}