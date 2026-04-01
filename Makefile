.PHONY: test clean
.SUFFIXES:

# --- Config
RENDERER ?= stb
#RENDERER ?= freetype
ifeq (${RENDERER}, stb)
  RENDERER_IMPL = renderer-stb.c
  CPPFLAGS += -DRENDERER='"STB static renderer"'
else ifeq (${RENDERER}, freetype)
  RENDERER_IMPL = renderer-freetype.c
  CPPFLAGS += $$(pkg-config --cflags freetype2)
  LDLIBS   += $$(pkg-config --libs freetype2)
  CPPFLAGS += -DRENDERER='"FreeType2 dynamic renderer"'
else
  $(error $$RENDERER must be [stb|freetype], but was '${RENDERER}'.)
endif

# --- Paths / files
SOURCE.d  := source/
OBJECT.d  := object/
LIBRARY.d := library/

SOURCE += main.c opts.c colorscheme.c ttf_quadruplet.c ${RENDERER_IMPL}
OBJECT := ${SOURCE}
OBJECT := $(subst .c,.o,${OBJECT})

GENSOURCE := dimensions.yy.c xeen.yy.c
GENOBJECT := ${GENSOURCE}
GENOBJECT := $(subst .c,.o,${GENOBJECT})

vpath %.c ${SOURCE.d}
vpath %.l ${SOURCE.d}
vpath %.yy.c ${OBJECT.d}
vpath %.o ${OBJECT.d}
vpath %.yy.o ${OBJECT.d}

OUT := xeen

# --- Tools/Flags
CFLAGS.D += -Wall -Wextra -Wpedantic
CFLAGS += -std=c23

CPPFLAGS += -D_GNU_SOURCE
CPPFLAGS += -I${SOURCE.d} -I${OBJECT.d} -I${LIBRARY.d}
CPPFLAGS += --embed-dir=resource/dejavu/
LDLIBS   += -lm

ifeq (${DEBUG}, 1)
  LFLAGS   += --debug --trace

  CPPFLAGS += -DDEBUG
  CFLAGS.D += -O0 -ggdb -fno-inline
  CFLAGS.D += -fsanitize=address,undefined
  CFLAGS   += ${CFLAGS.D}
  CXXFLAGS += ${CFLAGS.D}
else
  CFLAGS += -O3 -g -flto=auto -fno-stack-protector
endif

# --- Rule Section ---
all: ${OUT}

${OUT}: ${GENOBJECT} ${OBJECT}
	${LINK.c} -o $@ $(addprefix ${OBJECT.d}/,${OBJECT} ${GENOBJECT}) ${LDLIBS}

%.o: %.c
	${COMPILE.c} -o ${OBJECT.d}/$@ $<

%.yy.c: %.l
	flex ${LFLAGS} -o ${OBJECT.d}/$@ --header=${OBJECT.d}/$(subst .c,.h,$@) $?

%.yy.o: %.yy.c
	${COMPILE.c} -o ${OBJECT.d}/$@ ${OBJECT.d}/$< -fpermissive -Wno-sign-compare

test:
	cmdtest --fast

clean:
	-${RM} $(or ${OBJECT.d},#)/*
	-${RM} ${OUT}
