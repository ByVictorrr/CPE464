# Lab 1: Linux and You
## Author(s)

Victor Delaplaine

## Level 0

level0@management:~$ ls

README

level0@management:~$ cat README 
Congratulations! You figured out how to print.  

flag{level1:RjMY4zR6,4kaFXeBB;%K4#KiKbK6qYmoj}keAGVc}

## Level 1

level1@management:~$ ls
README
level1@management:~$ cat README 
There is a file hidden in this directory.  Can you use ls to find it?

level1@management:~$ ls -la
total 48
drwx------  4 level1 level1 4096 Jun 25  2019 .
drwxr-xr-x 12 root   root   4096 Aug 29  2016 ..
-rw-r--r--  1 level1 level1  220 Aug 29  2016 .bash_logout
-rw-r--r--  1 level1 level1 3771 Aug 29  2016 .bashrc
drwx------  2 level1 level1 4096 Jun 25  2019 .cache
-rw-------  1 level1 level1   28 Jun 25  2019 .lesshst
-rw-r--r--  1 level1 level1  655 Aug 29  2016 .profile
-rw-r--r--  1 level1 root     70 Aug 29  2016 README
-rw-r--r--  1 level1 root     82 Jun 24  2019 .README
drwx------  2 level1 level1 4096 Jun 25  2019 .ssh
-rw-------  1 level1 level1  741 Jun 24  2019 .viminfo
-rw-------  1 level1 level1  168 Jun 25  2019 .Xauthority

level1@management:~$ cat .README 
Congrats! You found the hidden file.

flag{level2:W7RU>asPZP846[P6J6(44PVXAW7R6p}

## Level 2

level2@management:~$ cat this\ file\ has\ tons\ of\ spaces 

flag{level3:t{4Z49Z9RUj4MZ2}bMg32e7u9ek;yv}

## Level 3
level3@management:~$ ls
in here
level3@management:~$ cd in\ here/
level3@management:~/in here$ ls
in here
level3@management:~/in here$ cd in\ here/
level3@management:~/in here/in here$ ls
README
level3@management:~/in here/in here$ cat README 
Here it is

flag{level4:Rneb9Prg82m3J2c8T%x266V*9L*dLm}

## Level 4
level4@management:~$ find . -type f

./.bashrc
./.viminfo
./.cache/motd.legal-displayed
./README
./.profile
./.ssh/known_hosts
./5/2/8/7/FOUND
./listmyfolder.txt
./.Xauthority
./.bash_logout

level4@management:~$ cat 5/2/8/7/FOUND 

flag{level5;fYFJYEL47u8A4k78k6^*ho$y32ouE8}

## Level 5

level5@management:~$ cat FINDIT | grep -o 'flag{.*}'

flag{level6:64%7?Emr6bWa697yALNJJd39>gT8jP}

## Level 6

level6@management:~$ ls HERE* | xargs cat | grep -o "flag{.*}"

flag{level7:E%o7JCj4G7qVm8e9JX382gA23)G#NL}

## Level 7

$ cat spinning | tr '[q-za-p]' '[a-z]' | tr '[Q-ZA-P]' '[A-Z]'

flag{level8:WPcK8d*o9orNo47>MPeYo4638XC?66}

## Level 8

level8@management:~$ cat README 

flag{Congratulations!; bp7GDV6RdVhk92f6o9G^74k4.8az?L}





