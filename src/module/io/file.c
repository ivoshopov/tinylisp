#include <io.h>
#include <stddef.h>
#include <stdio.h>

static char file[] = "(define file '(This is a test string))";
static size_t pos = 0;

static int file_read() {
  char c = file[pos];
  if (pos == sizeof(file))
    return EOF;
  pos++;
  return c;
}

static struct io_primitive file_io = {
  .read = file_read,
  .write = NULL,
};

lexp stream_read(struct io_primitive *port);

static lexp vfs_read(struct io_typ *port) {
  struct io_primitive *port_primitive = port->private;
  return stream_read(port_primitive);
}

PORTS_SECTION struct io_typ file_port = {
  .private = &file_io,
  .read = vfs_read,
  .write = NULL,
  .proto = "file",
};
