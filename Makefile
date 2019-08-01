CC:= g++-5.4
SRC_DIR:= ./src
INC_DIR:= ./inc
OBJ_DIR:= ./obj
BIN_DIR:= ./bin
CFLAG:= -std=c++11 -Wall #-g
SRCEXT:= cpp
#LIB=-lm -H -c 

APP= RefreshCounter

all: file_config.o  main.o mem_map.o access_refresh.o refresh_counter.o 
	g++ $(CFLAG) -g -o $(APP) \
			$(OBJ_DIR)/file_config.o \
			$(OBJ_DIR)/main.o \
			$(OBJ_DIR)/mem_map.o \
			$(OBJ_DIR)/access_refresh.o \
			$(OBJ_DIR)/refresh_counter.o 
	mv $(APP) $(BIN_DIR)

main.o: $(SRC_DIR)/main.$(SRCEXT) \
	$(INC_DIR)/refresh_counter.h \
	$(INC_DIR)/main.h \
	$(INC_DIR)/mem_map.h 
	$(CC) $(INC_DIR) $(CFLAG) -g -c $(SRC_DIR)/main.$(SRCEXT)
	mv *.o obj/ 	

file_config.o: $(SRC_DIR)/file_config.$(SRCEXT) $(INC_DIR)/refresh_counter.h  
	$(CC) $(INC_DIR) $(CFLAG) -g -c $(SRC_DIR)/file_config.$(SRCEXT) 
	mv *.o obj/

mem_map.o: $(SRC_DIR)/mem_map.$(SRCEXT) $(INC_DIR)/mem_map.h  
	$(CC) $(INC_DIR) $(CFLAG) -g -c $(SRC_DIR)/mem_map.$(SRCEXT) 
	mv *.o obj/

access_refresh.o : $(SRC_DIR)/access_refresh.$(SRCEXT) $(INC_DIR)/refresh_counter.h 
	$(CC) $(INC_DIR) $(CFLAG) -lm -g -c $(SRC_DIR)/access_refresh.$(SRCEXT)
	mv *.o obj/

refresh_counter.o : $(SRC_DIR)/refresh_counter.$(SRCEXT) $(INC_DIR)/refresh_counter.h 
	$(CC) $(INC_DIR) $(CFLAG) -lm -g -c $(SRC_DIR)/refresh_counter.$(SRCEXT)
	mv *.o obj/

run_1:
	$(BIN_DIR)/$(APP) pattern/access_pattern2.csv 1

run_2:
	$(BIN_DIR)/$(APP) pattern/access_pattern2.csv 2

run_all:
	$(BIN_DIR)/$(APP) pattern/access_pattern2.csv 3

run_3:
	$(BIN_DIR)/$(APP) pattern/test3.csv 3

run_4:
	$(BIN_DIR)/$(APP) pattern/test4.csv 3

run_5:
	$(BIN_DIR)/$(APP) pattern/test5.csv 3

run_6:
	$(BIN_DIR)/$(APP) pattern/test6.csv 3

clean:
clean:
	clear
	rm  obj/*.o
#	rm  bin/*
	rm  inc/*.h.gch
	rm src/*.swp
