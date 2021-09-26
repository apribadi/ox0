.PHONY: test

bin/ox: src/*.c 
	clang \
		-std=c17 \
		-Wall -Wextra -Werror \
		-Wno-unused-function \
		-O2 \
		-fno-strict-aliasing \
		-o bin/ox \
		src/ox.c

test: bin/ox test/foo.ox
	./bin/ox test/foo.ox
