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
infinite(void)
{
	printf(1,"infinite pid is %d and tid is %d\n",getpid(), kthread_id());
    for (;;)
        printf(0, ".");
}

void
test3(void){
	printf(1,"test3 pid is %d and tid is %d\n",getpid(), kthread_id());
	printf(1,"test3 done\n");
	kthread_exit();
}

int 
test1(void)
{
 	void* stack = (void*)malloc(4000);
	printf(1,"test1 pid is %d and tid is %d\n",getpid(), kthread_id());
    kthread_create(infinite, stack);
    sleep(60);
    exit();
}

int
main(int argc, char *argv[])
{
	if(fork() == 0){
		test1();
	}else{
		wait();
		printf(0, "\ntest1 done\n");
		if(fork() == 0){
			printf(1,"test2 pid is %d and tid is %d\n",getpid(), kthread_id());
			char* empty = "";
			exec("ls", &empty);
		}else{
			wait();
			printf(1,"test2 done\n");
			void* stack = (void*)malloc(4000);
			int tid = kthread_create(&test3, stack);
			printf(1,"main pid is %d and tid is %d\n",getpid(), kthread_id());
			kthread_join(tid);	
			printf(1,"main done\n");
			exit();
		}
	}
}


