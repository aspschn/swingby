I_PATH=../../include
L_PATH=../../build

default:
	$(CC) main.c window.c -I$(I_PATH) -L$(L_PATH) -lswingby -lm -lGL

run:
	LD_LIBRARY_PATH=$(L_PATH) ./a.out
