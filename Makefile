all: build run

build:
	mkdir -p ./bin
	gcc ./src/main.c -o ./bin/adifetch

run: build
	./bin/adifetch

clean:
	rm -f /adkbin/adifetch
	rm -rf ./bin

install: build
	cp -f ./bin/adifetch /adkbin
