SC_SRC = # SET THIS VARIABLE TO SUPERCOLLIDER SOURCE DIRECTORY
SC_EXT = # SET THIS VARIABLE TO SUPERCOLLIDER EXTENSIONS DIRECTORY
SC_INC = -I $(SC_SRC)/include/plugin_interface -I $(SC_SRC)/include/common -I $(SC_SRC)/server/scsynth
CC_OPT = -shared -fpic -m64 -std=c++17
LN_OPT = -lstdc++

install: make_lib
	cp VSTTriggers.scx $(SC_EXT)/VSTPlugin/plugins
	cp VSTTriggers.sc $(SC_EXT)/VSTPlugin/classes


make_lib:
	gcc $(SC_INC) $(CC_OPT) -o VSTTriggers.scx VSTTriggers.cpp $(LN_OPT)


