#
#Makefile
#
#ThisisaMakefileSkeletonfortheprogrammerproject
#
#Copyright(C)2014byWilliamP.Foster,allrightsreserved
#
#
# Generated via Programmer, a programmer's utility.
#
DebugDir=Debug
ReleaseDir=Release

ALL_H = BackupSources.h Codify.h Header.h MakeMakefile.h  \
		PLex.h SourceCleaner.h stdafx.h  

ALL_CPP = src/BackupSources.cpp src/Flow.cpp src/Header.cpp src/MakeMakefile.cpp  \
		src/plex.cpp src/programmer.cpp src/SourceCleaner.cpp src/stdafx.cpp  

ALL_OBJ = src/BackupSources.o src/Flow.o src/Header.o src/MakeMakefile.o  \
		src/plex.o src/programmer.o src/SourceCleaner.o src/stdafx.o  

ALL_DEBUG_OBJ = $(DebugDir)/BackupSources.o $(DebugDir)/Flow.o $(DebugDir)/Header.o $(DebugDir)/MakeMakefile.o  \
		$(DebugDir)/plex.o $(DebugDir)/programmer.o $(DebugDir)/SourceCleaner.o $(DebugDir)/stdafx.o  

ALL_RELEASE_OBJ = $(ReleaseDir)/BackupSources.o $(ReleaseDir)/Flow.o $(ReleaseDir)/Header.o $(ReleaseDir)/MakeMakefile.o  \
		$(ReleaseDir)/plex.o $(ReleaseDir)/programmer.o $(ReleaseDir)/SourceCleaner.o $(ReleaseDir)/stdafx.o  

srcdir = src
BIN_INSTALL = /usr/local/bin
LIBIN = $(HOME)/Libraries/Unix
SOIN = $(HOME)/Libraries/Unix
EXLIB = -ldl -lpthread -lrt -lm
FOXSO = -lFOX-1.6
# FOXSO = /usr/local/lib/libFOX-1.6.so -lGLU -lGL -lz
FOXLIB = -L/usr/X11R6/lib -lXrandr $(SOIN)/libFOX-1.6.a -lm
includes = ../../Engines/BFC/include
DEFS = -D UNIX
CCPP = gcc
CPPDEBUG = -g -D _DEBUG
CPPFLAGS = -I$(srcdir) -I$(includes) -fPIC $(DEFS)

DEBUG_BFC = $(SOIN)/BFC_d.so.1 $(SOIN)/BFCScript_d.so.1 $(SOIN)/BFCGUI_d.so.1 $(SOIN)/BFCOS_d.so.1 $(SOIN)/BFCData_d.so.1 $(SOIN)/BFCVector_d.so.1 $(SOIN)/BFCImage_d.so.1
RELEASE_BFC = $(LIBIN)/BFC.lib $(LIBIN)/BFCScript.lib $(LIBIN)/BFCGUI.lib $(LIBIN)/BFCOS.lib $(LIBIN)/BFCData.lib $(LIBIN)/BFCVector.lib $(LIBIN)/BFCImage.lib

# ifeq ($(CC),gcc)
#   libs=$(libs_for_gcc)
# else
#   libs=$(normal_libs)
# endif

Debug/%.o ($srcdir)/%.cpp :
	$(CCPP) -c $(CPPFLAGS) $(CPPDEBUG) $< -o $@

Release/%.o ($srcdir)/%.cpp :
	$(CCPP) -c $(CPPFLAGS) $< -o $@

all : create debug 

backup:
	programmer -b .

create :
	mkdir -p Debug
	mkdir -p Release
	programmer -q -mp src

