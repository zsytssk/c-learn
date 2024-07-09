wl_display - 负责管理 Wayland 客户端与服务器之间的连接。它提供了创建、管理和销毁连接的功能。

wlr_renderer_autocreate - 是一个用于自动创建适当的 wlr_renderer 实例的函数
renderer - 在 Wayland 合成器和图形编程中起着非常关键的作用。它负责绘制和渲染图形内容到显示设备上

allocator = wlr_allocator_autocreate(backend, state.renderer);
在图形编程和合成器开发中，allocator 负责管理图形资源的内存分配。这包括帧缓冲区、纹理等资源的分配和释放
