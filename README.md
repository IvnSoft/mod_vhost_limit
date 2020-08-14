# mod_vhost_limit
Apache mod_vhost_limit module

Since Apache 2.4, there have been some changes that broke or at least made some of the modules to "not work" correctly.

So, i've rewritten the mod to work correctly with the new Apache and with other MPMs (such as the Event MPM). The main difference now, is that the mod is written in C++ which makes the compiling process a little different.

To compile, clone the repo and make. Then install and launch.

```
$ git clone https://github.com/IvnSoft/mod_vhost_limit

Cloning into 'mod_vhost_limit'...
remote: Enumerating objects: 4, done.
remote: Counting objects: 100% (4/4), done.
remote: Compressing objects: 100% (3/3), done.
remote: Total 4 (delta 0), reused 0 (delta 0), pack-reused 0
Unpacking objects: 100% (4/4), done.


$ cd mod_vhost_limit/
$ make

g++ -c -I/usr/include/httpd/ -I/usr/include/apr-1/ -fPIC mod_vhost_limit.cpp
g++ -c -I/usr/include/httpd/ -I/usr/include/apr-1/ -fPIC shm.cpp
g++ -shared -o mod_vhost_limit.so mod_vhost_limit.o shm.o 

$ ls -las
total 72
 0 drwxr-xr-x 3 root root   210 Aug 14 10:41 .
 0 drwxr-xr-x 5 root root    54 Aug 14 09:45 ..
 0 drwxr-xr-x 8 root root   163 Aug 14 10:40 .git
12 -rw-r--r-- 1 root root 11357 Aug  9 21:13 LICENSE
 4 -rw-r--r-- 1 root root   393 Aug 14 10:41 Makefile
 8 -rw-r--r-- 1 root root  4349 Aug 14 10:39 mod_vhost_limit.cpp
 4 -rw-r--r-- 1 root root   439 Aug 10 22:17 mod_vhost_limit.h
 8 -rw-r--r-- 1 root root  7888 Aug 14 10:41 mod_vhost_limit.o
20 -rwxr-xr-x 1 root root 18352 Aug 14 10:41 mod_vhost_limit.so
 4 -rw-r--r-- 1 root root   812 Aug 14 10:37 README.md
 4 -rw-r--r-- 1 root root  1082 Aug 14 10:39 shm.cpp
 4 -rw-r--r-- 1 root root   188 Aug 13 19:08 shm.h
 4 -rw-r--r-- 1 root root  3368 Aug 14 10:41 shm.o
```

That's it. Now you have to install it.

