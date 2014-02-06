CC := g++ # This is the main compiler
# CC := clang --analyze # and comment out the linker last line for sanity
SRCDIR := src
BUILDDIR := build
LIBDIR := lib
LIBNAME := libcarmine.a
JLIBDIR := ../lib
JINCDIR := ../include
 
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g -Wall -std=c++0x
LIB := -L $(JLIBDIR)
INC := -I include -I $(JINCDIR) -I $(OPENNI2_INCLUDE) -I $(NITE2_INCLUDE)
LIBS := -ljester -pthread


all: Carmine

Carmine: $(OBJECTS)
	@mkdir -p $(LIBDIR)
	@echo " Creating Carmine Sensor Library..."
	@echo " ar rcs $(LIBDIR)/$(LIBNAME) $(OBJECTS) ../lib/libjester.a"; \
	ar rcs $(LIBDIR)/$(LIBNAME) $(OBJECTS) ../lib/libjester.a

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) $< -c -o $@ $(LIB) $(LIBS)"; \
	$(CC) $(CFLAGS) $(INC) $< -c -o $@ $(LIB) $(LIBS)

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR)"; $(RM) -r $(BUILDDIR) $(LIBDIR)

.PHONY: clean

