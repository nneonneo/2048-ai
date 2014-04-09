CPPFLAGS += -O3 -g -Wall -Wextra -std=c++11

all: bin/2048

bin:
	mkdir bin 2>/dev/null || true

bin/%: bin/%.o
	$(LINK.cpp) $^ $(LOADLIBES) $(LDLIBS) -o $@

bin/%.o : %.cpp | bin
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

clean:
	$(RM) -rf bin/*

.PHONY: all clean
