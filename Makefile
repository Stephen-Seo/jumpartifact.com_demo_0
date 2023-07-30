ifdef DEBUG
	COMMON_FLAGS = -g -O0
else
	COMMON_FLAGS = -DNDEBUG -O3
endif

CXX_FLAGS = -Wall -Wextra -Wpedantic -Weffc++ ${COMMON_FLAGS}
LINKER_FLAGS = -lraylib

OBJDIR = objdir

SOURCES = \
		src/main.cc

OBJECTS = $(addprefix ${OBJDIR}/,$(subst .cc,.cc.o,${SOURCES}))

all: demo_0

demo_0: ${OBJECTS}
	${CXX} -o demo_0 ${LINKER_FLAGS} $^

.PHONY: clean format

clean:
	rm -rf ${OBJDIR}
	rm -f demo_0

.SECONDEXPANSION:

${OBJDIR}/%.cc.o: %.cc
	@mkdir -p $(dir $@)
	${CXX} -c ${CXX_FLAGS} -o $@ $^
