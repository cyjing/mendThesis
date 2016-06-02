/*
 *  Copyright (c) 2010-2013, Rutgers, The State University of New Jersey
 *  All rights reserved.
 *  
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *      * Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *      * Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *      * Neither the name of the organization(s) stated above nor the
 *        names of its contributors may be used to endorse or promote products
 *        derived from this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 \mainpage MobilityFirst's API documentation page
 
 We are building a proof-of-concept protocol stack to include a basic transport layer (message chunking, e-2-e acks, flow control), GUID service layer, storage aware routing, and hop-by-hop link data transport. The clean slate implementation will export a GUID-centered API with message-based communication primitives, and rich delivery service options (e.g., DTN, real-time, multicast, etc). We will also explore a 'network interface manager' functionality to enable policy driven multi-homing that could flexibly address user/application stated goals of performance, reliability, battery lifetime, network svc. payments, etc. The protocol stack will be implemented in C/C++ and will be standalone user-level process, while apps will link to the service API library to use the new stack.
 */

/**
 * @file   mfapi.c
 * @author wontoniii (bronzino@winlab.rutgers.edu), czhang
 * @date   July, 2011
 * @brief  MobilityFirst's API.
 *
 * C/C++ MobilityFirst API. Used to communicate with MobilityFirst's network protocol stack
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <linux/un.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>

#ifndef __linux
#include <sys/endian.h>
#include <sys/types.h>
#endif

#include <include/mfflags.h>
#include <include/mfclientipc.h>

#include "mfapi.h"
#include "log.h"
#include "mferrors.h"

//TODO: complete the close implementation

/*! Function that insert in the list the passed element
 * \param h Handle of the socket
 * \param wtid structure to insert
 */
static void insertWTID(struct Handle *h, struct WaitingTIDs *element){
	element->next = h->first;
 	h->first = element;
	element->elected = 0;
}

/*! Function that looks in the list of current TIDs' structures and return the one with the corresponding tid if found, NULL otherwise
 * \param h Handle of the socket
 * \param tid Unique ID of the current request
 * \return return the request structure with the corresponding tid if found, NULL otherwise. */
static struct WaitingTIDs *getWTID(struct Handle *h, u_int tid){
	struct WaitingTIDs *ret;
	ret = h->first;
	while(ret!=NULL){
		if(ret->tid == tid){
			return ret;
		}
		else{
			ret = ret->next;
		}
	}
	return NULL;
}


/*! Function that looks in the list of current TIDs' structures and return the one with the corresponding tid if found, creates a new one otherwise
 * \param h Handle of the socket
 * \param tid Unique ID of the current request
 * \return if tid exists than return the WTID structure. If it doesn't it creates a new WTID with id tid. */
static struct WaitingTIDs *createWTID(struct Handle *h, u_int tid){
	struct WaitingTIDs *ret;
	ret = getWTID(h,tid);
	if(ret == NULL){
		if(h->garbage != NULL){
			ret = h->garbage;
			h->garbage = h->garbage->next;
			ret->tid = tid;
			ret->next = h->first;
			h->first = ret;
			ret->elected = 0;
		}
		else{
			ret = (struct WaitingTIDs *)calloc(1, sizeof(struct WaitingTIDs));
			ret->tid = tid;
			sem_init(&(ret->wsem), 0, 0);
			ret->next = h->first;
			h->first = ret;
			ret->elected = 0;
		}
	}
	return ret;
}

/*! Function that removes a WaitingTIDs structure if found in the list
 * \param h Handle of the socket
 * \param tid Unique ID of the current request
 * \return the structure if found, NULL otherwise
 */
static struct WaitingTIDs * removeWTID(struct Handle *h, u_int tid){
	struct WaitingTIDs *ret;
	struct WaitingTIDs *prev;
	//check if the id is in the list
	ret = h->first;
	prev = NULL;
	int cont = 1;
	while(cont && ret != NULL){
		if(ret->tid == tid){
			cont = 0;
		}
		else{
			prev = ret;
			ret = ret->next;
		}
	}
	if(ret!=NULL){
		if(prev != NULL){
			//remove from active list
			prev->next = ret->next;
		}
		if(h->first->tid == ret->tid){
			h->first = h->first->next;
		}
	}
	return ret;
}

/*! Function that moves to garbage collection a WaitingTIDs structure
 * \param h Handle of the socket
 * \param toGarb Structure to put in the garbage collector
 */
static void placeWTIDInGarbage(struct Handle *h, struct WaitingTIDs *toGarb){
	//Place in garbage collector
	toGarb->tid = 0;
	toGarb->elected = 0;
	toGarb->msg = 0;
	toGarb->GUID = 0;
	toGarb->size = 0;
	toGarb->next = h->garbage;
	h->garbage = toGarb;
}

/* \deprecated { 
 *! Function that deletes (move to garbage collection) a WaitingTIDs structure if found in the list
 * \param h Handle of the socket
 * \param tid Unique ID of the current request
 *}
 */

/*
static void deleteWTID(struct Handle *h, u_int tid){
	struct WaitingTIDs *ret;
	ret = removeWTID(h, tid);
	if(ret!=NULL) placeWTIDInGarbage(h, ret);
	
}
*/

/*! Cleans all the parameters and lists in the socket handle
 * \param h Handle of the socket
 */
static void cleanHandle(struct Handle *h){
	stdLog("clean all\n");
	h->UID = -1;
	close(h->sock);
	h->sock = -1;
	close(h->datasock);
	h->datasock = -1;
	h->waiting = 0;
	struct WaitingTIDs *temp;
	while(h->first!=NULL){
		temp = h->first;
		h->first = h->first->next;
		free(temp);
	}
	h->first = NULL;
	h->lastReq = -1;
	while(h->garbage!=NULL){
		temp = h->garbage;
		h->garbage = h->garbage->next;
		free(temp);
	}
	h->garbage = NULL;
}


