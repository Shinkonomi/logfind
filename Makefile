CC=gcc
CFLAGS=-g -O2 -Wall -Wextra -Isrc -rdynamic -DNDEBUG $(OPTFLAGS)
LIBS=-ldl $(OPTLIBS)
PREFIX?=/usr/local

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TEST_SRC=$(wildcard test/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

TARGET=build/logfind
#SO_TARGET=$(patsubst %.a,%.so,$(TARGET))

 # The Target Build
all: $(TARGET) tests

dev: CFLAGS=-g -Wall -Isrc -Wall -Wextra $(OPTFLAGS)
dev: all

$(TARGET): CFLAGS += -fPIC
$(TARGET): build $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)

#$(SO_TARGET): $(TARGET) $(OBJECTS)
#$(CC) -shared -o $@ $(OBJECTS)
#chmod +x $(TARGET)

build:
	@mkdir -p build
	@mkdir -p bin

 # The Unit Tests
.PHONY: tests
tests: CFLAGS += $(TEST_SRC)
tests: 
	$(CC) $(CFLAGS) -o $(TESTS)
	sh ./test/runtests.sh

 # The doc generator
.PHONY: doc
doc:
	@mkdir -p doc
	rm -rf doc/html
	rm -rf doc/latex
	doxygen doc/docgen.conf

 # The Cleaner
clean:
	rm -rf build $(OBJECTS) $(TESTS)
	rm -f test/tests.log
	rm -rf bin
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`

 # The Install
install: all
	install -d $(DESTDIR)/$(PREFIX)/lib/
	install $(TARGET) $(DESTDIR)/$(PREFIX)/lib/

 # The Checker
check:
	@echo Files with potentially dangerous functions.
	@egrep '[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)|stpn?cpy|a?sn?printf|byte_)' $(SOURCES) || true
