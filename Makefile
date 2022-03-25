CXX = g++
ifeq ($(BUILD),debug)   
CXXFLAGS += -fdiagnostics-color=always -DUNICODE -std=c++20 -Wall -Og -g
else
CXXFLAGS += -DUNICODE -DNDEBUG -std=c++20 -O3 -flto -s -march=native
endif

STATIC_LIB = -luser32 -lgdi32 -lopengl32 -lgdiplus -lShlwapi -ldwmapi -lstdc++fs -lsetupapi -lhid -static

.PHONY: all client tbp clean makedir

all: client

client: makedir
	@echo Building Lemon Tea client...
	@$(CXX) $(CXXFLAGS) "bot\*.cpp" "client\*.cpp" -o "bin\client\client.exe"
	@echo Finished building Lemon Tea client!

tbp: makedir
	@echo Building Lemon Tea tbp...
	@$(CXX) $(CXXFLAGS) "bot\*.cpp" "tbp\*.cpp" -o "bin\tbp\tbp.exe"
	@echo Finished building Lemon Tea tbp!

clean: makedir
	@echo Cleaning the bin directory
	@rmdir /s /q bin
	@make makedir

makedir:
	@IF NOT exist bin ( mkdir bin )
	@IF NOT exist bin\client ( mkdir bin\client )
	@IF NOT exist bin\tbp ( mkdir bin\tbp )

.DEFAULT_GOAL := client