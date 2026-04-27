#ifndef OUI_DATABASE_H
#define OUI_DATABASE_H

#include <stdint.h>
#include <string.h>
#include "config.h"

typedef struct {
    const char* manufacturer;
    const uint8_t oui[3];
} OUI_Entry_t;

class OUIDatabase {
public:
    static const char* lookupManufacturer(const uint8_t* mac_oui);
    static uint8_t getDeviceCount(const char* manufacturer);
    
private:
    static const OUI_Entry_t oui_table[];
    static const uint32_t oui_table_size;
};

#endif // OUI_DATABASE_H
