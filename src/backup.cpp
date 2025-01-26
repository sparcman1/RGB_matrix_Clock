//
// Created by sparcman on 2024-02-24.
//

#include "backup.h"
#include "shell_mini.h"

///////////////////////////////////////////////////////
// EEPROM UPDATE
typedef enum eeprom_work_t{
    UPDATE_EVENT_WAIT,
    UPDATE_COMP_WAIT,
}TypeDef_EepromWork_t;

void backup_update(){
///////////////////////////////////////////////////////////////
    static uint64_t old_time;
    if ((GD.TickTime - old_time) >= 100)
        old_time = GD.TickTime;
    else return;
    ///////////////////////////////////////////////////////////////

    static TypeDef_EepromWork_t state;

    switch(state){
        case UPDATE_COMP_WAIT :
            ws_println("Eeprom update completed...");
            state = UPDATE_EVENT_WAIT;
            break;

        default :
            if(GD.sys.magic_code==CFG_MAGIC_UPDATE){

                ws_println("EEPROM Update event...");

                // 재진입 방지를 위해 Update 한다
                GD.sys.magic_code = CFG_MAGIC_GOOD;

                char *p = (char*)&GD.sys;

                for(int32_t i=0; i<sizeof(TypeDef_Backup_t); i++){
                    //if(i%16==0) Serial.println(""), WebSerial.println("");
                    EEPROM.write(i, *(p+i));
                    //Serial.printf("%02X ", *(p+i));
                    //ws_printf("%02X ", *(p+i));
                }

                //Serial.println("");
                //ws_printf("");

                EEPROM.commit();

                state = UPDATE_COMP_WAIT;
            }
            break;
    }
}

///////////////////////////////////////////////////////
// CONNECT BACKUP
void connectBACKUP(){
    // initialize EEPROM with predefined size
    EEPROM.begin(sizeof(TypeDef_Backup_t));

    // read backup data
    uint8_t *p = &GD.sys.magic_code;

    for(int i=0; i<sizeof(TypeDef_Backup_t); i++){
        *(p+i) = EEPROM.read(i);
        //if(i%16==0) Serial.println("");
        //Serial.printf("%02X ", *(p+i));
    }

    ws_println("");

    // check magic code
    if(GD.sys.magic_code != CFG_MAGIC_GOOD){
        ws_println("EEPROM Initialize...");

        strcpy(GD.sys.wifi_ssid, MY_WIFI_SSID);
        strcpy(GD.sys.wifi_pass, MY_WIFI_PASS);
        strcpy(GD.sys.rss_url, MY_LOCATION_RSS);

        GD.sys.bright_ctrl_on = true;
        GD.sys.bright = CFG_BRIGHT_MID;

        GD.sys.bright_day_offset   = 0;
        GD.sys.bright_night_offset = 0;

        strcpy(GD.sys.my_sido,      MY_DUST_SIDO);
        strcpy(GD.sys.dust_url,     MY_DUST_URL);
        strcpy(GD.sys.my_station,   MY_DUST_STATION_NAME);
        strcpy(GD.sys.my_key,       MY_KEY_1);
        GD.sys.my_page = MY_DUST_PAGE;

        // EEPROM Update process magic code update
        GD.sys.magic_code = CFG_MAGIC_UPDATE;
    }

    // 이전 펌웨어에서 업데이트 된 것 이라면 추가된 것들을 여기서 처리
    if(GD.sys.bright_night_offset == 0xFF){
        GD.sys.bright_night_offset = 0;
        GD.sys.magic_code = CFG_MAGIC_UPDATE;
    }
}