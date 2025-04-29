.PHONY: test clean
.SUFFIXES:

# --- Paths / files
SOURCE.d := source/
OBJECT.d := object/
LIB.d	 := library/

SOURCE := main.c opts.c
OBJECT := ${SOURCE}
OBJECT := $(subst .cpp,.o,${OBJECT})
OBJECT := $(subst .c,.o,${OBJECT})

GENSOURCE :=

vpath %.c ${SOURCE.d}
vpath %.l ${SOURCE.d}
vpath %.yy.c ${OBJECT.d}

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
LDLIBS += -ldictate # XXX

# --- Rule Section ---
all: ${OUT}

${OUT}: ${GENSOURCE} ${OBJECT}
	${LINK.cpp} -o $@ $(addprefix ${OBJECT.d}/,${OBJECT} ${GENSOURCE}) ${LDLIBS}

%.o: %.c
	${COMPILE.c} -o ${OBJECT.d}/$@ $<

%.o: %.cpp
	${COMPILE.cpp} -o ${OBJECT.d}/$@ $<

%.yy.c: %.l
	flex -o ${OBJECT.d}/$@ --header=${OBJECT.d}/$(subst .c,.h,$@) $? 

%.yy.o: ${OBJECT.d}/%.yy.c
	${COMPILE.c} -o $@ $<

test:
	cmdtest --fast

clean:
	-${RM} $(or ${OBJECT.d},#)/*
	-${RM} ${OUT}
