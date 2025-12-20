#include <lexp.h>
#include <module/module.h>
#include <module/error/mk_error.h>
#include <primitive.h>
#include <stdlib.h>

lexp car(lexp p);
lexp eval(lexp, lexp);
extern lexp nil;


lexp f_exit(lexp args, lexp e) {
  exit(eval(car(args), e) == 0 ? 0 : 1);
  /* Following return is unreachable. */
  return nil;
}

PRIMITIVE_SECTION struct primitive exit_primitive[] = {
        {"exit!", f_exit},
};
