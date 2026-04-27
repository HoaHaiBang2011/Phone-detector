#ifndef OUI_DATABASE_H
#define OUI_DATABASE_H

#include <Arduino.h>
#include <pgmspace.h>

typedef struct {
    uint8_t oui[3];
    const char* manufacturer;
} OUI_Entry_t;

class OUIDatabase {
public:
    static const char* lookupManufacturer(const uint8_t* mac);
private:
    static const OUI_Entry_t oui_table[] PROGMEM; // Chuyển hẳn khai báo vào PROGMEM
    static const uint32_t oui_table_size;
};

#endif
