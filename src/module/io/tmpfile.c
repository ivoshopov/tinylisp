/* Temporary file. Used for writing and reading an expression to the RAM */

#include <module/io/file.h>
#include <module/io/codec/text.h>

static char tmpfile_data[256];

static struct file_typ tmpfile = {
  .data = tmpfile_data,
  .pos = 0,
  .size = sizeof(tmpfile_data),
};


static struct io_primitive tmpfile_io = {
  .read = file_read,
  .write = file_write,
  .private = (void*)&tmpfile,
};


PORTS_SECTION struct io_typ tmpfile_port = {
  .private = &tmpfile_io,
  .read = text_read,
  .write = text_write,
  .proto = "tmpfile",
};

