#SUBDIRS = $(shell ls -l | awk '/^d/ { print $$9 }')
SUBDIRS = $(shell find . -maxdepth 1 -mindepth 1 -type d -printf "%f\n")

all install uninstall clean disclean:
	@list='$(SUBDIRS)'; for subdir in $$list; do \
	   echo "=============== making $@ in $$subdir ================="; \
	   (cd $$subdir && make $@) \
	done

# создание дерева примеров
tree:
	tree > tree.txt

# создание компактного архива примеров
arch:	disclean tree
	@rm -f *.tgz
	tar -czvf ../PRACTIS.Tasks.tgz * > /dev/null
