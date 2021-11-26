#ifndef __UPGRADE_H__
#define __UPGRADE_H__

#define USER_BIN1               0x00
#define USER_BIN2               0x01

#define UPGRADE_FLAG_IDLE       0x00
#define UPGRADE_FLAG_START      0x01
#define UPGRADE_FLAG_FINISH     0x02

#define UPGRADE_FW_BIN1         0x00
#define UPGRADE_FW_BIN2         0x01

#include "user_config.h"

void system_upgrade_init();
bool system_upgrade(uint8 *data, uint32 len, DATOS_APLICACION *datosApp);
void fota_event_cb(DATOS_APLICACION *datosApp);


#endif
