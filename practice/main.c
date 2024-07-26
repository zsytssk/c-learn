#include <stdlib.h>
#include <wlr/backend.h>
#include <wlr/render/allocator.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_output_layout.h>
#include <wlr/types/wlr_scene.h>
#include <wlr/types/wlr_subcompositor.h>
#include <wlr/types/wlr_xcursor_manager.h>
#include <wlr/util/log.h>

// @TODO-cursor_status
static char *cursor_status[] = {"default", "pointer", "text",
                                "move",    "wait",    "crosshair"};
struct local_server {
  struct wl_display *display;
  struct wlr_backend *backend;
  struct wlr_renderer *renderer;
  struct wlr_allocator *allocator;

  struct wlr_scene *scene;
  struct wlr_scene_output_layout *scene_layout;

  struct wlr_seat *seat;
  struct wlr_cursor *cursor;
  struct wlr_xcursor_manager *cursor_mgr;
  struct wl_listener cursor_motion;
  struct wl_listener cursor_motion_absolute;
  // @TODO-cursor_status
  char *cursor_status;

  struct wlr_output_layout *output_layout;
  struct wl_list outputs;
  struct wl_listener new_output;
  struct wl_listener new_input;
};

struct local_output {
  struct local_server *server;
  struct wlr_output *output;
  struct wl_listener frame;
  struct wl_listener destroy;
  struct wl_listener request_state;
};

struct local_keyboard {
  struct local_server *server;
  struct wlr_keyboard *wlr_keyboard;
  struct wl_listener key;
  struct wl_listener destroy;
};

static void output_frame_notify(struct wl_listener *listener, void *data) {
  struct local_output *output = wl_container_of(listener, output, frame);
  struct wlr_scene *scene = output->server->scene;

  struct wlr_scene_output *scene_output =
      wlr_scene_get_scene_output(scene, output->output);

  /* Render the scene if needed and commit the output */
  wlr_scene_output_commit(scene_output, NULL);

  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  wlr_scene_output_send_frame_done(scene_output, &now);
}
void output_destroy_notify(struct wl_listener *listener, void *data) {
  wlr_log(WLR_ERROR, "output_destroy_notify");
  struct local_output *local_output =
      wl_container_of(listener, local_output, destroy);
  wl_list_remove(&local_output->frame.link);
  wl_list_remove(&local_output->destroy.link);
  wl_list_remove(&local_output->request_state.link);
  free(local_output);
}
void output_request_state_notify(struct wl_listener *listener, void *data) {
  wlr_log(WLR_ERROR, "output_request_state_notify");
  struct local_output *local_output =
      wl_container_of(listener, local_output, request_state);
  const struct wlr_output_event_request_state *event = data;
  wlr_output_commit_state(local_output->output, event->state);
}

void server_new_output(struct wl_listener *listener, void *data) {
  struct wlr_output *output = data;
  struct local_server *server = wl_container_of(listener, server, new_output);
  wlr_output_init_render(output, server->allocator, server->renderer);

  struct local_output *local_output = calloc(1, sizeof(*local_output));
  local_output->output = output;
  local_output->server = server;

  wlr_log(WLR_ERROR, "server_new_output: %s", output->name);

  local_output->frame.notify = output_frame_notify;
  wl_signal_add(&output->events.frame, &local_output->frame);
  local_output->destroy.notify = output_destroy_notify;
  wl_signal_add(&output->events.destroy, &local_output->destroy);
  local_output->request_state.notify = output_request_state_notify;
  wl_signal_add(&output->events.request_state, &local_output->request_state);

  struct wlr_output_state state;
  wlr_output_state_init(&state);
  wlr_output_state_set_enabled(&state, true);
  struct wlr_output_mode *mode = wlr_output_preferred_mode(output);
  if (mode != NULL) {
    wlr_output_state_set_mode(&state, mode);
  }
  wlr_output_commit_state(output, &state);
  wlr_output_state_finish(&state);

  struct wlr_output_layout_output *layout_output =
      wlr_output_layout_add_auto(server->output_layout, output);
  struct wlr_scene_output *scene_output =
      wlr_scene_output_create(server->scene, output);
  wlr_scene_output_layout_add_output(server->scene_layout, layout_output,
                                     scene_output);
}

