
#-----------------------------------------------------------------------

TARGET = gems

SRCS = \
	time.cpp \
	log.cpp \
	sdl.cpp \
	background.cpp \
	worker.cpp \
	game.cpp \
	board.cpp \
	main.cpp

#-----------------------------------------------------------------------

SDL_LDFLAGS := $(shell sdl-config --libs)

CFLAGS =
CFLAGS += -DDEBUG
CFLAGS += -g -O0 -Wall -Werror
CFLAGS += -D_THREAD_SAFE -D_GNU_SOURCE -D__USE_GNU

LDFLAGS =
LDFLAGS += -ldl -lpthread -lrt
LDFLAGS += -lGL -lGLU $(SDL_LDFLAGS)

CC = g++
LD = ld
AR = ar
OBJCOPY = objcopy
STRIP   = strip

BUILD_DIR=.builtobjs

#-----------------------------------------------------------------------

ifneq ($(VERBOSE),1)
QUIET = @
endif

ifeq ($(QUIET),@)
MAKE_OPTS = --no-print-directory
endif

OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)

BUILT_OBJS = $(foreach obj, $(OBJS), $(BUILD_DIR)/$(notdir $(obj)))
BUILT_DEPS = $(foreach dep, $(DEPS), $(BUILD_DIR)/$(notdir $(dep)))

#-----------------------------------------------------------------------
default all:
	$(QUIET) $(MAKE) $(MAKE_OPTS) $(TARGET)

$(TARGET): $(BUILT_OBJS) | $(BUILD_DIR)
	$(QUIET) $(CC) $(CFLAGS) $(BUILT_OBJS) $(LDFLAGS) -o $@
	@echo " + Built $@"

clean:
	$(QUIET) rm -rf $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	$(QUIET) mkdir -p $@

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	@echo " + $(notdir $@)"
	$(QUIET) $(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.d: %.cpp | $(BUILD_DIR)
	$(QUIET) $(CC) -M -MM -MT $(BUILD_DIR)/$(patsubst %.cpp,%.o,$<) $(CFLAGS) $< -o $@

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include $(BUILT_DEPS)
endif
endif

