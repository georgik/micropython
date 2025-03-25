#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/objstr.h"
#include "py/mphal.h"

#include "SDL.h"
#include "SDL_render.h"  // For SDL_RenderFillRect and SDL_RenderDrawRect
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
static mp_obj_t sdl_clear(mp_obj_t renderer_obj, mp_obj_t r_obj, mp_obj_t g_obj, mp_obj_t b_obj) {
    SDL_Renderer *renderer = (SDL_Renderer *)(uintptr_t)mp_obj_get_int(renderer_obj);
    uint8_t r = mp_obj_get_int(r_obj);
    uint8_t g = mp_obj_get_int(g_obj);
    uint8_t b = mp_obj_get_int(b_obj);

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

// sdl.set_render_draw_color(renderer, r, g, b, a)
static mp_obj_t sdl_set_render_draw_color(mp_obj_t renderer_obj, mp_obj_t r_obj, mp_obj_t g_obj, mp_obj_t b_obj, mp_obj_t a_obj) {
    SDL_Renderer *renderer = (SDL_Renderer *)(uintptr_t)mp_obj_get_int(renderer_obj);
    uint8_t r = mp_obj_get_int(r_obj);
    uint8_t g = mp_obj_get_int(g_obj);
    uint8_t b = mp_obj_get_int(b_obj);
    uint8_t a = mp_obj_get_int(a_obj);
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(sdl_set_render_draw_color_obj, 5, 5, sdl_set_render_draw_color);

// sdl.render_fill_rect(renderer, x, y, w, h)
static mp_obj_t sdl_render_fill_rect(mp_obj_t renderer_obj, mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t w_obj, mp_obj_t h_obj) {
    SDL_Renderer *renderer = (SDL_Renderer *)(uintptr_t)mp_obj_get_int(renderer_obj);
    float x = mp_obj_get_float(x_obj);
    float y = mp_obj_get_float(y_obj);
    float w = mp_obj_get_float(w_obj);
    float h = mp_obj_get_float(h_obj);
    SDL_FRect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_RenderFillRect(renderer, &rect);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(sdl_render_fill_rect_obj, 5, 5, sdl_render_fill_rect);

// sdl.render_draw_rect(renderer, x, y, w, h)
static mp_obj_t sdl_render_draw_rect(mp_obj_t renderer_obj, mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t w_obj, mp_obj_t h_obj) {
    SDL_Renderer *renderer = (SDL_Renderer *)(uintptr_t)mp_obj_get_int(renderer_obj);
    float x = mp_obj_get_float(x_obj);
    float y = mp_obj_get_float(y_obj);
    float w = mp_obj_get_float(w_obj);
    float h = mp_obj_get_float(h_obj);
    SDL_FRect frect;
    frect.x = x;
    frect.y = y;
    frect.w = w;
    frect.h = h;
    // Convert the float rect to an integer rect for the outline
    SDL_Rect irect;
    irect.x = (int)frect.x;
    irect.y = (int)frect.y;
    irect.w = (int)frect.w;
    irect.h = (int)frect.h;
    SDL_RenderRect(renderer, &irect);
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
    { MP_ROM_QSTR(MP_QSTR_SDL_Init), MP_ROM_PTR(&sdl_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_SDL_CreateWindow), MP_ROM_PTR(&sdl_create_window_obj) },
    { MP_ROM_QSTR(MP_QSTR_SDL_Clear), MP_ROM_PTR(&sdl_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_SDL_Update), MP_ROM_PTR(&sdl_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_SDL_DestroyWindow), MP_ROM_PTR(&sdl_destroy_window_obj) },
    { MP_ROM_QSTR(MP_QSTR_SDL_SetRenderDrawColor), MP_ROM_PTR(&sdl_set_render_draw_color_obj) },
    { MP_ROM_QSTR(MP_QSTR_SDL_RenderFillRect), MP_ROM_PTR(&sdl_render_fill_rect_obj) },
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
