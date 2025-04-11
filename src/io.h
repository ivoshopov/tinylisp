#include <lexp.h>

struct io_typ {
  /* The name of the protocol. There is one to one matching between protocol
   * name and read/write function */
  char *proto;
  /* TODO: add memomry manager as parameter to read function
   * (like: lexp (*read) (struct io_primitive *port, struct mm_typ *heap); )
   * This way we can control which memory will be used for construction of
   * s-expr that will be evaluated. */
  lexp (*read) (struct io_typ *port);
  lexp (*write) (struct io_typ *port, lexp exp);
  void *private;
};


extern struct io_typ *ports[];
