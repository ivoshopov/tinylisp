
NM = nm
export NM
MAKE = make
export MAKE
CFLAGS = -Wall -Wextra -g -Isrc
export CFLAGS


subdirs := src

$(MAKECMDGOALS) all: $(subdirs)
	@:

.PHONY: $(subdirs)
$(subdirs):
	$(MAKE) -f Makefile.base subdir=$@ $(MAKECMDGOALS)



