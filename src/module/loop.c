#include <lexp.h>
#include <module/module.h>
#include <module/error/mk_error.h>
#include <primitive.h>

lexp car(lexp p);
lexp cdr(lexp p);
lexp eval(lexp, lexp);
extern hsptyp sp;
extern lexp nil;
extern lexp env;


lexp f_loop(lexp exprs, lexp e) {
  while (1) {
    hsptyp orig_sp = sp;
    while (typof(exprs) != NIL) {
      eval(car(exprs), e);
      exprs = cdr(exprs);
    }
    /* Do we have new environment created by define function? */
    if (ord(env) < ord(e)) {
      /* Since we have new environment in the stack we have to move the
       * stack pointer forward after the new environment */
      sp = ord(env);
      /* Let's use the new environment in the next loop */
      e = env;
    } else {
      /* return back the stack pointer to a place before the loop
       * (like a garbage collection) */
      sp = orig_sp;
    }
  }
  return nil;
}

PRIMITIVE_SECTION struct primitive loop_primitive[] = {
        {"loop!", f_loop},
};
