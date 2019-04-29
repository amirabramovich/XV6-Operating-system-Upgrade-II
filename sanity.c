#include "types.h"
#include "stat.h"
#include "user.h"
#include "tournament_tree.h"

trnmnt_tree* tree;

void 
infinite(void)
{
	printf(1,"infinite pid is %d and tid is %d\n",getpid(), kthread_id());
    for (;;)
        printf(0, ".");
}

void
test3(void){
	sleep(20);
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

void
test6(void){
	printf(1,"test6 pid is %d and tid is %d\n",getpid(), kthread_id());
	int mutex = kthread_mutex_alloc();
	printf(1,"test6 alloced mutex %d\n",mutex);
	if(kthread_mutex_lock(mutex)==0)
		printf(1,"test6 locked mutex %d\n",mutex);
	sleep(100);
	if(kthread_mutex_unlock(mutex)==0)
		printf(1,"test6 unlocked mutex %d\n",mutex);
	printf(1,"test6 done\n");
	kthread_exit();
}

void
test7(void){
	if(trnmnt_tree_acquire(tree, 0)==0)
		printf(1,"test7 locked tree\n");
	sleep(50);
	if(trnmnt_tree_release(tree, 0)==0)
		printf(1,"test7 unlocked tree\n");
	kthread_exit();
}

void
test8(void){
	if(trnmnt_tree_acquire(tree, 1)==0)
		printf(1,"test8 locked tree\n");
	sleep(50);
	if(trnmnt_tree_release(tree, 1)==0)
		printf(1,"test8 unlocked tree\n");
	kthread_exit();
}

int
main(int argc, char *argv[])
{
	int user_select = atoi(argv[1]);
	int tid, tid2;
    switch (user_select){
        case 1: // Checks proccess exit while another thread is running
            test1();
		case 2: // Checks EXEC
            printf(1,"test2 pid is %d and tid is %d\n",getpid(), kthread_id());
			char* empty = "";
			exec("ls", &empty);
		case 3: // Checks thread join
			{void* stack = (void*)malloc(4000);
			tid = kthread_create(&test3, stack);
			printf(1,"main pid is %d and tid is %d\n",getpid(), kthread_id());
			kthread_join(tid);	
			printf(1,"main done\n");
			exit();}
        case 4: // Checks all 3 tests together in different proccesses
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
					tid = kthread_create(&test3, stack);
					printf(1,"main pid is %d and tid is %d\n",getpid(), kthread_id());
					kthread_join(tid);	
					printf(1,"main done\n");
					exit();
				}
			}
		case 5: // Checks Mutex
			{void* stack = (void*)malloc(4000);
			printf(1,"main pid is %d and tid is %d\n",getpid(), kthread_id());
			tid = kthread_create(&test6, stack);
			if(kthread_mutex_lock(0)==0)
				printf(1,"main locked mutex %d\n",0);
			else if(kthread_mutex_lock(0)<0)
				printf(1,"main can't lock mutex %d\n",0);
			sleep(20);
			if(kthread_mutex_lock(0)==0)
				printf(1,"main locked mutex %d\n",0);
			else if(kthread_mutex_lock(0)<0)
				printf(1,"main can't lock mutex %d\n",0);
			if(kthread_mutex_unlock(0)==0)
				printf(1,"main unlocked mutex %d\n",0);
			if(kthread_mutex_dealloc(0)==0)
				printf(1,"main dealloced mutex %d\n",0);
			printf(1,"main done\n");
			exit();}
		case 6: // Checks tournament tree
			{void* stack = (void*)malloc(4000);
			void* stack2 = (void*)malloc(4000);
			printf(1,"main pid is %d and tid is %d\n",getpid(), kthread_id());
			tree = trnmnt_tree_alloc(3);
			if (tree!=0)
				printf(1,"main alloced tree\n");
			tid = kthread_create(&test7, stack);
			tid2 = kthread_create(&test8, stack2);
			kthread_join(tid);	
			kthread_join(tid2);	
			if(trnmnt_tree_dealloc(tree)==0)
				printf(1,"main dealloced tree\n");
			printf(1,"main done\n");
			exit();}

		default:
			printf(1,"usage: sanity <1-6>\n");
			exit();
	}
}
