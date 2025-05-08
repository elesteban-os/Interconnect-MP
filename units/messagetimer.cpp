
#include "clock.h"
#include "schedulers/scheduler.h"
#include "memory.h"
#include <functional>

struct inTransitMessage {
    int remaining_cycles;
    std::function<void()> function;
};

class MessageTimer {
    private:
        Clock* clock;
        std::vector<inTransitMessage> messages;

    public:
        MessageTimer(Clock* clk) : clock(clk) {}

        void addMessage(int cycles, std::function<void()> func) {
            inTransitMessage msg;
            msg.remaining_cycles = cycles;
            msg.function = func;
            messages.push_back(msg);
        }

        void update() {
            for (auto it = messages.begin(); it != messages.end();) {
                if (it->remaining_cycles-- <= 0) {
                    it->function();
                    it = messages.erase(it);
                } else {
                    ++it;
                }
            }
        }
};

// Funcion de ejemplo para pasar parametros
void exampleFunction(int a, int b) {
    std::cout << "Function executed with parameters: " << a << ", " << b << std::endl;
}
// Funcion de ejemplo para pasar un puntero
void exampleFunctionPointer(int* ptr) {
    std::cout << "Function executed with pointer: " << *ptr << std::endl;
}

int main() {
    Clock clk;
    MessageTimer timer(&clk);
    
    // Ejemplo de uso con funciones con parametros
    int a = 5, b = 10;
    timer.addMessage(3, [a, b]() { exampleFunction(a, b); });

    // Ejemplo de uso con punteros
    int* ptr = new int(20);
    timer.addMessage(2, [ptr]() { exampleFunctionPointer(ptr); });

    // Example usage
    timer.addMessage(5, []() { std::cout << "Message 1 executed!" << std::endl; });
    timer.addMessage(3, []() { std::cout << "Message 2 executed!" << std::endl; });

    for (int i = 0; i < 6; ++i) {
        std::cout << "Cycle: " << i << std::endl;
        clk.update();
        timer.update();
    }

    return 0;
}