/*
 * TODO
 * Unblock all threads waiting on semaphore with an error message
 * Clean all the used memory
 */
/*! Clean all the resources used by the socket.
 * \param h Handle of the socket
 */
static void cleanAll(struct Handle *h){
	stdLog("clean all\n");
	char tempName[128];
	memset(tempName, 0, 128);
	sprintf(tempName, "%s%010x", CLIENT_PATH, h->UID);
	unlink(tempName);
	cleanHandle(h);
}


//TODO: Check this is correct, in particular the error code
/*! Notify all the waiting requests that the socket has been closed and their request will not be satisfied.
 * \param h Handle of the socket
 * \param wtid Unique WaitingTIDs of the current request
 */
static void notifyAll(struct Handle *h, struct WaitingTIDs *wtid){
	stdLogVal("notifyAll process %u\n", wtid->tid);
	struct WaitingTIDs *list = h->first;
	while(list!=NULL){
		removeWTID(h, list->tid);
		list->msg = CLOSED_ERROR;
		if(list->tid != wtid->tid){
			sem_post(&(list->wsem));
		}
	}
}


/*! Transfers the leadership of the waiting process to another thread. If no other thread is waiting then sets the waiting parameter to false. This function is assumed to be ran inside a critical session
 * \param h Handle of the socket
 * \param wtid Unique WaitingTIDs of the current request
 */
static void transferLeadership(struct Handle *h, struct WaitingTIDs *wtid){
	stdLogVal("transferLeadership process%u \n", wtid->tid);
	struct WaitingTIDs *first = h->first;
	if(first != NULL) {
		stdLogVal("transferLeadership transfer leadership to %u ", first->tid);
		stdLogVal("process %u\n", wtid->tid);
		first->elected = 1;
		sem_post(&(first->wsem));
	}
	else{
		h->waiting = 0;
	}
}


/*! The leader waiting thread. When a new message is received it can check if there is the correspondent thread waiting for a message. If the message is intended for the same thread, it trasnfers leadership to another thread.
 * \param h Handle of the socket
 * \param wtid Unique WaitingTIDs of the current request
 */
