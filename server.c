#include"BlockChain.h"

void createGenesisBlock(BlockChain *chain)
{
	Block genBlock;
	BlockDataForHash  data;
	memset(&genBlock, 0x00, sizeof(Block));
	genBlock.timestamp=((int)time(NULL));
	genBlock.nonce =20;
	data = getDataForHash(genBlock);
	genBlock.hash = calcHash(data);
	addBlockToChain(chain,genBlock);
}

int isValidBlock(BlockChain chain,Block blockToCheck)
{
	BlockDataForHash  data;
	int hash;
	Block L_Block = getLastBlock(&chain);
	if(L_Block.height + 1 ==  blockToCheck.height){
		if(L_Block.hash == blockToCheck.prev_hash){
			if(isValidHash(blockToCheck.hash, DIFFICULTY)){
				data = getDataForHash(blockToCheck);
				hash = calcHash(data);
				if(hash == blockToCheck.hash){
					return 1;
				}
			}
		}
	}

	return 0;

}

void printNewBlock(Block block){

	printf("Server: New block added by %d ,attributes : height (%d), timestamp(%d), hash(0x%04x), prev_hash(0x%04x), difficulty(%d), nonce(%d)\n",block.relayed_by,block.height,block.timestamp, block.hash,block.prev_hash,block.difficulty,block.nonce);

}



void updateChain(BlockChain* chain, Block block)

{
	printNewBlock(block);
    addBlockToChain(chain,block);
	 
}

mqd_t addNewListener(MSG_T*  msg,Block block){
	int listenerID = ((REQUEST_MSG_DATA_T*)(msg->data))->pid;
	char queueName[MAX_QUEUE_NAME_SIZE] = {0}; 
	convertAPidToString(listenerID, queueName);
	mqd_t mq =  connectToQueue(queueName);
	sendBlockMsg(mq, block);
	return mq;
}

void notifyAllListeners(mqd_t* listeners, int numOfListeners, Block block){
	int i;
	for( i = 0 ; i < numOfListeners; i++){
		sendBlockMsg(listeners[i], block);
	}
}

int msgIsBlock(MSG_T* msg){
	return msg->type == BLOCK;
}

void ServerRoutine(){
	Block blockToCheck;
	mqd_t my_queue = createServerQueue();
	mqd_t listeners[NUM_OF_MINERS];
	//struct mq_attr mqAttr = {0};
	BlockChain chain = getEmptyBlockChain();
	createGenesisBlock(&chain);
	MSG_T* msg = malloc(MQ_MAX_MSG_SIZE); 
	while(1){
		int numOfListeners;
		mq_receive(my_queue , (char*)msg, MQ_MAX_MSG_SIZE, NULL);
		 if(msgIsBlock(msg)){
			 blockToCheck = getBlockFromMessage(msg);
			 if(isValidBlock(chain,blockToCheck)){
				 updateChain(&chain, blockToCheck);
				 notifyAllListeners(listeners, numOfListeners, blockToCheck);
			 }
			 else{
				 printf("Server reject the block of %d\n", blockToCheck.relayed_by);
			 }
		 }

		if(msg->type == JOIN_REQUEST){
			listeners[numOfListeners++] = addNewListener(msg,getLastBlock(&chain));
		}
	}
}

int main()
{
    ServerRoutine();
}