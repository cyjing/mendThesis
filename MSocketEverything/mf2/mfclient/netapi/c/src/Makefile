SHAREDLIB=libmfapi.so
SHAREDLIBDIR = $(DESTDIR)/usr/lib/
MF_HOME=/root/MSocketEverything/mf2
CC=gcc
CFLAGS=-g -c -Wall -fPIC -DLINUX -I$(MF_HOME)/common/
LDFLAGS=-shared -Wl,-soname,libmfapi.so
SOURCES=mfapi.c log.c
HEADERS=mfapi.h $(MF_HOME)/common/include/mfflags.h

OBJECTS=$(SOURCES:.c=.o)

ifdef API_DEBUG
	CFLAGS += -DAPI_DEBUG
endif

all: $(SHAREDLIB)

$(SHAREDLIB): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

mfapi.o: mfapi.c
	$(CC) $(CFLAGS) mfapi.c

log.o: log.c
	$(CC) $(CFLAGS) log.c
	
install: $(SHAREDLIB)
	install -d $(SHAREDLIBDIR) $(DESTDIR)/usr $(DESTDIR)/usr/include $(DESTDIR)/usr/include/mobilityfirst
	install -m 755 $(SHAREDLIB) $(SHAREDLIBDIR)
	install -m 644 $(HEADERS) $(DESTDIR)/usr/include/mobilityfirst	

clean:
	rm -f *.o $(SHAREDLIB)
