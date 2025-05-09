#include "messagetimer.h"

// Constructor
MessageTimer::MessageTimer(Clock* clk) : clock(clk) {}

MessageTimer::MessageTimer() {}

// Agregar un mensaje con un retraso en ciclos
void MessageTimer::addMessage(int cycles, std::function<void()> func) {
    std::lock_guard<std::mutex> lock(timerMutex);
    inTransitMessage msg;
    msg.remaining_cycles = cycles;
    msg.function = func;
    messages.push_back(msg);
}

// Actualizar el temporizador y ejecutar mensajes vencidos
void MessageTimer::update() {
    for (auto it = messages.begin(); it != messages.end();) {
        if (it->remaining_cycles-- <= 0) {
            it->function(); // Ejecutar la función asociada al mensaje
            it = messages.erase(it); // Eliminar el mensaje de la lista
        } else {
            ++it; // Avanzar al siguiente mensaje
        }
    }
}

int MessageTimer::getCycles() {
    return clock->getCycle();
}

void MessageTimer::setClock(Clock* clk) {
    
    this->clock = clk;
}

Clock* MessageTimer::getClock() {
    return this->clock;
}