CPP = g++-4.8
TARGET = gravebot_mm
ARCHFLAG = i686
BASEFLAGS = -Dstricmp=strcasecmp -Dstrcmpi=strcasecmp -DMETAMOD_BUILD
OPTFLAGS = 
CPPFLAGS = ${BASEFLAGS} ${OPTFLAGS} -march=${ARCHFLAG} \
	-mtune=generic -msse -msse2 -O2 -m32 -mfpmath=sse -w \
	-I"../common" -I"../dlls" -I"../engine" -I"../pm_shared" -I"../metamod" 

OBJ = 	bot.o \
	bot_client.o \
	bot_combat.o \
	bot_navigate.o \
	bot_start.o \
	bot_weapons.o \
	bot_weapons_hldm.o \
	bot_weapons_si.o \
	dll.o \
	engine.o \
	h_export.o \
	util.o \
	waypoint.o

${TARGET}.so: ${OBJ}
	${CPP} -fPIC -shared -static -o $@ ${OBJ} -Xlinker -Map -Xlinker ${TARGET}.map -ldl
	strip -s ${TARGET}.so 
#	mv *.o Release_Metamod
#	mv *.map Release_Metamod
#	mv $@ Release_Metamod

clean:
	rm -f *.o
	rm -f *.so
	rm -f *.map

distclean:
	rm -rf Release_Metamod
	mkdir Release_Metamod	

%.o:	%.cpp
	${CPP} ${CPPFLAGS} -c $< -o $@

%.o:	%.c
	${CPP} ${CPPFLAGS} -c $< -o $@
