#include <stdio.h>
#include <string.h>
#include "nmea_idf.h"
#include "py/obj.h"
#include "py/runtime.h"

// Bridge for hello()
// Declaration in nmea_idf.h: extern const void* hello();
static mp_obj_t bridge_hello(void) {
    const char *msg = hello();
    return mp_obj_new_str(msg, strlen(msg));
}
static MP_DEFINE_CONST_FUN_OBJ_0(bridge_hello_obj, bridge_hello);

// Bridge for nmea_size()
// Declaration: extern uint32_t nmea_size();
static mp_obj_t bridge_nmea_size(void) {
    uint32_t size = nmea_size();
    return mp_obj_new_int_from_uint(size);
}
static MP_DEFINE_CONST_FUN_OBJ_0(bridge_nmea_size_obj, bridge_nmea_size);

// Bridge for nmea_gga_altitude()
// Declaration: extern float nmea_gga_altitude(const char* gga_str);
static mp_obj_t bridge_nmea_gga_altitude(mp_obj_t gga_str_obj) {
    const char *gga_str = mp_obj_str_get_str(gga_str_obj);
    float alt = nmea_gga_altitude(gga_str);
    return mp_obj_new_float(alt);
}
static MP_DEFINE_CONST_FUN_OBJ_1(bridge_nmea_gga_altitude_obj, bridge_nmea_gga_altitude);

// Bridge for parse_nmea_gga()
// Declaration: struct CGgaData parse_nmea_gga(const char* gga_cstr);
static mp_obj_t bridge_parse_nmea_gga(mp_obj_t gga_str_obj) {
    const char *gga_str = mp_obj_str_get_str(gga_str_obj);
    struct CGgaData data = parse_nmea_gga(gga_str);
    mp_obj_t dict = mp_obj_new_dict(0);
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_fix_hour), mp_obj_new_int(data.fix_hour));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_fix_minute), mp_obj_new_int(data.fix_minute));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_fix_second), mp_obj_new_int(data.fix_second));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_fix_type), mp_obj_new_int(data.fix_type));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_latitude), mp_obj_new_float(data.latitude));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_longitude), mp_obj_new_float(data.longitude));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_fix_satellites), mp_obj_new_int(data.fix_satellites));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_hdop), mp_obj_new_float(data.hdop));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_altitude), mp_obj_new_float(data.altitude));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_geoid_separation), mp_obj_new_float(data.geoid_separation));
    return dict;
}
static MP_DEFINE_CONST_FUN_OBJ_1(bridge_parse_nmea_gga_obj, bridge_parse_nmea_gga);

// Module globals table
static const mp_rom_map_elem_t nmea_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_nmea) },
    { MP_ROM_QSTR(MP_QSTR_hello), MP_ROM_PTR(&bridge_hello_obj) },
    { MP_ROM_QSTR(MP_QSTR_nmea_size), MP_ROM_PTR(&bridge_nmea_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_nmea_gga_altitude), MP_ROM_PTR(&bridge_nmea_gga_altitude_obj) },
    { MP_ROM_QSTR(MP_QSTR_parse_nmea_gga), MP_ROM_PTR(&bridge_parse_nmea_gga_obj) },
};

static MP_DEFINE_CONST_DICT(nmea_module_globals, nmea_module_globals_table);

const mp_obj_module_t nmea_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&nmea_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_nmea, nmea_user_cmodule);
