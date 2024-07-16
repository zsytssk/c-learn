#include <wayland-server.h>
#include <stdio.h>

// 全局对象注册处理
static void registry_handle_global(void *data, struct wl_registry *registry, uint32_t name,
                                   const char *interface, uint32_t version)
{
    printf("Global object registered: name=%d, interface=%s, version=%d\n", name, interface, version);
}

// 客户端连接处理
static void handle_new_client(void *data, struct wl_client *client)
{
    printf("New client connected: id=%d\n", client->id);

    // 创建注册表对象
    struct wl_registry *registry = wl_display_get_registry(client->display);
    if (registry)
    {
        // 添加全局对象注册监听器
        wl_registry_add_listener(registry, &registry_listener, NULL);
    }
}

// 注册表监听器
static const struct wl_registry_listener registry_listener = {
    registry_handle_global,
    NULL};

int main()
{
    // 创建 Wayland 显示服务器
    struct wl_display *display = wl_display_create();
    if (!display)
    {
        fprintf(stderr, "Failed to create Wayland display\n");
        return 1;
    }

    // 设置客户端连接事件监听器
    wl_display_add_new_client_listener(display, handle_new_client, NULL);

    // 运行 Wayland 事件循环
    while (wl_display_dispatch(display) != -1)
    {
        // 处理事件
    }

    // 销毁 Wayland 显示服务器
    wl_display_destroy(display);

    return 0;
}
