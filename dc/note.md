https://way-cooler.org/book/basic_output_cursor.html

## 练习

## 2024-07-01 15:13:10

- @ques

  - 理清 `sample.c`
  - 显示鼠标
  - 窗口打下
  - 打开窗口

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
