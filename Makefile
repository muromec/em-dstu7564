CC=gcc
CFLAGS=-I. -DCRYPTONITE_EXPORT=

LIBS=subrowcol.o byte_utils_internal.o dstu7564.o
TEST=utest.o utest_dstu7564.o test_utils.o


%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)


dstu7564.js: $(LIBS)
	$(CC) -o $@ $^ \
	-s EXPORTED_FUNCTIONS="[ \
          '_dstu7564_alloc', \
          '_dstu7564_init', \
          '_dstu7564_free', \
          '_dstu7564_update', \
          '_dstu7564_final' \
        ]" \
	-s 'EXPORT_NAME="emdstu7564"' \
	-s NO_EXIT_RUNTIME=1 -O3

utest: $(LIBS) $(TEST)
	$(CC) -o $@ $^

clean:
	rm *.o utest
