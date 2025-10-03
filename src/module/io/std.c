#include <stdio.h>
#include <io.h>


static int stdout_write(char c) {
  if (putchar((char)c) != EOF)
    return 0;
  /* TODO: we need user readable error code, -1 isn't understandable */
  return EOF;
}


static struct io_primitive stdio = {
  .read = getchar,
  .write = stdout_write,
};

lexp stream_write(struct io_primitive *port, lexp);
lexp stream_read(struct io_primitive *port);

static lexp std_write(struct io_typ *port, lexp exp) {
  struct io_primitive *port_primitive = port->private;
  exp = stream_write(port_primitive, exp);
  port_primitive->write('\n');
  return exp;
}

static lexp std_read(struct io_typ *port) {
  struct io_primitive *port_primitive = port->private;
  return stream_read(port_primitive);
}

/* the "std" protocol will behave like interactive interpreter */
PORTS_SECTION struct io_typ std_port = {
  .private = &stdio,
  .read = std_read,
  .write = std_write,
  .proto = "std",
};
