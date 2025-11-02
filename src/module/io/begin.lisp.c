#include <module/io/file.h>
#include <module/io/codec/text.h>

char begin_data[] = "\
 (define begin \
  (lambda (x . args) \
    (if args (begin . args) x)))";

static struct file_typ begin_buff = {
  .data = begin_data,
  .pos = 0,
  .size = sizeof(begin_data),
};


static struct io_primitive begin_io = {
  .read = file_read,
  .write = NULL,
  .private = (void*)&begin_buff,
};


PORTS_SECTION struct io_typ begin_port = {
  .private = &begin_io,
  .read = text_read,
  .write = NULL,
  .proto = "begin.lisp",
};
