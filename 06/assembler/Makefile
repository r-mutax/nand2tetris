# -Wall -Wextra -Winit-self -Wno-missing-field-initializers
COMPILER	= g++
CFLAGS    = -g -MMD -MP -std=c++17
INCDIR		= -I./inc
TARGET		= ./bin/$(shell basename `readlink -f .`)
SRCDIR		= ./src
SOURCES		= $(wildcard $(SRCDIR)/*.cpp)
OBJDIR		= ./load
OBJECTS		= $(addprefix $(OBJDIR)/,$(notdir $(SOURCES:.cpp=.o)))
DEPENDS   	= $(OBJECTS:.o=.d)

$(TARGET): $(OBJECTS)
	echo $(OBJECTS)
	$(COMPILER) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	-mkdir -p $(OBJDIR)
	$(COMPILER) $(CFLAGS) $(INCDIR) -o $@ -c $<

clean:
	-rm -f $(OBJECTS) $(DEPENDS) $(TARGET)

.PHONY: clean

-include $(DEPENDS)
