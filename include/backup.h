//
// Created by sparcman on 2024-02-24.
//

#ifndef PROJECT_BACKUP_H
#define PROJECT_BACKUP_H

// include library to read and write from flash memory

#include "DataTypes.h"
#include <EEPROM.h>

extern void backup_update();
extern void connectBACKUP();

#endif //PROJECT_BACKUP_H
