#include <lexp.h>
#include <module/module.h>
#include <module/error/mk_error.h>
#include <primitive.h>

lexp car(lexp p);
lexp cdr(lexp p);
lexp evlis(lexp t, lexp e);

void print(lexp);
lexp f_print(lexp t, lexp e) {
        t = evlis(t, e);
        while (typof(t) != NIL) {
                print(car(t));
                t = cdr(t);
        }
        return t;
}


PRIMITIVE_SECTION struct primitive print_primitive[] = {
        {"print!", f_print},
};

static void init()
{
}

MODULE_SECTION struct module print_mod = {
        .setup = init,
};
