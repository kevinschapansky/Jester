CC := g++ # This is the main compiler
# CC := clang --analyze # and comment out the linker last line for sanity

CFLAGS := -g -Wall -std=c++0x

SENSOR_LIB := ../sensor/lib
SENSOR_INC := ../sensor/include
JESTER_LIB := ../lib
JESTER_INC := ../include
LIBS := -lcarmine -ljester -lpthread -lOpenNI2 -lNiTE2 -DGL_GLEXT_PROTOTYPES -lGL -lglut -lGLU
DLLIBS := -lpthread -lOpenNI2 -lNiTE2
CARMINELIB := -L $(OPENNI2_REDIST) -L $(NITE2_REDIST)
CARMINEINC := -I $(OPENNI2_INCLUDE) -I $(NITE2_INCLUDE)

TLIB := -L $(JESTER_LIB) -L $(SENSOR_LIB)
TINC := -I $(JESTER_INC) -I $(SENSOR_INC)

DLLIB := $(CARMINELIB) 
DLINC := $(CARMINEINC)

all: SkeletonVisualizer SensorDataLogger

SkeletonVisualizer:
	@echo " Building tests"
	@echo " $(CC) $(CFLAGS) $(TINC) $(CARMINEINC) GLSL_helper.cpp GeometryCreator.cpp $@.cpp -o ../bin/$@ $(TLIB) $(CARMINELIB) $(LIBS)"; \
	 $(CC) $(CFLAGS) $(TINC) $(CARMINEINC) GLSL_helper.cpp GeometryCreator.cpp $@.cpp -o ../bin/$@ $(TLIB) $(CARMINELIB) $(LIBS)
	 
SensorDataLogger:
	@echo " Building data logger"
	@echo " $(CC) $(CFLAGS) $(DLINC) SensorDataLogger.cpp -o ../bin/$@ $(DLLIB) $(DLLIBS)"; \
	$(CC) $(CFLAGS) $(DLINC) SensorDataLogger.cpp -o ../bin/$@ $(DLLIB) $(DLLIBS) 
