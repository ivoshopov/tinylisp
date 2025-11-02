/* Text serializer/deserializer operations.
 * It convert lisp expressions to/from stream of chars. */

#include <io.h>

lexp stream_write(struct io_primitive *port, lexp);
lexp stream_read(struct io_primitive *port);

lexp text_write(struct io_typ *port, lexp exp) {
  struct io_primitive *port_primitive = port->private;
  exp = stream_write(port_primitive, exp);
  port_primitive->write(port_primitive, '\n');
  return exp;
}

lexp text_read(struct io_typ *port) {
  struct io_primitive *port_primitive = port->private;
  return stream_read(port_primitive);
}

