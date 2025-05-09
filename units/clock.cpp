#include "clock.h"

// Constructor
Clock::Clock() {}

// Destructor
Clock::~Clock() {}

// Update the clock
void Clock::update() {
    std::lock_guard<std::mutex> lock(clockMutex);
    cycle++; // Increment cycle counter
    clockCV.notify_all(); // Notificar a todos los hilos que el ciclo ha avanzado
}

// Get the current clock state
bool Clock::getClock() {
    return clk;
}

// Get the current cycle count
int Clock::getCycle() {
    return cycle;
}

void Clock::waitForCycle(int targetCycle) {
    std::unique_lock<std::mutex> lock(clockMutex);
    clockCV.wait(lock, [this, targetCycle]() { return cycle >= targetCycle; });
}