static void waitingLeader(struct Handle *h, struct WaitingTIDs *wtid){
	stdLogVal("waitingLeader starting waiting process: %u \n", wtid->tid);
	int size = 0;
	char cont = 1, close = 0;
	u_int dst;
	char buffer[MAX_MSG_SIZE];
	struct MsgRecvReply *mrr;
	struct MsgSendReply *srr;
	struct MsgSendComplete *src;
	struct MsgCloseReply *mcr;
	struct MsgAttachReply *mar;
	struct MsgDetachReply *mdr;
	struct MsgGetReply *mgr;
	struct MsgGetSent *mgs;
	struct MsgGetComplete *mgc;
	struct MsgDoGetReply *mdgr;
	struct MsgGetResponseReply *mgrr;
	struct MsgGetResponseComplete *mgrc;
	struct WaitingTIDs *temp;
	while(cont){
		size = recv(h->sock, buffer, MAX_MSG_SIZE, 0);
		if(size < 0){
			errLogVal("waitingLeader receive error %d\n", errno);
			wtid->msg = -1;
			cont = 0;
		}
		else{
			msgType type = ((struct Msg *)buffer)->type;
			stdLogVal("recProc: received message of type %d\n", type);
			if(type == MSG_TYPE_RECV_REPLY){
				mrr = (struct MsgRecvReply *)buffer;
				sem_wait(&(h->hsem));			
				stdLogVal("waitingLeader receive reply, unlocking %u ", mrr->reqID);
				stdLogVal("process: %u \n", wtid->tid);
				temp = removeWTID(h, mrr->reqID);
				if(!temp){
					errLogVal("waitingLeader I don't have request MSG_TYPE_RECV_REPLY %u\n", mrr->reqID);
					sem_post(&(h->hsem));
					continue;
				}
				temp->size = mrr->size;
				temp->msg = mrr->retValue;
				temp->GUID = mrr->sGUID;
				dst = mrr->reqID;
			}
			else if(type == MSG_TYPE_SEND_REPLY){
				srr = (struct MsgSendReply *)buffer;
				sem_wait(&(h->hsem));			
				stdLogVal("waitingLeader send reply, unlocking %u ", srr->reqID);
				stdLogVal("process: %u \n", wtid->tid);
				temp = removeWTID(h, srr->reqID);
				if(!temp){
					errLogVal("waitingLeader I don't have request MSG_TYPE_SEND_REPLY %u\n", srr->reqID);
					sem_post(&(h->hsem));
					continue;
				}
				temp->msg = srr->retValue;
				dst = srr->reqID;
			}
			else if(type == MSG_TYPE_SEND_COMPLETE){
				src = (struct MsgSendComplete *)buffer;
				sem_wait(&(h->hsem));			
				stdLogVal("waitingLeader send complete, unlocking %u ", src->reqID);
				stdLogVal("process: %u \n", wtid->tid);
				temp = removeWTID(h, src->reqID);
				if(!temp){
					errLogVal("waitingLeader I don't have request MSG_TYPE_SEND_COMPLETE %u\n", src->reqID);
					sem_post(&(h->hsem));
					continue;
				}
				dst = src->reqID;
			}
			else if(type == MSG_TYPE_GET_SEND_REPLY){
				mgr = (struct MsgGetReply*)buffer;
				sem_wait(&(h->hsem));
				stdLogVal("waitingLeader send complete, unlocking %u ", mgr->reqID);
				stdLogVal("process: %u \n", wtid->tid);
				temp = removeWTID(h, mgr->reqID);
				if(!temp){
					errLogVal("waitingLeader I don't have request MSG_TYPE_GET_SEND_REPLY %u\n", mgr->reqID);
					sem_post(&(h->hsem));
					continue;
				}
				temp->msg = mgr->retValue;
				dst = mgr->reqID;
			}
			else if(type == MSG_TYPE_GET_SEND_COMPLETE){
				mgs = (struct MsgGetSent*)buffer;
				sem_wait(&(h->hsem));
				stdLogVal("waitingLeader send complete, unlocking %u ", mgs->reqID);
				stdLogVal("process: %u \n", wtid->tid);
				temp = removeWTID(h, mgs->reqID);
				if(!temp){
					errLogVal("waitingLeader I don't have request MSG_TYPE_GET_SEND_COMPLETE %u\n", mgs->reqID);
					sem_post(&(h->hsem));
					continue;
				}
				dst = mgs->reqID;
			}
			else if (type == MSG_TYPE_GET_SEND_RECV){
				mgc = (struct MsgGetComplete*)buffer;
				sem_wait(&(h->hsem));
				stdLogVal("waitingLeader send complete, unlocking %u ", mgc->reqID);
				stdLogVal("process: %u \n", wtid->tid);
				temp = removeWTID(h, mgc->reqID);
				if(!temp){
					errLogVal("waitingLeader I don't have request MSG_TYPE_GET_SEND_RECV %u\n", mgc->reqID);
					sem_post(&(h->hsem));
					continue;
				}
				temp->msg = mgc->retValue;
				temp->size = mgc->size;
				dst = mgc->reqID;
			}
			else if (type == MSG_TYPE_DO_GET_REPLY){
				mdgr = (struct MsgDoGetReply*)buffer;
				sem_wait(&(h->hsem));
				stdLogVal("waitingLeader send complete, unlocking %u ", mdgr->reqID);
				stdLogVal("process: %u \n", wtid->tid);
				temp = removeWTID(h, mdgr->reqID);
				if(!temp){
					errLogVal("waitingLeader I don't have request MSG_TYPE_DO_GET_REPLY %u\n", mdgr->reqID);
					sem_post(&(h->hsem));
					continue;
				}
				temp->msg = mdgr->retValue;
				temp->GUID = mdgr->srcGUID;
				temp->dstGUID = mdgr->dstGUID;
				temp->opID = mdgr->getID;
				temp->size = mdgr->size;
				dst = mdgr->reqID;
			}
			else if (type == MSG_TYPE_GET_RESPONSE_REPLY){
				mgrr = (struct MsgGetResponseReply*)buffer;
				sem_wait(&(h->hsem));
				stdLogVal("waitingLeader send complete, unlocking %u ", mgrr->reqID);
				stdLogVal("process: %u \n", wtid->tid);
				temp = removeWTID(h, mgrr->reqID);
				if(!temp){
					errLogVal("waitingLeader I don't have request MSG_TYPE_GET_RESPONSE_REPLY %u\n", mgrr->reqID);
					sem_post(&(h->hsem));
					continue;
				}
				temp->msg = mgrr->retValue;
				dst = mgrr->reqID;
			}
			else if (type == MSG_TYPE_GET_RESPONSE_COMPLETE){
				mgrc = (struct MsgGetResponseComplete*)buffer;
				sem_wait(&(h->hsem));
				stdLogVal("waitingLeader send complete, unlocking %u ", mgrc->reqID);
				stdLogVal("process: %u \n", wtid->tid);
				temp = removeWTID(h, mgrc->reqID);
				if(!temp){
					errLogVal("waitingLeader I don't have request MSG_TYPE_GET_RESPONSE_COMPLETE %u\n", mgrc->reqID);
					sem_post(&(h->hsem));
					continue;
				}
				dst = mgrc->reqID;
			}
			else if(type == MSG_TYPE_ATTACH_REPLY){
				mar = (struct MsgAttachReply *)buffer;
				sem_wait(&(h->hsem));			
				stdLogVal("waitingLeader attach reply, unlocking %u ", mar->reqID);
				stdLogVal("process: %u \n", wtid->tid);
				temp = removeWTID(h, mar->reqID);
				if(!temp){
					errLogVal("waitingLeader I don't have request MSG_TYPE_ATTACH_REPLY %u\n", mrr->reqID);
					sem_post(&(h->hsem));
					continue;
				}
				temp->msg = mar->retValue;
				dst = mar->reqID;
			}
			else if(type == MSG_TYPE_DETACH_REPLY){
				mdr = (struct MsgDetachReply *)buffer;
				sem_wait(&(h->hsem));			
				stdLogVal("waitingLeader detach reply, unlocking %u ", mdr->reqID);
				stdLogVal("process: %u \n", wtid->tid);
				temp = removeWTID(h, mdr->reqID);
				if(!temp){
					errLogVal("waitingLeader I don't have request MSG_TYPE_DETACH_REPLY %u\n", mrr->reqID);
					sem_post(&(h->hsem));
					continue;
				}
				temp->msg = mdr->retValue;
				dst = mdr->reqID;
			}
			else if(type == MSG_TYPE_CLOSE_REPLY){
				mcr = (struct MsgCloseReply *)buffer;
				sem_wait(&(h->hsem));			
				stdLogVal("waitingLeader close reply, unlocking %u ", mcr->reqID);
				stdLogVal("process: %u \n", wtid->tid);
				temp = removeWTID(h, mcr->reqID);
				if(!temp){
					errLogVal("waitingLeader I don't have request MSG_TYPE_CLOSE_REPLY %u\n", mrr->reqID);
					sem_post(&(h->hsem));
					continue;
				}
				temp->msg = mcr->retValue;
				dst = mcr->reqID;
				close = 1;
			}
			else{
				sem_wait(&(h->hsem));
				dst = 0;
				errLog("recProc: received something wrong\n");
			}
			if(close){
				notifyAll(h,wtid);
				//There must be a point where I clean up the memory allocated
				cleanAll(h);
			}
			if(dst == wtid->tid){
				transferLeadership(h, wtid);
				cont = 0;
			}
			else{
				sem_post(&(temp->wsem));
			}
			sem_post(&(h->hsem));
		}
	}
}


