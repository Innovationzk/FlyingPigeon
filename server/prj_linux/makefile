
PROJROOT   = ../
HEADPATH   = $(PROJROOT)include/
SRCPATH    = $(PROJROOT)source/
COMMONPATH = $(PROJROOT)../common/
LIBPATH    = $(PROJROOT)../lib/Debug/
OUTPATH    = $(PROJROOT)linux/

TARGET     = $(OUTPATH)server
SRC        = $(wildcard $(SRCPATH)*.cpp)
OBJ        = $(addprefix $(OUTPATH), $(patsubst %.cpp,%.o,$(notdir $(SRC))))


g++ = g++ -m32 -I $(HEADPATH) -I $(COMMONPATH) -D _LINUX_ -D _DEBUG

$(TARGET): $(OBJ)
	$(g++) -o $@ $(OBJ) -L$(LIBPATH) -losp -ljsoncpp -lpthread
	export LD_LIBRARY_PATH=$(LIBPATH)

$(OUTPATH)%.o : $(SRCPATH)%.cpp $(COMMONPATH)commondef.h
	$(g++) -o $@ -c $<

.PHONY: clean print printsrc printobj cleantarget
clean :
	-rm $(OUTPATH)*.o $(TARGET)
cleantarget :
	-rm $(TARGET)
printobj :
	echo $(OBJ)
printsrc :
	echo $(SRC)












