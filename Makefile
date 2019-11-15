src = $(wildcard src/*.c)
obj = $(patsubst src%,out%,$(src:.c=.o))
dep = $(obj:.o=.d))
exe = bin/test

override CFLAGS = -Ilibuwsc/src -Ilibuwsc/src/buffer \
                  -Ilibev
LDFLAGS = -Llibev/.libs -l:libev.a \
          -Llibuwsc/build/src -l:libuwsc.a \
          -pthread -ldl -lldap -llber -lz -lssl -lcrypto -lm

.PHONY: all
all: pre $(exe)

.PHONY: pre
pre:
	@mkdir -p out
	@mkdir -p bin

$(exe): $(obj)
	$(CC) -o $(exe) $(SOURCE_EXT) $^ $(LDFLAGS)

out/%.o: src/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

.PHONY: vendor
vendor: libuwsc

.PHONY: libuwsc
libuwsc: libev
	cd libuwsc; \
	mkdir -p build; \
	cd build; \
	CMAKE_INCLUDE_PATH="../../libev" CMAKE_LIBRARY_PATH="../../libev/.libs" cmake ..; \
	make

.PHONY: libev
libev:
	cd libev; \
	chmod +x ./autogen.sh; \
	./autogen.sh; \
	./configure; \
	make
