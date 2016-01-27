SUBDIRS = ../EidInfraTest ../pcscproxy ../dlopenproxy ../pcsctool

all:
	list='$(SUBDIRS)'; \
	for subdir in $$list; do \
		cd $$subdir && make && cd -; \
	done;

clean:
	list='$(SUBDIRS)'; \
	for subdir in $$list; do \
		cd $$subdir && make clean && cd -; \
	done;
