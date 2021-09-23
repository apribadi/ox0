default: bin/ox

bin/ox: src/*.c 
	clang \
		-std=c17 \
		-Wall -Wextra -Werror \
		-Wno-unused-function \
		-Wno-unused-parameter \
		-O2 \
		-o bin/ox \
		src/ox.c
