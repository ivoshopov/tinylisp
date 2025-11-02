#ifndef __IO_H
#define __IO_H

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

/* A character stream type
 * It is used for reading and writing of char stream while we
 * serialize/deserialize lisp expressions. */
struct io_primitive {
  int (*read) (struct io_primitive *stream);
  int (*write) (struct io_primitive *stream, char p);
  void *private;
};


#define PORTS_SECTION __attribute__((section("ports")))

// Begin and end of port list (the linker initialize them)
extern struct io_typ __start_ports;
extern struct io_typ __stop_ports;

#endif /* __IO_H */