/*! The follower waiting thread. It will be unlocked if it is elected to leader or if the message from the stack is received.
 * \param h Handle of the socket
 * \param wtid Unique WaitingTIDs of the current request
 */
static void waitingFollower(struct Handle *h, struct WaitingTIDs *wtid){
	sem_wait(&(wtid->wsem));
	stdLogVal("waitFollower unlocked process: %u \n", wtid->tid);
	if (wtid->elected) {
		stdLogVal("waitFollower elected to be the new leader process: %u \n", wtid->tid);
		waitingLeader(h, wtid);
	}
}


/*! Called by thread waiting for a reply from the stack. If no leader has been elected, the thread self-elects to leadership, otherwise waits as a follower.
 * \param h Handle of the socket
 * \param wtid Unique WaitingTIDs of the current request
 */
static void waitResponse(struct Handle *h, struct WaitingTIDs *wtid){
	stdLogVal("waitResponse starting waiting process: %u \n", wtid->tid);
	sem_wait(&(h->hsem));
	if(h->waiting){
		if(wtid->elected){
			stdLogVal("waitResponse somebody else named me leader process: %u \n", wtid->tid);
			sem_trywait(&(wtid->wsem));
			sem_post(&(h->hsem));
			waitingLeader(h, wtid);
		}
		else{
			stdLogVal("waitResponse somebody else is already the leader process: %u \n", wtid->tid);
			sem_post(&(h->hsem));
			waitingFollower(h, wtid);
		}
	}
	else{
		stdLogVal("waitResponse no other thread waiting, i will be the leader process: %u \n", wtid->tid);
		h->waiting = 1;
		sem_post(&(h->hsem));
		waitingLeader(h, wtid);
	}
}


/*! Temp.
 * \param sock temp
 * \param buffer temp
 * \param size temp
 */
static int sendCtlMsg(int sock, void* buffer, u_int size) {
	struct sockaddr_un ctlAddr;
	int ret;
	ctlAddr.sun_family = AF_LOCAL;
	strncpy(ctlAddr.sun_path, SERVER_PATH, sizeof(ctlAddr.sun_path));
	ret = sendto(sock, buffer, size, 0, (struct sockaddr*)&ctlAddr, sizeof(ctlAddr));
	if (ret < 0) {
		errLogStr("sendCtlMsg: sendto error for %s\n", ctlAddr.sun_path);
	    return errno;
	}
	return 0;
}


/*! Temp.
 * \param sock temp
 * \param buffer temp
 * \param size temp
 */
static int sendData(struct Handle *h, void *buffer, u_int datasize){
	int size, sent=0;
	while(sent<datasize){
		if(datasize - sent > MAX_SOCK_SEND){
			size = MAX_SOCK_SEND;
		}
		else{
			size = datasize - sent;
		}
		size = send(h->datasock, buffer + sent, size, 0);
		if(size<0){
			// notify of error
			return -1;
		}
		stdLogVal("sendData sent %d bytes\n", size);
		sent += size;
	}
	return (int)datasize;
}


//	int i = 0;
//	while(reminSize) {
//		if ( (reminSize<=FIRST_PAYLOAD_SIZE) && (i%MAX_CHUNK_SIZE==0) ) {
//			size = sendto(h->sock, buffer, reminSize, 0, (struct sockaddr *)&dataAddr, sizeof(dataAddr));
//			reminSize -= size;
//			i++;
//		}
//		else if ( (reminSize>FIRST_PAYLOAD_SIZE) && (i%MAX_CHUNK_SIZE==0) ) {
//			size = sendto(h->sock, buffer, FIRST_PAYLOAD_SIZE, 0, (struct sockaddr *)&dataAddr, sizeof(dataAddr));
//			reminSize -= size;
//			i++;
//		}
//		else if ( (reminSize<=MAX_PAYLOAD_SIZE) && (i%MAX_CHUNK_SIZE!=0) ) {
//			size = sendto(h->sock, buffer+msgsize-reminSize, reminSize, 0, (struct sockaddr *)&dataAddr, sizeof(dataAddr));
//			reminSize -= size;
//			i++;
//		}
//		else if ( (reminSize>MAX_PAYLOAD_SIZE) && (i%MAX_CHUNK_SIZE!=0) ) {
//			size = sendto(h->sock, buffer+msgsize-reminSize, MAX_PAYLOAD_SIZE, 0, (struct sockaddr *)&dataAddr, sizeof(dataAddr));
//			reminSize -= size;
//			i++;
//		}
//	}
/*! Temp.
 * \param sock temp
 * \param buffer temp
 * \param size temp
 */
static void receiveData(struct Handle *h, void *buffer, int datasize){
	int ret, received = 0;
	while(received != datasize){
		ret = recv(h->datasock, buffer + received, datasize - received, 0);
		received += ret;
		stdLogVal("sendRecvReq received %d bytes\n", ret);
	}
}


/*! Temp.
 * \param sock temp
 * \param buffer temp
 * \param size temp
 */
/*
static int getMaxSocketBuffer(int sockfd){
//	int sendbuff;
//	socklen_t optlen;
//	optlen = sizeof(sendbuff);
//	//Check if this option actually gets you the max size
//	int ret = getsockopt(sockfd, SOL_SOCKET, SO_MAX_MSG_SIZE, &sendbuff, &optlen);
//	if(ret == -1){
//		return -1;
//	}
//	else return sendbuff;
	return 0;
}
*/

/*! Temp.
 * \param sock temp
 * \param buffer temp
 * \param size temp
 */
/*
static int increaseSocketBuffer(int sockfd){
	int maxBuff = getMaxSocketBuffer(sockfd);
	int sendbuff;
	if(maxBuff < 0){
		return -1;
	}
	int res = setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sendbuff, sizeof(sendbuff));
	if(res == -1)
		return -1;
	else return 1;
}
*/

