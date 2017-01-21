CC = g++
CXX = g++

##################################################

##################################################

CPPFLAGS := -g 

OBJS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
OBJS += $(patsubst %.c,%.o,$(wildcard *.c))
DEP := $(foreach obj, $(OBJS), $(dir $(obj)).$(basename $(notdir $(obj))).d)

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(INC) -c $< -o $@

%.o: %.c
	gcc $(CPPFLAGS) $(INC) -c $< -o $@

TARGET=testproxy
OST=
CPPFLAGS += -DOST=\"${OST}\"

INSTALL_PATH=/usr/local/app/${TARGET}

$(TARGET): $(OBJS)
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LIB) ./libcrypto.a -lz -ldl

clean:
	rm -rf *.o

init:
	mkdir -p ${INSTALL_PATH}/
	mkdir -p ${INSTALL_PATH}/bin
	mkdir -p ${INSTALL_PATH}/conf
	mkdir -p ${INSTALL_PATH}/log
	mkdir -p ${INSTALL_PATH}/data
	mkdir -p ${INSTALL_PATH}/day

install:
	cp -rf $(TARGET) ${INSTALL_PATH}/bin/
	cp -rf *.conf ${INSTALL_PATH}/conf/

tar:
	./maketar.sh tsf-tconfig-ccs-${OST}

help:
	@echo "make clean : remove all obj files and target"
	@echo "make init : init install path"
	@echo "make install : copy target, conf to install path"
	@echo "make tar : make install packet"    

-include ${DEP}

# 自动计算文件的依赖性
.%.d: %.cpp
	$(CC) $(INC) -MM $< > $@
	@$(CC) $(INC) -MM $< | sed s/"^"/"\."/  |  sed s/"^\. "/" "/  | \
                sed s/"\.o"/"\.d"/  >> $@

.%.d: %.c
	$(CC) $(INC) -MM $< > $@
	@$(CC) $(INC) -MM $< | sed s/"^"/"\."/  |  sed s/"^\. "/" "/  | \
                sed s/"\.o"/"\.d"/  >> $@
