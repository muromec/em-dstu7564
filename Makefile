CC=gcc
CFLAGS=-I. -DCRYPTONITE_EXPORT=
#DEPS = hellomake.h
# $(DEPS)
%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

utest: utest.o utest_dstu7564.o dstu7624.o byte_array.o byte_array_internal.o byte_utils_internal.o dstu7564.o test_utils.o stacktrace.o word_internal.o math_gf2m_internal.o math_int_internal.o rs.o
	$(CC) -o $@ $^