/*! Temp.
 * \param sock temp
 * \param buffer temp
 * \param size temp
 */
static u_int getUniqueId(char *addr){
	srand(time(NULL));
	int nExist = 1;
	u_int rnd;
	struct stat buf;
	while(nExist){
		rnd = (u_int)rand()+(u_int)rand();
		memset((void *)addr, 0, 128);
		sprintf(addr, "%s%010x", CLIENT_PATH, rnd);
		if(stat(addr, &buf)){
			nExist = 0;
		}
		else{
			stdLogStr("rnd val address %s already in use\n", addr);
			stdLogStr("error %s\n", strerror(errno));
		}
	}
	return rnd;
}


/*! The open function shall define the initialization of a communication session between the application layer and the underneath layers. On doing so the system is informed of the intent of the application and can choose the appropriate transportation service and initialize the needed resources.
 * \param h Handle of the socket
 * \param profile Descriptive element (a string) that informs the system about the intent of the application
 * \param opts is optional are additional parameters that can be provided to request additional services expressed as flags. If no value is provided, the system will make the initialization based on the sole Profile
 * \param src_GUID is optional and represents the set of GUIDs that the application is willing of listening at. If no value is provided, the system should select the system default GUID identifier for the application.
 * \return 0 if the socket was opened succesfully, an error code otherwise (look at error.h for error definitions)
 */
int mfopen(struct Handle *h, const char *profile, mfflag_t opts, const int src_GUID){
	stdLog("open new mf socket\n");
	if(h == NULL || profile == NULL || strlen(profile)>=LEN_PROF) {
		errLog("error: errors in parameters\n");
		return PARAM_ERROR;
	}
	int sock, sock2;
	int size, ret;
	u_char buffer[256];//used both as send and receive buffer
	struct MsgOpen *open;
	struct MsgOpenReply *openR;
	struct sockaddr_un myAddr, myAddr2;
	memset(buffer, 0, 256);
	
	/*
	 * create local sockets to do IPC with mfandroidstack
	 */
	stdLog("mfopen creating socket for communication with stack\n");
	sock = socket(AF_LOCAL, SOCK_DGRAM, 0);
	if (sock < 0) {
		errLogVal("mfopen_u: socket() error %d\n", errno);
		return SOCKET_ERROR;
	}
	
	//bind, so we could just use recv() to receive reply message instead of recvfrom();
	char tempAddr[128];
	h->UID = getUniqueId(tempAddr);
	myAddr.sun_family = AF_LOCAL;
	strcpy(myAddr.sun_path, tempAddr);
	unlink(myAddr.sun_path);
	ret = bind(sock, (struct sockaddr *)&myAddr, sizeof(myAddr));
	if (ret<0) {
		errLogStr("mfopen_u: bind() error %s\n",strerror(errno));
		errLogStr("mfopen_u: bind() error socket address %s\n",myAddr.sun_path);
		close(sock);
		return errno;
	}
	
	/*
	 * send MsgOpen message to mfandroidstack
	 */
	stdLog("mfopen preaparing message for stack\n");
	open = (struct MsgOpen *)buffer;
	open->type = MSG_TYPE_OPEN;
	open->UID = h->UID;
	strncpy(open->profile, profile, strlen(profile));
	open->opts = opts;
	open->srcGUID = src_GUID;
	stdLog("mfopen send open control message to stack\n");
	if((ret=sendCtlMsg(sock, buffer, sizeof(struct MsgOpen)))!=0){
		cleanHandle(h);
		return ret;
	}
	
	/*
	 * receive reply and check the return value
	 */
	size = recv(sock, buffer, sizeof(struct MsgOpenReply), 0);
	stdLog("mfopen stack replied to open request\n");
	if (size < 0) {
		errLogVal("mfopen_u: recv() error %d\n", errno);
		cleanHandle(h);
		return SOCKET_ERROR;
	}
	openR = (struct MsgOpenReply *)buffer;
	if (openR->type != MSG_TYPE_OPEN_REPLY) {
		errLog("mfopen_u: openR->type != MSG_TYPE_OPEN_REPLY, something wrong, this should not happen\n");
		cleanHandle(h);
		return WRONG_MESSAGE;
	}
	//Check the different possible ret values
	if (openR->retValue == 1) {
		errLog("mfopen_u: openR->retValue == 1, this should not happen\n");
		cleanHandle(h);
		return -3;
	} else if (openR->retValue == 2) {
		errLog("mfopen_u: openR->retValue == 2, this should not happen\n");
		cleanHandle(h);
		return -4;
	}
	
	//TODO: move to connection oriented model
	/*
	 * The open was succesful
	 *   => connect to data socket created by the stack to exchange data
	 */
	sock2 = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (sock2 < 0) {
		errLog("mfopen_u: socket() error for client receiving socket\n");
		return errno;
	}
	/*
	 * bind, so we could just use recv() to receive reply message instead of recvfrom();
	 */
	myAddr2.sun_family = AF_LOCAL;
	sprintf(myAddr2.sun_path, "%s%010x", SERVER_DATA_PATH, h->UID);
	ret = connect(sock2, (struct sockaddr *)&myAddr2, sizeof(myAddr2));
	if (ret<0) {
		errLogStr("mfopen_u: connect() connection error creating data path with stack %s\n",strerror(errno));
		close(sock2);
		return errno;
	}
	
	/*
	 * Finish creating handle variable
	 */
	h->sock = sock;
	h->datasock = sock2;
	//h->cont = 1;
	h->waiting = 0;
	h->first = NULL;
	h->lastReq = 0;
	h->garbage = NULL;
	stdLog("mfopen: create new semaphore for thread\n");
	if(sem_init(&(h->hsem), 0, 1)) {
		errLogVal("mfopen_u: sem_init() error %d\n", errno);
		cleanAll(h);
		return SEMAPHORE_ERROR;
	}
//	stdLog("create new thread for socket\n");
//	if(pthread_create(&(h->rect), 0, &recProc, h)) {
//		errLogVal("mfopen_u: pthread_create() error %d\n", errno);
//		cleanHandle(h);
//		return THREAD_ERROR;
//	}
	return 0;
}


