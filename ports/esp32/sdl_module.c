#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/objstr.h"
#include "py/mphal.h"

#include "SDL.h"
#include "SDL_render.h"
#include "esp_log.h"

static mp_obj_t bridge_SDL_Init(mp_obj_t flags_obj) {
    int flags = mp_obj_get_int(flags_obj);
    if (!SDL_Init(flags)) {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("SDL_Init Error: %s"), SDL_GetError());
    }
    return mp_obj_new_bool(true);
}
static MP_DEFINE_CONST_FUN_OBJ_1(bridge_SDL_Init_obj, bridge_SDL_Init);

static mp_obj_t bridge_SDL_CreateWindow(mp_obj_t title_obj, mp_obj_t width_obj, mp_obj_t height_obj, mp_obj_t mode) {
    const char *title = mp_obj_str_get_str(title_obj);
    int width = mp_obj_get_int(width_obj);
    int height = mp_obj_get_int(height_obj);

    SDL_Window *window = SDL_CreateWindow(title, width, height, 0);
    if (!window) {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("SDL_CreateWindow Error: %s"), SDL_GetError());
    }
    return mp_obj_new_int_from_uint((uintptr_t)window);
}
static MP_DEFINE_CONST_FUN_OBJ_3(bridge_SDL_CreateWindow_obj, bridge_SDL_CreateWindow);

static mp_obj_t bridge_SDL_CreateRenderer(mp_obj_t window_obj, mp_obj_t render_driver) {
    SDL_Window *window = (SDL_Window *)(uintptr_t)mp_obj_get_int(window_obj);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_DestroyWindow(window);
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("SDL_CreateRenderer Error: %s"), SDL_GetError());
    }

    return mp_obj_new_int_from_uint((uintptr_t)renderer);
}
static MP_DEFINE_CONST_FUN_OBJ_2(bridge_SDL_CreateRenderer_obj, bridge_SDL_CreateRenderer);

static mp_obj_t bridge_SDL_SetRenderDrawColor(size_t n_args, const mp_obj_t *args) {
    if (n_args != 5) {
        mp_raise_TypeError("Expected 5 arguments");
    }
    SDL_Renderer *renderer = (SDL_Renderer *)(uintptr_t)mp_obj_get_int(args[0]);
    uint8_t r = mp_obj_get_int(args[1]);
    uint8_t g = mp_obj_get_int(args[2]);
    uint8_t b = mp_obj_get_int(args[3]);
    uint8_t a = mp_obj_get_int(args[4]);

    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(bridge_SDL_SetRenderDrawColor_obj, 5, 5, bridge_SDL_SetRenderDrawColor);

static mp_obj_t bridge_SDL_RenderClear(mp_obj_t renderer_obj) {
    SDL_Renderer *renderer = (SDL_Renderer *)(uintptr_t)mp_obj_get_int(renderer_obj);
    SDL_RenderClear(renderer);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(bridge_SDL_RenderClear_obj, bridge_SDL_RenderClear);

static mp_obj_t sdl_update(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {}
    SDL_Delay(1);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(sdl_update_obj, sdl_update);

static mp_obj_t sdl_destroy_window(mp_obj_t window_obj, mp_obj_t renderer_obj) {
    SDL_Window *window = (SDL_Window *)(uintptr_t)mp_obj_get_int(window_obj);
    SDL_Renderer *renderer = (SDL_Renderer *)(uintptr_t)mp_obj_get_int(renderer_obj);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sdl_destroy_window_obj, sdl_destroy_window);

// Bridge SDL_RenderFillRect
static mp_obj_t bridge_SDL_RenderFillRect(mp_obj_t render_obj, mp_obj_t rect) {
    SDL_Renderer *renderer = (SDL_Renderer *)(uintptr_t)mp_obj_get_int(render_obj);
    SDL_FRect *frect = (SDL_FRect *)(uintptr_t)mp_obj_get_int(rect);
    SDL_RenderFillRect(renderer, frect);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(bridge_SDL_RenderFillRect_obj, bridge_SDL_RenderFillRect);


// sdl.render_draw_rect(renderer, x, y, w, h)
static mp_obj_t sdl_render_draw_rect(size_t n_args, const mp_obj_t *args) {
    if (n_args != 5) {
        mp_raise_TypeError("Expected 5 arguments");
    }
    SDL_Renderer *renderer = (SDL_Renderer *)(uintptr_t)mp_obj_get_int(args[0]);
    float x = mp_obj_get_float(args[1]);
    float y = mp_obj_get_float(args[2]);
    float w = mp_obj_get_float(args[3]);
    float h = mp_obj_get_float(args[4]);

    SDL_FRect frect;
    frect.x = x;
    frect.y = y;
    frect.w = w;
    frect.h = h;

    SDL_RenderFillRect(renderer, &frect);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(sdl_render_draw_rect_obj, 5, 5, sdl_render_draw_rect);

// sdl.render_present(renderer)
static mp_obj_t sdl_render_present(mp_obj_t renderer_obj) {
    SDL_Renderer *renderer = (SDL_Renderer *)(uintptr_t)mp_obj_get_int(renderer_obj);
    SDL_RenderPresent(renderer);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(sdl_render_present_obj, sdl_render_present);


// Module globals
static const mp_rom_map_elem_t sdl_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_sdl3) },
    { MP_ROM_QSTR(MP_QSTR_SDL_Init), MP_ROM_PTR(&bridge_SDL_Init_obj) },
    { MP_ROM_QSTR(MP_QSTR_SDL_INIT_VIDEO), MP_ROM_INT(SDL_INIT_VIDEO) },
    { MP_ROM_QSTR(MP_QSTR_SDL_CreateWindow), MP_ROM_PTR(&bridge_SDL_CreateWindow_obj) },
    { MP_ROM_QSTR(MP_QSTR_SDL_CreateRenderer), MP_ROM_PTR(&bridge_SDL_CreateRenderer_obj) },
    { MP_ROM_QSTR(MP_QSTR_SDL_RenderClear), MP_ROM_PTR(&bridge_SDL_RenderClear_obj) },
    { MP_ROM_QSTR(MP_QSTR_SDL_SetRenderDrawColor), MP_ROM_PTR(&bridge_SDL_SetRenderDrawColor_obj) },
    { MP_ROM_QSTR(MP_QSTR_SDL_RenderFillRect), MP_ROM_PTR(&bridge_SDL_RenderFillRect_obj) },
    { MP_ROM_QSTR(MP_QSTR_SDL_Update), MP_ROM_PTR(&sdl_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_SDL_DestroyWindow), MP_ROM_PTR(&sdl_destroy_window_obj) },
    { MP_ROM_QSTR(MP_QSTR_SDL_RenderDrawRect), MP_ROM_PTR(&sdl_render_draw_rect_obj) },
    { MP_ROM_QSTR(MP_QSTR_SDL_RenderPresent), MP_ROM_PTR(&sdl_render_present_obj) },
};
static MP_DEFINE_CONST_DICT(sdl_module_globals, sdl_module_globals_table);

// Define module object
const mp_obj_module_t sdl_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&sdl_module_globals,
};

// Module registration
MP_REGISTER_MODULE(MP_QSTR_sdl3, sdl_user_cmodule);
