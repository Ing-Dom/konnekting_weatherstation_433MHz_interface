#define MANUFACTURER_ID 7337
#define DEVICE_ID 30
#define REVISION 0

#define COMOBJ_temperature 0
#define COMOBJ_humidity 1
#define COMOBJ_abs_humidity 2
#define COMOBJ_dewpoint 3
#define COMOBJ_airpressure 4
#define COMOBJ_windspeed 5
#define COMOBJ_windgust 6
#define COMOBJ_winddirection 7
#define COMOBJ_rainvolume 8
#define COMOBJ_rainvolume1 9
#define COMOBJ_rainvolume1_reset 10
#define COMOBJ_randomidw132 11
#define COMOBJ_randomidw174 12
#define COMOBJ_error_code 13
        
KnxComObject KnxDevice::_comObjectsList[] = {
    /* Index 0 - temperature */ KnxComObject(KNX_DPT_9_001, 0x3c),
    /* Index 1 - humidity */ KnxComObject(KNX_DPT_9_007, 0x3c),
    /* Index 2 - abs_humidity */ KnxComObject(KNX_DPT_7_001, 0x3c),
    /* Index 3 - dewpoint */ KnxComObject(KNX_DPT_9_001, 0x3c),
    /* Index 4 - airpressure */ KnxComObject(KNX_DPT_9_006, 0x3c),
    /* Index 5 - windspeed */ KnxComObject(KNX_DPT_9_005, 0x3c),
    /* Index 6 - windgust */ KnxComObject(KNX_DPT_9_005, 0x3c),
    /* Index 7 - winddirection */ KnxComObject(KNX_DPT_5_003, 0x3c),
    /* Index 8 - rainvolume */ KnxComObject(KNX_DPT_9_026, 0x3c),
    /* Index 9 - rainvolume1 */ KnxComObject(KNX_DPT_9_026, 0x3c),
    /* Index 10 - rainvolume1_reset */ KnxComObject(KNX_DPT_1_015, 0x28),
    /* Index 11 - randomidw132 */ KnxComObject(KNX_DPT_5_010, 0x34),
    /* Index 12 - randomidw174 */ KnxComObject(KNX_DPT_5_010, 0x34),
    /* Index 13 - error_code */ KnxComObject(KNX_DPT_12_001, 0x34)
};
const byte KnxDevice::_numberOfComObjects = sizeof (_comObjectsList) / sizeof (KnxComObject); // do not change this code
       
byte KonnektingDevice::_paramSizeList[] = {

};
const int KonnektingDevice::_numberOfParams = sizeof (_paramSizeList); // do not change this code