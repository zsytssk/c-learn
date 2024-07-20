#define _POSIX_C_SOURCE 199309L
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wayland-server-core.h>
#include <wlr/backend.h>
#include <wlr/backend/session.h>
#include <wlr/render/allocator.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_input_device.h>
#include <wlr/types/wlr_keyboard.h>
#include <wlr/util/log.h>
#include <xkbcommon/xkbcommon.h>
#include <unistd.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_subcompositor.h>

struct local_server
{
    struct wl_display *wl_display;
    struct wl_listener new_output;
    struct wl_listener new_input;
    struct wlr_renderer *renderer;
    struct wlr_allocator *allocator;
    struct timespec last_frame;
    float color[4];
    int dec;
};

struct sample_output
{
    struct local_server *sample;
    struct wlr_output *output;
    struct wl_listener frame;
    struct wl_listener destroy;
    struct wl_listener request_state;
};

struct sample_keyboard
{
    struct local_server *sample;
    struct wlr_keyboard *wlr_keyboard;
    struct wl_listener key;
    struct wl_listener destroy;
};

static void output_frame_notify(struct wl_listener *listener, void *data)
{
    struct sample_output *sample_output =
        wl_container_of(listener, sample_output, frame);
    struct local_server *sample = sample_output->sample;
    struct wlr_output *wlr_output = sample_output->output;

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    // long ms = (now.tv_sec - sample->last_frame.tv_sec) * 1000 +
    //           (now.tv_nsec - sample->last_frame.tv_nsec) / 1000000;
    // int inc = (sample->dec + 1) % 3;

    // sample->color[inc] += ms / 2000.0f;
    // sample->color[sample->dec] -= ms / 2000.0f;

    // if (sample->color[sample->dec] < 0.0f)
    // {
    //     sample->color[inc] = 1.0f;
    //     sample->color[sample->dec] = 0.0f;
    //     sample->dec = inc;
    // }

    struct wlr_output_state state;
    wlr_output_state_init(&state);

    struct wlr_render_pass *pass = wlr_output_begin_render_pass(wlr_output, &state, NULL, NULL);
    wlr_render_pass_add_rect(pass, &(struct wlr_render_rect_options){
                                       .box = {.width = wlr_output->width, .height = wlr_output->height},
                                       .color = {
                                           .r = sample->color[0],
                                           .g = sample->color[1],
                                           .b = sample->color[2],
                                           .a = sample->color[3],
                                       },
                                   });
    wlr_render_pass_submit(pass);

    wlr_output_commit_state(wlr_output, &state);
    wlr_output_state_finish(&state);
    sample->last_frame = now;
}

static void output_remove_notify(struct wl_listener *listener, void *data)
{
    struct sample_output *sample_output =
        wl_container_of(listener, sample_output, destroy);
    wlr_log(WLR_DEBUG, "Output removed");
    wl_list_remove(&sample_output->frame.link);
    wl_list_remove(&sample_output->destroy.link);
    free(sample_output);
}
static void request_state_notify(struct wl_listener *listener, void *data)
{
    struct sample_output *sample_output =
        wl_container_of(listener, sample_output, request_state);
    wlr_log(WLR_ERROR, "request_state_notify:> %d", sample_output->output->width);
    const struct wlr_output_event_request_state *event = data;
    wlr_output_commit_state(sample_output->output, event->state);
}

static void new_output_notify(struct wl_listener *listener, void *data)
{
    struct wlr_output *output = data;
    struct local_server *sample =
        wl_container_of(listener, sample, new_output);

    wlr_log(WLR_ERROR, "new_output_notify:> %d", output->width);
    wlr_output_init_render(output, sample->allocator, sample->renderer);

    struct sample_output *sample_output = calloc(1, sizeof(*sample_output));
    sample_output->output = output;
    sample_output->sample = sample;
    wl_signal_add(&output->events.frame, &sample_output->frame);
    sample_output->frame.notify = output_frame_notify;
    wl_signal_add(&output->events.destroy, &sample_output->destroy);
    sample_output->destroy.notify = output_remove_notify;
    wl_signal_add(&output->events.request_state, &sample_output->request_state);
    sample_output->request_state.notify = request_state_notify;

    struct wlr_output_state state;
    wlr_output_state_init(&state);
    wlr_output_state_set_enabled(&state, true);
    struct wlr_output_mode *mode = wlr_output_preferred_mode(output);
    if (mode != NULL)
    {
        wlr_output_state_set_mode(&state, mode);
    }
    wlr_output_commit_state(output, &state);
    wlr_output_state_finish(&state);
}

