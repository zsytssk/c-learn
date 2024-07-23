#include <stdlib.h>
#include <wlr/backend.h>
#include <wlr/util/log.h>
#include <wlr/render/allocator.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_subcompositor.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_output_layout.h>

struct local_server
{
    struct wl_display *display;
    struct wlr_backend *backend;
    struct wlr_renderer *renderer;
    struct wlr_allocator *allocator;

    struct wlr_output_layout *output_layout;
    struct wl_list outputs;
    struct wl_listener new_output;
};

void server_new_output(struct wl_listener *listener, void *data)
{
    struct local_server *server = wl_container_of(listener, server, new_output);
    struct wlr_output *output = data;
    wlr_log(WLR_ERROR, "server_new_output: %s", output->name);
}

int main(void)
{
    wlr_log_init(WLR_DEBUG, NULL);
    struct local_server server = {0};
    server.display = wl_display_create();
    server.backend = wlr_backend_autocreate(wl_display_get_event_loop(server.display), NULL);

    if (server.backend == NULL)
    {
        wlr_log(WLR_ERROR, "failed to create wlr_backend");
        return 1;
    }

    server.renderer = wlr_renderer_autocreate(server.backend);
    if (server.renderer == NULL)
    {
        wlr_log(WLR_ERROR, "failed to create wlr_renderer");
        return 1;
    }
    wlr_renderer_init_wl_display(server.renderer, server.display);

    server.allocator = wlr_allocator_autocreate(server.backend, server.renderer);
    if (server.allocator == NULL)
    {
        wlr_log(WLR_ERROR, "failed to create wlr_allocator");
        return 1;
    }
    wlr_compositor_create(server.display, 5, server.renderer);
    wlr_subcompositor_create(server.display);
    wlr_data_device_manager_create(server.display);

    server.output_layout = wlr_output_layout_create(server.display);
    wl_list_init(&server.outputs);
    server.new_output.notify = server_new_output;
    wl_signal_add(&server.backend->events.new_output, &server.new_output);

    if (!wlr_backend_start(server.backend))
    {
        wlr_log(WLR_ERROR, "Failed to start backend");
        wlr_backend_destroy(server.backend);
        return 1;
    }
    wl_display_run(server.display);

    wlr_backend_destroy(server.backend);
    wl_display_destroy(server.display);
    return 0;
}