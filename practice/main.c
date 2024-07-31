#include <stdlib.h>
#include <unistd.h>
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
#include <wlr/types/wlr_xdg_shell.h>
#include <wlr/util/log.h>

enum local_cursor_mode {
  LOCAL_CURSOR_PASSTHROUGH,
  LOCAL_CURSOR_MOVE,
  LOCAL_CURSOR_RESIZE,
};

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
  struct wl_listener cursor_button;
  // @TODO-cursor_status
  char *cursor_status;

  struct wlr_output_layout *output_layout;
  struct wl_list outputs;
  struct wl_listener new_output;
  struct wl_listener new_input;

  struct wlr_xdg_shell *xdg_shell;
  struct wl_listener new_xdg_toplevel;
  struct wl_listener new_xdg_popup;
  struct wl_list toplevels;

  enum local_cursor_mode cursor_mode;
  struct local_toplevel *grab_toplevel;
  double grab_x, grab_y;
  struct wlr_box grab_geobox;
  uint32_t resize_edges;
};

struct local_output {
  struct local_server *server;
  struct wlr_output *output;
  struct wl_listener frame;
  struct wl_listener destroy;
  struct wl_listener request_state;
};

struct local_toplevel {
  struct wl_list link;
  struct local_server *server;
  struct wlr_xdg_toplevel *xdg_toplevel;
  struct wlr_scene_tree *scene_tree;
  struct wl_listener map;
  struct wl_listener unmap;
  struct wl_listener commit;
  struct wl_listener destroy;
  struct wl_listener request_move;
  struct wl_listener request_resize;
  struct wl_listener request_maximize;
  struct wl_listener request_fullscreen;
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
  // if (modifiers & (WLR_MODIFIER_LOGO | WLR_MODIFIER_ALT | WLR_MODIFIER_CTRL |
  //                  WLR_MODIFIER_SHIFT)) {
  //   wlr_log(WLR_ERROR, "keyboard_key_notify:> win+alt+ctrl+shift is
  //   pressed");
  // }
  int nsyms =
      xkb_state_key_get_syms(keyboard->wlr_keyboard->xkb_state, keycode, &syms);
  wlr_log(WLR_ERROR, "keyboard_key_notify:>2 %d", nsyms);
  for (int i = 0; i < nsyms; i++) {
    xkb_keysym_t sym = syms[i];
    if (sym == XKB_KEY_Escape) {
      wl_display_terminate(server->display);
      continue;
    }
    if (sym == XKB_KEY_T && (modifiers & WLR_MODIFIER_ALT)) {
      if (fork() == 0) {
        execl("/bin/sh", "/bin/sh", "-c", "alacritty", (void *)NULL);
      }
      continue;
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

void process_cursor_motion(struct local_server *server, uint32_t time) {
  if (server->cursor_mode == LOCAL_CURSOR_MOVE) {
    struct local_toplevel *toplevel = server->grab_toplevel;
    wlr_scene_node_set_position(&toplevel->scene_tree->node,
                                server->cursor->x - server->grab_x,
                                server->cursor->y - server->grab_y);
    return;
  }
}

static void reset_cursor_mode(struct local_server *server) {
  server->cursor_mode = LOCAL_CURSOR_PASSTHROUGH;
  server->grab_toplevel = NULL;
}

static void server_cursor_motion_absolute(struct wl_listener *listener,
                                          void *data) {
  // wlr_log(WLR_ERROR, "server_cursor_motion_absolute");
  struct local_server *server =
      wl_container_of(listener, server, cursor_motion_absolute);
  struct wlr_pointer_motion_absolute_event *event = data;
  wlr_cursor_warp_absolute(server->cursor, &event->pointer->base, event->x,
                           event->y);
  wlr_cursor_set_xcursor(server->cursor, server->cursor_mgr, "default");
  process_cursor_motion(server, event->time_msec);
}

static void server_cursor_button(struct wl_listener *listener, void *data) {
  struct local_server *server =
      wl_container_of(listener, server, cursor_button);

  struct wlr_pointer_button_event *event = data;
  wlr_seat_pointer_notify_button(server->seat, event->time_msec, event->button,
                                 event->state);
  wlr_log(WLR_ERROR, "server_cursor_button");

  // double sx, sy;
  // struct wlr_surface *surface = NULL;
  // struct local_toplevel *toplvel = desktop_toplevel_at();

  if (event->state = WL_POINTER_BUTTON_STATE_RELEASED) {
    reset_cursor_mode(server);
  } else {
    // focus_toplevel(toplevel, surface);
  }
}

void xdg_toplevel_map(struct wl_listener *listener, void *data) {
  wlr_log(WLR_ERROR, "xdg_toplevel_map");
  struct local_toplevel *toplevel = wl_container_of(listener, toplevel, map);
  wl_list_insert(&toplevel->server->toplevels, &toplevel->link);
  // focus_toplevel(toplevel, toplevel->xdg_toplevel->base->surface);
}
void xdg_toplevel_unmap(struct wl_listener *listener, void *data) {
  wlr_log(WLR_ERROR, "xdg_toplevel_unmap");
  struct local_toplevel *toplevel = wl_container_of(listener, toplevel, unmap);
  wl_list_remove(&toplevel->link);
}
void xdg_toplevel_commit(struct wl_listener *listener, void *data) {
  wlr_log(WLR_ERROR, "xdg_toplevel_commit");
  struct local_toplevel *toplevel = wl_container_of(listener, toplevel, commit);

  if (toplevel->xdg_toplevel->base->initial_commit) {
    wlr_xdg_toplevel_set_size(toplevel->xdg_toplevel, 0, 0);
  }
}
void xdg_toplevel_destroy(struct wl_listener *listener, void *data) {
  wlr_log(WLR_ERROR, "xdg_toplevel_destroy");
  struct local_toplevel *toplevel =
      wl_container_of(listener, toplevel, destroy);

  wl_list_remove(&toplevel->map.link);
  wl_list_remove(&toplevel->unmap.link);
  wl_list_remove(&toplevel->commit.link);
  wl_list_remove(&toplevel->destroy.link);
  free(toplevel);
}

void begin_interactive(struct local_toplevel *toplevel,
                       enum local_cursor_mode cursor_mode, uint32_t edges) {
  struct local_server *server = toplevel->server;
  struct wlr_surface *focus_surface =
      server->seat->pointer_state.focused_surface;

  if (toplevel->xdg_toplevel->base->surface !=
      wlr_surface_get_root_surface(focus_surface)) {
    return;
  }

  wlr_log(WLR_ERROR, "begin_interactive");
  server->cursor_mode = cursor_mode;
  server->grab_toplevel = toplevel;

  if (cursor_mode = LOCAL_CURSOR_MOVE) {
    server->grab_x = server->cursor->x - toplevel->scene_tree->node.x;
    server->grab_y = server->cursor->y - toplevel->scene_tree->node.y;
  }
}

void xdg_toplevel_move(struct wl_listener *listener, void *data) {
  wlr_log(WLR_ERROR, "xdg_toplevel_move");
  struct local_toplevel *toplevel = wl_container_of(listener, toplevel, unmap);
  begin_interactive(toplevel, LOCAL_CURSOR_MOVE, 0);
}
void xdg_toplevel_resize(struct wl_listener *listener, void *data) {
  wlr_log(WLR_ERROR, "xdg_toplevel_resize");
  struct wlr_xdg_toplevel_resize_event *event = data;
  struct local_toplevel *toplevel = wl_container_of(listener, toplevel, unmap);
  begin_interactive(toplevel, LOCAL_CURSOR_RESIZE, event->edges);
}

void server_new_xdg_toplevel(struct wl_listener *listener, void *data) {
  struct local_server *server =
      wl_container_of(listener, server, new_xdg_toplevel);
  struct wlr_xdg_toplevel *xdg_toplevel = data;

  struct local_toplevel *toplevel = calloc(1, sizeof(*toplevel));
  toplevel->server = server;
  toplevel->xdg_toplevel = xdg_toplevel;
  toplevel->scene_tree = wlr_scene_xdg_surface_create(
      &toplevel->server->scene->tree, xdg_toplevel->base);
  toplevel->scene_tree->node.data = toplevel;
  xdg_toplevel->base->data = toplevel->scene_tree;

  toplevel->map.notify = xdg_toplevel_map;
  wl_signal_add(&xdg_toplevel->base->surface->events.map, &toplevel->map);
  toplevel->unmap.notify = xdg_toplevel_unmap;
  wl_signal_add(&xdg_toplevel->base->surface->events.unmap, &toplevel->unmap);
  toplevel->commit.notify = xdg_toplevel_commit;
  wl_signal_add(&xdg_toplevel->base->surface->events.commit, &toplevel->commit);
  toplevel->destroy.notify = xdg_toplevel_destroy;
  wl_signal_add(&xdg_toplevel->events.destroy, &toplevel->destroy);

  toplevel->request_move.notify = xdg_toplevel_move;
  wl_signal_add(&xdg_toplevel->events.destroy, &toplevel->request_move);
  toplevel->request_resize.notify = xdg_toplevel_resize;
  wl_signal_add(&xdg_toplevel->events.destroy, &toplevel->request_resize);

  wlr_log(WLR_ERROR, "server_new_xdg_toplevel:> %d", sizeof(*toplevel));
}

void server_new_xdg_popup(struct wl_listener *listener, void *data) {
  wlr_log(WLR_ERROR, "server_new_xdg_popup");
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

  wlr_compositor_create(server.display, 5, server.renderer);
  wlr_subcompositor_create(server.display);
  wlr_data_device_manager_create(server.display);

  server.output_layout = wlr_output_layout_create(server.display);
  wl_list_init(&server.outputs);

  server.scene = wlr_scene_create();
  server.scene_layout =
      wlr_scene_attach_output_layout(server.scene, server.output_layout);

  wl_list_init(&server.toplevels);
  server.xdg_shell = wlr_xdg_shell_create(server.display, 3);
  server.new_xdg_toplevel.notify = server_new_xdg_toplevel;
  wl_signal_add(&server.xdg_shell->events.new_toplevel,
                &server.new_xdg_toplevel);
  server.new_xdg_popup.notify = server_new_xdg_popup;
  wl_signal_add(&server.xdg_shell->events.new_popup, &server.new_xdg_popup);

  server.cursor = wlr_cursor_create();
  wlr_cursor_attach_output_layout(server.cursor, server.output_layout);
  server.cursor_mgr = wlr_xcursor_manager_create(NULL, 24);
  server.cursor_motion_absolute.notify = server_cursor_motion_absolute;
  wl_signal_add(&server.cursor->events.motion_absolute,
                &server.cursor_motion_absolute);
  server.cursor_button.notify = server_cursor_button;
  wl_signal_add(&server.cursor->events.button, &server.cursor_button);

  server.new_output.notify = server_new_output;
  wl_signal_add(&server.backend->events.new_output, &server.new_output);
  server.new_input.notify = server_new_input;
  wl_signal_add(&server.backend->events.new_input, &server.new_input);
  server.seat = wlr_seat_create(server.display, "seat0");

  if (!wlr_backend_start(server.backend)) {
    wlr_log(WLR_ERROR, "Failed to start backend");
    wlr_backend_destroy(server.backend);
    wl_display_destroy(server.display);
    return 1;
  }
  const char *socket = wl_display_add_socket_auto(server.display);
  if (!socket) {
    wlr_backend_destroy(server.backend);
    return 1;
  }
  setenv("WAYLAND_DISPLAY", socket, true);
  wlr_log(WLR_INFO, "Running Wayland compositor on WAYLAND_DISPLAY=%s", socket);
  wl_display_run(server.display);

  wl_display_destroy_clients(server.display);
  wlr_scene_node_destroy(&server.scene->tree.node);
  wlr_xcursor_manager_destroy(server.cursor_mgr);
  wlr_cursor_destroy(server.cursor);
  wlr_allocator_destroy(server.allocator);
  wlr_renderer_destroy(server.renderer);
  wlr_backend_destroy(server.backend);
  wl_display_destroy(server.display);
  return 0;
}