static void keyboard_key_notify(struct wl_listener *listener, void *data)
{
    struct sample_keyboard *keyboard = wl_container_of(listener, keyboard, key);
    struct local_server *sample = keyboard->sample;
    struct wlr_keyboard_key_event *event = data;
    uint32_t keycode = event->keycode + 8;
    const xkb_keysym_t *syms;
    int nsyms = xkb_state_key_get_syms(keyboard->wlr_keyboard->xkb_state,
                                       keycode, &syms);
    wlr_log(WLR_ERROR, "keyboard_key_notify:> code=%d, nsyms=%d", event->keycode, nsyms);
    for (int i = 0; i < nsyms; i++)
    {
        xkb_keysym_t sym = syms[i];
        if (sym == XKB_KEY_Escape)
        {
            wl_display_terminate(sample->wl_display);
        }
        if (sym == XKB_KEY_F1)
        {
            if (fork() == 0)
            {
                execl("/bin/sh", "/bin/sh", "-c", "alacritty", (void *)NULL);
            }
        }
    }
}

static void keyboard_destroy_notify(struct wl_listener *listener, void *data)
{
    wlr_log(WLR_ERROR, "keyboard_destroy_notify");
    struct sample_keyboard *keyboard =
        wl_container_of(listener, keyboard, destroy);
    wl_list_remove(&keyboard->destroy.link);
    wl_list_remove(&keyboard->key.link);
    free(keyboard);
}

static void new_input_notify(struct wl_listener *listener, void *data)
{
    struct wlr_input_device *device = data;
    struct local_server *sample = wl_container_of(listener, sample, new_input);
    wlr_log(WLR_ERROR, "new_input_notify:> %d", device->type);
    switch (device->type)
    {

    case WLR_INPUT_DEVICE_KEYBOARD:;
        struct sample_keyboard *keyboard = calloc(1, sizeof(*keyboard));
        keyboard->wlr_keyboard = wlr_keyboard_from_input_device(device);
        keyboard->sample = sample;
        wl_signal_add(&device->events.destroy, &keyboard->destroy);
        keyboard->destroy.notify = keyboard_destroy_notify;
        wl_signal_add(&keyboard->wlr_keyboard->events.key, &keyboard->key);
        keyboard->key.notify = keyboard_key_notify;
        struct xkb_context *context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
        if (!context)
        {
            wlr_log(WLR_ERROR, "Failed to create XKB context");
            exit(1);
        }
        struct xkb_keymap *keymap = xkb_keymap_new_from_names(context, NULL,
                                                              XKB_KEYMAP_COMPILE_NO_FLAGS);
        if (!keymap)
        {
            wlr_log(WLR_ERROR, "Failed to create XKB keymap");
            exit(1);
        }
        wlr_keyboard_set_keymap(keyboard->wlr_keyboard, keymap);
        xkb_keymap_unref(keymap);
        xkb_context_unref(context);
        break;
    default:
        break;
    }
}

int main(void)
{
    wlr_log_init(WLR_DEBUG, NULL);
    struct wl_display *wl_display = wl_display_create();
    struct local_server server = {
        .color = {0.1, 0.1, 0.1, 0.8},
        .dec = 0,
        .last_frame = {0},
        .wl_display = wl_display};

    struct wlr_backend *backend = wlr_backend_autocreate(wl_display_get_event_loop(wl_display), NULL);
    if (!backend)
    {
        wlr_log(WLR_ERROR, "failed to create wlr_backend");
        exit(1);
    }

    server.renderer = wlr_renderer_autocreate(backend);
    server.allocator = wlr_allocator_autocreate(backend, server.renderer);

    wl_signal_add(&backend->events.new_output, &server.new_output);
    server.new_output.notify = new_output_notify;
    wl_signal_add(&backend->events.new_input, &server.new_input);
    server.new_input.notify = new_input_notify;

    clock_gettime(CLOCK_MONOTONIC, &server.last_frame);

    if (!wlr_backend_start(backend))
    {
        wlr_log(WLR_ERROR, "Failed to start backend");
        wlr_backend_destroy(backend);
        exit(1);
    }
    wl_display_run(server.wl_display);
    wl_display_destroy(server.wl_display);
}