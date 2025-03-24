#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/objstr.h"
#include "py/mphal.h"

#include "SDL.h"
#include "esp_log.h"

// sdl.init()
static mp_obj_t sdl_init(void) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("SDL_Init Error: %s"), SDL_GetError());
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(sdl_init_obj, sdl_init);

// sdl.create_window(title, width, height)
static mp_obj_t sdl_create_window(mp_obj_t title_obj, mp_obj_t width_obj, mp_obj_t height_obj) {
    const char *title = mp_obj_str_get_str(title_obj);
    int width = mp_obj_get_int(width_obj);
    int height = mp_obj_get_int(height_obj);

    SDL_Window *window = SDL_CreateWindow(title, width, height, 0);
    if (!window) {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("SDL_CreateWindow Error: %s"), SDL_GetError());
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_DestroyWindow(window);
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("SDL_CreateRenderer Error: %s"), SDL_GetError());
    }

    mp_obj_t tuple[2] = {
        mp_obj_new_int_from_uint((uintptr_t)window),
        mp_obj_new_int_from_uint((uintptr_t)renderer),
    };
    return mp_obj_new_tuple(2, tuple);
}
static MP_DEFINE_CONST_FUN_OBJ_3(sdl_create_window_obj, sdl_create_window);

// sdl.clear(renderer, r, g, b)
static mp_obj_t sdl_clear(size_t n_args, const mp_obj_t *args) {
    SDL_Renderer *renderer = (SDL_Renderer *)(uintptr_t)mp_obj_get_int(args[0]);
    uint8_t r = mp_obj_get_int(args[1]);
    uint8_t g = mp_obj_get_int(args[2]);
    uint8_t b = mp_obj_get_int(args[3]);

    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(sdl_clear_obj, 4, 4, sdl_clear);

// sdl.update()
static mp_obj_t sdl_update(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) { }
    SDL_Delay(1);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(sdl_update_obj, sdl_update);

// sdl.destroy_window(window_handle, renderer_handle)
static mp_obj_t sdl_destroy_window(mp_obj_t window_obj, mp_obj_t renderer_obj) {
    SDL_Window *window = (SDL_Window *)(uintptr_t)mp_obj_get_int(window_obj);
    SDL_Renderer *renderer = (SDL_Renderer *)(uintptr_t)mp_obj_get_int(renderer_obj);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sdl_destroy_window_obj, sdl_destroy_window);

// Module globals
static const mp_rom_map_elem_t sdl_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_sdl) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&sdl_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_create_window), MP_ROM_PTR(&sdl_create_window_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&sdl_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&sdl_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_destroy_window), MP_ROM_PTR(&sdl_destroy_window_obj) },
};
static MP_DEFINE_CONST_DICT(sdl_module_globals, sdl_module_globals_table);

// Define module object
const mp_obj_module_t sdl_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&sdl_module_globals,
};

// Module registration
MP_REGISTER_MODULE(MP_QSTR_sdl, sdl_user_cmodule);