void keyboard_destroy_notify(struct wl_listener *listener, void *data) {
  wlr_log(WLR_ERROR, "keyboard_destroy_notify");
  struct local_keyboard *keyboard =
      wl_container_of(listener, keyboard, destroy);
  wl_list_remove(&keyboard->destroy.link);
  wl_list_remove(&keyboard->key.link);
  free(keyboard);
}
void keyboard_key_notify(struct wl_listener *listener, void *data) {
  struct local_keyboard *keyboard = wl_container_of(listener, keyboard, key);
  struct local_server *server = keyboard->server;
  struct wlr_keyboard_key_event *event = data;

  uint32_t keycode = event->keycode + 8;
  const xkb_keysym_t *syms;
  uint32_t modifiers = wlr_keyboard_get_modifiers(keyboard->wlr_keyboard);
  if (modifiers & (WLR_MODIFIER_LOGO | WLR_MODIFIER_ALT | WLR_MODIFIER_CTRL |
                   WLR_MODIFIER_SHIFT)) {
    wlr_log(WLR_ERROR, "keyboard_key_notify:> win+alt+ctrl+shift is pressed");
  }
  int nsyms =
      xkb_state_key_get_syms(keyboard->wlr_keyboard->xkb_state, keycode, &syms);
  wlr_log(WLR_ERROR, "keyboard_key_notify:>2 %d", nsyms);
  for (int i = 0; i < nsyms; i++) {
    xkb_keysym_t sym = syms[i];
    if (sym == XKB_KEY_Escape) {
      wl_display_terminate(server->display);
    }
    if (sym == XKB_KEY_Tab) {
      if (event->state == WL_KEYBOARD_KEY_STATE_RELEASED) {
        return;
      }
      size_t index = 0;
      size_t len = sizeof(cursor_status) / sizeof(cursor_status[0]);
      // @TODO-cursor_status
      char *cur = server->cursor_status;
      while (cur != NULL) {
        if (cursor_status[index] == cur || index == len) {
          break;
        }
        index++;
      }

      int next_index = (index + 1) % len;
      wlr_cursor_set_xcursor(server->cursor, server->cursor_mgr,
                             cursor_status[next_index]);
      server->cursor_status = cursor_status[next_index];
    }
  }
}

void server_new_input(struct wl_listener *listener, void *data) {
  struct wlr_input_device *device = data;
  struct local_server *server = wl_container_of(listener, server, new_input);

  wlr_log(WLR_ERROR, "server_new_input: %d", device->type);
  switch (device->type) {
    case WLR_INPUT_DEVICE_KEYBOARD:
      struct local_keyboard *keyboard = calloc(1, sizeof(*keyboard));
      keyboard->wlr_keyboard = wlr_keyboard_from_input_device(device);
      keyboard->server = server;
      wl_signal_add(&device->events.destroy, &keyboard->destroy);
      keyboard->destroy.notify = keyboard_destroy_notify;
      wl_signal_add(&keyboard->wlr_keyboard->events.key, &keyboard->key);
      keyboard->key.notify = keyboard_key_notify;
      struct xkb_context *context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
      if (!context) {
        wlr_log(WLR_ERROR, "Failed to create XKB context");
        exit(1);
      }
      struct xkb_keymap *keymap =
          xkb_keymap_new_from_names(context, NULL, XKB_KEYMAP_COMPILE_NO_FLAGS);
      if (!keymap) {
        wlr_log(WLR_ERROR, "Failed to create XKB keymap");
        exit(1);
      }
      wlr_keyboard_set_keymap(keyboard->wlr_keyboard, keymap);
      xkb_keymap_unref(keymap);
      xkb_context_unref(context);
      break;
    case WLR_INPUT_DEVICE_POINTER:
      wlr_log(WLR_ERROR, "WLR_INPUT_DEVICE_POINTER");
      wlr_cursor_attach_input_device(server->cursor, device);
      break;
    default:
      break;
  }
}

void server_cursor_motion_absolute(struct wl_listener *listener, void *data) {
  // wlr_log(WLR_ERROR, "server_cursor_motion_absolute");
  struct local_server *server =
      wl_container_of(listener, server, cursor_motion_absolute);
  struct wlr_pointer_motion_absolute_event *event = data;
  wlr_cursor_warp_absolute(server->cursor, &event->pointer->base, event->x,
                           event->y);
  wlr_cursor_set_xcursor(server->cursor, server->cursor_mgr, "default");
}

int main(void) {
  wlr_log_init(WLR_DEBUG, NULL);
  struct local_server server = {0};
  server.display = wl_display_create();
  server.backend =
      wlr_backend_autocreate(wl_display_get_event_loop(server.display), NULL);

  if (server.backend == NULL) {
    wlr_log(WLR_ERROR, "failed to create wlr_backend");
    return 1;
  }

  server.renderer = wlr_renderer_autocreate(server.backend);
  if (server.renderer == NULL) {
    wlr_log(WLR_ERROR, "failed to create wlr_renderer");
    return 1;
  }
  wlr_renderer_init_wl_display(server.renderer, server.display);

  server.allocator = wlr_allocator_autocreate(server.backend, server.renderer);
  if (server.allocator == NULL) {
    wlr_log(WLR_ERROR, "failed to create wlr_allocator");
    return 1;
  }
  server.output_layout = wlr_output_layout_create(server.display);
  wl_list_init(&server.outputs);

  server.scene = wlr_scene_create();
  server.scene_layout =
      wlr_scene_attach_output_layout(server.scene, server.output_layout);

  server.cursor = wlr_cursor_create();
  wlr_cursor_attach_output_layout(server.cursor, server.output_layout);
  server.cursor_mgr = wlr_xcursor_manager_create(NULL, 24);
  server.cursor_motion_absolute.notify = server_cursor_motion_absolute;
  wl_signal_add(&server.cursor->events.motion_absolute,
                &server.cursor_motion_absolute);

  server.new_output.notify = server_new_output;
  wl_signal_add(&server.backend->events.new_output, &server.new_output);
  server.new_input.notify = server_new_input;
  wl_signal_add(&server.backend->events.new_input, &server.new_input);

  if (!wlr_backend_start(server.backend)) {
    wlr_log(WLR_ERROR, "Failed to start backend");
    wlr_backend_destroy(server.backend);
    return 1;
  }
  wl_display_run(server.display);
  wlr_backend_destroy(server.backend);
  wl_display_destroy(server.display);
  return 0;
}