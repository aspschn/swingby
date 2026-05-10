I_PATH=../../include
L_PATH=../../build

default:
	$(CC) main.c -I$(I_PATH) -L$(L_PATH) -lswingby -lm -lGL

run:
	MESA_GL_VERSION_OVERRIDE=4.6COMPAT LD_LIBRARY_PATH=$(L_PATH) ./a.out
