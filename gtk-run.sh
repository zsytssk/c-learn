#!/bin/bash
while true; do
    # 检查 dwm 是否运行
    if ! pgrep -x "test" > /dev/null; then
        echo "test 正在重启..."
        gcc ./src/test.c -o ./bin/test $(pkg-config --cflags --libs gtk4 glib-2.0)
        ./bin/test
    fi
done

# # 定义嵌套窗口的分辨率（如 800x600）
# RESOLUTION="1400x900"

# # 启动 Xephyr 嵌套窗口
# Xephyr -br -ac -noreset -screen "$RESOLUTION" -resizeable :1 &
# sleep 1

# # 在 Xephyr 中启动 dwm，并持续监听 dwm 进程
# while pgrep -x "Xephyr" > /dev/null; do
#     # 检查 dwm 是否运行
#     if ! pgrep -x "test" > /dev/null; then
#         echo "test 正在重启..."
#         DISPLAY=:1 gcc ./src/test.c -o ./bin/test $(pkg-config --cflags --libs gtk4 glib-2.0)
#         # DISPLAY=:1 gcc ./src/test.c -o ./bin/test -I/usr/include/cairo -I/usr/include/glib-2.0 -I/usr/include/pango-1.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/gtk-4.0 -I/usr/include/graphene-1.0 -I/usr/include/gdk-pixbuf-2.0 -I/usr/lib/x86_64-linux-gnu/graphene-1.0/include -I/usr/include/harfbuzz $(pkg-config --cflags --libs gtk4 glib-2.0)
#         DISPLAY=:1 ./bin/test &
#     fi
#     sleep 1
# done

# # 当 Xephyr 关闭后，脚本退出
# echo "Xephyr 已关闭，脚本退出。"