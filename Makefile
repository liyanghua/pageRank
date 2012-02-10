CPPFLAGS=-g -Wall 

BOOST_HOME=/home/yichen.lyh/boost_home

LIBS= -L /usr/include/

CPLUS_INCLUDE_PATH=${BOOST_HOME}/include
export CPLUS_INCLUDE_PATH

.PHONY : clean all

all: $(subst .cpp,.o,$(SOURCES))  page_rank	


%.O: %.cpp
	$(CXX) $(CPPFLAGS) ${LIBS} $^ $@
page_rank: page_rank.cpp 
	$(CXX) $(CPPFLAGS) $^  ${LIBS} -o $@

clean:
	rm -rf  *.o  page_rank
