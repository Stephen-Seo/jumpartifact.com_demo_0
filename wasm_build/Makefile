ifdef RELEASE
	OTHER_FLAGS = -DNDEBUG -O3
else
	OTHER_FLAGS = -O0
endif

SOURCES = \
		../src/main.cc \
		../src/ems.cc \
		../src/game.cc \
		../src/screen.cc \
		../src/screen_test.cc

HEADERS = \
		../src/ems.h \
		../src/game.h \
		../src/screen.h \
		../src/screen_test.h

CXX = source ${HOME}/git/emsdk/emsdk_env.sh && em++

all: | format jumpartifact.com_demo_0.html

jumpartifact.com_demo_0.html: ${SOURCES} ${HEADERS}
	${CXX} -o jumpartifact.com_demo_0.html \
		-s USE_GLFW=3 -I../wasm_include -L../wasm_lib -lraylib \
		--shell-file custom_shell.html \
		-sEXPORTED_FUNCTIONS=_main \
		-sEXPORTED_RUNTIME_METHODS=ccall \
		${OTHER_FLAGS} \
		${SOURCES}

.PHONY: clean format

clean:
	rm -f jumpartifact.com_demo_0.html
	rm -f jumpartifact.com_demo_0.js
	rm -f jumpartifact.com_demo_0.wasm
	rm -f jumpartifact.com_demo_0.data

format:
	clang-format -i --style=google ${SOURCES} ${HEADERS}
