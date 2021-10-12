# ldig - link digout
ldig prints symbolic link chains recursively. It makes it easy for you to find out the file that a symbolic link finally points to.  
## How to use
Support you have such files:  
```
usr@dom:/usr/bin$ ls -lh | grep gcc
lrwxrwxrwx  1 root   root        5 Jul 1 2021 gcc -> gcc-9*
lrwxrwxrwx  1 root   root       22 Jul 1 2021 gcc-9 -> x86_64-linux-gnu-gcc-9*
-rwxr-xr-x  1 root   root     1.2M Jul 1 2021 x86_64-linux-gnu-gcc-9*
```
Use `ldig gcc`, you will get:  
```
usr@dom:/usr/bin$ ldig gcc
gcc -> gcc-9 -> x86_64-linux-gnu-gcc-9
```
It's clearer to view these files.  
# License
Written by Yibang Heng.  
Licensed under the [GNU General Public License v3.0](LICENSE.txt) license.
