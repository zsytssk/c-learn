#include <wlr/backend.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_output.h>
#include <wlr/util/log.h>
#include <wlr/render/wlr_render_pass.h>
#include <wlr/types/wlr_output_damage.h>

void render_frame(struct wlr_output *output)
{
    struct wlr_renderer *renderer = wlr_renderer_autocreate(output->backend);
    struct wlr_allocator *allocator = wlr_allocator_autocreate(output->backend, renderer);

    if (!renderer || !allocator)
    {
        wlr_log(WLR_ERROR, "Failed to get renderer or allocator");
        return;
    }

    struct wlr_output_damage *output_damage = wlr_output_damage_create(output);
    struct pixman_region32 damage;
    pixman_region32_init(&damage);

    if (!wlr_output_damage_attach_render(output_damage, &damage))
    {
        wlr_log(WLR_ERROR, "Failed to attach render");
        return;
    }

    struct wlr_render_pass *pass = wlr_output_begin_render_pass(output, &damage, NULL);
    if (pass == NULL)
    {
        wlr_log(WLR_ERROR, "Failed to begin render pass");
        return;
    }

    // Clear the frame buffer
    float color[4] = {0.1f, 0.2f, 0.3f, 1.0f};
    wlr_render_pass_add_rect(pass, &(struct wlr_render_rect_options){
                                       .box = {
                                           .x = 0,
                                           .y = 0,
                                           .width = output->width,
                                           .height = output->height,
                                       },
                                       .color = color,
                                   });

    // Commit the render pass
    wlr_render_pass_end(pass);

    // Commit the output frame
    wlr_output_commit(output);

    pixman_region32_fini(&damage);
}

int main()
{
    struct wl_display *display = wl_display_create();
    struct wlr_backend *backend = wlr_backend_autocreate(display);
    if (!backend)
    {
        wlr_log(WLR_ERROR, "Failed to create backend");
        return 1;
    }

    struct wlr_output *output; // 假设你已经有了输出设备
    render_frame(output);

    // 运行 Wayland 显示服务器事件循环
    wl_display_run(display);

    // 清理资源
    wlr_backend_destroy(backend);
    wl_display_destroy(display);

    return 0;
}
