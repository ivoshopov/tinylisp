#include <stdio.h>
#include <io.h>
#include <unused.h>


static int stdout_write(struct io_primitive *port, char c) {
  UNUSED(port);
  if (putchar((char)c) != EOF)
    return 0;
  /* TODO: we need user readable error code, -1 isn't understandable */
  return EOF;
}

static int stdin_read(struct io_primitive *port) {
  UNUSED(port);
  return getchar();
}

static struct io_primitive stdio = {
  .read = stdin_read,
  .write = stdout_write,
};

lexp stream_write(struct io_primitive *port, lexp);
lexp stream_read(struct io_primitive *port);

static lexp std_write(struct io_typ *port, lexp exp) {
  struct io_primitive *port_primitive = port->private;
  exp = stream_write(port_primitive, exp);
  port_primitive->write(port_primitive, '\n');
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
