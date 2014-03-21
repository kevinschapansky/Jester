CC := g++ # This is the main compiler
# CC := clang --analyze # and comment out the linker last line for sanity

CFLAGS := -g -Wall -std=c++0x

SENSOR_LIB := ../sensor/lib
SENSOR_INC := ../sensor/include
JESTER_LIB := ../lib
JESTER_INC := ../include
LIBS := -lcarmine -lleap -ljester -lpthread -lLeap -lOpenNI2 -lNiTE2 -DGL_GLEXT_PROTOTYPES -lGL -lglut -lGLU
CARMINELIB := -L $(OPENNI2_REDIST) -L $(NITE2_REDIST)
CARMINEINC := -I $(OPENNI2_INCLUDE) -I $(NITE2_INCLUDE)
LEAPLIB := -L $(LEAP_REDIST)
LEAPINC := -I $(LEAP_INCLUDE)

TLIB := -L $(JESTER_LIB) -L $(SENSOR_LIB)
TINC := -I $(JESTER_INC) -I $(SENSOR_INC)

all: SkeletonVisualizer SensorDataLogger

SkeletonVisualizer:
	@echo " Building tests"
	@echo " $(CC) $(CFLAGS) $(TINC) $(CARMINEINC) $(LEAPINC) GLSL_helper.cpp GeometryCreator.cpp $@.cpp -o ../bin/$@ $(TLIB) $(CARMINELIB) $(LEAPLIB) $(LIBS)"; \
	 $(CC) $(CFLAGS) $(TINC) $(CARMINEINC) $(LEAPINC) GLSL_helper.cpp GeometryCreator.cpp $@.cpp -o ../bin/$@ $(TLIB) $(CARMINELIB) $(LEAPLIB) $(LIBS)
