CC:= g++-5.4
SRC_DIR:= ./src
INC_DIR:= ./inc
OBJ_DIR:= ./obj
BIN_DIR:= ./bin
CFLAG:= -std=c++11 -Wall
SRCEXT:= cpp
#LIB=-lm -H -c -Wall

APP= RefreshCounter

all: file_config.o  main.o  mem_map.o  refresh_counter.o 
	g++ $(CFLAG) -g -o $(APP) \
			$(OBJ_DIR)/file_config.o \
			$(OBJ_DIR)/main.o \
			$(OBJ_DIR)/mem_map.o \
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

mem_map.o : $(SRC_DIR)/mem_map.$(SRCEXT) $(INC_DIR)/mem_map.h 
	$(CC) $(INC_DIR) $(CFLAG) -g -c $(SRC_DIR)/mem_map.$(SRCEXT)
	mv *.o obj/

refresh_counter.o : $(SRC_DIR)/refresh_counter.$(SRCEXT) $(INC_DIR)/refresh_counter.h 
	$(CC) $(INC_DIR) $(CFLAG) -lm -g -c $(SRC_DIR)/refresh_counter.$(SRCEXT)
	mv *.o obj/

run:
	$(BIN_DIR)/$(APP) access_pattern.csv

clean:
	clear
	rm  obj/*.o
#	rm  bin/*
	rm  inc/*.h.gch
