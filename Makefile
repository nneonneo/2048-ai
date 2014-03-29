CPPFLAGS += -O3 -Wall -Wextra -fPIC

all: bin/2048 bin/2048.so

bin:
	mkdir bin 2>/dev/null || true

bin/%: bin/%.o
	$(LINK.cpp) $^ $(LOADLIBES) $(LDLIBS) -o $@

bin/%.so: bin/%.o
	$(LINK.cpp) -shared $^ $(LOADLIBES) $(LDLIBS) -o $@

bin/%.o : %.cpp | bin
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

clean:
	$(RM) -rf bin/*

.PHONY: all clean
