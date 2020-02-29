INC_PATH = ./
SRC_FILE = main.cpp
COM_FLAG = -std=c++11 -lpthread
COMM_LIB = commlib/cwslib.a
OUT_FILE = cws_server
CLIENT_FILE = cws_client

FRAME_DIR = ./frame ./src
COMPILE_SUBDIR = $(shell find $(FRAME_DIR) -type d)
COMPILE_SRC += $(foreach dir,$(COMPILE_SUBDIR),$(wildcard $(dir)/*.cpp))
COMPILE_INC += $(foreach dir,$(COMPILE_SUBDIR),$(wildcard $(dir)/*.h))
COM_OBJ_FILE = $(patsubst %.cpp, %.o, $(COMPILE_SRC))

TEST_PATH = ./test/
TEST_SRC_FILE = $(wildcard $(TEST_PATH)*.cpp)
TEST_HED_FILE = $(wildcard $(TEST_PATH)*.h)
TEST_OBJ_FILE = $(patsubst %.cpp, %.o, $(TEST_SRC_FILE))
COM_OBJ_FILE += $(TEST_OBJ_FILE)

$(OUT_FILE) : $(SRC_FILE) $(COM_OBJ_FILE)
	cd commlib; make; cd ..;\
	g++ -o $(OUT_FILE) $(SRC_FILE) $(COM_OBJ_FILE) $(COMM_LIB) $(COM_FLAG) -I$(INC_PATH)

$(CLIENT_FILE) : $(SRC_FILE) $(COM_OBJ_FILE)
	cd commlib; make; cd ..;\
	g++ -o $(CLIENT_FILE) $(SRC_FILE) $(COM_OBJ_FILE) $(COMM_LIB) $(COM_FLAG) -I$(INC_PATH) -DCLIENT_MAIN

%.o:%.cpp
	g++ -o $@ -c -I$(INC_PATH) $< $(COM_FLAG)

.PHONY:clean
clean:
	cd commlib; make clean; cd ..;\
	rm -f $(OUT_FILE) $(COM_OBJ_FILE) $(CLIENT_FILE)