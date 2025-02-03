CFLAGS=	-Iinclude -Ivendor/include
SRCS= 	src/pubsub.c

libbingo.so: $(SRCS)
	$(CC) -shared -fPIC $(CFLAGS) -o $@ $(SRCS)


pubsub_test: tests/pubsub_test.c
	$(CC) -L. $(CFLAGS) -o $@ $< -lbingo
