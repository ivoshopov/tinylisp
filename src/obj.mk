obj-y += tinylisp-commented.o
obj-y += lexp.o
obj-$(CONFIG_ERROR_ATOM_ID) += module/error/atom_id.o
obj-$(CONFIG_ERROR_SINGLE) += module/error/single.o
obj-$(CONFIG_PRIM_ENV) += module/env.o
obj-$(CONFIG_BOXES_ARRAY) += module/boxes/array.o
obj-$(CONFIG_BOXES_STACK) += module/boxes/stack.o
obj-$(CONFIG_MAIN) += main.o
elf-y += tinylisp
