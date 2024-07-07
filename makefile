CC = gcc
CFLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors


PYTHON_CONFIG = python3-config
INCLUDE_PY = $(shell $(PYTHON_CONFIG) --includes)
LIB_PY = $(shell $(PYTHON_CONFIG) --ldflags)
EXTENSION = symnmf

all: $(EXTENSION).so

$(EXTENSION).so: symnmfmodule.o symnmf.o
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LIB_PY)

symnmfmodule.o: symnmfmodule.c symnmf.h
	$(CC) $(CFLAGS) $(INCLUDE_PY) -c $< -o $@

symnmf.o: symnmf.c symnmf.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.so