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
		../src/walker.cc \
		../src/surface_triangle.cc \
		../src/screen_walker_hack.cc \
		../src/electricity_effect.cc \
		../src/spark_effect.cc \
		../src/common_constants.cc

HEADERS = \
		../src/ems.h \
		../src/game.h \
		../src/screen.h \
		../src/screen_test.h \
		../src/screen_trunner.h \
		../src/3d_helpers.h \
		../src/walker.h \
		../src/surface_triangle.h \
		../src/screen_walker_hack.h \
		../src/electricity_effect.h \
		../src/spark_effect.h \
		../src/common_constants.h

OBJECTS = $(addprefix ${OBJDIR}/,$(subst ..,PREVDIR,$(subst .cc,.cc.o,${SOURCES})))

EMSDK_ENV_SH ?= ${HOME}/git/emsdk/emsdk_env.sh

CXX = source ${EMSDK_ENV_SH} &>/dev/null && em++

all: | format out/jumpartifact.com_demo_0.html

raylib/lib/libraylib.a raylib/include/raylib.h raylib/include/raymath.h:
	../third_party_build/wasm_make_raylib.sh -o raylib -c raylib_git -e ${EMSDK_ENV_SH}

out/jumpartifact.com_demo_0.html: raylib/lib/libraylib.a raylib/include/raylib.h raylib/include/raymath.h ${OBJECTS} ${HEADERS}
	@mkdir -p out
	${CXX} -std=c++20 -o out/jumpartifact.com_demo_0.html \
		-s USE_GLFW=3 -Iraylib/include -Lraylib/lib -lraylib \
		--shell-file custom_shell.html \
		-sEXPORTED_FUNCTIONS=_main \
		-sEXPORTED_RUNTIME_METHODS=ccall \
		--preload-file ../res \
		${OTHER_FLAGS} \
		${OBJECTS}
	ln -s jumpartifact.com_demo_0.html out/index.html

.PHONY: clean format

clean:
	rm -rf out
	rm -rf "${OBJDIR}"

format:
	clang-format -i --style=google ${SOURCES} ${HEADERS}

.SECONDEXPANSION:

${OBJDIR}/%.cc.o: $$(subst PREVDIR,..,%.cc) ${HEADERS}
	@mkdir -p "$(dir $@)"
	${CXX} -std=c++20 -Iraylib/include -c ${OTHER_FLAGS} -o $@ $<
