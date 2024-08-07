#include <lexp.h>

#define PRIMITIVE_SECTION __attribute__((section("primitives")))

// Begin and end of primitive list (the linker initialize them)
extern struct primitive __start_primitives;
extern struct primitive __stop_primitives;


struct primitive{
  const char *s;
  lexp (*f)(lexp, lexp);
};
