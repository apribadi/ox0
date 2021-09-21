default: bin/ox

bin/ox: src/*.c 
	clang \
		-std=c17 \
		-Wall -Wextra -Werror -Wno-unused-function \
		-O2 \
		-o bin/ox \
		src/ox.c
