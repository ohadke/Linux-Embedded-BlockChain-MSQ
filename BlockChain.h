#ifndef __BLOCKCHAIN__
#define __BLOCKCHAIN__
#define MQ_MAX_SIZE         10
#define MQ_MAX_MSG_SIZE     100 		
#define NUM_OF_MINERS 4
#define SERVER_Q_NAME  "/server_my1"
#define DIFFICULTY 16
#define MAX_QUEUE_NAME_SIZE 50
#include<mqueue.h>
#include<stdlib.h>
#include<stdio.h>
#include <zlib.h>
#include <time.h>
#include<string.h>
#include <errno.h>
typedef struct Block{
	int height;
	int timestamp;
	unsigned int hash;
	unsigned int prev_hash;
	int difficulty;
	int nonce;
	int relayed_by;
}Block;

typedef struct BlockDataForHash{
	int height;
	int timestamp;
	unsigned int prev_hash;
	int nonce;
	int relayed_by;
}BlockDataForHash;

typedef struct BlockNode{
	struct BlockNode* next;
	struct Block value;
	int size;
}BlockNode;
typedef struct BlockChain{
	BlockNode* head;
	BlockNode* tail;
	int size;
}BlockChain;

typedef enum{
	JOIN_REQUEST,	BLOCK
} MSG_TYPE_E;


/* Data that will be passed from the Writer to the reader


should hold the actual application data */ 


typedef struct msg{
	MSG_TYPE_E type;
	char data[]; // Dynamic/flexible array - place holder for unknown size data
}MSG_T;

typedef struct block_msg_data{
	Block block;
} BLOCK_MSG_DATA_T;

typedef struct request_msg_data{
	int pid;
} REQUEST_MSG_DATA_T;


BlockChain getEmptyBlockChain();
void addBlockToChain(BlockChain* chain, Block newBlock);
void freeBlockChain(BlockChain* chain);
Block getLastBlock(BlockChain* chain);
void createGenesisBlock(BlockChain *chain);
int isValidBlock();
int isValidHash(unsigned int hash,int difficulty);
int getBlockChainSize();
void printBlock(Block block, char* str);
unsigned int calcHash(BlockDataForHash data);
BlockDataForHash getDataForHash(Block block);
mqd_t connectToQueue(char* str);
void convertAPidToString(int num, char* buffer);
Block getBlockFromMessage(MSG_T* msg);
void sendBlockMsg(mqd_t mq, Block block);
void sendRequestMsg(mqd_t mq, int pid);
mqd_t createQueue(char* str);
mqd_t createServerQueue();
#endif