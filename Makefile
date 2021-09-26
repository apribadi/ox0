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

test: bin/ox test/exp0.ox test/exp1.ox test/exp2.ox
	./bin/ox test/exp0.ox
	./bin/ox test/exp1.ox
	./bin/ox test/exp2.ox
