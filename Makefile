
elf=tinylisp
NM=nm
src=src/tinylisp-commented.c src/lexp.c
CFLAGS=-Wall -Wextra

all: src/$(elf)

src/$(elf): $(src)
	$(CC) $(CFLAGS) $^ -o $@

%.s: %.c
	$(CC) $(CFLAGS) -S -fverbose-asm $< -o $@

%.lst: %.s
	$(AS) -alhnd $< > $@

src/$(elf).sym: src/$(elf)
	$(NM) -S --size-sort -t d  <$< >$@

