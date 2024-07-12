#include <stdio.h>
#include <stdlib.h>
#include <wayland-client.h>

struct wl_display *display;
struct wl_compositor *compositor;
struct wl_surface *surface;
struct wl_region *region;
struct wl_registry *registry;

// 回调函数，当全局对象被添加时调用
void registry_handle_global(void *data, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version)
{
    if (strcmp(interface, wl_compositor_interface.name) == 0)
    {
        compositor = wl_registry_bind(registry, id, &wl_compositor_interface, 1);
    }
}

// 回调函数，当全局对象被移除时调用
void registry_handle_global_remove(void *data, struct wl_registry *registry, uint32_t id)
{
    // 通常不需要处理
}

// 注册表监听器
static const struct wl_registry_listener registry_listener = {
    registry_handle_global,
    registry_handle_global_remove};

int main(int argc, char **argv)
{
    // 连接到 Wayland 显示服务器
    display = wl_display_connect(NULL);
    if (display == NULL)
    {
        fprintf(stderr, "Failed to connect to the display\n");
        return -1;
    }

    // 获取注册表
    registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, NULL);

    // 处理挂起的事件，直到注册表事件被处理完
    wl_display_roundtrip(display);

    // 检查是否成功获取 compositor
    if (compositor == NULL)
    {
        fprintf(stderr, "Failed to get compositor\n");
        wl_display_disconnect(display);
        return -1;
    }

    // 创建表面
    surface = wl_compositor_create_surface(compositor);

    // 创建区域
    region = wl_compositor_create_region(compositor);

    // 定义区域
    wl_region_add(region, 10, 10, 100, 100); // 添加一个矩形区域 (10, 10) 到 (110, 110)
    wl_region_add(region, 50, 50, 50, 50);   // 添加一个重叠的矩形区域 (50, 50) 到 (100, 100)

    // 应用区域到表面
    wl_surface_set_input_region(surface, region);

    // 提交表面
    wl_surface_commit(surface);

    // 清理并断开连接
    wl_region_destroy(region);
    wl_surface_destroy(surface);
    wl_display_disconnect(display);

    printf("Region defined and applied successfully\n");
    return 0;
}
