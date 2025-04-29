.PHONY: test clean
.SUFFIXES:

# --- Paths / files
SOURCE.d := source/
OBJECT.d := object/
LIB.d	 := library/

SOURCE := main.c opts.c sds.c renderer.c exporter.c
OBJECT := ${SOURCE}
OBJECT := $(subst .c,.o,${OBJECT})

GENSOURCE := dimensions.yy.c
GENOBJECT := ${GENSOURCE}
GENOBJECT := $(subst .c,.o,${GENOBJECT})

vpath %.c ${SOURCE.d}
vpath %.l ${SOURCE.d}
vpath %.yy.c ${OBJECT.d}
vpath %.o ${OBJECT.d}
vpath %.yy.o ${OBJECT.d}

OUT := example

# --- Tools/Flags
ifeq (${DEBUG}, 1)
  LFLAGS   += --debug --trace

  CPPFLAGS += -DDEBUG
  CFLAGS.D += -Wall -Wextra -Wpedantic
  CFLAGS.D += -O0 -ggdb -fno-inline
  CFLAGS.D += -fsanitize=address,undefined
  CFLAGS   += ${CFLAGS.D}
  CXXFLAGS += ${CFLAGS.D}
else
  CFLAGS += -O3 -flto=auto -fno-stack-protector
endif

CFLAGS += -std=c2x

CPPFLAGS += -I${SOURCE.d} -I${OBJECT.d} -I${LIB.d}
LDLIBS := -lpng

# --- Rule Section ---
all: ${OUT}

${OUT}: ${GENOBJECT} ${OBJECT}
	${LINK.c} -o $@ $(addprefix ${OBJECT.d}/,${OBJECT} ${GENOBJECT}) ${LDLIBS}

%.o: %.c
	${COMPILE.c} -o ${OBJECT.d}/$@ $<

%.yy.c: %.l
	flex -o ${OBJECT.d}/$@ --header=${OBJECT.d}/$(subst .c,.h,$@) $?

%.yy.o: %.yy.c
	${COMPILE.c} -o ${OBJECT.d}/$@ $< -fpermissive

test:
	cmdtest --fast

clean:
	-${RM} $(or ${OBJECT.d},#)/*
	-${RM} ${OUT}
