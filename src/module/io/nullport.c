#include <module/io/file.h>
#include <module/io/codec/text.h>
#include <unused.h>

char null_data[] = "()";

static struct file_typ null_buff = {
  .data = null_data,
  .pos = 0,
  .size = sizeof(null_data),
};

int null_write(struct io_primitive *file, char c) {
  UNUSED(file);
  UNUSED(c);
  return 0;
}

static struct io_primitive null_io = {
  .read = file_read,
  .write = null_write,
  .private = (void*)&null_buff,
};


PORTS_SECTION struct io_typ null_port = {
  .private = &null_io,
  .read = text_read,
  .write = text_write,
  .proto = "null",
};
