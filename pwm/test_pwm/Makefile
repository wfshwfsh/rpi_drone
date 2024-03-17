CC ?= gcc
TAR = pwm_test

all: clean build

build:
	$(CC) $(TAR).c -o $(TAR).app

clean:
	rm -rf *.app

exec:
	./$(TAR).app

help:
	@echo "HELP"

update:
	cp $(TAR).app /mnt/share
