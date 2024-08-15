#include <lexp.h>
#include <module/module.h>
#include <module/error/mk_error.h>
#include <primitive.h>

static unsigned int top = 0;
static lexp boxes[100];


lexp car(lexp p);
lexp cdr(lexp p);
lexp evlis(lexp t, lexp e);
extern lexp nil;
extern lexp tru;

lexp f_set_box(lexp t, lexp e) {
        lexp arg;
        /* TODO: verify t is a BOX */
        t = evlis(t, e);
        arg = car(cdr(t));
        if( typof(arg) == CONS
                        || typof(arg) == CLOS
                        || typof(arg) == MACR )
                return mk_error("Not supported type, box can not be initialized with:", arg);
        boxes[ord(car(t))] = arg;
        return nil;
}

lexp f_box(lexp t, lexp e) {
        t = car(evlis(t, e));
        /* TODO: Following line will set the box to point to a value in the stack
         * Therefore it works only for numbers or ATOMs */
        if( typof(t) == CONS
                        || typof(t) == CLOS
                        || typof(t) == MACR )
                return mk_error("Not supported type, box can not be initialized with:", car(t));
        top++;
        /* TODO: Check for overflow */
        boxes[top-1] = t;
        return box(BOX, top-1);
}

lexp f_unbox(lexp t, lexp e) {
        t = evlis(t, e);
        t = car(t);
        if( typof(t) == BOX )
                return boxes[ord(t)];
        else
                return mk_error("Argument is not a box:", t);
}

lexp f_isbox(lexp t, lexp e) {
        t = evlis(t, e);
        t = car(t);
        return typof(t) == BOX ? tru : nil;
}

PRIMITIVE_SECTION struct primitive box_primitive[] = {
        {"box",    f_box},
        {"set-box!",f_set_box},
        {"unbox",  f_unbox},
        {"isbox",  f_isbox},
};

static void init()
{
}

MODULE_SECTION struct module boxes_array = {
        .setup = init,
};

