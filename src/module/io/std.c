#include <stdio.h>
#include <io.h>
#include <unused.h>
#include <module/io/codec/text.h>


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

/* the "std" protocol will behave like interactive interpreter */
PORTS_SECTION struct io_typ std_port = {
  .private = &stdio,
  .read = text_read,
  .write = text_write,
  .proto = "std",
};
