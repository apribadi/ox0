default: bin/ox

bin/ox: src/*.c 
	clang -std=c17 -Wall -Wextra -Werror -O2 -o bin/ox src/ox.c
