https://way-cooler.org/book/basic_output_cursor.html
https://wayland.freedesktop.org/docs/html/ch04.html

## 练习

- @save

  - `calloc(1, sizeof(*sample_output))`
  - `struct timespec now; clock_gettime(CLOCK_MONOTONIC, &now);`

- @ques c 语言中的宏 `wl_container_of`

## 2024-07-01 15:13:10

- @ques 如何显示 app | 点击关闭按钮关闭 app

- @ques 能不能直接使用 wlroots 源代码编译

  - 这样我就可以加 log 了
  - 自己去编译?

- @ques `wlroots` 调用都是 server 的接口, 他的全部功能是如何实现的?

  - 这应该太复杂了

- @ques `wl_compositor_interface`

- @ques `xprop for wayland`

- @ques `wl_display` 的具体内容不需要定义?

- @ques 我是不是需要先去学习下 wayland 的教程

  - 用对象组成结构, 联系成体系 -> 这比关注方法应该更有效

- @ques `xkb_keymap_unref(keymap);xkb_context_unref(context);`

  - 在使用之后这两个变量就可以回收了吗? 难道不需要将状态保存吗?

- @ques c 如何处理内存问题 有 zig 中的 allocator 吗?

- dynamic memory allocator + array 这个是不是就可以搞一个动态的数组了
  - https://www.learn-c.org/en/Arrays_and_Pointers

### end

- @ques 如何改变背景的颜色
- @ques `river` 中 zig 如何监听事件的

- @ques 默认的鼠标图片是如何设置的?

- @ques

  - wlr_scene_create -> scene 是干嘛的
  - 等等

```
struct wlr_output_mode mode;
mode.width = new_width;     // 新宽度
mode.height = new_height;   // 新高度
mode.refresh = 60 * 1000;   // 刷新率 (单位是 Hz，这里设置为 60 Hz)

wlr_output_set_mode(output, &mode);
```

- `cairo_surface_destroy(buffer->surface);`

  - buffer 是一个指向结构体的指针，则使用箭头运算符：buffer->surface。

- @ques 显示鼠标

  - 鼠标的样式没有设置 ?
  - 难道是必须要有 scene 设置吗?
  - ***
  - `wlr_output_layout_create` ?
  - 可能需要 `wlr_seat` `wlr_input_device` `wlr_pointer`
  - 鼠标的状态

- @ques `events.motion` vs `events.motion_absolute`

- @ques `server.renderer` 这个 renderer 是干嘛的

  - 创建 `allocator` `compositor`
  - `wlr_renderer_init_wl_display` -> `wlr_renderer_init_wl_shm` ...

- @ques tinywl 中的大小自定义是如何做到的

  - 也许是一大堆代码综合作用的
  - `server.output_layout`

- @ques 为什么 sample 显示有问题

  - 因为没有合适的渲染

- @ques c printf NULL
- @ques zig 中 linklist 如何实现, 他又么有泛型?
- @ques `wlroots` 设置窗口的 className, 这样就可以在其他的 tag 中打开, 不影响 code

- @ques `-Wall -Wextra`

- @rem `node_t **` 是什么意思

  - `node_t **`是 node_t 指针的指针? 这怎么理解?
    - 如果你在外面的函数中想改变`node_t *`的值, 你必须传过去`node_t **`

- @ques `int (*cmp)(const void *, const void *);`

- @ques `void (*pf[])(int) = {f1, f2, f3};`
- @ques c 怎么引用其他文件 `#include 'test.c'`

- @ques `struct point p = {x : 10, y : 10};` 这个怎么 print 出来
- `cairo.h` 里面为啥只有申明, 没有具体的实现
- @ques c 字符串拼接 -> strncat
- @ques c print char `%s`

- @ques ` char name[] = "zsy";` 怎么创建一个 pointer

- `unsigned int`
