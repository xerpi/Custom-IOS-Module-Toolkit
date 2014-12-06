TARGET = stripios
CXXFLAGS = -O2

all: $(TARGET)

$(TARGET): main.cpp
	g++ $(CXXFLAGS) $< -o $@

clean:
	@rm -rf $(TARGET)

install: $(TARGET)
	cp $(TARGET) $(DEVKITPPC)/bin

