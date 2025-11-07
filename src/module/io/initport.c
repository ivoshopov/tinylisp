#include <module/io/file.h>
#include <module/io/codec/text.h>

char init_data[] = "\
 (define begin \
  (lambda (x . args) \
   (if args (begin . args) x))) \
 (loop! \
  (write-to! 'std \
   (eval \
    (read-from! 'std))))";

static struct file_typ init_buff = {
  .data = init_data,
  .pos = 0,
  .size = sizeof(init_data),
};


static struct io_primitive init_io = {
  .read = file_read,
  .write = NULL,
  .private = (void*)&init_buff,
};


PORTS_SECTION struct io_typ init_port = {
  .private = &init_io,
  .read = text_read,
  .write = NULL,
  .proto = "init.lisp",
};
