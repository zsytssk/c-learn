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

// cursor
#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_pointer.h>
#include <wlr/types/wlr_xcursor_manager.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/types/wlr_scene.h>

// @TODO-tab 切换鼠标状态

// @TODO-tab
static char *status[] = {"default", "pointer", "text", "move", "wait", "crosshair"};
struct local_server
{
    struct wl_display *wl_display;
    struct wlr_backend *backend;
    struct wl_listener new_output;
    struct wl_listener new_input;
    struct wlr_renderer *renderer;
    struct wlr_allocator *allocator;

    struct wlr_cursor *cursor;
    struct wlr_xcursor_manager *cursor_mgr;
    struct wl_listener cursor_motion;
    struct wl_listener cursor_motion_absolute;

    struct wlr_seat *seat;
    struct wl_listener request_cursor;
    struct wlr_output_layout *output_layout;

    struct wlr_scene *scene;
    struct wlr_scene_output_layout *scene_layout;

    // @TODO-tab
    char *cursor_status;
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
    struct sample_output *output = wl_container_of(listener, output, frame);
    struct wlr_scene *scene = output->sample->scene;

    struct wlr_scene_output *scene_output = wlr_scene_get_scene_output(
        scene, output->output);

    /* Render the scene if needed and commit the output */
    wlr_scene_output_commit(scene_output, NULL);

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    wlr_scene_output_send_frame_done(scene_output, &now);
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

    struct wlr_output_layout_output *l_output = wlr_output_layout_add_auto(sample->output_layout, output);
    struct wlr_scene_output *scene_output = wlr_scene_output_create(sample->scene, output);
    wlr_scene_output_layout_add_output(sample->scene_layout, l_output, scene_output);
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
    wlr_log(WLR_ERROR, "keyboard_key_notify:> code=%d, state=%d", event->keycode, event->state);
    if (event->state == WL_KEYBOARD_KEY_STATE_RELEASED)
    {
        return;
    }
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
        // tab 切换按钮状态
        if (sym == XKB_KEY_Tab)
        {
            size_t index = 0;
            size_t len = sizeof(status) / sizeof(status[0]);
            // @TODO-tab
            char *cur = sample->cursor_status;
            while (cur != NULL)
            {
                if (status[index] == cur || index == len)
                {
                    break;
                }
                index++;
            }

            int next_index = (index + 1) % len;
            wlr_cursor_set_xcursor(sample->cursor, sample->cursor_mgr, status[next_index]);
            sample->cursor_status = status[next_index];
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

    case WLR_INPUT_DEVICE_KEYBOARD:
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
    case WLR_INPUT_DEVICE_POINTER:
        wlr_log(WLR_ERROR, "WLR_INPUT_DEVICE_POINTER");
        wlr_cursor_attach_input_device(sample->cursor, device);
        break;
    default:
        break;
    }

    uint32_t caps = WL_SEAT_CAPABILITY_POINTER;
    caps |= WL_SEAT_CAPABILITY_KEYBOARD;
    wlr_seat_set_capabilities(sample->seat, caps);
}

static void server_cursor_motion_absolute(struct wl_listener *listener, void *data)
{
    struct local_server *server =
        wl_container_of(listener, server, cursor_motion_absolute);
    struct wlr_pointer_motion_absolute_event *event = data;
    wlr_cursor_warp_absolute(server->cursor, &event->pointer->base, event->x, event->y);
    wlr_cursor_set_xcursor(server->cursor, server->cursor_mgr, "default");
}

int main(void)
{
    wlr_log_init(WLR_ERROR, NULL);
    struct wl_display *wl_display = wl_display_create();
    struct local_server server = {.wl_display = wl_display};

    server.backend = wlr_backend_autocreate(wl_display_get_event_loop(wl_display), NULL);
    if (!server.backend)
    {
        wlr_log(WLR_ERROR, "failed to create wlr_backend");
        exit(1);
    }

    server.renderer = wlr_renderer_autocreate(server.backend);
    server.allocator = wlr_allocator_autocreate(server.backend, server.renderer);
    server.output_layout = wlr_output_layout_create(server.wl_display);

    server.scene = wlr_scene_create();
    server.scene_layout = wlr_scene_attach_output_layout(server.scene, server.output_layout);

    server.cursor = wlr_cursor_create();
    wlr_cursor_attach_output_layout(server.cursor, server.output_layout);

    server.cursor_mgr = wlr_xcursor_manager_create(NULL, 24);
    server.cursor_motion_absolute.notify = server_cursor_motion_absolute;
    wl_signal_add(&server.cursor->events.motion_absolute,
                  &server.cursor_motion_absolute);

    server.seat = wlr_seat_create(server.wl_display, "seat0");

    wl_signal_add(&server.backend->events.new_output, &server.new_output);
    server.new_output.notify = new_output_notify;
    wl_signal_add(&server.backend->events.new_input, &server.new_input);
    server.new_input.notify = new_input_notify;

    if (!wlr_backend_start(server.backend))
    {
        wlr_log(WLR_ERROR, "Failed to start backend");
        wlr_backend_destroy(server.backend);
        exit(1);
    }
    wl_display_run(server.wl_display);

    wlr_scene_node_destroy(&server.scene->tree.node);
    wlr_xcursor_manager_destroy(server.cursor_mgr);
    wlr_renderer_destroy(server.renderer);
    wlr_backend_destroy(server.backend);
    wlr_cursor_destroy(server.cursor);
    wl_display_destroy(server.wl_display);
}
