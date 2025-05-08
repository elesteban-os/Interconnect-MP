#include "clock.h"

// Constructor
Clock::Clock() {}

// Destructor
Clock::~Clock() {}

// Update the clock
void Clock::update() {
    clk = !clk; // Toggle clock state
    if (clk) cycle++; // Increment cycle counter
}

// Get the current clock state
bool Clock::getClock() {
    return clk;
}

// Get the current cycle count
int Clock::getCycle() {
    return cycle;
}