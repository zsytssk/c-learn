#include <stdio.h>
#include <stddef.h>

struct my_struct
{
    int a;
    float b;
};

#define container_of(ptr, sample, member) \
    (__typeof__(sample))((char *)(ptr) -  \
                         offsetof(__typeof__(*sample), member))

int main()
{
    struct my_struct example;
    example.a = 10;
    example.b = 3.14;

    // 获取成员 b 的指针
    float *b_ptr = &example.b;

    // 使用 container_of 获取包含 b 成员的结构体指针
    struct my_struct *struct_ptr = container_of(b_ptr, struct_ptr, b);

    // 打印结果以验证
    printf("struct_ptr->a = %d\n", struct_ptr->a);
    printf("struct_ptr->b = %f\n", struct_ptr->b);

    return 0;
}