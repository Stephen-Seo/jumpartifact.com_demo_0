ifdef DEBUG
	COMMON_FLAGS = -g -O0
else
	COMMON_FLAGS = -DNDEBUG -O3
endif

CXX_FLAGS = -Wall -Wextra -Wpedantic -Weffc++ ${COMMON_FLAGS}
LINKER_FLAGS = -lraylib

OBJDIR = objdir

SOURCES = \
		src/main.cc \
		src/game.cc \
		src/screen.cc \
		src/screen_test.cc

HEADERS = \
		src/game.h \
		src/screen.h \
		src/screen_test.h

OBJECTS = $(addprefix ${OBJDIR}/,$(subst .cc,.cc.o,${SOURCES}))

all: | format demo_0

demo_0: ${OBJECTS}
	${CXX} ${CXX_FLAGS} ${LINKER_FLAGS} -o $@ $^

.PHONY: clean format

clean:
	rm -rf ${OBJDIR}
	rm -f demo_0

format:
	clang-format -i --style=google ${HEADERS} ${SOURCES}

.SECONDEXPANSION:

${OBJDIR}/%.cc.o: $$(subst ${OBJDIR}/,,%.cc) ${HEADERS}
	@mkdir -p $(dir $@)
	${CXX} -c ${CXX_FLAGS} -o $@ $<
