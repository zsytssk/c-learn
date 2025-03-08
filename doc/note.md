https://way-cooler.org/book/basic_output_cursor.html
https://wayland.freedesktop.org/docs/html/ch04.html

- 学习 + 练习

## 2025-03-05 11:15:03

https://zetcode.com/gfx/cairo/cairobackends/
https://www.cairographics.org/tutorial/

## 2025-03-05 10:08:23

- @ques Cairo demo

- @ques

  - c 传递参数后就会变成指针吗？ -> 函数的确是的， 数据类型呢？

- @练习
  - `void *` -> 转换类型
  - switch union -> ..

```c
void *callFn(void *(*fn)())
{
    fn();
}

char *test()
{
    return "this is a test";
}

int main()
{
    char *a = callFn((void *(*)())test);
    printf("%s\n", a);
}
```

## 2025-03-03 13:06:57

- layout 第三种的名字显示不正确
  - 都找不出来这值在哪设置的。。。

```
如果我的layouts里面的item数目为3, 切换到第三个时，m->ltsymbol就会变成[1], 这是在哪设置的
```

```c
/* function implementations */
void applyrules(Client *c)
{
  const char *class, *instance;
  unsigned int i;
  const Rule *r;
  Monitor *m;
  XClassHint ch = {NULL, NULL};

  /* rule matching */
  c->isfloating = 0;
  c->tags = 0;
  XGetClassHint(dpy, c->win, &ch);
  class = ch.res_class ? ch.res_class : broken;
  instance = ch.res_name ? ch.res_name : broken;

  for (i = 0; i < LENGTH(rules); i++)
  {
    r = &rules[i];
    if ((!r->title || strstr(c->name, r->title)) &&
        (!r->class || strstr(class, r->class)) &&
        (!r->instance || strstr(instance, r->instance)))
    {
      c->isfloating = r->isfloating;
      c->tags |= r->tags;
      for (m = mons; m && m->num != r->monitor; m = m->next)
        ;
      if (m)
        c->mon = m;
    }
  }
  if (ch.res_class)
    XFree(ch.res_class);
  if (ch.res_name)
    XFree(ch.res_name);
  c->tags =
      c->tags & TAGMASK ? c->tags & TAGMASK : c->mon->tagset[c->mon->seltags];
}
```

## 2025-02-27 16:53:50

https://www.youtube.com/watch?v=arLQA52Ieik

```c
void togglefloating(const Arg *arg)
{
  if (!selmon->sel)
    return;
  if (selmon->sel->isfullscreen) /* no support for fullscreen windows */
    return;
  selmon->sel->isfloating = !selmon->sel->isfloating || selmon->sel->isfixed;
  if (selmon->sel->isfloating)
    resize(selmon->sel, selmon->sel->x, selmon->sel->y, selmon->sel->w,
           selmon->sel->h, 0);
  arrange(selmon);
}
```

```c
void spawn(const Arg *arg) {
  struct sigaction sa;

  // if (arg->v == dmenucmd)
  // 	dmenumon[0] = '0' + selmon->num;
  if (fork() == 0) {
    if (dpy) close(ConnectionNumber(dpy));
    setsid();

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = SIG_DFL;
    sigaction(SIGCHLD, &sa, NULL);

    execvp(((char **)arg->v)[0], (char **)arg->v);
    die("dwm: execvp '%s' failed:", ((char **)arg->v)[0]);
  }
}
```

```c

void setlayout(const Arg *arg) {
  if (!arg || !arg->v || arg->v != selmon->lt[selmon->sellt])
    selmon->sellt ^= 1;
  if (arg && arg->v) selmon->lt[selmon->sellt] = (Layout *)arg->v;
  strncpy(selmon->ltsymbol, selmon->lt[selmon->sellt]->symbol,
          sizeof selmon->ltsymbol);
  if (selmon->sel)
    arrange(selmon);
  else
    drawbar(selmon);
}
```

```c
static void (*handler[LASTEvent])(XEvent *) = {
    [ButtonPress] = buttonpress,
    [ClientMessage] = clientmessage,
    [ConfigureRequest] = configurerequest,
    [ConfigureNotify] = configurenotify,
    [DestroyNotify] = destroynotify,
    [EnterNotify] = enternotify,
    [Expose] = expose,
    [FocusIn] = focusin,
    [KeyPress] = keypress,
    [MappingNotify] = mappingnotify,
    [MapRequest] = maprequest,
    [MotionNotify] = motionnotify,
    [PropertyNotify] = propertynotify,
    [UnmapNotify] = unmapnotify};
```

## 2025-02-24 15:39:42

15.5 + 补贴 260
试用期 3 隔月 8 则 15500
入职 下个星期
体检

## 练习

- @save

  - `calloc(1, sizeof(*sample_output))`
  - `struct timespec now; clock_gettime(CLOCK_MONOTONIC, &now);`

- @ques c 语言中的宏 `wl_container_of`

## 2024-07-01 15:13:10

- @ques 为什么 sample 显示有问题

  - 因为没有合适的渲染

- @ques tinywl 中的大小自定义是如何做到的

  - 也许是一大堆代码综合作用的
  - `server.output_layout`

- @ques `events.motion` vs `events.motion_absolute`

- @ques `server.renderer` 这个 renderer 是干嘛的

  - 创建 `allocator` `compositor`
  - `wlr_renderer_init_wl_display` -> `wlr_renderer_init_wl_shm` ...

- @ques `wlroots` 调用都是 server 的接口, 他的全部功能是如何实现的?

  - 这应该太复杂了

- @ques `wl_compositor_interface`

- @ques `xprop for wayland`

- @ques `wl_display` 的具体内容不需要定义?

- @ques 我是不是需要先去学习下 wayland 的教程
  - 用对象组成结构, 联系成体系 -> 这比关注方法应该更有效

```
struct wlr_output_mode mode;
mode.width = new_width;     // 新宽度
mode.height = new_height;   // 新高度
mode.refresh = 60 * 1000;   // 刷新率 (单位是 Hz，这里设置为 60 Hz)

wlr_output_set_mode(output, &mode);
```

- @ques `xkb_keymap_unref(keymap);xkb_context_unref(context);`

  - 在使用之后这两个变量就可以回收了吗? 难道不需要将状态保存吗?

- @ques c 如何处理内存问题 有 zig 中的 allocator 吗?

- `cairo_surface_destroy(buffer->surface);`

  - buffer 是一个指向结构体的指针，则使用箭头运算符：buffer->surface。

- dynamic memory allocator + array 这个是不是就可以搞一个动态的数组了
  - https://www.learn-c.org/en/Arrays_and_Pointers

### end

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
