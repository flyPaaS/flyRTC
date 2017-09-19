CC = g++
CXX = g++

##################################################

##################################################

CPPFLAGS := -g 
CPPFLAGS +=-fPIC

OBJS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
OBJS += $(patsubst %.c,%.o,$(wildcard *.c))
DEP := $(foreach obj, $(OBJS), $(dir $(obj)).$(basename $(notdir $(obj))).d)

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(INC) -c $< -o $@

%.o: %.c
	gcc $(CPPFLAGS) $(INC) -c $< -o $@

TARGET=sktlv

$(TARGET): $(OBJS)
	ld -r -o $(TARGET) $(OBJS)
clean:
	-rm -f $(TARGET) $(OBJS) $(DEP)


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


