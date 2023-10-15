



```bash
g++ -o program program.cpp -ldl
strace -e trace=open,openat -f ./program
```