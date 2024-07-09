#include <wlr/backend.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/render/allocator.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_output_layout.h>
#include <wlr/types/wlr_xdg_shell.h>
#include <wlr/util/log.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

struct my_state
{
    struct wl_display *wl_display;
    struct wlr_backend *backend;
    struct wlr_renderer *renderer;
    struct wlr_allocator *allocator;
    struct wlr_compositor *compositor;
    struct wlr_output_layout *output_layout;
    struct wlr_xdg_shell *xdg_shell;
};

static void handle_new_xdg_surface(struct wl_listener *listener, void *data)
{
    struct wlr_xdg_surface *xdg_surface = data;
    if (xdg_surface->role != WLR_XDG_SURFACE_ROLE_TOPLEVEL)
    {
        return;
    }
    wlr_log(WLR_DEBUG, "New xdg toplevel surface: title=%s", xdg_surface->toplevel->title);
}

int main(int argc, char *argv[])
{
    wlr_log_init(WLR_DEBUG, NULL);

    struct my_state state = {0};
    state.wl_display = wl_display_create();
    state.backend = wlr_backend_autocreate(wl_display_get_event_loop(state.wl_display), NULL);
    ;
    if (!state.backend)
    {
        wlr_log(WLR_ERROR, "Failed to create backend");
        return -1;
    }

    state.renderer = wlr_renderer_autocreate(state.backend);
    if (!state.renderer)
    {
        wlr_log(WLR_ERROR, "Failed to create renderer");
        wlr_backend_destroy(state.backend);
        return -1;
    }

    state.allocator = wlr_allocator_autocreate(state.backend, state.renderer);
    if (!state.allocator)
    {
        wlr_log(WLR_ERROR, "Failed to create allocator");
        wlr_renderer_destroy(state.renderer);
        wlr_backend_destroy(state.backend);
        return -1;
    }

    struct wl_event_loop *event_loop = wl_display_get_event_loop(state.wl_display);

    state.compositor = wlr_compositor_create(state.wl_display, 5, state.renderer);
    state.output_layout = wlr_output_layout_create(state.wl_display);
    state.xdg_shell = wlr_xdg_shell_create(state.wl_display, 3);

    struct wl_listener new_xdg_surface_listener;
    new_xdg_surface_listener.notify = handle_new_xdg_surface;
    wl_signal_add(&state.xdg_shell->events.new_surface, &new_xdg_surface_listener);

    const char *socket = wl_display_add_socket_auto(state.wl_display);
    if (!socket)
    {
        wlr_log(WLR_ERROR, "Failed to add socket");
        wl_display_destroy(state.wl_display);
        wlr_allocator_destroy(state.allocator);
        wlr_renderer_destroy(state.renderer);
        wlr_backend_destroy(state.backend);
        return -1;
    }

    setenv("WAYLAND_DISPLAY", socket, 1);

    if (fork() == 0)
    {
        // 子进程: 启动终端程序
        execl("/bin/sh", "/bin/sh", "-c", "firefox", (void *)NULL);

        _exit(1); // 如果 exec 失败，退出子进程
    }

    wlr_backend_start(state.backend);
    wlr_log(WLR_INFO, "Running Wayland compositor on WAYLAND_DISPLAY=%s", socket);

    wl_display_run(state.wl_display);

    wlr_log(WLR_INFO, "Exiting");

    wl_display_destroy(state.wl_display);
    wlr_allocator_destroy(state.allocator);
    wlr_renderer_destroy(state.renderer);
    wlr_backend_destroy(state.backend);

    return 0;
}