clean :
	rm -f Debug/*.*
	rm -f Release/*.*

head :
	programmer -h header src/*.cpp
	programmer -h header src/*.h

bfcscript :
	./makebfcscript

debug : $(ALL_DEBUG_OBJ)
	g++ -o Debug/programmer.bin $(ALL_DEBUG_OBJ) $(FOXSO) $(DEBUG_BFC)  $(EXLIB)

release : $(ALL_RELEASE_OBJ)
	g++ -o Release/programmer.bin $(ALL_RELEASE_OBJ) $(RELEASE_BFC) $(FOXSO) $(EXLIB)

#	g++ -Wl -o Release/programmer.bin $(ALL_RELEASE_OBJ) $(SOIN)/BFC.so $(SOIN)/BFCScript.so $(SOIN)/BFCOS.so $(SOIN)/BFCGUI.so $(SOIN)/BFCData.so $(SOIN)/BFCImage.so $(SOIN)/BFCVector.so $(FOXLIB)

install: release
	sudo cp Release/programmer.bin $(BIN_INSTALL)/programmer



$(DebugDir)/BackupSources.o : src/BackupSources.cpp 
		$(CCPP) -c $(CPPFLAGS) $(CPPDEBUG) -o $(DebugDir)/BackupSources.o src/BackupSources.cpp

$(ReleaseDir)/BackupSources.o : src/BackupSources.cpp 
		$(CCPP) -c $(CPPFLAGS) -o $(ReleaseDir)/BackupSources.o src/BackupSources.cpp

$(DebugDir)/Flow.o : src/Flow.cpp 
		$(CCPP) -c $(CPPFLAGS) $(CPPDEBUG) -o $(DebugDir)/Flow.o src/Flow.cpp

$(ReleaseDir)/Flow.o : src/Flow.cpp 
		$(CCPP) -c $(CPPFLAGS) -o $(ReleaseDir)/Flow.o src/Flow.cpp

$(DebugDir)/Header.o : src/Header.cpp 
		$(CCPP) -c $(CPPFLAGS) $(CPPDEBUG) -o $(DebugDir)/Header.o src/Header.cpp

$(ReleaseDir)/Header.o : src/Header.cpp 
		$(CCPP) -c $(CPPFLAGS) -o $(ReleaseDir)/Header.o src/Header.cpp

$(DebugDir)/MakeMakefile.o : src/MakeMakefile.cpp 
		$(CCPP) -c $(CPPFLAGS) $(CPPDEBUG) -o $(DebugDir)/MakeMakefile.o src/MakeMakefile.cpp

$(ReleaseDir)/MakeMakefile.o : src/MakeMakefile.cpp 
		$(CCPP) -c $(CPPFLAGS) -o $(ReleaseDir)/MakeMakefile.o src/MakeMakefile.cpp

$(DebugDir)/plex.o : src/plex.cpp 
		$(CCPP) -c $(CPPFLAGS) $(CPPDEBUG) -o $(DebugDir)/plex.o src/plex.cpp

$(ReleaseDir)/plex.o : src/plex.cpp 
		$(CCPP) -c $(CPPFLAGS) -o $(ReleaseDir)/plex.o src/plex.cpp

$(DebugDir)/programmer.o : src/programmer.cpp 
		$(CCPP) -c $(CPPFLAGS) $(CPPDEBUG) -o $(DebugDir)/programmer.o src/programmer.cpp

$(ReleaseDir)/programmer.o : src/programmer.cpp 
		$(CCPP) -c $(CPPFLAGS) -o $(ReleaseDir)/programmer.o src/programmer.cpp

$(DebugDir)/SourceCleaner.o : src/SourceCleaner.cpp 
		$(CCPP) -c $(CPPFLAGS) $(CPPDEBUG) -o $(DebugDir)/SourceCleaner.o src/SourceCleaner.cpp

$(ReleaseDir)/SourceCleaner.o : src/SourceCleaner.cpp 
		$(CCPP) -c $(CPPFLAGS) -o $(ReleaseDir)/SourceCleaner.o src/SourceCleaner.cpp

$(DebugDir)/stdafx.o : src/stdafx.cpp 
		$(CCPP) -c $(CPPFLAGS) $(CPPDEBUG) -o $(DebugDir)/stdafx.o src/stdafx.cpp

$(ReleaseDir)/stdafx.o : src/stdafx.cpp 
		$(CCPP) -c $(CPPFLAGS) -o $(ReleaseDir)/stdafx.o src/stdafx.cpp
