PROGRAM = src/mfstack

CONFDIR = $(DESTDIR)/etc/mfstack/

ifdef OML
        OML_FLAG = "OML=1"
endif


all: check-env build

build:
	 $(MAKE) -C src $(OML_FLAG)

check-env:
ifndef MF_HOME
	$(info MF_HOME is undefined)
	$(error MF_HOME has to be the folder where the common header files are)
else
	$(info MF_HOME is $(MF_HOME))
endif

install:
	install -d $(CONFDIR) $(DESTDIR)/etc
	$(MAKE) -C src install
	install -m 644 conf/* $(CONFDIR)

clean: 
	$(MAKE) -C src clean $(OML_FLAG)
