#ifndef MESSAGETIMER_H
#define MESSAGETIMER_H

#include "clock.h"
#include <vector>
#include <mutex>
#include <functional>


// Estructura para mensajes en tránsito
struct inTransitMessage {
    int remaining_cycles;
    std::function<void()> function;
};

// Clase MessageTimer
class MessageTimer {
    private:
        Clock* clock; // Puntero al reloj
        std::vector<inTransitMessage> messages; // Lista de mensajes en tránsito
        std::mutex timerMutex; // Mutex para proteger el acceso a los mensajes

    public:
        // Constructor
        MessageTimer(Clock* clk);

        // Constructor
        MessageTimer();

        // Agregar un mensaje con un retraso en ciclos
        void addMessage(int cycles, std::function<void()> func);

        // Actualizar el temporizador y ejecutar mensajes vencidos
        void update();

        // Obtener el numero de ciclos
        int getCycles();

        // Set the clock
        void setClock(Clock* clk);

        // Get the clock
        Clock* getClock();
};

#endif // MESSAGETIMER_H