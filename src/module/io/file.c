#include <io.h>
#include <stddef.h>
#include <stdio.h>
#include <unused.h>

struct file_typ {
  char *data;
  size_t pos;
  size_t size;
};

#define TESTFILE_STR "(define file '(This is a test string))"
static struct file_typ testfile = {
  .data = TESTFILE_STR,
  .pos = 0,
  .size = sizeof(TESTFILE_STR),
};

static int file_read(struct io_primitive *file) {
  struct file_typ *fp = file->private;
  char c = fp->data[fp->pos];
  if (fp->pos == fp->size)
    return EOF;
  fp->pos++;
  return c;
}

static struct io_primitive testfile_io = {
  .read = file_read,
  .write = NULL,
  .private = (void*)&testfile,
};

lexp stream_read(struct io_primitive *port);

static lexp vfs_read(struct io_typ *port) {
  struct io_primitive *port_primitive = port->private;
  return stream_read(port_primitive);
}

PORTS_SECTION struct io_typ testfile_port = {
  .private = &testfile_io,
  .read = vfs_read,
  .write = NULL,
  .proto = "testfile",
};
