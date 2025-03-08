#!/bin/bash

# while true; do
#     # 检查 dwm 是否运行
#     if ! pgrep -x "main" > /dev/null; then
#         echo "main 正在启动..."
#         gcc ./src/main.c -o ./bin/main $(pkg-config --cflags --libs cairo x11 gtk4 glib-2.0)
#         ./bin/main
#     fi
# done

while true; do
    # 检查 dwm 是否运行
    if ! pgrep -x "main" > /dev/null; then
        echo "main 正在启动..."

        # 直接在 if 语句中检查 gcc 成功与否
        gcc ./src/main.c -o ./bin/main $(pkg-config --cflags --libs cairo x11 gtk4 glib-2.0)

        if [ $? -ne 0 ]; then
            echo -e "\033[1;31m编译失败～～～\033[0m"
            exit 1
        fi

        # 运行程序
        ./bin/main
         if [ $? -ne 0 ]; then
            echo -e "\033[1;31m执行失败～～～\033[0m"
            exit 1
        fi
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
#     if ! pgrep -x "main" > /dev/null; then
#         echo "main 正在重启..."
#         # DISPLAY=:1 gcc ./src/main.c -o ./bin/main -I/usr/include/cairo  -lcairo -lX11
#         DISPLAY=:1 gcc ./src/main.c -o ./bin/main $(pkg-config --cflags --libs cairo x11)
#         DISPLAY=:1 ./bin/main &
#     fi
#     sleep 1
# done

# 当 Xephyr 关闭后，脚本退出
# echo "Xephyr 已关闭，脚本退出。"