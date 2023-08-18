ifdef RELEASE
	OTHER_FLAGS = -DNDEBUG -O3
else
	OTHER_FLAGS = -Og
endif

OBJDIR = objdir

SOURCES = \
		../src/main.cc \
		../src/ems.cc \
		../src/game.cc \
		../src/screen.cc \
		../src/screen_test.cc \
		../src/screen_trunner.cc \
		../src/3d_helpers.cc \
		../src/raymath.cc \
		../src/walker.cc

HEADERS = \
		../src/ems.h \
		../src/game.h \
		../src/screen.h \
		../src/screen_test.h \
		../src/screen_trunner.h \
		../src/3d_helpers.h \
		../src/walker.h

OBJECTS = $(addprefix ${OBJDIR}/,$(subst ..,PREVDIR,$(subst .cc,.cc.o,${SOURCES})))

CXX = source ${HOME}/git/emsdk/emsdk_env.sh && em++

all: | format jumpartifact.com_demo_0.html

jumpartifact.com_demo_0.html: ${OBJECTS} ${HEADERS}
	${CXX} -std=c++20 -o jumpartifact.com_demo_0.html \
		-s USE_GLFW=3 -I../wasm_include -L../wasm_lib -lraylib \
		--shell-file custom_shell.html \
		-sEXPORTED_FUNCTIONS=_main \
		-sEXPORTED_RUNTIME_METHODS=ccall \
		--preload-file ../res \
		${OTHER_FLAGS} \
		${OBJECTS}

.PHONY: clean format

clean:
	rm -f jumpartifact.com_demo_0.html
	rm -f jumpartifact.com_demo_0.js
	rm -f jumpartifact.com_demo_0.wasm
	rm -f jumpartifact.com_demo_0.data
	rm -rf "${OBJDIR}"

format:
	clang-format -i --style=google ${SOURCES} ${HEADERS}

.SECONDEXPANSION:

${OBJDIR}/%.cc.o: $$(subst PREVDIR,..,%.cc) ${HEADERS}
	@mkdir -p "${OBJDIR}/$(dir $(subst ..,PREVDIR,$<))"
	${CXX} -std=c++20 -I../wasm_include -c ${OTHER_FLAGS} -o $@ $<
