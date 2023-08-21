ifdef DEBUG
	COMMON_FLAGS = -g -Og
else
	COMMON_FLAGS = -DNDEBUG -O3
endif

CXX_FLAGS = -Wall -Wextra -Wpedantic -Weffc++ ${COMMON_FLAGS} -std=c++20
LINKER_FLAGS = -lraylib

OBJDIR = objdir

SOURCES = \
		src/main.cc \
		src/game.cc \
		src/screen.cc \
		src/screen_test.cc \
		src/screen_trunner.cc \
		src/3d_helpers.cc \
		src/raymath.cc \
		src/ems.cc \
		src/walker.cc \
		src/surface_triangle.cc

HEADERS = \
		src/game.h \
		src/screen.h \
		src/screen_test.h \
		src/screen_trunner.h \
		src/3d_helpers.h \
		src/ems.h \
		src/walker.h \
		src/surface_triangle.h

OBJECTS = $(addprefix ${OBJDIR}/,$(subst .cc,.cc.o,${SOURCES}))

TEST_SOURCES = \
			src/test/test.cc

TEST_OBJECTS = $(addprefix ${OBJDIR}/,$(subst .cc,.cc.o,${TEST_SOURCES}))

all: | format demo_0

demo_0: ${OBJECTS}
	${CXX} ${CXX_FLAGS} ${LINKER_FLAGS} -o $@ $^

test: $(filter-out ${OBJDIR}/src/main.cc.o,${OBJECTS}) ${TEST_OBJECTS}
	${CXX} ${CXX_FLAGS} ${LINKER_FLAGS} -o $@ $^

.PHONY: clean format

clean:
	rm -rf ${OBJDIR}
	rm -f demo_0
	rm -f test

format:
	clang-format -i --style=google ${HEADERS} ${SOURCES} ${TEST_SOURCES}

${OBJDIR}/%.cc.o: %.cc ${HEADERS}
	@mkdir -p $(dir $@)
	${CXX} -c ${CXX_FLAGS} -o $@ $<
