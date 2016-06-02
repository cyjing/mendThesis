ifdef API_DEBUG
	API_FLAG = "API_DEBUG=1"
endif

all: build

build:
	$(MAKE) -C src $(API_FLAG)

install: $(SHAREDLIB)
	$(MAKE) -C src

clean:
	$(MAKE) -C src clean
