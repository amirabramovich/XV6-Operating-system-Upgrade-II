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
test0(void){
	printf(1,"test0 pid is %d and tid is %d\n",getpid(), kthread_id());
	printf(1,"test0 done\n");
	kthread_exit();
}

int 
test1(void)
{
 	void* stack = (void*)malloc(4000);
	printf(1,"test1 pid is %d and tid is %d\n",getpid(), kthread_id());
    kthread_create(infinite, stack);
    sleep(60);
    printf(0, "test1 done\n");
    exit();
}

int
main(int argc, char *argv[]){
	int pid = fork();
	if(pid == 0){
		test1();
	}else{
		wait();
		void* stack = (void*)malloc(4000);
		int tid = kthread_create(&test0, stack);
		printf(1,"main pid is %d and tid is %d\n",getpid(), kthread_id());
		kthread_join(tid);	
		printf(1,"main done\n");
		exit();
	}
}

