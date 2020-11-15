CC=gcc
CFLAGS=-I. -DCRYPTONITE_EXPORT=
#DEPS = hellomake.h
# $(DEPS)
%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

utest: utest.o utest_dstu7564.o subrowcol.o byte_array.o byte_array_internal.o byte_utils_internal.o dstu7564.o test_utils.o  word_internal.o
	$(CC) -o $@ $^

clean:
	rm *.o utest
