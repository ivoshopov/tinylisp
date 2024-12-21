#include <lexp.h>
#include <module/module.h>
#include <module/error/mk_error.h>
#include <primitive.h>

lexp car(lexp p);
lexp cdr(lexp p);
lexp eval(lexp, lexp);
extern hsptyp sp;
extern lexp nil;


lexp f_loop(lexp t, lexp e) {
  lexp exprs;
  while (1) {
    hsptyp orig_sp = sp;
    exprs = t;
    while (typof(exprs) != NIL) {
      eval(car(exprs), e);
      exprs = cdr(exprs);
    }
    sp = orig_sp;
  }
  return nil;
}

PRIMITIVE_SECTION struct primitive loop_primitive[] = {
        {"loop!", f_loop},
};

static void init()
{
}

MODULE_SECTION struct module loop_mod = {
        .setup = init,
};
