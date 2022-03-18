CXX = g++
ifeq ($(BUILD),debug)   
CXXFLAGS += -fdiagnostics-color=always -DUNICODE -std=c++20 -Wall -Og -g
else
CXXFLAGS += -DUNICODE -DNDEBUG -std=c++20 -O3 -flto -s -march=native
endif

STATIC_LIB = -luser32 -lgdi32 -lopengl32 -lgdiplus -lShlwapi -ldwmapi -lstdc++fs -lsetupapi -lhid -static

.PHONY: all client clean makedir

all: client

client: makedir
	@echo Building Lemon Tea client...
	@$(CXX) $(CXXFLAGS) "bot\*.cpp" "client\*.cpp" -o "bin\client\client.exe"
	@echo Finished building Lemon Tea client!

clean: makedir
	@echo Cleaning the bin directory
	@rmdir /s /q bin
	@make makedir

makedir:
	@IF NOT exist bin ( mkdir bin )
	@IF NOT exist bin\client ( mkdir bin\client )

.DEFAULT_GOAL := client