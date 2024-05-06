obj-y += tinylisp-commented.o
obj-y += lexp.o
obj-$(CONFIG_ERROR_ATOM_ID) += module/error/atom_id.o
obj-$(CONFIG_ERROR_SINGLE) += module/error/single.o
elf-y += tinylisp
