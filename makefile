# Made by 0x223252
# All rigth reserved
include .config/*

# project root folder
ifeq ($(ROOT_DIR),)
	ROOT_DIR=.
endif
# source folder name
ifeq ($(SOURCE_DIR),)
	SOURCE_DIR=src
endif
# exec folder name
ifeq ($(OUTFOLDER),)
	OUTFOLDER=bin
endif
# exec file name
ifeq ($(EXEC),)
	EXEC=exec
endif
# debug / release
ifeq ($(MODE),)
	MODE=debug
endif

ifneq ($(HARD_ARCH),natif)
	CXX=$($(HARD_ARCH)_CROSS_CXX)
	CC=$($(HARD_ARCH)_CROSS_CC)
	FLAGS+=$($(HARD_ARCH)_FLAGS)
	LDFLAGS+=$($(HARD_ARCH)_LIBS)
	EXEC_AFTER=$($(HARD_ARCH)_EXEC_AFTER)
else
	LDFLAGS+=$(natif_LIBS)
	FLAGS+=$(natif_FLAGS)
	EXEC_AFTER=$(natif_EXEC_AFTER)
endif

ifeq ($(MODE),release)
	FLAGS+= -D'MODE_RELEASE'
else
	FLAGS+= -D'MODE_DEBUG'
endif

ifeq ($(LINKAGE),static)
	LDFLAGS+= -static
endif

ifeq ($(OUT_DLL),dynamic)
	FLAGS+= -fPIC
	LDFLAGS+= -shared -fPIC
endif

ifeq ($(WARNING),all)
	FLAGS+= -Wall -Wshadow -Wextra -fexceptions
else ifeq ($(WARNING), none)
	FLAGS+= -w
endif

ifeq ($(GPROF),on)
	FLAGS+= -pg
	LDFLAGS+= -pg
endif

ifeq ($(GDB),on)
	FLAGS+= -g
	LDFLAGS+= -g
endif

LAST_HARD_ARCH=null
LAST_HARD_ARCH=$(shell [ -f ".last_config" ] && cat .last_config | grep ^HARD_ARCH | cut -d '=' -f2-)
EQ_HARD_ARCH=$(shell [ -f ".last_config" ] && [ "$(HARD_ARCH)" = "$(LAST_HARD_ARCH)" ] && echo true)

ifneq ($(EQ_HARD_ARCH),true)
	DEPEND=mrproper
endif

LAST_MODE=null
LAST_MODE=$(shell [ -f ".last_config" ] && cat .last_config | grep ^MODE | cut -d '=' -f2-)
EQ_MODE=$(shell [ -f ".last_config" ] && [ "$(MODE)" = "$(LAST_MODE)" ] && echo true)

ifneq ($(EQ_MODE),true)
	DEPEND=mrproper
endif

LAST_WARNING=null
LAST_WARNING=$(shell [ -f ".last_config" ] && cat .last_config | grep ^WARNING | cut -d '=' -f2-)
EQ_WARNING=$(shell [ -f ".last_config" ] && [ "$(WARNING)" = "$(LAST_WARNING)" ] && echo true)

ifneq ($(EQ_WARNING),true)
	DEPEND=mrproper
endif

LAST_GPROF=null
LAST_GPROF=$(shell [ -f ".last_config" ] && cat .last_config | grep ^GPROF | cut -d '=' -f2-)
EQ_GPROF=$(shell [ -f ".last_config" ] && [ "$(GPROF)" = "$(LAST_GPROF)" ] && echo true)

ifneq ($(EQ_GPROF),true)
	DEPEND=mrproper
endif

LAST_GDB=null
LAST_GDB=$(shell [ -f ".last_config" ] && cat .last_config | grep ^GDB | cut -d '=' -f2-)
EQ_GDB=$(shell [ -f ".last_config" ] && [ "$(GDB)" = "$(LAST_GDB)" ] && echo true)

ifneq ($(EQ_GDB),true)
	DEPEND=mrproper
endif

LAST_LINKAGE=null
LAST_LINKAGE=$(shell [ -f ".last_config" ] && cat .last_config | grep ^LINKAGE | cut -d '=' -f2-)
EQ_LINKAGE=$(shell [ -f ".last_config" ] && [ "$(LINKAGE)" = "$(LAST_LINKAGE)" ] && echo true)

ifneq ($(EQ_LINKAGE),true)
	DEPEND=mrproper
endif

LAST_OUT_DLL=null
LAST_OUT_DLL=$(shell [ -f ".last_config" ] && cat .last_config | grep ^OUT_DLL | cut -d '=' -f2-)
EQ_OUT_DLL=$(shell [ -f ".last_config" ] && [ "$(OUT_DLL)" = "$(LAST_OUT_DLL)" ] && echo true)

ifneq ($(EQ_OUT_DLL),true)
	DEPEND=mrproper
endif

LAST_FLAGS=null
LAST_FLAGS=$(shell [ -f ".last_config" ] && cat .last_config | grep ^FLAGS | cut -d '=' -f2-)
EQ_FLAGS=$(shell [ -f ".last_config" ] && [ "$(FLAGS)" = "$(LAST_FLAGS)" ] && echo true)

ifneq ($(EQ_FLAGS),true)
	DEPEND=mrproper
endif

ifeq ($(pthread),on)
	LDFLAGS+= -lpthread
endif

ifeq ($(sdl.1),on)
	LDFLAGS+= -lSDL
endif

ifeq ($(sdl.2),on)
	LDFLAGS+= -lSDL2
endif

ifeq ($(realTime),on)
	LDFLAGS+= -lrt
endif

ifeq ($(math),on)
	LDFLAGS+= -lm
endif

ifeq ($(ssl),on)
	LDFLAGS+= -lssl
endif

ifeq ($(cares),on)
	LDFLAGS+= -lcares
endif

ifeq ($(crypto),on)
	LDFLAGS+= -lcrypto
endif

ifeq ($(use_DLL),on)
	LDFLAGS+= -ldl
endif

ifeq ($(create_DLL),on)
	LDFLAGS+= -shared
endif

ifeq ($(FULL_CPP),on)
	CC=$(CXX)
endif

CPP_SRC=$(shell find ./$(SOURCE_DIR) -name *.cpp)
CPP_OBJ=$(CPP_SRC:.cpp=.o)

C_SRC=$(shell find ./$(SOURCE_DIR) -name *.c)
C_OBJ=$(C_SRC:.c=.o)

OBJ=$(C_OBJ) $(CPP_OBJ)

all: testVar $(ROOT_DIR) cible $(DEPEND) makedeps $(ROOT_DIR)/$(EXEC) $(CONFIG) $(ROOT_DIR)/$(OUTFOLDER) exec_after
	@printf "+=======================================+\n"
	@printf "|      \033[1;92m%-22s\033[0m           |\n" "Buid done"
	@printf "| for hard_arch : \033[1;92m%-22s\033[0m|\n" "$(shell file $(ROOT_DIR)/$(EXEC) | cut -d ',' -f2)"
	@printf "| in mode : \033[1;92m%-22s\033[0m      |\n" "$(MODE)"
	@printf "+=======================================+\n"

testVar:
	@echo "assert input var, do not remove this else it should be \033[1;31mdangerous\033[0m"
ifeq ($(EXEC),)
	@echo "\\033[1;91mEXEC: $(EXEC) not valide\\033[0m"
	@exit 1
endif
ifeq ($(EXEC),/)
	@echo "\\033[1;91mEXEC: $(EXEC) not valide\\033[0m"
	@exit 1
endif
ifeq ($(OUTFOLDER),)
	@echo "\\033[1;91mOUTFOLDER: $(OUTFOLDER) not valide\\033[0m"
	@exit 1
endif
ifeq ($(OUTFOLDER),/)
	@echo "\\033[1;91mOUTFOLDER: $(OUTFOLDER) not valide\\033[0m"
	@exit 1
endif
ifeq ($(ROOT_DIR),)
	@echo "\\033[1;91mROOT_DIR: $(ROOT_DIR) not valide\\033[0m"
	@exit 1
endif
ifeq ($(ROOT_DIR),/)
	@echo "\\033[1;91mROOT_DIR: $(ROOT_DIR) not valide\\033[0m"
	@exit 1
endif
ifeq ($(DOC_DIR),)
	@echo "\\033[1;91mDOC_DIR: $(DOC_DIR) not valide\\033[0m"
	@exit 1
endif
ifeq ($(DOC_DIR),/)
	@echo "\\033[1;91mDOC_DIR: $(DOC_DIR) not valide\\033[0m"
	@exit 1
endif
ifeq ($(SOURCE_DIR),)
	@echo "\\033[1;91mSOURCE_DIR: $(SOURCE_DIR) not valide\\033[0m"
	@exit 1
endif
ifeq ($(SOURCE_DIR),/)
	@echo "\\033[1;91mSOURCE_DIR: $(SOURCE_DIR) not valide\\033[0m"
	@exit 1
endif
ifeq ($(PATCH_DIR),)
	@echo "\\033[1;91mPATCH_DIR: $(PATCH_DIR) not valide\\033[0m"
	@exit 1
endif
ifeq ($(PATCH_DIR),/)
	@echo "\\033[1;91mPATCH_DIR: $(PATCH_DIR) not valide\\033[0m"
	@exit 1
endif
ifeq ($(RESSOURCES_DIR),)
	@echo "\\033[1;91mRESSOURCES_DIR: $(RESSOURCES_DIR) not valide\\033[0m"
	@exit 1
endif
ifeq ($(RESSOURCES_DIR),/)
	@echo "\\033[1;91mRESSOURCES_DIR: $(RESSOURCES_DIR) not valide\\033[0m"
	@exit 1
endif
ifeq ($(CONFIG_DIR),)
	@echo "\\033[1;91mCONFIG_DIR: $(CONFIG_DIR) not valide\\033[0m"
	@exit 1
endif
ifeq ($(CONFIG_DIR),/)
	@echo "\\033[1;91mCONFIG_DIR: $(CONFIG_DIR) not valide\\033[0m"
	@exit 1
endif
ifeq ($(LIB_DIR),)
	@echo "\\033[1;91mLIB_DIR: $(LIB_DIR) not valide\\033[0m"
	@exit 1
endif
ifeq ($(LIB_DIR),/)
	@echo "\\033[1;91mLIB_DIR: $(LIB_DIR) not valide\\033[0m"
	@exit 1
endif
	@echo "\033[1;92mVar tests valides, continue\033[0m"

$(ROOT_DIR):
	@$(shell [ -d "$(ROOT_DIR)" ] || mkdir -p "$(ROOT_DIR)" )

$(ROOT_DIR)/$(OUTFOLDER): $(ROOT_DIR)
	@$(shell [ ! -d "$(ROOT_DIR)/$(OUTFOLDER)" ] && mkdir -p "$(ROOT_DIR)/$(OUTFOLDER)" )

exec_after:
	$(EXEC_AFTER)

cible:
	@echo  > .last_config
	@echo "HARD_ARCH=$(HARD_ARCH)" >> .last_config
	@echo "MODE=$(MODE)" >> .last_config
	@echo "WARNING=$(WARNING)" >> .last_config
	@echo "GPROF=$(GPROF)" >> .last_config
	@echo "GDB=$(GDB)" >> .last_config
	@echo "LINKAGE=$(LINKAGE)" >> .last_config
	@echo "OUT_DLL=$(OUT_DLL)" >> .last_config
	@echo "FLAGS=$(FLAGS)" >> .last_config

docs:
	@doxywizard doc/Doxyfile

$(ROOT_DIR)/$(EXEC): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(FLAGS) -DDATE_BUILD="\"$(shell date +'%Y.%m.%d %H:%M:%S')\""
%.o: %.cpp
	$(CXX) -c -o $@ $< $(FLAGS) -DDATE_BUILD="\"$(shell date +'%Y.%m.%d %H:%M:%S')\""


clean:
	rm -f $(shell find ./$(SOURCE_DIR)/ -name *.o) $(shell find ./$(SOURCE_DIR)/ -name *.so)

cleanDoc:
	rm -f -r doc/html/*
	rm -f -r doc/latex/*
	rm -f -r doc/xml/*
	rm -f -r doc/man/*
	rm -f -r doc/rtf/*

cleanDump:
	rm -f -r $(ROOT_DIR)/$(OUTFOLDER)/*

mrproper: clean cleanDump cleanDoc
	rm -f $(ROOT_DIR)/$(EXEC)

empty: mrproper
	@echo "#!/bin/bash" > script.sh
	@echo 'rm -r $$(ls -a | grep -v -E "(^((\.+)|(Configure)|(configure)|($(SOURCE_DIR))|($(PATCH_DIR))|($(RESSOURCES_DIR))|($(CONFIG_DIR))|($(LIB_DIR)))$$|(^\.git)|(^README)|(^readme))")' >> script.sh
	
	@cat script.sh | grep rm

	@chmod +x script.sh
	@./script.sh

makedeps:
	@makedepend -Y $(C_SRC) $(CPP_SRC) 2> /dev/null

$(CONFIG):
	@[ ! -f $(CONFIG) ] && echo > $(CONFIG)

printEnv:
	@echo "LAST_HARD_ARCH=$(LAST_HARD_ARCH)"
	@echo "HARD_ARCH=$(HARD_ARCH)"
	@echo "EQ_HARD_ARCH=\033[1;93m$(EQ_HARD_ARCH)\033[0m"
	@echo "LAST_MODE=$(LAST_MODE)"
	@echo "MODE=$(MODE)"
	@echo "EQ_MODE=\033[1;93m$(EQ_MODE)\033[0m"
	@echo "LAST_WARNING=$(LAST_WARNING)"
	@echo "WARNING=$(WARNING)"
	@echo "EQ_WARNING=\033[1;93m$(EQ_WARNING)\033[0m"
	@echo "LAST_GPROF=$(LAST_GPROF)"
	@echo "GPROF=$(GPROF)"
	@echo "EQ_GPROF=\033[1;93m$(EQ_GPROF)\033[0m"
	@echo "LAST_GDB=$(LAST_GDB)"
	@echo "GDB=$(GDB)"
	@echo "EQ_GDB=\033[1;93m$(EQ_GDB)\033[0m"
	@echo "LAST_LINKAGE=$(LAST_LINKAGE)"
	@echo "LINKAGE=$(LINKAGE)"
	@echo "EQ_LINKAGE=\033[1;93m$(EQ_LINKAGE)\033[0m"
	@echo "LAST_OUT_DLL=$(LAST_OUT_DLL)"
	@echo "OUT_DLL=$(OUT_DLL)"
	@echo "EQ_OUT_DLL=\033[1;93m$(EQ_OUT_DLL)\033[0m"
	@echo "LAST_FLAGS=$(LAST_FLAGS)"
	@echo "FLAGS=$(FLAGS)"
	@echo "EQ_FLAGS=\033[1;93m$(EQ_FLAGS)\033[0m"
# DO NOT DELETE

./src/Client_TCP/clientTCP.o: ./src/Client_TCP/clientTCP.h
./src/Client_TCP/clientTCP.o: ./src/struct/vector.h ./src/Agent/agent.h
./src/Client_TCP/clientTCP.o: ./src/struct/agent.h ./src/struct/vector.h
./src/Client_TCP/clientTCP.o: ./src/struct/slot.h ./src/struct/pattern.h
./src/Client_TCP/clientTCP.o: ./src/parserXML/parserXML.h ./src/mxml/mxml.h
./src/Client_TCP/clientTCP.o: ./src/struct/pattern.h
./src/Client_TCP/clientTCP.o: ./src/Agent/gestionWatcher.h
./src/Client_TCP/clientTCP.o: ./src/Agent/agent.h ./src/Agent/gestionSlot.h
./src/Client_TCP/clientTCP.o: ./src/freeOnExit/freeOnExit.h ./src/log/log.h
./src/mxml/mxml-node.o: ./src/mxml/config.h ./src/mxml/mxml.h
./src/mxml/mxml-file.o: ./src/mxml/mxml-private.h ./src/mxml/config.h
./src/mxml/mxml-file.o: ./src/mxml/mxml.h
./src/mxml/mxml-index.o: ./src/mxml/config.h ./src/mxml/mxml.h
./src/mxml/mxml-entity.o: ./src/mxml/mxml-private.h ./src/mxml/config.h
./src/mxml/mxml-entity.o: ./src/mxml/mxml.h
./src/mxml/mxml-set.o: ./src/mxml/config.h ./src/mxml/mxml.h
./src/mxml/mxml-attr.o: ./src/mxml/config.h ./src/mxml/mxml.h
./src/mxml/mxml-private.o: ./src/mxml/mxml-private.h ./src/mxml/config.h
./src/mxml/mxml-private.o: ./src/mxml/mxml.h
./src/mxml/mmd.o: ./src/mxml/mmd.h
./src/mxml/mxml-get.o: ./src/mxml/config.h ./src/mxml/mxml.h
./src/mxml/mxml-search.o: ./src/mxml/config.h ./src/mxml/mxml.h
./src/mxml/mxml-string.o: ./src/mxml/config.h
./src/freeOnExit/freeOnExit.o: ./src/freeOnExit/freeOnExit.h
./src/signalHandler/signalHandler.o: ./src/signalHandler/signalHandler.h
./src/Agent/gestionWatcher.o: ./src/Agent/gestionWatcher.h
./src/Agent/gestionWatcher.o: ./src/Agent/agent.h ./src/struct/slot.h
./src/Agent/gestionWatcher.o: ./src/struct/vector.h ./src/struct/pattern.h
./src/Agent/gestionWatcher.o: ./src/struct/pattern.h ./src/mxml/mxml.h
./src/Agent/gestionSlot.o: ./src/Agent/gestionSlot.h ./src/Agent/agent.h
./src/Agent/gestionSlot.o: ./src/struct/slot.h ./src/struct/vector.h
./src/Agent/gestionSlot.o: ./src/struct/pattern.h ./src/struct/pattern.h
./src/Agent/gestionOrganizer.o: ./src/Agent/gestionOrganizer.h
./src/Agent/gestionOrganizer.o: ./src/Agent/agent.h ./src/struct/slot.h
./src/Agent/gestionOrganizer.o: ./src/struct/vector.h ./src/struct/pattern.h
./src/Agent/gestionOrganizer.o: ./src/struct/pattern.h
./src/Agent/agent.o: ./src/Agent/agent.h
./src/log/log.o: ./src/log/log.h
./src/config/config_arg.o: ./src/config/config_arg.h
./src/config/config_arg.o: ./src/config/config_data.h
./src/config/config_file.o: ./src/config/config_file.h
./src/config/config_file.o: ./src/config/config_data.h
./src/parserXML/parserXML.o: ./src/parserXML/parserXML.h ./src/mxml/mxml.h
./src/parserXML/parserXML.o: ./src/struct/pattern.h ./src/struct/vector.h
./src/struct/vector.o: ./src/struct/vector.h
./src/sharedMem/sharedMem.o: ./src/freeOnExit/freeOnExit.h
./src/main.o: ./src/./signalHandler/signalHandler.h ./src/./log/log.h
./src/main.o: ./src/./freeOnExit/freeOnExit.h ./src/./config/config_arg.h
./src/main.o: ./src/config/config_data.h ./src/./config/config_file.h
./src/main.o: ./src/./parserXML/parserXML.h ./src/mxml/mxml.h
./src/main.o: ./src/struct/pattern.h ./src/struct/vector.h
./src/main.o: ./src/./Client_TCP/clientTCP.h ./src/Agent/agent.h
./src/main.o: ./src/struct/agent.h ./src/struct/vector.h ./src/struct/slot.h
./src/main.o: ./src/struct/pattern.h ./src/parserXML/parserXML.h
./src/main.o: ./src/Agent/gestionWatcher.h ./src/Agent/agent.h
./src/main.o: ./src/Agent/gestionSlot.h ./src/./struct/vector.h
./src/main.o: ./src/./struct/slot.h ./src/./Agent/agent.h
