CC := g++ # This is the main compiler
# CC := clang --analyze # and comment out the linker last line for sanity
SRCDIR := src
BUILDDIR := build
LIBDIR := lib
CLIBNAME := libcarmine.a
LLIBNAME := libleap.a
JLIBDIR := ../lib
JINCDIR := ../include
 
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
COBJECTS := $(BUILDDIR)/PrimeSenseCarmineFactory.o $(BUILDDIR)/PrimeSenseCarmineImpl.o
LOBJECTS := $(BUILDDIR)/LeapMotionFactory.o $(BUILDDIR)/LeapMotionImpl.o
CFLAGS := -g -Wall -std=c++0x
LIB := -L $(JLIBDIR) -L $(OPENNI2_REDIST) -L $(NITE2_REDIST) -L $(LEAP_REDIST)
INC := -I include -I $(JINCDIR) -I $(OPENNI2_INCLUDE) -I $(NITE2_INCLUDE) -I $(LEAP_INCLUDE)
LIBS := -ljester -pthread -lLeap -lOpenNI2 -lNiTE2


all: Carmine Leap

Carmine: $(COBJECTS)
	@mkdir -p $(LIBDIR)
	@echo " Creating Carmine Sensor Library..."
	@echo " ar rcs $(LIBDIR)/$(CLIBNAME) $(COBJECTS) ../lib/libjester.a"; \
	ar rcs $(LIBDIR)/$(CLIBNAME) $(COBJECTS) ../lib/libjester.a
	
Leap: $(LOBJECTS)
	@mkdir -p $(LIBDIR)
	@echo " Creating Leap Sensor Library..."
	@echo " ar rcs $(LIBDIR)/$(LLIBNAME) $(LOBJECTS) ../lib/libjester.a"; \
	ar rcs $(LIBDIR)/$(LLIBNAME) $(LOBJECTS) ../lib/libjester.a

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) $< -c -o $@ $(LIB) $(LIBS)"; \
	$(CC) $(CFLAGS) $(INC) $< -c -o $@ $(LIB) $(LIBS)

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(LIBDIR)"; \
	$(RM) -r $(BUILDDIR) $(LIBDIR)

.PHONY: clean

