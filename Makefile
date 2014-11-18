CXX=c++
CC=c++
CFLAGS=-I/usr/local/include -Wall -O -g -std=c++11
D_CFLAGS=-DDEBUG $(CFLAGS)
LFLAGS=-L/usr/local/lib -lcurl -lpng -liconv -lboost_filesystem -lboost_system -lboost_thread

all: test apps itpub_forum_dbg

apps: itpub_forum itpub_dircheck

itpub_forum: itpub_forum.o itpub_config.o itpub_session.o simil.o itpub_cap_ImgUtil.o itpub_cap_PngPixelIter.o
	$(CXX) $^ -o itpub_forum $(LFLAGS)
	
itpub_forum_dbg: itpub_forum_dbg.o itpub_config.o itpub_session_dbg.o simil.o itpub_cap_ImgUtil.o itpub_cap_PngPixelIter.o
	$(CXX) $^ -o itpub_forum_dbg $(LFLAGS)

itpub_dircheck: itpub_dircheck.o itpub_config.o
	$(CXX) $^ -o itpub_dircheck $(LFLAGS)

	
test: itpub_http.t itpub_session.t itpub_logger.t itpub_session_dbg.t

itpub_logger.t: itpub_logger.t.o
	$(CXX) $^ -o itpub_logger.t $(LFLAGS)
	
itpub_http.t: itpub_http.t.o itpub_config.o
	$(CXX) $^ -o itpub_http.t $(LFLAGS)

itpub_session.t: itpub_session.t.o itpub_config.o itpub_session.o simil.o itpub_cap_ImgUtil.o itpub_cap_PngPixelIter.o
	$(CXX) $^ -o itpub_session.t $(LFLAGS)

itpub_session_dbg.t: itpub_session_dbg.t.o itpub_config.o itpub_session_dbg.o simil.o itpub_cap_ImgUtil.o itpub_cap_PngPixelIter.o
	$(CXX) $^ -o itpub_session_dbg.t $(LFLAGS)

%_dbg.o: %.cpp
	$(CXX) $(D_CFLAGS) -c $< -o $@
	
%_dbg.t.o: %.t.cpp
	$(CXX) $(D_CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@
	
clean:
	rm -rf *.o
	rm -rf *.t
	rm -rf *_dbg
	rm -rf itpub_forum
	rm -rf itpub_dircheck
