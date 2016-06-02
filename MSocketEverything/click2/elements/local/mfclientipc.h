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
 * @file   mfapi.h
 * @author wontoniii (bronzino@winlab.rutgers.edu)
 * @date   April, 2014
 * @brief  Common IPC structures.
 *
 * Common IPC structures used in communications between the API and the network protocol stack
 */

#ifndef MFCLIENTIPC_H_
#define MFCLIENTIPC_H_

#include <include/mfflags.h>

#ifdef __ANDROID__
#ifdef ANDROID2
#define SERVER_PATH "/data/mfdemo/mfstack"
#define CLIENT_PATH "/data/mfdemo/mfclient" // +uid
#define SERVER_DATA_PATH "/data/mfdemo/mfdata" // +uid
#else
#define SERVER_PATH "/sdcard/mfdemo/mfstack"
#define CLIENT_PATH "/sdcard/mfdemo/mfclient"
#define SERVER_DATA_PATH "/sdcard/mfdemo/mfdata"
#endif
#else
#define SERVER_PATH "/data/mfdemo/mfstack"
#define CLIENT_PATH "/data/mfdemo/mfclient"
#define SERVER_DATA_PATH "/data/mfdemo/mfdata"
#endif

#define LEN_PROF 12
#define LEN_OPTS 12
#define MAX_ATTACH 10

//Make sure that this is at least as big as the biggest message
#define MAX_MSG_SIZE 255

#define MAX_SOCKET_PDU 1
#define MAX_SOCK_SEND 66560

#ifdef __cplusplus
extern "C" {
#endif
	
	typedef enum {
		MSG_TYPE_OPEN = 1,
		MSG_TYPE_OPEN_REPLY, //2
		MSG_TYPE_SEND, //3
		MSG_TYPE_SEND_REPLY, //4
		MSG_TYPE_SEND_COMPLETE, //5
		MSG_TYPE_RECV, //6
		MSG_TYPE_RECV_REPLY, //7
		MSG_TYPE_CLOSE, //8
		MSG_TYPE_CLOSE_REPLY, //9
		MSG_TYPE_ATTACH, //10
		MSG_TYPE_ATTACH_REPLY, //11
		MSG_TYPE_DETACH, //12
		MSG_TYPE_DETACH_REPLY, //13
		MSG_TYPE_AFTER_CLOSE, //14
		MSG_TYPE_GET_SEND, //15
		MSG_TYPE_GET_SEND_REPLY, //16
		MSG_TYPE_GET_SEND_COMPLETE, //17
		MSG_TYPE_GET_SEND_RECV, //18
		MSG_TYPE_DO_GET, //19
		MSG_TYPE_DO_GET_REPLY, //20
		MSG_TYPE_GET_RESPONSE, //21
		MSG_TYPE_GET_RESPONSE_REPLY, //22
		MSG_TYPE_GET_RESPONSE_COMPLETE, //23
	} msgType;
	
	struct Msg{
		msgType type;
	};
	
	struct MsgOpen {
		msgType type;
		unsigned int UID;
		//Fixed size is bad... but it will stay like this for now
		char profile[LEN_PROF];
		mfflag_t opts;
		unsigned int srcGUID;
	};
	
	struct MsgOpenReply {
		msgType type;
		unsigned int retValue;
	};
	
	struct MsgSend {
		msgType type;
		unsigned int UID;
		unsigned int reqID;
		unsigned int size;
		int dst_GUID;
		mfflag_t opts;
	};
	
	struct MsgSendReply {
		msgType type;
		unsigned int UID;
		unsigned int reqID;
		unsigned int retValue;
	};
	
	struct MsgSendComplete {
		msgType type;
		unsigned int UID;
		unsigned int reqID;
	};
	
	struct MsgRecv {
		msgType type;
		unsigned int UID;
		unsigned int reqID;
		char blk;
		unsigned int size;
	};
	
	struct MsgRecvReply {
		msgType type;
		unsigned int UID;
		unsigned int reqID;
		unsigned int retValue;
		unsigned int size;
		unsigned int sGUID;
	};
	
	struct MsgAttach {
		msgType type;
		unsigned int UID;
		unsigned int reqID;
		unsigned int GUIDs[MAX_ATTACH];
		unsigned int nGUID;
	};
	
	struct MsgAttachReply {
		msgType type;
		unsigned int UID;
		unsigned int reqID;
		unsigned int retValue;
	};
	
	struct MsgDetach {
		msgType type;
		unsigned int UID;
		unsigned int reqID;
		unsigned int GUIDs[MAX_ATTACH];
		unsigned int nGUID;
	};
	
	struct MsgDetachReply {
		msgType type;
		unsigned int UID;
		unsigned int reqID;
		unsigned int retValue;
	};
	
	struct MsgClose {
		msgType type;
		unsigned int UID;
		unsigned int reqID;
	};
	
	struct MsgCloseReply {
		msgType type;
		unsigned int UID;
		unsigned int reqID;
		unsigned int retValue;
	};
	
	// IPC messages to perform get operation
	
	struct MsgGet {
		msgType type;
		unsigned int UID;
		unsigned int reqID;
		unsigned int size;
		int dst_GUID;
		mfflag_t opts;
	};
	
	struct MsgGetReply {
		msgType type;
		unsigned int UID;
		unsigned int reqID;
		unsigned int retValue;
	};
	
	struct MsgGetSent {
		msgType type;
		unsigned int UID;
		unsigned int reqID;
	};
	
	struct MsgGetComplete {
		msgType type;
		unsigned int UID;
		unsigned int reqID;
		unsigned int retValue;
		unsigned int size;
	};
	
	struct MsgDoGet{
		msgType type;
		unsigned int UID;
		unsigned int reqID;
		unsigned int size;
	};
	
	struct MsgDoGetReply{
		msgType type;
		unsigned int UID;
		unsigned int reqID;
		unsigned int retValue;
		unsigned int size;
		unsigned int srcGUID;
		unsigned int dstGUID;
		unsigned int getID;
	};
	
	struct MsgGetResponse{
		msgType type;
		unsigned int UID;
		unsigned int reqID;
		unsigned int size;
		unsigned int srcGUID;
		unsigned int dstGUID;
		unsigned int getID;
		mfflag_t opts;
	};
	
	struct MsgGetResponseReply{
		msgType type;
		unsigned int UID;
		unsigned int reqID;
		unsigned int retValue;
	};
	
	struct MsgGetResponseComplete {
		msgType type;
		unsigned int UID;
		unsigned int reqID;
	};
	
#ifdef __cplusplus
}
#endif

#endif /* MFCLIENTIPC_H_*/
