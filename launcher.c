#include"BlockChain.h"


int main()
{
	mqd_t mq;
	int i;
	int pid[NUM_OF_MINERS+1];
	char* argv[] = {0,0};
	pid[0] = fork();
	if(pid[0] == 0){
	argv[0] = "./server.out";
	execv("./server.out",argv);
	}
	sleep(2);
	argv[0] = "./miner.out";
	for(i = 1; i <= NUM_OF_MINERS; i++){
		pid[i] = fork();
		if(pid[i] == 0){
			execv("./miner.out",argv);
		}
	}
}
