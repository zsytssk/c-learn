#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/X.h>
#include <X11/keysym.h>

#include "util.h"

typedef struct Monitor Monitor;

typedef struct
{
    const char *class;
    const char *instance;
    const char *title;
    unsigned int *tags;
    int infloating;
    int monitor;
} Rule;
static const Rule rules[] = {
    {"Chrome", NULL, NULL, 0, 1, -1},
    {"Firefox", NULL, NULL, 0, 1, -1},
};

typedef struct
{
    const char *symbol;
    void (*arrange)(Monitor *);
} Layout;

struct Monitor
{
};

static const Layout layouts[] = {
    {"[]=", NULL},
    {"><>", NULL},
    {"[M]", NULL},
};

#define MODKEY Mod1Mask
#define TAGKEYS(KEY, TAG) \
    {MODKEY, KEY, NULL, {.ui = 1 << TAG}},

typedef enum
{
    Int,
    UnsignedInt,
    Float,
    VoidP,
} ArgEnum;
typedef union
{
    int i;
    unsigned int ui;
    float f;
    const void *v;

} Arg;

typedef struct
{
    unsigned int mod;
    KeySym keysym;
    void (*func)(const Arg *, const ArgEnum *);
    const Arg arg;
    const ArgEnum argType;
} Key;

void test(const Arg *arg, const ArgEnum *argType)
{
    switch (*argType)
    {
    case Int:
        printf("arg.i=%d\n", arg->i);
        break;
    case UnsignedInt:
        printf("arg.ui=%d\n", arg->ui);
        break;
    case Float:
        printf("arg.f=%f\n", arg->f);
        break;
    case VoidP:
        printf("arg.v=%s\n", (char *)arg->v);
        break;

    default:
        break;
    }
}

// void (*)(const Arg *, const ArgEnum *)
// void * (*)(const Arg *, const ArgEnum *)
static const Key keys[] = {
    {MODKEY, XK_P, test, {1}, Int},
    {MODKEY, XK_P, test, {.f = 1.2}, Float},
    {MODKEY, XK_P, test, {.ui = 1}, UnsignedInt},
    {MODKEY, XK_P, test, {.v = "this is a test"}, VoidP},
};
enum
{
    ClkTagBar,
    ClkLtSymbol,
    ClkLtStatusText,
    ClkWinTitle,
    ClkClientWin,
    ClkRootWin,
    ClkLast,
};

typedef struct
{
    unsigned int click;
    unsigned int mask;
    unsigned int button;
    void (*func)(const Arg *arg);
    const Arg arg;
} Button;

// static const Button buttons[] = {
//     {ClkLtSymbol, 0, Button1, setLayoyt, {0}},
// };

int main()
{
    for (int i = 0; i < LENGTH(keys); i++)
    {
        keys[i].func(&keys[i].arg, &keys[i].argType);
    }
}