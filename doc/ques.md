## output_request_state_notify 中更新 output 的大小, output_frame_notify 中绘制颜色方块会报错

```
00:00:04.586 [render/swapchain.c:98] No free output buffer slot
[1]    33798 segmentation fault (core dumped)  /home/zsy/Documents/zsytssk/github/c-learn/bin/wlr1
```

```c
void output_frame_notify(struct wl_listener *listener, void *data)
{
    // wlr_log(WLR_ERROR, "output_frame_notify");
    struct local_output *local_output = wl_container_of(listener, local_output, frame);
    struct local_server *server = local_output->server;
    struct wlr_output *output = local_output->output;

    struct wlr_output_state state;
    wlr_output_state_init(&state);
    struct wlr_render_pass *pass = wlr_output_begin_render_pass(output, &state, NULL, NULL);
    wlr_render_pass_add_rect(pass, &(struct wlr_render_rect_options){
                                       .box = {
                                           .width = output->width,
                                           .height = output->height,
                                       },
                                       .color = {
                                           .r = 1,
                                           .g = 0,
                                           .b = 1,
                                           .a = 0.2,
                                       }});
    wlr_render_pass_submit(pass);
    wlr_output_commit_state(output, &state);
    wlr_output_state_finish(&state);
}
```
