/* Single line ascii text buffer operation */

#include <module/io/file.h>
#include <stdio.h>
#include <assert.h>


/* Read a char from a file */
int file_read(struct io_primitive *file) {
  struct file_typ *fp = file->private;
  assert(fp != NULL);
  char c = fp->data[fp->pos];
  if (fp->pos == fp->size)
    return EOF;
  fp->pos++;
  /* When we reach the end of the string in the file we reset the pos pointer */
  if (c == '\0' || c == '\n')
    fp->pos = 0;
  return c;
}


/* Write a char to a file */
int file_write(struct io_primitive *file, char c) {
  struct file_typ *fp = file->private;
  assert(fp != NULL);
  if (fp->pos == fp->size)
    return EOF;
  fp->data[fp->pos] = c;
  fp->pos++;
  /* When we reach the end of the string we reset the pos pointer */
  if (c == '\0' || c == '\n')
    fp->pos = 0;
  return 0;
}
