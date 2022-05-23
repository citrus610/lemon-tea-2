CXX = g++

ifeq ($(BUILD), debug)   
CXXFLAGS += -fdiagnostics-color=always -DUNICODE -std=c++20 -Wall -Og -g
else
CXXFLAGS += -DUNICODE -DNDEBUG -std=c++20 -O3 -flto -s -march=native
endif

ifeq ($(USE_PEXT), true)   
CXXFLAGS += -DUSE_PEXT
endif

STATIC_LIB = -luser32 -lgdi32 -lopengl32 -lgdiplus -lShlwapi -ldwmapi -lstdc++fs -lsetupapi -lhid -static

.PHONY: all client tbp ppt tuner clean makedir

all: client tbp ppt tuner

client: makedir
	@echo Building Lemon Tea client...
	@$(CXX) $(CXXFLAGS) "bot\*.cpp" "client\*.cpp" -o "bin\client\client.exe"
	@echo Finished building Lemon Tea client!

tbp: makedir
	@echo Building Lemon Tea tbp...
	@$(CXX) $(CXXFLAGS) "bot\*.cpp" "tbp\*.cpp" -o "bin\tbp\tbp.exe"
	@echo Finished building Lemon Tea tbp!

ppt: makedir
	@echo Building Lemon Tea Puyo Puyo Tetris...
	@$(CXX) $(CXXFLAGS) "bot\*.cpp" "ppt\*.cpp" "lib\ppt_sync\libppt_sync.dll.lib" $(STATIC_LIB) -o bin\ppt\ppt.exe
	@echo Finished building Lemon Tea Puyo Puyo Tetris!

tuner: makedir
	@echo Building Lemon Tea tuner...
	@$(CXX) $(CXXFLAGS) "bot\*.cpp" "tuner\*.cpp" -o "bin\tuner\tuner.exe"
	@echo Finished building Lemon Tea tuner!

clean: makedir
	@echo Cleaning the bin directory
	@rmdir /s /q bin
	@make makedir

makedir:
	@IF NOT exist bin ( mkdir bin )
	@IF NOT exist bin\client ( mkdir bin\client )
	@IF NOT exist bin\tbp ( mkdir bin\tbp )
	@IF NOT exist bin\ppt ( mkdir bin\ppt )
	@IF NOT exist bin\ppt\libppt_sync.dll ( copy lib\ppt_sync\libppt_sync.dll bin\ppt )
	@IF NOT exist bin\ppt\ppt-sync.exe ( copy lib\ppt_sync\ppt-sync.exe bin\ppt )
	@IF NOT exist bin\tuner ( mkdir bin\tuner )
	@IF NOT exist bin\tuner\gen ( mkdir bin\tuner\gen )

.DEFAULT_GOAL := client