# mod_vhost_limit for Apache >= 2.4

Since Apache 2.4, there have been some changes that broke or at least made some of the modules to "not work" correctly.

So, i've rewritten the mod to work correctly with the new Apache and with other MPMs (such as the Event MPM). The main difference now, is that the mod is written in C++ which makes the compiling process a little different.

To compile, clone the repo and make. Then install and launch.

```bash
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

mod_vhost_limit.so is your module. That's it. Now you have to install it.
This is different for each system, so i will show how to install it in a Fedora install :

```bash
$ cp -f mod_vhost_limit.so /etc/httpd/modules/
$ echo -e "\nLoadModule vhost_limit_module modules/mod_vhost_limit.so" >> /etc/httpd/conf.modules.d/00-base.conf
$ systemctl restart httpd
```
<pre>
$ tail /var/log/httpd/error_log

[today's date] [suexec:notice] [pid 126265:tid 126265] AH01232: suEXEC mechanism enabled (wrapper: /usr/sbin/suexec)
<b>[today's date] [vhost_limit:notice] [pid 126265:tid 126265] mod_vhost_limit: Created SHM block at 0x7f5c6a55c008 , size 800</b>
[today's date] [lbmethod_heartbeat:notice] [pid 126265:tid 126265] AH02282: No slotmem from mod_heartmonitor
[today's date] [http2:warn] [pid 126265:tid 126265] AH02951: mod_ssl does not seem to be enabled
[today's date] [mpm_event:notice] [pid 126265:tid 126265] AH00489: Apache/2.4.43 (Fedora) configured -- resuming normal operations
[today's date] [core:notice] [pid 126265:tid 126265] AH00094: Command line: '/usr/sbin/httpd -D FOREGROUND'
</pre>

The highlighted line is the mod letting you know it is initialized with its own memory block.
To make it DO something, you need to use the only directive supported (for now) inside a VirtualHost block :

```apache
<VirtualHost *>

  ServerName test.mvh.info
  DocumentRoot /var/www/html/

<font color=red>  MaxVhostClients 3</font>

</VirtualHost>
```

And restart httpd/apache. 

Everytime you get a visit to that Virtualhost, a per-vhost counter will increment. When that connection is finished, the counter will decrement. In this case, we are limiting the vhost to 3 simultaneous connections, and every excess visit will receive a HTTP_SERVICE_UNAVAILABLE response.

> Please note : Using a fastcgi script (e.g. mod_fcgi/php-fpm) might not close the connection immediately after finished. It might take a little longer. So if you are testing, have this in mind before thinking something broke.