/*! The send operation shall send data to a destination GUID as messages with no size limits except for the ones defined by system resources. The way the message is delivered to the entity defined by the GUID is determined by the session characteristics defined during the open call and eventually by additional service options.
 * \param h Handle of the socket
 * \param buffer Data message to be sent
 * \param size Size of the message to be sent (smaller or equal to the size of buffer)
 * \param dst_GUID Destination GUID
 * \param opts Additional service options used to determine how the message is delivered expressed as flags
 * \return The number of sent bytes if succesfully executed, an error code otherwise
 */
int mfsend(struct Handle *h, void *buffer, u_int size, const int dst_GUID, mfflag_t opts, const int dst_NA){
	stdLog("mfsend\n");
	if (h == NULL || buffer == NULL || size == 0) {
		errLog("mfsend: error: errors in parameters\n");
		return PARAM_ERROR;
	}
	struct MsgSend send;
	struct WaitingTIDs *wtid;
	sem_wait(&(h->hsem));
	u_int reqID = ++h->lastReq;
	wtid = createWTID(h, reqID);
	sem_post(&(h->hsem));
	send.type = MSG_TYPE_SEND;
	send.size = size;
	send.UID = h->UID;
	send.reqID = reqID;
	send.dst_GUID = dst_GUID;
	send.opts = opts;
	send.dst_NA = dst_NA;
	stdLog("mfsend send control message to stack\n");
	sendCtlMsg(h->sock, (void*)(&send), sizeof(struct MsgSend));
	/*
	 * receive reply and check the return value
	 */
	waitResponse(h, wtid);
	//Check ret values from stack
	if (wtid->msg == 1) {
		errLog("mfsend_u: interface not available.\n");
		return -4;
	}
	if (wtid->msg == 2) {
		errLog("mfsend_u: GNRS lookup failed.\n");
		return -5;
	}
	if (wtid->msg == 3) {
		errLog("mfsend_u: unable to allocate resource.\n");
		return -6;
	}
	stdLog("mfsend transfer data to stack\n");
	/*
	 * wait to know that the message has been completely transfered to the stack
	 */
	sem_wait(&(h->hsem));
    //reqID = ++h->lastReq;
	//wtid->tid = reqID;
	insertWTID(h, wtid);
    sem_post(&(h->hsem));
	sendData(h, buffer, size);
	waitResponse(h, wtid);
	sem_wait(&(h->hsem));
	placeWTIDInGarbage(h, wtid);
	sem_post(&(h->hsem));
	stdLog("mfsend complete\n");
	return size;
}


/*! Temp.
 * \param sock temp
 * \param buffer temp
 * \param size temp
 */
static int sendRecvReq(struct Handle *h, int *sGUID, void *buffer, u_int size, const int *src_GUID, const int n_GUID, char blk){
	struct MsgRecv *rc;
	struct WaitingTIDs *wtid;
	u_char b[256];
	memset(b, 0, 256);
	
	sem_wait(&(h->hsem));
	u_int reqID = ++h->lastReq;
	wtid = createWTID(h, reqID);
	sem_post(&(h->hsem));
	
	rc = (struct MsgRecv *)b;
	rc->UID = h->UID;
	rc->type = MSG_TYPE_RECV;
	rc->size = size;
	rc->blk = blk;
	rc->reqID = reqID;
	sendCtlMsg(h->sock, b, sizeof(struct MsgRecv));
	stdLog("sendRecvReq request sent\n");
	waitResponse(h, wtid);
	stdLog("sendRecvReq reply received\n");
	
	//Check ret codes
	if (wtid->msg == 2) { //size is too small. This should not happen at all
		stdLog("sendRecvReq buffer size is too small\n");
		return(-2);
	}
	if (wtid->msg == 1) { //nothing in queue. this should not happen for blocking
		stdLog("sendRecvReq nothing in queue (should not happen for blocking\n");
		return(-1);
	}
	/*
	 * receive data
	 */
	if(sGUID!=NULL){
		*sGUID = (int)wtid->GUID;
	}
	stdLogVal("sendRecvReq expecting %d bytes\n", (int)(wtid->size));
	receiveData(h, buffer, wtid->size);
	int recvd = wtid->size;
	stdLog("sendRecvReq received all data\n");
	sem_wait(&(h->hsem));
	placeWTIDInGarbage(h, wtid);
	sem_post(&(h->hsem));
	return recvd;
}


/*! The recv operation shall write into the buffer passed the oldest not read message received. If no message is available then waits for a new message to be received.
 * \param h Handle of the socket
 * \param sGUID pointer to GUID. If pointer is not NULL it will be written the GUID of the sender of the received message.
 * \param buffer Buffer used to pass the message to the application
 * \param size Size of the provided buffer
 * \param src_GUID Array of source GUIDs to listen messages from. If NULL, any GUID is considered.
 * \param n_GUID Number of GUIDs passed with src_GUID
 * \return The number of passed bytes if succesfully exectued, an error code otherwise
 */
int mfrecv_blk(struct Handle *h, int *sGUID, void *buffer, u_int size, const int *src_GUID, const int n_GUID) {
	stdLog("mfrecv_blk\n");
	return sendRecvReq(h, sGUID, buffer, size, src_GUID, n_GUID, (char)1);
}

