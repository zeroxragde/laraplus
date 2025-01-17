# Nombre del ejecutable
TARGET = laraplus

# Compilador y opciones
CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -O0

# Rutas de inclusión
INCLUDES = -I./include -I./Libs

# Carpeta(s) con .cpp
SRCS = $(wildcard include/*.cpp) \
       $(wildcard Libs/*.cpp) \
       main.cpp

# Generar la lista de .o sustituyendo .cpp -> .o
OBJS = $(SRCS:.cpp=.o)

# Regla principal
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^

# Regla genérica para .cpp -> .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Limpiar
clean:
	rm -f $(OBJS) $(TARGET)
