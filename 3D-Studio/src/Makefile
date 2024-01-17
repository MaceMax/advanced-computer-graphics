#
#  Linux and MS Windows Makefile for Workshop 1
#  Computer Graphics course
#  Dept Computing Science, Umea University
#  Stefan Johansson, stefanj@cs.umu.se
# 
#
SRC = .
TARGET = 3D-Studio
BUILD_DIR = ../target
SUB_DIR = Application ImGui Loader Managers Util Renderer Scene Scene/Lights Scene/Mesh Scene/World
LIBDIR = $(SRC)/../lib

# All source files (they can be listed explicitly if needed)
# Add $(wildcard $(SRC)/dir1/*.cpp) to add another subdirectory
CPPS =  $(SRC)/main.cpp \
	$(shell find $(SRC) -name "*.cpp") \
	$(wildcard $(LIBDIR)/ImGui/*.cpp) \
	$(wildcard $(LIBDIR)/ImGuiFileDialog/*.cpp)

# All .o files are put in the build directory
OBJS = $(CPPS:%.cpp=$(BUILD_DIR)/%.o)
# gcc/clang put the dependencies in the .d files
DEP = $(OBJS:%.o=%.d)

CXX = g++

DBFLAGS = -O0 -g3 -ggdb3 -fno-inline
#DBFLAGS = -O2
WFLAGS  = -Wall -std=c++11 -Wno-unknown-pragmas

# Uncomment if you have local libraries or headers in subfolders lib and include
IFLAGS = -I$(LIBDIR)/ImGui -I$(LIBDIR)/ImGuiFileDialog $(addprefix -I, $(SUB_DIR))
LFLAGS = #-Llib 

IMGUIFLAGS = -DIMGUI_IMPL_OPENGL_LOADER_GLEW

ifeq ($(OS), Windows_NT)
# -DWINDOWS_BUILD needed to deal with Windows use of \ instead of / in path
# unless it's completely unnecessary and handled by the compiler.
DEFS      = -DWINDOWS_BUILD
GLFLAGS   = -DGLEW_STATIC
OSLDFLAGS = -static -lglew32 -lglfw3 -lopengl32 -lgdi32 -luser32 -lkernel32
else
DEFS     = 
GLFLAGS  = `pkg-config --cflags glfw3` 
LGLFLAGS = `pkg-config --libs glew glfw3 gl`
ELDFLAGS = -export-dynamic -lXext -lX11 
endif

CXXFLAGS = $(WFLAGS) $(DFLAGS) $(GLFLAGS) $(IMGUIFLAGS)

CXXFLAGS = $(DBFLAGS) $(DEFS) $(WFLAGS) $(IFLAGS) $(DFLAGS) $(GLFLAGS)
LDFLAGS  =  $(LGLFLAGS) $(ELDFLAGS) $(OSLDFLAGS)


all: $(BUILD_DIR)/$(TARGET) run

# Binary target, depends on all .o files
$(BUILD_DIR)/$(TARGET) : $(OBJS)
	mkdir -p $(@D)
	$(CXX) $(LFLAGS) $^ -o $@ $(LDFLAGS) 

# Object files, depends on all .cpp files
# Headers are included by the next line
-include $(DEP)
$(BUILD_DIR)/%.o : %.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -MMD -c $< $(INCPATH) -o $@

$(BUILD_DIR)/%.o : %.c
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -MMD -c $<

run: $(BUILD_DIR)/$(TARGET)
	./$(BUILD_DIR)/$(TARGET)

clean:
ifeq ($(OS), Windows_NT)
	del /Q /S *.o *.d
else
	rm -f $(OBJS) $(DEP) $(BUILD_DIR)/$(TARGET)
endif
