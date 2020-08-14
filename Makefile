
all: mod_vhost_limit.so

shm.o : shm.cpp
	g++ -c -I/usr/include/httpd/ -I/usr/include/apr-1/ -fPIC shm.cpp

mod_vhost_limit.o : mod_vhost_limit.cpp
	g++ -c -I/usr/include/httpd/ -I/usr/include/apr-1/ -fPIC mod_vhost_limit.cpp

mod_vhost_limit.so : mod_vhost_limit.o shm.o
	g++ -shared -o mod_vhost_limit.so mod_vhost_limit.o shm.o 

clean:
	rm -rf mod_vhost_limit.o mod_vhost_limit.so shm.o

