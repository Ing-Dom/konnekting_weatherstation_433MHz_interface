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
#define COMOBJ_batteryloww132 9
#define COMOBJ_batteryloww174 10
#define COMOBJ_randomidw132 11
#define COMOBJ_randomidw174 12
#define COMOBJ_error_code 13
#define PARAM_filter_randomID_W132 0
#define PARAM_filter_randomID_W174 1
#define PARAM_max_data_age 2
#define PARAM_send_on_change_temperature 3
#define PARAM_send_on_change_humidity 4
#define PARAM_send_on_change_windspeed 5
#define PARAM_send_on_change_windgust 6
#define PARAM_send_on_change_rain 7
#define PARAM_send_on_change_dewpoint 8
#define PARAM_send_on_change_abshumidity 9
#define PARAM_cyclic_send_rate_temperature 10
#define PARAM_cyclic_send_rate_humidity 11
#define PARAM_cyclic_send_rate_windspeed 12
#define PARAM_cyclic_send_rate_windgust 13
#define PARAM_cyclic_send_rate_winddirection 14
#define PARAM_cyclic_send_rate_rain 15
#define PARAM_cyclic_send_rate_dewpoint 16
#define PARAM_cyclic_send_rate_abs_humidity 17
        
KnxComObject KnxDevice::_comObjectsList[] = {
    /* Index 0 - temperature */ KnxComObject(KNX_DPT_9_001, 0x3c),
    /* Index 1 - humidity */ KnxComObject(KNX_DPT_9_007, 0x3c),
    /* Index 2 - abs_humidity */ KnxComObject(KNX_DPT_9_007, 0x3c),
    /* Index 3 - dewpoint */ KnxComObject(KNX_DPT_9_001, 0x3c),
    /* Index 4 - airpressure */ KnxComObject(KNX_DPT_9_006, 0x3c),
    /* Index 5 - windspeed */ KnxComObject(KNX_DPT_9_005, 0x3c),
    /* Index 6 - windgust */ KnxComObject(KNX_DPT_9_005, 0x3c),
    /* Index 7 - winddirection */ KnxComObject(KNX_DPT_5_003, 0x3c),
    /* Index 8 - rainvolume */ KnxComObject(KNX_DPT_9_026, 0x3c),
    /* Index 9 - batteryloww132 */ KnxComObject(KNX_DPT_1_002, 0x34),
    /* Index 10 - batteryloww174 */ KnxComObject(KNX_DPT_1_002, 0x34),
    /* Index 11 - randomidw132 */ KnxComObject(KNX_DPT_5_010, 0x34),
    /* Index 12 - randomidw174 */ KnxComObject(KNX_DPT_5_010, 0x34),
    /* Index 13 - error_code */ KnxComObject(KNX_DPT_12_001, 0x34)
};
const byte KnxDevice::_numberOfComObjects = sizeof (_comObjectsList) / sizeof (KnxComObject); // do not change this code
       
byte KonnektingDevice::_paramSizeList[] = {
    /* Index 0 - filter_randomID_W132 */ PARAM_UINT8,
    /* Index 1 - filter_randomID_W174 */ PARAM_UINT8,
    /* Index 2 - max_data_age */ PARAM_UINT8,
    /* Index 3 - send_on_change_temperature */ PARAM_UINT8,
    /* Index 4 - send_on_change_humidity */ PARAM_UINT8,
    /* Index 5 - send_on_change_windspeed */ PARAM_UINT8,
    /* Index 6 - send_on_change_windgust */ PARAM_UINT8,
    /* Index 7 - send_on_change_rain */ PARAM_UINT8,
    /* Index 8 - send_on_change_dewpoint */ PARAM_UINT8,
    /* Index 9 - send_on_change_abshumidity */ PARAM_UINT8,
    /* Index 10 - cyclic_send_rate_temperature */ PARAM_UINT8,
    /* Index 11 - cyclic_send_rate_humidity */ PARAM_UINT8,
    /* Index 12 - cyclic_send_rate_windspeed */ PARAM_UINT8,
    /* Index 13 - cyclic_send_rate_windgust */ PARAM_UINT8,
    /* Index 14 - cyclic_send_rate_winddirection */ PARAM_UINT8,
    /* Index 15 - cyclic_send_rate_rain */ PARAM_UINT8,
    /* Index 16 - cyclic_send_rate_dewpoint */ PARAM_UINT8,
    /* Index 17 - cyclic_send_rate_abs_humidity */ PARAM_UINT8
};
const int KonnektingDevice::_numberOfParams = sizeof (_paramSizeList); // do not change this code