/*! The recv operation shall write into the buffer passed the oldest not read message received. If no message is available then returns an error
 * \param h Handle of the socket
 * \param sGUID pointer to GUID. If pointer is not NULL it will be written the GUID of the sender of the received message.
 * \param buffer Buffer used to pass the message to the application
 * \param size Size of the provided buffer
 * \param src_GUID Array of source GUIDs to listen messages from. If NULL, any GUID is considered.
 * \param n_GUID Number of GUIDs passed with src_GUID
 * \return The number of passed bytes if succesfully exectued, an error code otherwise
 */
int mfrecv(struct Handle *h, int *sGUID, void *buffer, u_int size, const int *src_GUID, const int n_GUID){
	stdLog("mfrecv\n");
	return sendRecvReq(h, sGUID, buffer, size, src_GUID, n_GUID, (char)0);
}


/*! The attach function provides the functionalities to add additional GUIDs at the the one originally identified during the open call.
 GUID-set represents the set of GUIDs that the application wants to add at the set that it is listening at.
 * \param h Handle of the socket
 * \param GUIDs array of GUIDs to attach
 * \param nGUID number of GUIDs in the array
 * \return 0 if completed succesfully, an error code otherwise
 */
int mfattach(struct Handle *h, const int *GUIDs, const int nGUID){
	struct MsgAttach ma;
	struct WaitingTIDs *wtid;
	sem_wait(&(h->hsem));
	u_int reqID = ++h->lastReq;
	wtid = createWTID(h, reqID);
	sem_post(&(h->hsem));
	ma.type = MSG_TYPE_ATTACH;
	ma.UID = h->UID;
	ma.reqID = reqID;
	ma.nGUID = nGUID;
	int i;
	for(i = 0; i<nGUID; i++){
		ma.GUIDs[i] = GUIDs[i];
	}
	sendCtlMsg(h->sock, (char*)&ma, sizeof(struct MsgAttach));
	waitResponse(h, wtid);
	int ret = 0;
	if (wtid->msg == ATTACH_ERROR) {
		ret = ATTACH_ERROR;
	}
	sem_wait(&(h->hsem));
	placeWTIDInGarbage(h, wtid);
	sem_post(&(h->hsem));
	return ret;
}


/*! The detach function shall provide a way of removing GUIDs form the one originally identified during the open call or the ones added through the use of "attach".
 GUID-set represents the set of GUIDs that the application wants to remove from the set that it is listening at.
 * \param h Handle of the socket
 * \param GUIDs GUIDs array of GUIDs to detach
 * \param nGUID number of GUIDs in the array
 * \return 0 if completed succesfully, an error code otherwise
 */
int mfdetach(struct Handle *h, const int *GUIDs, const int nGUID){
	struct MsgDetach md;
	struct WaitingTIDs *wtid;
	sem_wait(&(h->hsem));
	u_int reqID = ++h->lastReq;
	wtid = createWTID(h, reqID);
	sem_post(&(h->hsem));
	md.type = MSG_TYPE_DETACH;
	md.UID = h->UID;
	md.reqID = reqID;
	md.nGUID = nGUID;
	int i;
	for(i = 0; i<nGUID; i++){
		md.GUIDs[i] = GUIDs[i];
	}
	sendCtlMsg(h->sock, (char*)&md, sizeof(struct MsgDetach));
	waitResponse(h, wtid);
	int ret = 0;
	if (wtid->msg == DETACH_ERROR) {
		ret = DETACH_ERROR;
	}
	sem_wait(&(h->hsem));
	placeWTIDInGarbage(h, wtid);
	sem_post(&(h->hsem));
	return ret;
}

//TODO: needs to be completed
/*! The close function shall close the session initiated with an open call and clear the resources that were allocated.
 * \param h Handle of the socket
 * \return 0 if the socket was closed succesfully, an error code otherwise
 */
int mfclose(struct Handle *h) {
	stdLog("mfclose\n");
	struct MsgClose *clo;
	struct WaitingTIDs *wtid;
	char buffer[256];
	memset(buffer, 0, 256);
	clo = (struct MsgClose *)buffer;
	sem_wait(&(h->hsem));
	u_int reqID = ++h->lastReq;
	wtid = createWTID(h, reqID);
	sem_post(&(h->hsem));
	clo->type = MSG_TYPE_CLOSE;
	clo->UID = h->UID;
	clo->reqID = reqID;
	sendCtlMsg(h->sock, buffer, sizeof(struct MsgClose));
	/*
	 * receive reply and check the return value
	 */
	waitResponse(h, wtid);
	if (wtid->msg == 1) {
		return(-1);
	}
	stdLog("close complete\n");
	return 0;
}


/*! Temp.
 * \param sock temp
 * \param buffer temp
 * \param size temp
 */
