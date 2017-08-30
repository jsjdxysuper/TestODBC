all:main.cpp OdbcWrap.cpp  OdbcWrap.h
	g++ -o all main.cpp OdbcWrap.cpp -I /usr/include -L /usr/lib  -lodbc  -w -g -O0
clean:
	-rm all