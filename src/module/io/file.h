
#include <io.h>
#include <stddef.h>

/* Stream buffer */
struct file_typ {
  char *data;
  size_t pos;
  size_t size;
};

/* io_primitive has private field which should be a pointer to file_typ */
int file_read(struct io_primitive *file);
int file_write(struct io_primitive *file, char c);