int mfget(struct Handle *h, const int dst_GUID, void *request, u_int requestSize, void *buffer, u_int bufferSize, mfflag_t opts){
	stdLog("mfget\n");
//	//INFO: remove this comments disables the operation
//	errLog("mfget: error: operation not available at the moment\n");
//	return OPERATION_NOT_PERMITTED;
	if (h == NULL || buffer == NULL || request == NULL || bufferSize == 0 || requestSize == 0) {
		errLog("mfget: error: errors in parameters\n");
		return PARAM_ERROR;
	}
	struct MsgGet get;
	struct WaitingTIDs *wtid;
	sem_wait(&(h->hsem));
	u_int reqID = ++h->lastReq;
	wtid = createWTID(h, reqID);
	sem_post(&(h->hsem));
	get.type = MSG_TYPE_GET_SEND;
	get.size = requestSize;
	get.UID = h->UID;
	get.reqID = reqID;
	get.dst_GUID = dst_GUID;
	get.opts = opts;
	stdLog("mfget send control message to stack\n");
	sendCtlMsg(h->sock, (void*)(&get), sizeof(struct MsgGet));
	/*
	 * receive reply and check the return value
	 */
	waitResponse(h, wtid);
	//Check ret values from stack
	if (wtid->msg == 1) {
		errLog("mfget: interface not available.\n");
		return -4;
	}
	if (wtid->msg == 2) {
		errLog("mfget: GNRS lookup failed.\n");
		return -5;
	}
	if (wtid->msg == 3) {
		errLog("mfget: unable to allocate resource.\n");
		return -6;
	}
	stdLog("mfget transfer data to stack\n");
	sem_wait(&(h->hsem));
	insertWTID(h, wtid);
	sem_post(&(h->hsem));
	sendData(h, request, requestSize);
	/*
	 * wait to know that the message has been completely transfered to the stack
	 */
	waitResponse(h, wtid);
	sem_wait(&(h->hsem));
	insertWTID(h, wtid);
	sem_post(&(h->hsem));
	
	//get request has been succesfully sent, wait for the actual reply
	stdLog("mfget wait for data to come\n");
	waitResponse(h, wtid);
	stdLog("mfget reply received\n");
	//Check ret codes
	if (wtid->msg == 2) { //size is too small. This should not happen at all
		stdLog("mfget buffer size is too small\n");
		return -2;
	}
	if (wtid->msg == 1) { //nothing in queue. this should not happen for blocking
		stdLog("mfget nothing in queue (should not happen for blocking\n");
		return -1;
	}
	/*
	 * receive data
	 */
	stdLogVal("mfget expecting %d bytes\n", (int)(wtid->size));
	//TODO: Modify the function so that it receives at max the buffer size and discard the rest of the data
	receiveData(h, buffer, wtid->size);
	int recvd = wtid->size;
	stdLog("mfget received all data\n");
	sem_wait(&(h->hsem));
	placeWTIDInGarbage(h, wtid);
	sem_post(&(h->hsem));
	return recvd;
}


/*! Temp.
 * \param sock temp
 * \param buffer temp
 * \param size temp
 */
int mfdo_get(struct Handle *h, struct GetInfo *info, void *buffer, u_int size){
//	//INFO: remove this comments disables the operation
//        errLog("mfdo_get: error: operation not available at the moment\n");
//        return OPERATION_NOT_PERMITTED;
	if (h == NULL || buffer == NULL || size == 0 || info == NULL) {
		errLog("mfdo_get: error: errors in parameters\n");
		return PARAM_ERROR;
	}
	struct MsgDoGet dg;
	struct WaitingTIDs *wtid;
	
	sem_wait(&(h->hsem));
	u_int reqID = ++h->lastReq;
	wtid = createWTID(h, reqID);
	sem_post(&(h->hsem));
	dg.UID = h->UID;
	dg.type = MSG_TYPE_DO_GET;
	dg.size = size;
	dg.reqID = reqID;
	sendCtlMsg(h->sock, &dg, sizeof(struct MsgDoGet));
	stdLog("mfdo_get request sent\n");
	waitResponse(h, wtid);
	stdLog("mfdo_get reply received\n");
	
	//Check ret codes
	if (wtid->msg == 2) { //size is too small. This should not happen at all
		stdLog("mfdo_get buffer size is too small\n");
		return(-2);
	}
	if (wtid->msg == 1) { //nothing in queue. this should not happen for blocking
		stdLog("mfdo_get nothing in queue (should not happen for blocking\n");
		return(-1);
	}
	/*
	 * receive data
	 */
	info->srcGuid = wtid->GUID;
	info->dstGuid = wtid->dstGUID;
	info->UID = wtid->opID;
	stdLogVal("mfdo_get expecting %d bytes\n", (int)(wtid->size));
	receiveData(h, buffer, wtid->size);
	int recvd = wtid->size;
	stdLog("mfdo_get received all data\n");
	sem_wait(&(h->hsem));
	placeWTIDInGarbage(h, wtid);
	sem_post(&(h->hsem));
	return recvd;
}


/*! Temp.
 * \param sock temp
 * \param buffer temp
 * \param size temp
 */
int mfget_response(struct Handle *h, struct GetInfo *info, void *buffer, u_int size, mfflag_t opts){
	stdLog("mfget_response\n");
//        //INFO: remove this comments disables the operation
	if (h == NULL || buffer == NULL || size == 0 || info==NULL) {
		errLog("mfget_response: error: errors in parameters\n");
		return PARAM_ERROR;
	}
	struct MsgGetResponse send;
	struct WaitingTIDs *wtid;
	sem_wait(&(h->hsem));
	u_int reqID = ++h->lastReq;
	wtid = createWTID(h, reqID);
	sem_post(&(h->hsem));
	send.type = MSG_TYPE_GET_RESPONSE;
	send.size = size;
	send.UID = h->UID;
	send.reqID = reqID;
	send.srcGUID = info->srcGuid;
	send.dstGUID = info->dstGuid;
	send.getID = info->UID;
	send.opts = opts;
	stdLog("mfget_response send control message to stack\n");
	sendCtlMsg(h->sock, (void*)(&send), sizeof(struct MsgGetResponse));
	/*
	 * receive reply and check the return value
	 */
	waitResponse(h, wtid);
	//Check ret values from stack
	if (wtid->msg == 1) {
		errLog("mfget_response: interface not available.\n");
		return -4;
	}
	if (wtid->msg == 2) {
		errLog("mfget_response: GNRS lookup failed.\n");
		return -5;
	}
	if (wtid->msg == 3) {
		errLog("mfget_response: unable to allocate resource.\n");
		return -6;
	}
	stdLog("mfget_response transfer data to stack\n");
	sem_wait(&(h->hsem));
	insertWTID(h, wtid);
    sem_post(&(h->hsem));
	sendData(h, buffer, size);
	/*
	 * wait to know that the message has been completely transfered to the stack
	 */
	waitResponse(h, wtid);
	sem_wait(&(h->hsem));
	placeWTIDInGarbage(h, wtid);
	sem_post(&(h->hsem));
	stdLog("mfget_response complete\n");
	return size;
}

