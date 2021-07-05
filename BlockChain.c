#include"BlockChain.h"
BlockChain getEmptyBlockChain(){
	BlockChain chain;
	chain.head = chain.tail = NULL;
	chain.size = 0;
	return chain;
}
void addBlockToChain(BlockChain* chain, Block newBlock){
	BlockNode* node = (BlockNode*)malloc(sizeof(BlockNode));
	node->value = newBlock;
	node->next = NULL;
	if(chain->tail != NULL)
		chain->tail->next = node;
	chain->tail = node;
	(chain->size)++;
}
void freeBlockChain(BlockChain* chain){
	BlockNode* current = chain->head;
	BlockNode* prev;
	while(current != NULL){
		prev = current;
		current = current->next;
		free(prev);
	}
	chain->head = chain->tail = NULL;
	chain->size = 0;
}
Block getLastBlock(BlockChain* chain){
	return chain->tail->value;
}
BlockDataForHash getDataForHash(Block block){
	BlockDataForHash data;
	data.height = block.height;
	data.timestamp = block.timestamp;
	data.prev_hash = block.prev_hash;
	data.nonce = block.nonce;
	data.relayed_by = block.relayed_by;
	return data;
}
void printBlock(Block block, char* str){
	//(0x%04x)
	printf("%s Block: heigh = %d : timestamp = %d : prev_hash = %d : nonce = %d : relayed_by = %d : hash = %d\n", str,block.height, block.timestamp, block.prev_hash, block.nonce, block.relayed_by, block.hash);
}

int getBlockChainSize(BlockChain chain)
{
	return chain.size;
}

unsigned int calcHash(BlockDataForHash data )
{
	int len = sizeof(BlockDataForHash);
    char buffer[sizeof(BlockDataForHash)+1];
    memset(buffer, 0x00, sizeof(BlockDataForHash)+1);
    memcpy(buffer, &data, sizeof(BlockDataForHash)); 
	return crc32(0,buffer,len);
}

int isValidHash(unsigned int hash,int difficulty)
{
	int mask=1;
	int steps = ((sizeof(hash)*8) -difficulty);
	mask = mask << steps;
	if(mask>hash)
	{
		return 1;
	}
	return 0;
}

mqd_t connectToQueue(char* str){
	mqd_t mq = mq_open(str,  O_RDWR);
	return mq;
}

int numOfDigit(int num)
{
	int counter;
	counter = 0;
	while (num>0)
	{
		num = num / 10;
		counter++;
	}
	return counter;
}

void convertAPidToString(int num, char* buffer)
{
	char* myQueue = "/MY_QUEUE";
	char*numstr;
	numstr = (char*)calloc( 1,numOfDigit(num) + 1);
	strcpy(buffer,myQueue);
	snprintf(numstr ,numOfDigit(num) +1,"%d", num);
	strcat(buffer,numstr);
	free(numstr);
}
Block getBlockFromMessage(MSG_T* msg){
	Block block = ((BLOCK_MSG_DATA_T*)msg->data)->block;
	return block;
}

void sendBlockMsg(mqd_t mq, Block block){
		MSG_T* msg = malloc(sizeof(MSG_T) + sizeof(BLOCK_MSG_DATA_T));
		msg->type = BLOCK;
		((BLOCK_MSG_DATA_T*)msg->data)->block = block;
        mq_send(mq, (char*)msg, MQ_MAX_MSG_SIZE, 0);
		free(msg);
}
void sendRequestMsg(mqd_t mq, int pid){
		MSG_T* msg = malloc(sizeof(MSG_T) + sizeof(REQUEST_MSG_DATA_T));
		msg->type = JOIN_REQUEST;
		((REQUEST_MSG_DATA_T*)(msg->data))->pid = pid;
		printf("id in msg %d\n",((REQUEST_MSG_DATA_T*)(msg->data))->pid);
        mq_send(mq, (char*)msg, MQ_MAX_MSG_SIZE, 0);
		free(msg);
}

mqd_t createQueue(char* str){
	mqd_t mq;
	struct mq_attr attr = {0};
    	attr.mq_maxmsg = MQ_MAX_SIZE;
    	attr.mq_msgsize = MQ_MAX_MSG_SIZE;
        mq_unlink(str); 
        mq = mq_open(str , O_CREAT, S_IRWXU | S_IRWXG, &attr);
		return mq;
}
mqd_t createServerQueue(){
	return createQueue(SERVER_Q_NAME );
}
