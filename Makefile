PIOBUILDPATH=.pio/build/micro/

all: install

build:
	pio run

debug:
	pio debug
	install -Dm755 $(PIOBUILDPATH)/firmware.hex /mnt/c/Users/Baydoun/main.hex

install: build
	 install -Dm755 $(PIOBUILDPATH)/firmware.hex /mnt/c/Users/Baydoun/main.hex
