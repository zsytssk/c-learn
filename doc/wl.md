https://wayland.freedesktop.org/docs/html/apa.html

## wlroots

- output_layout

  - 输出布局管理器在 wlroots 库中是一个用于管理和组织多个输出设备（如显示器）相对布局的工具
  - 关联 `scene_layout` `cursor`

- scene
  - 场景, 就像游戏中的"场景"概念, 每一个元素都需要在上面显示...

## save

wl_display - 负责管理 Wayland 客户端与服务器之间的连接。它提供了创建、管理和销毁连接的功能。

wlr_renderer_autocreate - 是一个用于自动创建适当的 wlr_renderer 实例的函数
renderer - 在 Wayland 合成器和图形编程中起着非常关键的作用。它负责绘制和渲染图形内容到显示设备上

allocator = wlr_allocator_autocreate(backend, state.renderer);
在图形编程和合成器开发中，allocator 负责管理图形资源的内存分配。这包括帧缓冲区、纹理等资源的分配和释放

`wl_container_of(ptr, sample, member)`
可以通过 struct sample 下的成员 member 的地址(ptr)计算出 sample 的地址, 从而赋值给 sample (非常神奇)

`wlr_keyboard_set_keymap` 在 Wayland 环境中，键盘设备需要知道每个键对应的键码和符号，以便正确地传输用户输入信息给客户端

## other

`xkbcommon` 键盘映射的库

```c
struct xkb_context *context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
struct xkb_keymap *keymap = xkb_keymap_new_from_names(context, NULL,
          XKB_KEYMAP_COMPILE_NO_FLAGS);
wlr_keyboard_set_keymap(keyboard->wlr_keyboard, keymap);
xkb_keymap_unref(keymap);
xkb_context_unref(context);
```

## structure

- wl_display

  - 负责管理 Wayland 客户端与服务器之间的连接。它提供了创建、管理和销毁连接的功能。

- wlr_backend

  - 它提供了与显示服务器交互的接口，允许应用程序创建和管理输出设备（显示器）、输入设备（键盘、鼠标等）以及处理与用户界面相关的事件和操作。

- wlr_renderer

  - 通过 wlr_renderer，开发者可以绘制窗口、图形元素、光标等内容，并将它们显示到屏幕上。

- wlr_allocator

  - 内存管理, wlr_output_init_render 需要

- wlr_xdg_toplevel

  - 处理窗口的事件 -> 最大化 最小化...

- wlr_seat

  - 输入设备:鼠标键盘等

- wl_data <复制粘贴 | 拖拽> 数据
  - wl_data_device_manager | wl_data_source | wl_data_source.offer | wl_data_offer.offer
  - wl_data_offer.receive | wl_data_source.send
  - wl_data_device.start_drag
