#include"BlockChain.h"


BlockDataForHash getBlockDataOfNextBlock(mqd_t my_q, int id, MSG_T* msg);
Block createBlock(BlockDataForHash blockData,unsigned int hash);
int isQueueNotEmpty(mqd_t mq);

void MinerRoutine(){
	char strQ[MAX_QUEUE_NAME_SIZE] = {0};
	int id = getpid();
	int hash, blocksCounter = 0;
	Block myBlock;
	BlockDataForHash blockData;
	MSG_T* msg = malloc(MQ_MAX_MSG_SIZE); 
	blockData.prev_hash = 0;
	convertAPidToString(id,strQ);
	mqd_t my_q =  createQueue(strQ);
	mqd_t server_q = connectToQueue(SERVER_Q_NAME);
	sendRequestMsg(server_q, id);
	blockData = getBlockDataOfNextBlock(my_q, id, msg);
	while(1){
		while(1){
			if(isQueueNotEmpty(my_q))
				blockData = getBlockDataOfNextBlock(my_q, id, msg);
			else
				blockData.nonce++;
			hash = calcHash(blockData);
			if(isValidHash(hash,DIFFICULTY)){ 
				myBlock = createBlock(blockData, hash);
				printf("Miner #%d: Mined a new block #%d, with the hash 0x%04x\n", id, ++blocksCounter, hash);
				break;
			}
		}
		sendBlockMsg(server_q, myBlock);
	} 
}


int isQueueNotEmpty(mqd_t mq){
	struct mq_attr mqAttr = {0};
	mq_getattr(mq, &mqAttr);
    if (mqAttr.mq_curmsgs > 0)
        {
            return 1;
        }
		return 0;
}


Block createBlock(BlockDataForHash blockData,unsigned int hash)
{
	Block newBlock;
	newBlock.hash=hash;
	newBlock.difficulty=DIFFICULTY;
	newBlock.timestamp=blockData.timestamp;
	newBlock.prev_hash=blockData.prev_hash;
	newBlock.relayed_by=blockData.relayed_by;
	newBlock.height=blockData.height;
	newBlock.nonce=blockData.nonce;
	return newBlock;
}


	BlockDataForHash getBlockDataOfNextBlock(mqd_t my_q, int id, MSG_T* msg)
	{
			BlockDataForHash data;
			Block newBlock;
			mq_receive(my_q , (char*)msg, MQ_MAX_MSG_SIZE, NULL);
			newBlock = getBlockFromMessage(msg);
			data.nonce = (id%10)*1000;
			data.prev_hash = newBlock.hash;
			data.height=(newBlock.height+1);
			data.timestamp=((int)time(NULL));
			data.relayed_by=id;
			return data;
			
	}
	
void main() 
{
	MinerRoutine();
}

