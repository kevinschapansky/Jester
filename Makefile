CC := g++ # This is the main compiler
# CC := clang --analyze # and comment out the linker last line for sanity
SRCDIR := src
BUILDDIR := build
LIBDIR := lib
JLIBNAME := libjester.a
SLIBDIR := sensor/lib
SINCDIR := sensor/include
 
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g -Wall
LIB := -L lib -L /usr/local/include
INC := -I include -I /usr/local/include

TLIB := -L $(LIBDIR) -L $(SLIBDIR)
TINC := -I include -I $(SINCDIR)

Jester: $(OBJECTS)
	@mkdir -p $(LIBDIR)
	@echo " Creating Jester Library..."
	@echo " ar rcs $(LIBDIR)/$(JLIBNAME) $(OBJECTS)"; ar rcs $(LIBDIR)/$(JLIBNAME) $(OBJECTS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR)"; $(RM) -r $(BUILDDIR)

sensors:
	@echo " Building sensor libraries"
	@make -C sensor/

# Tests
tests: Jester sensors
	@echo " Building tests"
	@echo " $(CC) $(CFLAGS) $(TINC) test/SceneGraphTest.cpp -o bin/sceneGraphTest $(TLIB) -lcarmine -ljester "; \
	 $(CC) $(CFLAGS) $(TINC) test/SceneGraphTest.cpp -o bin/sceneGraphTest $(TLIB) -lcarmine -ljester 

.PHONY: clean

.PHONY: sensors

.PHONY: tests

