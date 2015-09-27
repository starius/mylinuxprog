SUBDIRS = $(shell find . -maxdepth 1 -mindepth 1 -type d -printf "%f\n")

all clean disclean:
	@list='$(SUBDIRS)'; for subdir in $$list; do \
	   echo "=============== making $@ in $$subdir ================="; \
	   (cd $$subdir && make $@) \
	done

