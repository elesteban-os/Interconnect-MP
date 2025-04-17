// Modos de calendarizacion
#define QOS_ON true
#define QOS_OFF false


bool setShedulerMode(bool mode) {
    this->sheduler_mode = mode; // Cambia el modo de calendarización
    return this->sheduler_mode;
}