
# Forward all make commands to the Oroto sunflower seed directory
.PHONY: all clean release debug run install test help

all:
	cd "Oroto sunflower seed" && $(MAKE) all

clean:
	cd "Oroto sunflower seed" && $(MAKE) clean

release:
	cd "Oroto sunflower seed" && $(MAKE) release

debug:
	cd "Oroto sunflower seed" && $(MAKE) debug

run:
	cd "Oroto sunflower seed" && $(MAKE) run

install:
	cd "Oroto sunflower seed" && $(MAKE) install

test:
	cd "Oroto sunflower seed" && $(MAKE) test

help:
	cd "Oroto sunflower seed" && $(MAKE) help

copy-exec:
	cd "Oroto sunflower seed" && $(MAKE) release
	cp "Oroto sunflower seed/oroto-kernel" ./oroto-kernel

%:
	cd "Oroto sunflower seed" && $(MAKE) $@
