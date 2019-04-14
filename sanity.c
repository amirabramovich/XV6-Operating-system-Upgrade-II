#include "types.h"
#include "stat.h"
#include "user.h"

int
fibonacci(int num){
    if (num <= 1) 
      return num; 
   return fibonacci(num - 1) + fibonacci(num - 2);
}

void
test1(int num){
    printf(2, "fib %d is %d.\n", num, fibonacci(num));
}

int
main(int argc, char *argv[]){
    int num;
    num = (int)(*argv[1])-'0';
    if(argc==2 && num>=0 && num<=9)
        test1(num);
    else
        printf(1, "wrong argument.\n");
    exit();
}