#include <lexp.h>
#include <primitive.h>
#include <unused.h>

lexp f_env(lexp _, lexp e) {
  UNUSED(_);
  return e;
}

PRIMITIVE_SECTION struct primitive env_primitive[] = {
	{"env",    f_env},
};
