/*
 * Ruby SRT - Ruby binding for Secure, Reliable, Transport
 * Copyright (c) 2019 Klaas Speller, Recce.
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 */

/**
 * A Ruby wrapper for SRT (Secure Reliable Transport)
 * 
 * @author: Klaas Speller <klaas@recce.nl>
 */

// MARK: - Includes

// MARK: - System

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdint.h>

#include <stdatomic.h>


// MARK: Ruby

#include <ruby/ruby.h>
#include <ruby/util.h>
#include <ruby/intern.h>
#include <ruby/thread.h>
#include <ruby/thread_native.h>
#include <ruby/io.h>
#include <ruby/vm.h>

// MARK: SRT

#include <srt/srt.h>


// MARK: - API

#include "rbsrt.h"
#include "rbstats.h"



void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) 
    {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


// MARK: - Ruby Classes

VALUE mSRTModule            = Qnil;
VALUE mSRTSocketKlass       = Qnil;
VALUE mSRTClientKlass       = Qnil;
VALUE mSRTServerKlass       = Qnil;
VALUE mSRTConnectionKlass   = Qnil;
VALUE mSRTPollKlass         = Qnil;


// MARK: - Enums

typedef enum RBSRTServerFlag
{
    RBSRT_SERVER_UNINITIALIZED      = 0,
    RBSRT_SERVER_INITIALIZED        = (1 << 0),
    RBSRT_SERVER_BOUND              = (1 << 1),
    RBSRT_SERVER_LISTENING          = (1 << 2),
    RBSRT_SERVER_ACCEPTING          = (1 << 3),
    RBSRT_SERVER_CLOSED             = (1 << 4)
} rbsrt_server_flag_t;

typedef enum RBSRTClientFlag
{
    RBSRT_CLIENT_UNINITIALIZED      = 0,
    RBSRT_CLIENT_INITIALIZED        = (1 << 0),
    RBSRT_CLIENT_CONNECTED          = (1 << 1),
    RBSRT_CLIENT_CLOSED             = (1 << 2)
} rbsrt_client_flag_t;

// MARK: Errors

VALUE rbsrt_eStandardError          = Qnil;
VALUE rbstr_eUNKNOWNError           = Qnil;
VALUE rbstr_eCONNSETUPError         = Qnil;
VALUE rbstr_eNOSERVERError          = Qnil;
VALUE rbstr_eCONNREJError           = Qnil;
VALUE rbstr_eSOCKFAILError          = Qnil;
VALUE rbstr_eSECFAILError           = Qnil;
VALUE rbstr_eCONNFAILError          = Qnil;
VALUE rbstr_eCONNLOSTError          = Qnil;
VALUE rbstr_eNOCONNError            = Qnil;
VALUE rbstr_eRESOURCEError          = Qnil;
VALUE rbstr_eTHREADError            = Qnil;
VALUE rbstr_eNOBUFError             = Qnil;
VALUE rbstr_eFILEError              = Qnil;
VALUE rbstr_eINVRDOFFError          = Qnil;
VALUE rbstr_eRDPERMError            = Qnil;
VALUE rbstr_eINVWROFFError          = Qnil;
VALUE rbstr_eWRPERMError            = Qnil;
VALUE rbstr_eINVOPError             = Qnil;
VALUE rbstr_eBOUNDSOCKError         = Qnil;
VALUE rbstr_eCONNSOCKError          = Qnil;
VALUE rbstr_eINVPARAMError          = Qnil;
VALUE rbstr_eINVSOCKError           = Qnil;
VALUE rbstr_eUNBOUNDSOCKError       = Qnil;
VALUE rbstr_eNOLISTENError          = Qnil;
VALUE rbstr_eRDVNOSERVError         = Qnil;
VALUE rbstr_eRDVUNBOUNDError        = Qnil;
VALUE rbstr_eINVALMSGAPIError       = Qnil;
VALUE rbstr_eINVALBUFFERAPIError    = Qnil;
VALUE rbstr_eDUPLISTENError         = Qnil;
VALUE rbstr_eLARGEMSGError          = Qnil;
VALUE rbstr_eINVPOLLIDError         = Qnil;
VALUE rbstr_eASYNCFAILError         = Qnil;
VALUE rbstr_eASYNCSNDError          = Qnil;
VALUE rbstr_eASYNCRCVError          = Qnil;
VALUE rbstr_eTIMEOUTError           = Qnil;
VALUE rbstr_eCONGESTError           = Qnil;
VALUE rbstr_ePEERERRError           = Qnil;


VALUE rbsrt_error_get_code(VALUE self)
{
    VALUE klazz = rb_class_of(self);

    return rb_const_get(klazz, rb_intern("Code"));
}

VALUE rbstr_error_with_srt_error_code(SRT_ERRNO error_code)
{
    VALUE error;

    switch(error_code)
    {
    case SRT_EUNKNOWN:
        error = rbstr_eUNKNOWNError;
        break;

    case SRT_ECONNSETUP:
        error = rbstr_eCONNSETUPError;
        break;

    case SRT_ENOSERVER:
        error = rbstr_eNOSERVERError;
        break;

    case SRT_ECONNREJ:
        error = rbstr_eCONNREJError;
        break;

    case SRT_ESOCKFAIL:
        error = rbstr_eSOCKFAILError;
        break;

    case SRT_ESECFAIL:
        error = rbstr_eSECFAILError;
        break;

    case SRT_ECONNFAIL:
        error = rbstr_eCONNFAILError;
        break;

    case SRT_ECONNLOST:
        error = rbstr_eCONNLOSTError;
        break;

    case SRT_ENOCONN:
        error = rbstr_eNOCONNError;
        break;

    case SRT_ERESOURCE:
        error = rbstr_eRESOURCEError;
        break;

    case SRT_ETHREAD:
        error = rbstr_eTHREADError;
        break;

    case SRT_ENOBUF:
        error = rbstr_eNOBUFError;
        break;

    case SRT_EFILE:
        error = rbstr_eFILEError;
        break;

    case SRT_EINVRDOFF:
        error = rbstr_eINVRDOFFError;
        break;

    case SRT_ERDPERM:
        error = rbstr_eRDPERMError;
        break;

    case SRT_EINVWROFF:
        error = rbstr_eINVWROFFError;
        break;

    case SRT_EWRPERM:
        error = rbstr_eWRPERMError;
        break;

    case SRT_EINVOP:
        error = rbstr_eINVOPError;
        break;

    case SRT_EBOUNDSOCK:
        error = rbstr_eBOUNDSOCKError;
        break;

    case SRT_ECONNSOCK:
        error = rbstr_eCONNSOCKError;
        break;

    case SRT_EINVPARAM:
        error = rbstr_eINVPARAMError;
        break;

    case SRT_EINVSOCK:
        error = rbstr_eINVSOCKError;
        break;

    case SRT_EUNBOUNDSOCK:
        error = rbstr_eUNBOUNDSOCKError;
        break;

    case SRT_ENOLISTEN:
        error = rbstr_eNOLISTENError;
        break;

    case SRT_ERDVNOSERV:
        error = rbstr_eRDVNOSERVError;
        break;

    case SRT_ERDVUNBOUND:
        error = rbstr_eRDVUNBOUNDError;
        break;

    case SRT_EINVALMSGAPI:
        error = rbstr_eINVALMSGAPIError;
        break;

    case SRT_EINVALBUFFERAPI:
        error = rbstr_eINVALBUFFERAPIError;
        break;

    case SRT_EDUPLISTEN:
        error = rbstr_eDUPLISTENError;
        break;

    case SRT_ELARGEMSG:
        error = rbstr_eLARGEMSGError;
        break;

    case SRT_EINVPOLLID:
        error = rbstr_eINVPOLLIDError;
        break;

    case SRT_EASYNCFAIL:
        error = rbstr_eASYNCFAILError;
        break;

    case SRT_EASYNCSND:
        error = rbstr_eASYNCSNDError;
        break;

    case SRT_EASYNCRCV:
        error = rbstr_eASYNCRCVError;
        break;

    case SRT_ETIMEOUT:
        error = rbstr_eTIMEOUTError;
        break;

    case SRT_ECONGEST:
        error = rbstr_eCONGESTError;
        break;

    case SRT_EPEERERR:
        error = rbstr_ePEERERRError;
        break;

    default:
        error = rbsrt_eStandardError;
        break;
    }

    return error;
}

_Noreturn void rbsrt_raise_last_srt_error()
{
    int sys_errno = 0;
    int error_code = srt_getlasterror(&sys_errno);
    VALUE last_error = rbstr_error_with_srt_error_code(error_code);

    rb_raise(last_error, "%s", srt_getlasterror_str());
}

void rbsrt_init_errors()
{
    rbsrt_eStandardError                = rb_define_class_under(mSRTModule, "Error", rb_eStandardError);

    rb_define_method(rbsrt_eStandardError, "code", rbsrt_error_get_code, 0);


    // Error Types

    rbstr_eUNKNOWNError					= rb_define_class_under(mSRTModule, "UNKNOWN", rbsrt_eStandardError);
    rbstr_eCONNSETUPError				= rb_define_class_under(mSRTModule, "CONNSETUP", rbsrt_eStandardError);
    rbstr_eNOSERVERError				= rb_define_class_under(mSRTModule, "NOSERVER", rbsrt_eStandardError);
    rbstr_eCONNREJError					= rb_define_class_under(mSRTModule, "CONNREJ", rbsrt_eStandardError);
    rbstr_eSOCKFAILError				= rb_define_class_under(mSRTModule, "SOCKFAIL", rbsrt_eStandardError);
    rbstr_eSECFAILError					= rb_define_class_under(mSRTModule, "SECFAIL", rbsrt_eStandardError);
    rbstr_eCONNFAILError				= rb_define_class_under(mSRTModule, "CONNFAIL", rbsrt_eStandardError);
    rbstr_eCONNLOSTError				= rb_define_class_under(mSRTModule, "CONNLOST", rbsrt_eStandardError);
    rbstr_eNOCONNError					= rb_define_class_under(mSRTModule, "NOCONN", rbsrt_eStandardError);
    rbstr_eRESOURCEError				= rb_define_class_under(mSRTModule, "RESOURCE", rbsrt_eStandardError);
    rbstr_eTHREADError					= rb_define_class_under(mSRTModule, "THREAD", rbsrt_eStandardError);
    rbstr_eNOBUFError					= rb_define_class_under(mSRTModule, "NOBUF", rbsrt_eStandardError);
    rbstr_eFILEError					= rb_define_class_under(mSRTModule, "FILE", rbsrt_eStandardError);
    rbstr_eINVRDOFFError				= rb_define_class_under(mSRTModule, "INVRDOFF", rbsrt_eStandardError);
    rbstr_eRDPERMError					= rb_define_class_under(mSRTModule, "RDPERM", rbsrt_eStandardError);
    rbstr_eINVWROFFError				= rb_define_class_under(mSRTModule, "INVWROFF", rbsrt_eStandardError);
    rbstr_eWRPERMError					= rb_define_class_under(mSRTModule, "WRPERM", rbsrt_eStandardError);
    rbstr_eINVOPError					= rb_define_class_under(mSRTModule, "INVOP", rbsrt_eStandardError);
    rbstr_eBOUNDSOCKError				= rb_define_class_under(mSRTModule, "BOUNDSOCK", rbsrt_eStandardError);
    rbstr_eCONNSOCKError				= rb_define_class_under(mSRTModule, "CONNSOCK", rbsrt_eStandardError);
    rbstr_eINVPARAMError				= rb_define_class_under(mSRTModule, "INVPARAM", rbsrt_eStandardError);
    rbstr_eINVSOCKError					= rb_define_class_under(mSRTModule, "INVSOCK", rbsrt_eStandardError);
    rbstr_eUNBOUNDSOCKError			    = rb_define_class_under(mSRTModule, "UNBOUNDSOCK", rbsrt_eStandardError);
    rbstr_eNOLISTENError				= rb_define_class_under(mSRTModule, "NOLISTEN", rbsrt_eStandardError);
    rbstr_eRDVNOSERVError				= rb_define_class_under(mSRTModule, "RDVNOSERV", rbsrt_eStandardError);
    rbstr_eRDVUNBOUNDError			    = rb_define_class_under(mSRTModule, "RDVUNBOUND", rbsrt_eStandardError);
    rbstr_eINVALMSGAPIError			    = rb_define_class_under(mSRTModule, "INVALMSGAPI", rbsrt_eStandardError);
    rbstr_eINVALBUFFERAPIError	        = rb_define_class_under(mSRTModule, "INVALBUFFERAPI", rbsrt_eStandardError);
    rbstr_eDUPLISTENError				= rb_define_class_under(mSRTModule, "DUPLISTEN", rbsrt_eStandardError);
    rbstr_eLARGEMSGError				= rb_define_class_under(mSRTModule, "LARGEMSG", rbsrt_eStandardError);
    rbstr_eINVPOLLIDError				= rb_define_class_under(mSRTModule, "INVPOLLID", rbsrt_eStandardError);
    rbstr_eASYNCFAILError				= rb_define_class_under(mSRTModule, "ASYNCFAIL", rbsrt_eStandardError);
    rbstr_eASYNCSNDError				= rb_define_class_under(mSRTModule, "ASYNCSND", rbsrt_eStandardError);
    rbstr_eASYNCRCVError				= rb_define_class_under(mSRTModule, "ASYNCRCV", rbsrt_eStandardError);
    rbstr_eTIMEOUTError					= rb_define_class_under(mSRTModule, "TIMEOUT", rbsrt_eStandardError);
    rbstr_eCONGESTError					= rb_define_class_under(mSRTModule, "CONGEST", rbsrt_eStandardError);
    rbstr_ePEERERRError					= rb_define_class_under(mSRTModule, "PEERERR", rbsrt_eStandardError);


    // Error Codes

    rb_define_const(rbsrt_eStandardError, "Code", INT2FIX(0));
    rb_define_const(rbstr_eUNKNOWNError, "Code", INT2FIX(SRT_EUNKNOWN));
    rb_define_const(rbstr_eCONNSETUPError, "Code", INT2FIX(SRT_ECONNSETUP));
    rb_define_const(rbstr_eNOSERVERError, "Code", INT2FIX(SRT_ENOSERVER));
    rb_define_const(rbstr_eCONNREJError, "Code", INT2FIX(SRT_ECONNREJ));
    rb_define_const(rbstr_eSOCKFAILError, "Code", INT2FIX(SRT_ESOCKFAIL));
    rb_define_const(rbstr_eSECFAILError, "Code", INT2FIX(SRT_ESECFAIL));
    rb_define_const(rbstr_eCONNFAILError, "Code", INT2FIX(SRT_ECONNFAIL));
    rb_define_const(rbstr_eCONNLOSTError, "Code", INT2FIX(SRT_ECONNLOST));
    rb_define_const(rbstr_eNOCONNError, "Code", INT2FIX(SRT_ENOCONN));
    rb_define_const(rbstr_eRESOURCEError, "Code", INT2FIX(SRT_ERESOURCE));
    rb_define_const(rbstr_eTHREADError, "Code", INT2FIX(SRT_ETHREAD));
    rb_define_const(rbstr_eNOBUFError, "Code", INT2FIX(SRT_ENOBUF));
    rb_define_const(rbstr_eFILEError, "Code", INT2FIX(SRT_EFILE));
    rb_define_const(rbstr_eINVRDOFFError, "Code", INT2FIX(SRT_EINVRDOFF));
    rb_define_const(rbstr_eRDPERMError, "Code", INT2FIX(SRT_ERDPERM));
    rb_define_const(rbstr_eINVWROFFError, "Code", INT2FIX(SRT_EINVWROFF));
    rb_define_const(rbstr_eWRPERMError, "Code", INT2FIX(SRT_EWRPERM));
    rb_define_const(rbstr_eINVOPError, "Code", INT2FIX(SRT_EINVOP));
    rb_define_const(rbstr_eBOUNDSOCKError, "Code", INT2FIX(SRT_EBOUNDSOCK));
    rb_define_const(rbstr_eCONNSOCKError, "Code", INT2FIX(SRT_ECONNSOCK));
    rb_define_const(rbstr_eINVPARAMError, "Code", INT2FIX(SRT_EINVPARAM));
    rb_define_const(rbstr_eINVSOCKError, "Code", INT2FIX(SRT_EINVSOCK));
    rb_define_const(rbstr_eUNBOUNDSOCKError, "Code", INT2FIX(SRT_EUNBOUNDSOCK));
    rb_define_const(rbstr_eNOLISTENError, "Code", INT2FIX(SRT_ENOLISTEN));
    rb_define_const(rbstr_eRDVNOSERVError, "Code", INT2FIX(SRT_ERDVNOSERV));
    rb_define_const(rbstr_eRDVUNBOUNDError, "Code", INT2FIX(SRT_ERDVUNBOUND));
    rb_define_const(rbstr_eINVALMSGAPIError, "Code", INT2FIX(SRT_EINVALMSGAPI));
    rb_define_const(rbstr_eINVALBUFFERAPIError, "Code", INT2FIX(SRT_EINVALBUFFERAPI));
    rb_define_const(rbstr_eDUPLISTENError, "Code", INT2FIX(SRT_EDUPLISTEN));
    rb_define_const(rbstr_eLARGEMSGError, "Code", INT2FIX(SRT_ELARGEMSG));
    rb_define_const(rbstr_eINVPOLLIDError, "Code", INT2FIX(SRT_EINVPOLLID));
    rb_define_const(rbstr_eASYNCFAILError, "Code", INT2FIX(SRT_EASYNCFAIL));
    rb_define_const(rbstr_eASYNCSNDError, "Code", INT2FIX(SRT_EASYNCSND));
    rb_define_const(rbstr_eASYNCRCVError, "Code", INT2FIX(SRT_EASYNCRCV));
    rb_define_const(rbstr_eTIMEOUTError, "Code", INT2FIX(SRT_ETIMEOUT));
    rb_define_const(rbstr_eCONGESTError, "Code", INT2FIX(SRT_ECONGEST));
    rb_define_const(rbstr_ePEERERRError, "Code", INT2FIX(SRT_EPEERERR));

}


// MARK: - SRT Init

VALUE rbsrt_srt_startup(void *c)
{
    RBSRT_DEBUG_PRINT("srt startup");

    srt_startup();

    return Qnil;
}

void rbsrt_srt_cleanup(ruby_vm_t *vm)
{
    RBSRT_DEBUG_PRINT("srt cleanup");

    srt_cleanup();
}

// MARK: - Socket Base

// MARK: Socket State

static VALUE rbsrt_sym_state_ready;
static VALUE rbsrt_sym_state_opened;
static VALUE rbsrt_sym_state_listening;
static VALUE rbsrt_sym_state_connecting;
static VALUE rbsrt_sym_state_connected;
static VALUE rbsrt_sym_state_broken;
static VALUE rbsrt_sym_state_closing;
static VALUE rbsrt_sym_state_closed;
static VALUE rbsrt_sym_state_nonexist;

#define RBSRT_SOCKET_STATE_READY_SYM		rbsrt_sym_state_ready
#define RBSRT_SOCKET_STATE_OPENED_SYM		rbsrt_sym_state_opened
#define RBSRT_SOCKET_STATE_LISTENING_SYM	rbsrt_sym_state_listening
#define RBSRT_SOCKET_STATE_CONNECTING_SYM	rbsrt_sym_state_connecting
#define RBSRT_SOCKET_STATE_CONNECTED_SYM	rbsrt_sym_state_connected
#define RBSRT_SOCKET_STATE_BROKEN_SYM		rbsrt_sym_state_broken
#define RBSRT_SOCKET_STATE_CLOSING_SYM		rbsrt_sym_state_closing
#define RBSRT_SOCKET_STATE_CLOSED_SYM		rbsrt_sym_state_closed
#define RBSRT_SOCKET_STATE_NONEXIST_SYM		rbsrt_sym_state_nonexist

VALUE rbsrt_socket_get_socket_state(VALUE self)
{
    RBSRT_SOCKET_BASE_UNWRAP(self, socket);

    RBSRT_DEBUG_PRINT("socket get socket state for %d", socket->socket);

    SRT_SOCKSTATUS state = srt_getsockstate(socket->socket);

    VALUE state_sym;

    switch (state)
    {
    case SRTS_INIT:
        state_sym = RBSRT_SOCKET_STATE_READY_SYM;
        break;

    case SRTS_OPENED:
        state_sym = RBSRT_SOCKET_STATE_OPENED_SYM;
        break;

    case SRTS_LISTENING:
        state_sym = RBSRT_SOCKET_STATE_LISTENING_SYM;
        break;

    case SRTS_CONNECTING:
        state_sym = RBSRT_SOCKET_STATE_CONNECTING_SYM;
        break;

    case SRTS_CONNECTED:
        state_sym = RBSRT_SOCKET_STATE_CONNECTED_SYM;
        break;

    case SRTS_BROKEN:
        state_sym = RBSRT_SOCKET_STATE_BROKEN_SYM;
        break;

    case SRTS_CLOSING:
        state_sym = RBSRT_SOCKET_STATE_CLOSING_SYM;
        break;

    case SRTS_CLOSED:
        state_sym = RBSRT_SOCKET_STATE_CLOSED_SYM;
        break;

    case SRTS_NONEXIST:
        state_sym = RBSRT_SOCKET_STATE_NONEXIST_SYM;
        break;

    default:
        state_sym = Qnil;
        break;
    }

    return state_sym;
}

VALUE rbsrt_socket_is_ready(VALUE self)
{
	RBSRT_SOCKET_BASE_UNWRAP(self, socket);
	
	return srt_getsockstate(socket->socket) == SRTS_INIT ? Qtrue : Qfalse;
}

VALUE rbsrt_socket_is_opened(VALUE self)
{
	RBSRT_SOCKET_BASE_UNWRAP(self, socket);
	
	return srt_getsockstate(socket->socket) == SRTS_OPENED ? Qtrue : Qfalse;
}

VALUE rbsrt_socket_is_listening(VALUE self)
{
	RBSRT_SOCKET_BASE_UNWRAP(self, socket);
	
	return srt_getsockstate(socket->socket) == SRTS_LISTENING ? Qtrue : Qfalse;
}

VALUE rbsrt_socket_is_connecting(VALUE self)
{
	RBSRT_SOCKET_BASE_UNWRAP(self, socket);
	
	return srt_getsockstate(socket->socket) == SRTS_CONNECTING ? Qtrue : Qfalse;
}

VALUE rbsrt_socket_is_connected(VALUE self)
{
	RBSRT_SOCKET_BASE_UNWRAP(self, socket);
	
	return srt_getsockstate(socket->socket) == SRTS_CONNECTED ? Qtrue : Qfalse;
}

VALUE rbsrt_socket_is_broken(VALUE self)
{
	RBSRT_SOCKET_BASE_UNWRAP(self, socket);
	
	return srt_getsockstate(socket->socket) == SRTS_BROKEN ? Qtrue : Qfalse;
}

VALUE rbsrt_socket_is_closing(VALUE self)
{
	RBSRT_SOCKET_BASE_UNWRAP(self, socket);
	
	return srt_getsockstate(socket->socket) == SRTS_CLOSING ? Qtrue : Qfalse;
}

VALUE rbsrt_socket_is_closed(VALUE self)
{
	RBSRT_SOCKET_BASE_UNWRAP(self, socket);
	
	return srt_getsockstate(socket->socket) == SRTS_CLOSED ? Qtrue : Qfalse;
}

VALUE rbsrt_socket_is_nonexist(VALUE self)
{
	RBSRT_SOCKET_BASE_UNWRAP(self, socket);
	
	return srt_getsockstate(socket->socket) == SRTS_NONEXIST ? Qtrue : Qfalse;
}

void rbsrt_define_socket_state_api(VALUE klass)
{
    rb_define_method(klass, "state", rbsrt_socket_get_socket_state, 0);

    rb_define_method(klass, "ready?", rbsrt_socket_is_ready, 0);
    rb_define_method(klass, "opened?", rbsrt_socket_is_opened, 0);
    rb_define_method(klass, "listening?", rbsrt_socket_is_listening, 0);
    rb_define_method(klass, "connecting?", rbsrt_socket_is_connecting, 0);
    rb_define_method(klass, "connected?", rbsrt_socket_is_connected, 0);
    rb_define_method(klass, "broken?", rbsrt_socket_is_broken, 0);
    rb_define_method(klass, "closing?", rbsrt_socket_is_closing, 0);
    rb_define_method(klass, "closed?", rbsrt_socket_is_closed, 0);
    rb_define_method(klass, "nonexist?", rbsrt_socket_is_nonexist, 0);
}


// MARK: - GVL-free I/O helpers

struct rbsrt_accept_args {
    SRTSOCKET server_socket;
    SRTSOCKET client_socket;
    struct sockaddr_storage remote_address;
    int addr_size;
};

static void *rbsrt_accept_without_gvl(void *data)
{
    struct rbsrt_accept_args *args = data;
    args->addr_size = sizeof(args->remote_address);
    args->client_socket = srt_accept(args->server_socket,
                                      (struct sockaddr *)&args->remote_address,
                                      &args->addr_size);
    return NULL;
}

struct rbsrt_recvmsg_args {
    SRTSOCKET socket;
    char buf[RBSRT_PAYLOAD_SIZE * 2];
    int nbytes;
};

static void *rbsrt_recvmsg_without_gvl(void *data)
{
    struct rbsrt_recvmsg_args *args = data;
    args->nbytes = srt_recvmsg2(args->socket, args->buf, sizeof(args->buf), NULL);
    return NULL;
}

struct rbsrt_sendmsg_args {
    SRTSOCKET socket;
    const char *buf;
    int buf_len;
    int total_nbytes;
    int srt_error;
};

static void *rbsrt_sendmsg_without_gvl(void *data)
{
    struct rbsrt_sendmsg_args *args = data;
    int packet_size, nbytes;
    args->total_nbytes = 0;
    args->srt_error = 0;
    do {
        packet_size = (args->buf_len - args->total_nbytes) > RBSRT_PAYLOAD_SIZE
                    ? RBSRT_PAYLOAD_SIZE
                    : (args->buf_len - args->total_nbytes);
        nbytes = srt_sendmsg2(args->socket, args->buf + args->total_nbytes, packet_size, NULL);
        if (nbytes == SRT_ERROR) {
            args->srt_error = 1;
            return NULL;
        }
    } while ((args->total_nbytes += nbytes) < args->buf_len);
    return NULL;
}


// MARK: Connecting

VALUE rbsrt_socket_connect(VALUE self, VALUE host, VALUE port)
{
    Check_Type(host, T_STRING);
    Check_Type(port, T_STRING);

    RBSRT_DEBUG_PRINT("socket connect: host=%s, port=%s", StringValueCStr(host), StringValueCStr(port));

    RBSRT_SOCKET_BASE_UNWRAP(self, socket);

    struct addrinfo hints, *servinfo, *p;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    const char *host_s = StringValueCStr(host);
    const char *port_s = StringValueCStr(port);

    if (getaddrinfo(host_s, port_s, &hints, &servinfo) != 0)
    {
        rb_raise(rbsrt_eStandardError, "failed to get address info");
    }

    srt_clearlasterror();
    int result = SRT_ERROR;

    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if (srt_connect(socket->socket, p->ai_addr, p->ai_addrlen) != SRT_ERROR)
        {
            result = 0;
            break;
        }
    }

    freeaddrinfo(servinfo);

    if (result == SRT_ERROR)
    {
        rbsrt_raise_last_srt_error();
    }

    return Qtrue;
}

VALUE rbsrt_socket_accept(VALUE self)
{
    RBSRT_DEBUG_PRINT("server accept");

    RBSRT_SOCKET_BASE_UNWRAP(self, socket);

    struct rbsrt_accept_args args;
    args.server_socket = socket->socket;

    rb_thread_call_without_gvl(rbsrt_accept_without_gvl, &args, NULL, NULL);

    if (args.client_socket == SRT_ERROR)
    {
        rbsrt_raise_last_srt_error();
    }

    VALUE rbclient = rb_class_new_instance(0, NULL, mSRTSocketKlass);

    RBSRT_SOCKET_UNWRAP(rbclient, client);

    client->socket = args.client_socket;

    return rbclient;
}

VALUE rbsrt_socket_bind(VALUE self, VALUE address, VALUE port)
{
    int status;
    struct sockaddr_in sa;
    // int livemode = SRTT_LIVE;

    Check_Type(address, T_STRING);
    Check_Type(port, T_STRING);

    RBSRT_SOCKET_BASE_UNWRAP(self, socket);

    // get address info
    // TODO: Handle domain names

    sa.sin_family = AF_INET;
    sa.sin_port = htons(atoi(StringValuePtr(port)));
    
    if (inet_pton(AF_INET, StringValuePtr(address), &sa.sin_addr) != 1)
    {
        rb_raise(rb_eArgError, "bad adddress: %s", strerror(errno));
        
        return Qfalse;
    }

    
    // bind socket

    status = srt_bind(socket->socket, (struct sockaddr *)&sa, sizeof(sa));
    
    if (status == SRT_ERROR)
    {
        rbsrt_raise_last_srt_error();
        
        return Qfalse;
    }

    return Qtrue;
}

 VALUE rbsrt_socket_listen(VALUE self, VALUE nbacklog)
 {
    Check_Type(nbacklog, T_FIXNUM);

    RBSRT_SOCKET_BASE_UNWRAP(self, socket);
    
    int status = srt_listen(socket->socket, FIX2INT(nbacklog));
    
    if (status == SRT_ERROR)
    {
       rbsrt_raise_last_srt_error();
        
        return Qfalse;
    }

    return Qtrue;
}


// MARK: Creating and Configuring

VALUE rbsrt_socket_close(VALUE self)
{
    RBSRT_DEBUG_PRINT("socket close");

    RBSRT_SOCKET_BASE_UNWRAP(self, socket)

    if (srt_close(socket->socket) == SRT_ERROR)
    {
        rbsrt_raise_last_srt_error();

        return Qfalse;
    }

    return Qtrue;
}


// MARK: Transmission

VALUE rbsrt_socket_sendmsg(VALUE self, VALUE message)
{
    RBSRT_DEBUG_PRINT("socket sendmsg");

    Check_Type(message, T_STRING);

    RBSRT_SOCKET_BASE_UNWRAP(self, socket)

    long buf_len = RSTRING_LEN(message);

    // Copy message into a C buffer before releasing the GVL so the GC
    // cannot move the Ruby string while we are blocked in SRT.
    char *buf = malloc(buf_len);

    if (!buf)
    {
        rb_raise(rb_eNoMemError, "not enough memory for SRT send buffer");
    }

    memcpy(buf, RSTRING_PTR(message), buf_len);

    struct rbsrt_sendmsg_args args = {
        .socket      = socket->socket,
        .buf         = buf,
        .buf_len     = (int)buf_len,
    };

    rb_thread_call_without_gvl(rbsrt_sendmsg_without_gvl, &args, NULL, NULL);

    free(buf);

    if (args.srt_error)
    {
        rbsrt_raise_last_srt_error();
    }

    return INT2FIX(args.total_nbytes);
}

VALUE rbsrt_socket_recvmsg(VALUE self)
{
    RBSRT_DEBUG_PRINT("socket recvmsg");

    RBSRT_SOCKET_BASE_UNWRAP(self, socket)

    struct rbsrt_recvmsg_args args;
    args.socket = socket->socket;

    rb_thread_call_without_gvl(rbsrt_recvmsg_without_gvl, &args, NULL, NULL);

    if (args.nbytes == SRT_ERROR)
    {
        rbsrt_raise_last_srt_error();

        return Qnil;
    }

    if (args.nbytes == 0)
    {
        return Qnil;
    }

    return rb_str_new(args.buf, (long)args.nbytes);
}


// MARK: Socket Options

VALUE rbsrt_socket_get_id(VALUE self)
{
    RBSRT_SOCKET_BASE_UNWRAP(self, socket);

    return INT2FIX(socket->socket);
}

VALUE rbsrt_socket_set_rcvsyn(VALUE self, VALUE syn)
{
    int is_syn = RTEST(syn) ? 1 : 0;
    
    RBSRT_SOCKET_BASE_UNWRAP(self, socket);

    if (srt_setsockflag(socket->socket, SRTO_RCVSYN, &is_syn, sizeof(is_syn)) == SRT_ERROR)
    {
        rbsrt_raise_last_srt_error();

        return Qfalse;
    }

    return Qtrue;
}

VALUE rbsrt_socket_get_rcvsyn(VALUE self)
{
    int is_syn = 0;
    int is_syn_size = sizeof(is_syn);

    RBSRT_SOCKET_BASE_UNWRAP(self, socket);

    if (srt_getsockflag(socket->socket, SRTO_RCVSYN, &is_syn, &is_syn_size) == SRT_ERROR)
    {
        rbsrt_raise_last_srt_error();

        return Qnil;
    }

    return is_syn ? Qtrue : Qfalse;
}

VALUE rbsrt_socket_set_sndsyn(VALUE self, VALUE syn)
{
    int is_syn = RTEST(syn) ? 1 : 0;
    
    RBSRT_SOCKET_BASE_UNWRAP(self, socket);

    if (srt_setsockflag(socket->socket, SRTO_SNDSYN, &is_syn, sizeof(is_syn)) == SRT_ERROR)
    {
        rbsrt_raise_last_srt_error();

        return Qfalse;
    }

    return Qtrue;
}

VALUE rbsrt_socket_get_sndsyn(VALUE self)
{
    int is_syn = 0;
    int is_syn_size = sizeof(is_syn);

    RBSRT_SOCKET_BASE_UNWRAP(self, socket);

    if (srt_getsockflag(socket->socket, SRTO_SNDSYN, &is_syn, &is_syn_size) == SRT_ERROR)
    {
        rbsrt_raise_last_srt_error();

        return Qnil;
    }

    return is_syn ? Qtrue : Qfalse;
}

VALUE rbsrt_socket_set_streamid(VALUE self, VALUE streamid)
{
    Check_Type(streamid, T_STRING);

    long streamid_llen = rb_str_strlen(streamid);

    if (streamid_llen > 512)
    {
        rb_raise(rbsrt_eStandardError, "streamid must be 512 characters or less");

        return Qfalse;
    }

    int streamid_len = (int)streamid_llen;
    char *streamid_p = StringValuePtr(streamid);
    
    RBSRT_SOCKET_BASE_UNWRAP(self, socket);

    if (srt_setsockflag(socket->socket, SRTO_STREAMID, streamid_p, streamid_len + 1) == SRT_ERROR)
    {
        rbsrt_raise_last_srt_error();

        return Qfalse;
    }

    return Qtrue; 
}

VALUE rbsrt_socket_get_streamid(VALUE self)
{
    char streamid[1024];
    int streamid_len = sizeof(streamid);

    RBSRT_SOCKET_BASE_UNWRAP(self, socket);

    if (srt_getsockflag(socket->socket, SRTO_STREAMID, &streamid, &streamid_len) == SRT_ERROR)
    {
        rbsrt_raise_last_srt_error();

        return Qfalse;
    }

    if (streamid_len <= 0)
    {
        return Qnil;
    }

    VALUE result = rb_str_buf_new((long)streamid_len);
    rb_str_buf_cat(result, streamid, (long)streamid_len);

    return result;
}

VALUE rbsrt_socket_set_transtype(VALUE self, VALUE type)
{
    RBSRT_DEBUG_PRINT("socket set transmission type");

    // unwrap type argument

    int type_arg_type = rb_type(type);
    int srt_transtype;
    const char *type_name = NULL;

    RBSRT_SOCKET_BASE_UNWRAP(self, socket);

    switch (type_arg_type)
    {
        case T_STRING:
            type_name = StringValuePtr(type);
            break;

        case T_SYMBOL:
            type_name = rb_id2name(SYM2ID(type));
            break;

        case T_FIXNUM:
            if (FIX2INT(type) == SRTT_LIVE || FIX2INT(type) == SRTT_FILE)
            {
                srt_transtype = FIX2INT(type);

                goto set_flag;
            }

            else
            {
                goto invalid_argument;
            }

            break;

        default:
            goto invalid_argument;
            break;
    }

    // set option

    if (strncmp(type_name, "live", 5) == 0)
    {
        srt_transtype = SRTT_LIVE;
    }

    else if (strncmp(type_name, "file", 5) == 0)
    {
        srt_transtype = SRTT_FILE;
    }

    else
    {
        goto invalid_argument;
    }

    set_flag:

        if (srt_setsockflag(socket->socket, SRTO_TRANSTYPE, &srt_transtype, sizeof(srt_transtype)) == SRT_ERROR)
        {
            rbsrt_raise_last_srt_error();

            return Qfalse;
        }

        return Qtrue;

    invalid_argument:
        rb_raise(rb_eArgError, "transmission type must be live (:live, \"live\", %d), or file (:file, \"file\", %d)", SRTT_LIVE, SRTT_FILE);
}


VALUE rbsrt_socket_set_tsbpdmode(VALUE self, VALUE val)
{
    // timestamp-based Packet Delivery mode
    
    RBSRT_SOCKET_BASE_UNWRAP(self, socket);

    int32_t tsbpdmode = RTEST(val) ? 1 : 0;
    int tsbpdmode_size = sizeof(tsbpdmode);

    if (srt_setsockflag(socket->socket, SRTO_TSBPDMODE, &tsbpdmode, tsbpdmode_size) == SRT_ERROR)
    {
        rbsrt_raise_last_srt_error();

        return Qnil;
    }

    return val;
}

VALUE rbsrt_socket_get_tsbpdmode(VALUE self)
{
    // timestamp-based Packet Delivery mode
    
    RBSRT_SOCKET_BASE_UNWRAP(self, socket);

    int32_t tsbpdmode;
    int tsbpdmode_size = sizeof(tsbpdmode);

    if (srt_getsockflag(socket->socket, SRTO_TSBPDMODE, &tsbpdmode, &tsbpdmode_size) == SRT_ERROR)
    {
        rbsrt_raise_last_srt_error();

        return Qnil;
    }

    return tsbpdmode ? Qtrue : Qfalse;
}



#ifndef RBSRT_PASSPHRASE_MIN
#define RBSRT_PASSPHRASE_MIN 10
#endif

#ifndef RBSRT_PASSPHRASE_MAX
#define RBSRT_PASSPHRASE_MAX 79
#endif

VALUE rbsrt_socket_set_passphrase(VALUE self, VALUE passphrase)
{
    int passphrase_type = rb_type(passphrase);
    char const *passphrase_buf;
    int passphrase_size;

    switch (passphrase_type)
    {
        case T_NIL:
        passphrase_buf = NULL;
        passphrase_size = 0;
        break;

        case T_STRING:
        passphrase_buf = StringValuePtr(passphrase);

        // long passphrase_length = rb_str_length(passphrase);
        long passphrase_length = strnlen(passphrase_buf, RBSRT_PASSPHRASE_MAX * 2);
        passphrase_size = (int)passphrase_length + 1;

        if (passphrase_buf == 0)
        {
            passphrase_buf = NULL;
            passphrase_size = 0;
        }

        else if (passphrase_length != 0 && (passphrase_length < RBSRT_PASSPHRASE_MIN || passphrase_length > RBSRT_PASSPHRASE_MAX))
        {
            rb_raise(rb_eArgError, "passphrase must be between %d and %d characters or empty (got %ld)", RBSRT_PASSPHRASE_MIN, RBSRT_PASSPHRASE_MAX, passphrase_length);

            return Qfalse;
        }

        
        break;

        default:
        Check_Type(passphrase, T_STRING);
        return Qfalse;
        break;
    }

    RBSRT_SOCKET_BASE_UNWRAP(self, socket);

    if (srt_setsockflag(socket->socket, SRTO_PASSPHRASE, passphrase_buf, passphrase_size) == SRT_ERROR)
    {
        rbsrt_raise_last_srt_error();

        return Qfalse;
    }

    return Qtrue;
}


// MARK: Base Socket API

void rbsrt_socket_base_define_base_api(VALUE klass)
{
    rb_define_method(klass, "id", rbsrt_socket_get_id, 0);

    rb_define_method(klass, "read_sync=", rbsrt_socket_set_rcvsyn, 1);
    rb_define_method(klass, "read_sync?", rbsrt_socket_get_rcvsyn, 0);
    rb_alias(klass, rb_intern("rcvsyn="), rb_intern("read_sync="));
    rb_alias(klass, rb_intern("rcvsyn?"), rb_intern("read_sync?"));

    rb_define_method(klass, "passphrase=", rbsrt_socket_set_passphrase, 1);
    rb_alias(klass, rb_intern("password="), rb_intern("passphrase="));
}

void rbsrt_socket_base_define_option_api(VALUE klass)
{
    rb_define_method(klass, "write_sync=", rbsrt_socket_set_sndsyn, 1);
    rb_define_method(klass, "write_sync?", rbsrt_socket_get_sndsyn, 0);
    rb_alias(klass, rb_intern("sndsyn="), rb_intern("write_sync="));
    rb_alias(klass, rb_intern("sndsyn?"), rb_intern("write_sync?"));
    
    rb_define_method(klass, "streamid=", rbsrt_socket_set_streamid, 1);
    rb_define_method(klass, "streamid", rbsrt_socket_get_streamid, 0);
    
    rb_define_method(klass, "transmission_mode=", rbsrt_socket_set_transtype, 1);
    rb_alias(klass, rb_intern("trans_mode="), rb_intern("transmission_mode="));
    rb_alias(klass, rb_intern("transtype="), rb_intern("transmission_mode="));


    rb_define_method(klass, "timestamp_based_packet_delivery_mode=", rbsrt_socket_set_tsbpdmode, 1);
    rb_define_method(klass, "timestamp_based_packet_delivery_mode?", rbsrt_socket_get_tsbpdmode, 0);
    rb_alias(klass, rb_intern("tsbpdmode="), rb_intern("timestamp_based_packet_delivery_mode="));
    rb_alias(klass, rb_intern("tsbpdmode?"), rb_intern("timestamp_based_packet_delivery_mode?"));    
}

void rbsrt_socket_base_define_basic_api(VALUE klass)
{
    rb_define_method(klass, "close", rbsrt_socket_close, 0);
}

void rbsrt_socket_base_define_connection_api(VALUE klass)
{
    rb_define_method(klass, "connect", rbsrt_socket_connect, 2);
}

void rbsrt_socket_base_define_io_api(VALUE klass)
{
    rb_define_method(klass, "recvmsg", rbsrt_socket_recvmsg, 0);
    rb_alias(klass, rb_intern("read"), rb_intern("recvmsg"));

    rb_define_method(klass, "sendmsg", rbsrt_socket_sendmsg, 1);
    rb_alias(klass, rb_intern("write"), rb_intern("sendmsg"));
}



// MARK: - SRT::Socket Class

size_t rbsrt_socket_dsize(const void *socket)
{
    RBSRT_DEBUG_PRINT("dsize srt socket");

    return sizeof(rbsrt_client_t);
}

void rbsrt_socket_dmark(void *socket)
{
    RBSRT_DEBUG_PRINT("dmark srt socket");
}


// MARK: Initializers

void rbsrt_socket_deallocate(rbsrt_socket_t *socket)
{
    RBSRT_DEBUG_PRINT("deallocate srt socket");

    srt_close(socket->socket); // TODO: Check socket state before closing

    free(socket);
}

VALUE rbsrt_socket_allocate(VALUE klass)
{
    RBSRT_DEBUG_PRINT("allocate socket");

    rbsrt_socket_t *socket = malloc(sizeof(rbsrt_socket_t));
    if (!socket) { rb_raise(rb_eNoMemError, "not enough memory for SRT socket"); }

    memset(socket, 0, sizeof(rbsrt_socket_t));
    
    return TypedData_Wrap_Struct(klass, &rbsrt_socket_rbtype, socket);
}

VALUE rbsrt_socket_initialize(VALUE self)
{
    RBSRT_DEBUG_PRINT("initialize socket");

    RBSRT_SOCKET_UNWRAP(self, socket);

    socket->socket = srt_create_socket();

    int livemode = SRTT_LIVE;

    srt_setsockflag(socket->socket, SRTO_TRANSTYPE, &livemode, sizeof(livemode)); // set live mode

    return self;
}



// MARK: - SRT::Connection Class

// MARK: Ruby Type Helpers

size_t rbsrt_connection_dsize(const void *server)
{
    RBSRT_DEBUG_PRINT("connection dsize");
    
    return sizeof(rbsrt_connection_t);
}

void rbsrt_connection_dmark(void *data)
{
    RBSRT_DEBUG_PRINT("connection mark");

    rbsrt_connection_t *connection = (rbsrt_connection_t *)data;

    if (connection->at_data_block)
    {
        rb_gc_mark(connection->at_data_block);
    }

    if (connection->at_close_block)
    {
        rb_gc_mark(connection->at_close_block);
    }
}


// MARK: Initializers

VALUE rbsrt_connection_allocate(VALUE klass)
{
    RBSRT_DEBUG_PRINT("connection allocate");

    rbsrt_connection_t *connection = malloc(sizeof(rbsrt_connection_t));
    if (!connection) { rb_raise(rb_eNoMemError, "not enough memory for SRT connection"); }

    memset(connection, 0, sizeof(rbsrt_connection_t));

    return TypedData_Wrap_Struct(klass, &rbsrt_connection_rbtype, connection);
}

void rbsrt_connection_deallocate(rbsrt_connection_t *connection)
{
    RBSRT_DEBUG_PRINT("connection deallocate");

    SRT_SOCKSTATUS status = srt_getsockstate(connection->socket);

    if (status != SRTS_CLOSED && status != SRTS_CLOSING && status != SRTS_NONEXIST)
    {
        srt_close(connection->socket);
    }

    free(connection);
}


// MARK: API

VALUE rbsrt_connection_set_at_data_block(VALUE self)
{
    RBSRT_DEBUG_PRINT("connection set data block");

    rb_need_block();

    RBSRT_CONNECTION_UNWRAP(self, connection);

    connection->at_data_block = rb_block_proc();

    return Qtrue;
}

VALUE rbsrt_connection_set_at_close_block(VALUE self)
{
    RBSRT_DEBUG_PRINT("connection set close block");

    rb_need_block();

    RBSRT_CONNECTION_UNWRAP(self, connection);

    connection->at_close_block = rb_block_proc();
    
    return Qtrue;
}

// MARK: - SRT::Server Class

// MARK: Ruby Type Helpers

size_t rbsrt_server_dsize(const void *server)
{
    RBSRT_DEBUG_PRINT("server dsize");

    return sizeof(rbsrt_server_t);
}

void rbsrt_server_dmark(void *data)
{
    RBSRT_DEBUG_PRINT("server mark");

    rbsrt_server_t *server = (rbsrt_server_t *)data;

    if (server->acceptor_block)
    {
        rb_gc_mark(server->acceptor_block);
    }
}


// MARK: Utils

#define RBSRT_SERVER_NUM_CONNECTIONS(server) (size_t)atomic_load(&server->num_connections)


// MARK: SRT

int rbsrt_server_listen_callback(void* context, SRTSOCKET remote_socket, int hs_version, const struct sockaddr* peeraddr, const char* streamid)
{
    // TODO: Call into ruby and allow pre-setup of the socket

    return 1;
}


// MARK: Initializers

void rbsrt_server_deallocate(rbsrt_server_t *server)
{
    RBSRT_DEBUG_PRINT("deallocate srt server");

    SRT_SOCKSTATUS status = srt_getsockstate(server->socket);

    if (status != SRTS_CLOSED && status != SRTS_CLOSING)
    {
        srt_close(server->socket);
    }

    free(server);
}
 
VALUE rbsrt_server_allocate(VALUE klass)
{
    RBSRT_DEBUG_PRINT("server allocate");

    rbsrt_server_t *server = malloc(sizeof(rbsrt_server_t));
    if (!server) { rb_raise(rb_eNoMemError, "not enough memory for SRT server"); }

    memset(server, 0, sizeof(rbsrt_server_t));

    return TypedData_Wrap_Struct(klass, &rbsrt_server_rbtype, server);
}

VALUE rbsrt_server_initialize(VALUE self, VALUE address, VALUE port)
{
    RBSRT_DEBUG_PRINT("server initialize");

    rbsrt_server_t *server;
    int status;
    struct sockaddr_in sa;
    int livemode = SRTT_LIVE;
    int no = 1;

    Check_Type(address, T_STRING);
    Check_Type(port, T_STRING);

    TypedData_Get_Struct(self, rbsrt_server_t, &rbsrt_server_rbtype, server);

    // Initialize struct

    atomic_init(&server->num_connections, 0);


    // Ruby API

    VALUE connections_by_socket = rb_hash_new();

    rb_hash_clear(connections_by_socket);
    
    rb_ivar_set(self, rb_intern("@connections_by_socket"), connections_by_socket);

    
    // create socket

    server->socket = srt_create_socket();

    if (server->socket == SRT_ERROR)
    {
        rb_raise(rb_eStandardError, "%s", srt_getlasterror_str());

        return Qfalse;
    }


    // get address info

    sa.sin_family = AF_INET;
    sa.sin_port = htons(atoi(StringValuePtr(port)));
    
    if (inet_pton(AF_INET, StringValuePtr(address), &sa.sin_addr) != 1)
    {
        rb_raise(rb_eArgError, "bad adddress: %s", strerror(errno));
        
        return Qfalse;
    }
    

    // set up socket

    srt_setsockflag(server->socket, SRTO_TRANSTYPE, &livemode, sizeof(livemode)); // set live mode
    srt_setsockflag(server->socket, SRTO_RCVSYN, &no, sizeof(no)); // non-blocking mode
    srt_setsockflag(server->socket, SRTO_SNDSYN, &no, sizeof(no));
    

    // set up callback

    srt_listen_callback(server->socket, rbsrt_server_listen_callback, (void *)self);

    
    // bind server
    
    status = srt_bind(server->socket, (struct sockaddr *)&sa, sizeof(sa));
    
    if (status == SRT_ERROR)
    {
        rb_raise(rb_eStandardError, "%s", srt_getlasterror_str());
        
        return Qfalse;
    }
    
    
    // listen
    
    status = srt_listen(server->socket, 6); // TODO: Pass backlog size
    
    if (status == SRT_ERROR)
    {
        rb_raise(rb_eStandardError, "%s", srt_getlasterror_str());
        
        return Qfalse;
    }

    return self;
}

VALUE rbsrt_server_close(VALUE self)
{
    RBSRT_DEBUG_PRINT("server close");

    RBSRT_SERVER_UNWRAP(self, server);

    if (srt_close(server->socket) == SRT_ERROR)
    {
        rbsrt_raise_last_srt_error();
    }

    server->flags &= ~ (RBSRT_SERVER_ACCEPTING | RBSRT_SERVER_BOUND | RBSRT_SERVER_LISTENING);
    server->flags |= RBSRT_SERVER_CLOSED;

    return Qnil;
}

struct rbsrt_server_epoll_wait_args
{
    rbsrt_server_t *server;
    int64_t timeout;
    int num_events;
    SRT_EPOLL_EVENT *events;
};

void *rbsrt_server_epoll_wait(void *args)
{
    struct rbsrt_server_epoll_wait_args *arg = args;

    int num_events = srt_epoll_uwait(arg->server->epollid, arg->events, arg->num_events, arg->timeout);

    return (void *)(uintptr_t)num_events;
}

VALUE rbsrt_server_connection_count(VALUE self)
{
    RBSRT_SERVER_UNWRAP(self, server);
    
    return SIZET2NUM(RBSRT_SERVER_NUM_CONNECTIONS(server));
}

VALUE rbsrt_server_start(VALUE self)
{
    RBSRT_DEBUG_PRINT("server start");

    rb_need_block();

    RBSRT_SERVER_UNWRAP(self, server);

    // rb_iv_set(self, rb_intern("acceptor_block"), rb_block_proc());

    server->acceptor_block = rb_block_proc();

    server->epollid = srt_epoll_create();

    int event_types = SRT_EPOLL_IN;

    srt_epoll_add_usock(server->epollid, server->socket, &event_types);

    SRT_EPOLL_EVENT events[1024];

    struct rbsrt_server_epoll_wait_args args = {
        .server = server,
        .timeout = 100,
        .num_events = 1024,
        .events = events
    };

    struct sockaddr_storage remote_address;
    int addr_size = sizeof(remote_address);
    int remote_fd;
    int running = 1;

    VALUE connections_by_socket = rb_ivar_get(self, rb_intern("@connections_by_socket"));

    while (running)
    {
        int num_events = (int)(uintptr_t)rb_thread_call_without_gvl(rbsrt_server_epoll_wait, &args, RUBY_UBF_IO, 0);

        if (num_events == SRT_ERROR)
        {
            RBSRT_DEBUG_PRINT("epoll wait error: %s", srt_getlasterror_str());
            continue;
        }

        if (num_events == 0)
        {
            continue; // timeout
        }

        int read_buf_size = RBSRT_PAYLOAD_SIZE * 8;
        char read_buf[read_buf_size];
        int nbytes;

        // SRT_MSGCTRL msgctl;
        SRT_EPOLL_EVENT *event;
        SRTSOCKET sock;
        SRT_SOCKSTATUS status;
        rbsrt_connection_t *connection;

        VALUE rb_connection;

        int no = 0;
        int no_size = sizeof(no);
        int connection_epoll_events = SRT_EPOLL_OUT | SRT_EPOLL_IN | SRT_EPOLL_ERR;

        for (int i = 0; i < num_events; i++)
        {
            event = &args.events[i];
            sock = event->fd;
            status = srt_getsockstate(sock);
            
            switch (status)
            {
                // accept connections
                case SRTS_LISTENING:

                    remote_fd = srt_accept(sock, (struct sockaddr *)&remote_address, &addr_size);

                    srt_setsockflag(remote_fd, SRTO_RCVSYN, &no, no_size);
                    srt_setsockflag(remote_fd, SRTO_SNDSYN, &no, no_size);

                    connection = malloc(sizeof(rbsrt_connection_t));
                    if (!connection) { srt_close(remote_fd); rb_raise(rb_eNoMemError, "not enough memory for SRT connection"); }

                    memset(connection, 0, sizeof(rbsrt_connection_t));

                    rb_connection = TypedData_Make_Struct(mSRTConnectionKlass, rbsrt_connection_t, &rbsrt_connection_rbtype, connection);

                    connection->socket = remote_fd;

                    VALUE should_accept = rb_funcall_with_block(self, rb_intern("instance_exec"), 1, &rb_connection, server->acceptor_block);

                    if (RTEST(should_accept))
                    {
                        srt_epoll_add_usock(server->epollid, connection->socket, &connection_epoll_events);

                        atomic_fetch_add(&server->num_connections, 1);
                        
                        rb_hash_aset(connections_by_socket, INT2FIX(connection->socket), rb_connection);
                    }

                    else
                    {
                        atomic_fetch_sub(&server->num_connections, 1);
                        
                        srt_close(remote_fd);
                    }

                    break;

                case SRTS_CLOSED:
                case SRTS_NONEXIST:
                case SRTS_BROKEN:
                    RBSRT_DEBUG_PRINT("removing connection with socket: %d", sock);

                    srt_epoll_remove_usock(server->epollid, sock);

                    rb_connection = rb_hash_delete(connections_by_socket, INT2FIX(sock));

                    if (RTEST(rb_connection))
                    {
                        if (atomic_fetch_sub(&server->num_connections, 1) == 0)
                        {
                            DEBUG_ERROR_PRINT("removed to many connections");
                        }

                        RBSRT_DEBUG_PRINT("remove connection with socket %d, now %lu sockets", sock, RBSRT_SERVER_NUM_CONNECTIONS(server));

                        RBSRT_CONNECTION_UNWRAP(rb_connection, removed_connection);

                        if (removed_connection->at_close_block)
                        {
                            rb_funcall(removed_connection->at_close_block, rb_intern("call"), 0);

                            removed_connection->at_close_block = 0;
                        }

                        if (removed_connection->at_data_block)
                        {
                            removed_connection->at_data_block = 0;
                        }
                    }

                    break;

                case SRTS_CONNECTED:
                    if (event->events & SRT_EPOLL_IN)
                    {
                        RBSRT_DEBUG_PRINT("will read from socket %d (max bytes %d)", sock, read_buf_size);

                        nbytes = srt_recvmsg2(event->fd, read_buf, read_buf_size, NULL);

                        if (nbytes == SRT_ERROR)
                        {
                            RBSRT_DEBUG_PRINT("failed to read from socket %d: %s", sock, srt_getlasterror_str());

                            break;
                        }

                        if (nbytes > 0)
                        {
                            RBSRT_DEBUG_PRINT("received %d bytes from socket %d", nbytes, sock);

                            rb_connection = rb_hash_lookup(connections_by_socket, INT2FIX(sock));

                            if (RTEST(rb_connection))
                            {
                                RBSRT_DEBUG_PRINT("found connection for socket %d", sock);

                                TypedData_Get_Struct(rb_connection, rbsrt_connection_t, &rbsrt_connection_rbtype, connection);

                                if (connection->at_data_block)
                                {
                                    RBSRT_DEBUG_PRINT("found data handler for socket %d", sock);

                                    VALUE data = rb_str_new(read_buf, nbytes);

                                    rb_funcall(connection->at_data_block, rb_intern("call"), 1, data);
                                }
                            }
                        }
                    }

                    // TODO: Handle other events
                    break;

                default:
                    break;
            }
        }
    }
    
    return Qtrue;
}


// MARK: Connections

VALUE rbsrt_server_accept(VALUE self)
{
    RBSRT_DEBUG_PRINT("server accept");

    RBSRT_SERVER_UNWRAP(self, server);

    TypedData_Get_Struct(self, rbsrt_server_t, &rbsrt_server_rbtype, server);

    struct sockaddr_storage remote_address;
    int addr_size = sizeof(remote_address);

    SRTSOCKET socket = srt_accept(server->socket, (struct sockaddr *)&remote_address, &addr_size);

    VALUE rbclient = rb_class_new_instance(0, NULL, mSRTClientKlass);

    rbsrt_client_t *client;

    TypedData_Get_Struct(rbclient, rbsrt_client_t, &rbsrt_client_rbtype, client);

    client->socket = socket;
    
    client->flags |= RBSRT_CLIENT_CONNECTED;

    return rbclient;
}



// MARK: - SRT::Client Class

// MARK: Ruby Type Helpers

size_t rbsrt_client_dsize(const void *client)
{
    RBSRT_DEBUG_PRINT("dsize srt client");

    return sizeof(rbsrt_client_t);
}

void rbsrt_client_dmark(void *client)
{
    RBSRT_DEBUG_PRINT("dmark srt client");
}


// MARK: Initializers

void rbsrt_client_deallocate(rbsrt_client_t *client)
{
    RBSRT_DEBUG_PRINT("deallocate srt client");

    SRT_SOCKSTATUS status = srt_getsockstate(client->socket);

    switch (status)
    {

    case SRTS_CONNECTED:
    case SRTS_CONNECTING:
    case SRTS_OPENED:
        srt_close(client->socket);

        break;
    
    default:
        break;
    }

    free(client);
}

VALUE rbsrt_client_allocate(VALUE klass)
{
    RBSRT_DEBUG_PRINT("allocate client");

    rbsrt_client_t *client = malloc(sizeof(rbsrt_client_t));
    if (!client) { rb_raise(rb_eNoMemError, "not enough memory for SRT client"); }

    memset(client, 0, sizeof(rbsrt_client_t));
    
    return TypedData_Wrap_Struct(klass, &rbsrt_client_rbtype, client);
}

VALUE rbsrt_client_initialize(VALUE self)
{
    RBSRT_DEBUG_PRINT("initialize client");

    RBSRT_CLIENT_UNWRAP(self, client);

    client->socket = srt_create_socket();
    
    return self;
}


// MARK: - SRT::Poll Klass

size_t rbsrt_poll_dsize(const void *poll)
{
    return sizeof(rbsrt_poll_t);
}

void rbsrt_poll_dmark(void *data)
{
    rbsrt_poll_t *poll = (rbsrt_poll_t *)data;

    if (poll->sockets_by_id)
    {
        rb_gc_mark(poll->sockets_by_id);
    }
}

void rbsrt_poll_deallocate(rbsrt_poll_t *poll)
{
    RBSRT_DEBUG_PRINT("deallocate poll");

    srt_epoll_release(poll->epollid);

    free(poll);
}

int rbsrt_epoll_event_with_splat(VALUE splat, long splat_len)
{
    int events = 0;

    for (long i = 0; i < splat_len; i++)
    {
        VALUE event_type = rb_ary_entry(splat, i);

        ID event_id = rb_to_id(event_type);

        if (event_id == rb_intern("in") || event_id == rb_intern("read"))
        {
            RBSRT_DEBUG_PRINT("poll add SRT_EPOLL_IN");
            
            events |= SRT_EPOLL_IN;
        }

        else if (event_id == rb_intern("out") || event_id == rb_intern("write"))
        {
            RBSRT_DEBUG_PRINT("poll add SRT_EPOLL_OUT");

            events |= SRT_EPOLL_OUT;
        }

        else if (event_id == rb_intern("err") || event_id == rb_intern("error"))
        {
            RBSRT_DEBUG_PRINT("poll add SRT_EPOLL_ERR");

            events |= SRT_EPOLL_ERR;
        }

        else if (event_id == rb_intern("et") || event_id == rb_intern("edge"))
        {
            RBSRT_DEBUG_PRINT("poll add SRT_EPOLL_ET");

            events |= SRT_EPOLL_ET;
        }

        else
        {
            rb_raise(rb_eArgError, "poll event must be one of: :in, :out, :err, :edge");
        }
    }

    return events;
}

VALUE rbsrt_poll_allocate(VALUE klass)
{
    RBSRT_DEBUG_PRINT("allocate poll");

    rbsrt_poll_t *poll = malloc(sizeof(rbsrt_poll_t));
    if (!poll) { rb_raise(rb_eNoMemError, "not enough memory for SRT poll"); }

    memset(poll, 0, sizeof(rbsrt_poll_t));
    
    return TypedData_Wrap_Struct(klass, &rbsrt_poll_rbtype, poll);
}

VALUE rbsrt_poll_initialize(VALUE self)
{
    RBSRT_DEBUG_PRINT("initialize poll");

    RBSRT_POLL_UNWRAP(self, poll);

    poll->epollid = srt_epoll_create();

    VALUE objects_by_socket = rb_hash_new();

    rb_hash_clear(objects_by_socket);

    rb_ivar_set(self, rb_intern("@sockets"), objects_by_socket);
    
    return self;
}

VALUE rbsrt_poll_remove_socket(VALUE self, VALUE socket_to_remove)
{
    RBSRT_DEBUG_PRINT("poll remove socket");

    RBSRT_POLL_UNWRAP(self, poll);

    RBSRT_SOCKET_BASE_UNWRAP(socket_to_remove, socket);

    VALUE sockets = rb_ivar_get(self, rb_intern("@sockets"));

    VALUE removed_socket = rb_hash_delete(sockets, INT2FIX(socket->socket));

    if (srt_epoll_remove_usock(poll->epollid, socket->socket) == SRT_ERROR)
    {
        rbsrt_raise_last_srt_error();
    }

    return removed_socket;
}

VALUE rbsrt_poll_add_socket(int argc, VALUE* argv, VALUE self)
{
    RBSRT_DEBUG_PRINT("poll add socket");

    RBSRT_POLL_UNWRAP(self, poll);

    VALUE arg1;
    VALUE splat;

    rb_scan_args(argc, argv, "1*", &arg1, &splat);

    RBSRT_SOCKET_BASE_UNWRAP(arg1, socket);

    int events = rbsrt_epoll_event_with_splat(splat, rb_array_len(splat));

    if (srt_epoll_add_usock(poll->epollid, socket->socket, &events) == SRT_ERROR)
    {
        rbsrt_raise_last_srt_error();
    }

    VALUE sockets = rb_ivar_get(self, rb_intern("@sockets"));

    rb_hash_aset(sockets, INT2FIX(socket->socket), arg1);

    return self;
}

VALUE rbsrt_poll_update_socket(int argc, VALUE* argv, VALUE self)
{
    RBSRT_DEBUG_PRINT("poll update socket");

    RBSRT_POLL_UNWRAP(self, poll);

    VALUE arg1;
    VALUE splat;

    rb_scan_args(argc, argv, "1*", &arg1, &splat);

    int events = rbsrt_epoll_event_with_splat(splat, rb_array_len(splat));

    RBSRT_SOCKET_BASE_UNWRAP(arg1, socket);

    if (srt_epoll_update_usock(poll->epollid, socket->socket, &events) == SRT_ERROR)
    {
        rbsrt_raise_last_srt_error();
    }

    return Qnil;
}

typedef struct RBSRTPollWaitArg
{
    int epollid;
    int timeout;
    int num_sockets;
    int num_events;
    SRT_EPOLL_EVENT *events;
} rbsrt_poll_wait_arg_t;

void *rbsrt_poll_wait_without_gvl(void *context)
{
    rbsrt_poll_wait_arg_t *arg = (rbsrt_poll_wait_arg_t *)context;

    arg->num_events = srt_epoll_uwait(arg->epollid, arg->events, arg->num_sockets, arg->timeout);

    return arg;
}

VALUE rbsrt_poll_wait(int argc, VALUE* argv, VALUE self)
{
    RBSRT_DEBUG_PRINT("poll wait");

    RBSRT_POLL_UNWRAP(self, poll);

    VALUE timeout;
    VALUE block;

    rb_scan_args(argc, argv, "01&", &timeout, &block);

    int epoll_timeout;

    if (!NIL_P(timeout))
    {
        Check_Type(timeout, T_FIXNUM);

        epoll_timeout = FIX2INT(timeout);
    }

    else
    {
        epoll_timeout = -1;
    }
    
    VALUE sockets = rb_ivar_get(self, rb_intern("@sockets"));
    int num_sockets = FIX2INT(rb_hash_size(sockets));

    int events_capacity = num_sockets < 8 ? 8 : num_sockets + 8;
    SRT_EPOLL_EVENT *events = malloc(events_capacity * sizeof(SRT_EPOLL_EVENT));
    if (!events) { rb_raise(rb_eNoMemError, "not enough memory for poll events"); }

    VALUE readables = rb_ary_new();
    VALUE writables = rb_ary_new();
    VALUE errors = rb_ary_new();

    rbsrt_poll_wait_arg_t arg = {
        .epollid = poll->epollid,
        .timeout = epoll_timeout,
        .num_sockets = events_capacity,
        .num_events = 0,
        .events = events
    };

    rb_thread_call_without_gvl(rbsrt_poll_wait_without_gvl, &arg, RUBY_UBF_IO, NULL);

    RBSRT_DEBUG_PRINT("poll did wait");

    for (int i = 0; i < arg.num_events; i++)
    {
        SRT_EPOLL_EVENT *event = &events[i];

        VALUE sock = rb_hash_aref(sockets, INT2FIX(event->fd));

        if (!RTEST(sock))
        {
            RBSRT_DEBUG_PRINT("poll matched socket not in socket list");

            // TODO: remove unknown socket

            continue;
        }

        if (event->events & SRT_EPOLL_IN)
        {
            RBSRT_DEBUG_PRINT("poll add readables");

            rb_ary_push(readables, sock);
        }

        if (event->events & SRT_EPOLL_OUT)
        {
            RBSRT_DEBUG_PRINT("poll add writables");

            rb_ary_push(writables, sock);
        }

       if (event->events & SRT_EPOLL_ERR)
       {
           RBSRT_DEBUG_PRINT("poll add error");

           rb_ary_push(errors, sock);
       }
    }

    free(events);

    if (rb_block_given_p())
    {
        rb_yield_values(3, readables, writables, errors);

        return Qtrue;
    }

    return rb_ary_new_from_args(3, readables, writables, errors);
}



// MARK: - Ruby Module

void Init_rbsrt() 
{
    // Socket state symbols (cached to avoid rb_intern on every call)

    rbsrt_sym_state_ready      = ID2SYM(rb_intern("ready"));
    rbsrt_sym_state_opened     = ID2SYM(rb_intern("opened"));
    rbsrt_sym_state_listening  = ID2SYM(rb_intern("listening"));
    rbsrt_sym_state_connecting = ID2SYM(rb_intern("connecting"));
    rbsrt_sym_state_connected  = ID2SYM(rb_intern("connected"));
    rbsrt_sym_state_broken     = ID2SYM(rb_intern("broken"));
    rbsrt_sym_state_closing    = ID2SYM(rb_intern("closing"));
    rbsrt_sym_state_closed     = ID2SYM(rb_intern("closed"));
    rbsrt_sym_state_nonexist   = ID2SYM(rb_intern("nonexist"));


    // toplevel SRT module

	mSRTModule = rb_define_module("SRT");

    rb_define_const(mSRTModule, "SRT_VERSION", rb_str_new_cstr(SRT_VERSION_STRING));


    // SRT::Errors

    rbsrt_init_errors();


    // SRT::Socket Class

    mSRTSocketKlass = rb_define_class_under(mSRTModule, "Socket", rb_cObject);

    rb_define_alloc_func(mSRTSocketKlass, rbsrt_socket_allocate);

    rb_define_method(mSRTSocketKlass, "initialize", rbsrt_socket_initialize, 0);

    rbsrt_socket_base_define_basic_api(mSRTSocketKlass);
    rbsrt_socket_base_define_base_api(mSRTSocketKlass);
    rbsrt_socket_base_define_option_api(mSRTSocketKlass);
    rbsrt_socket_base_define_io_api(mSRTSocketKlass);
    rbsrt_define_socket_state_api(mSRTSocketKlass);

    rb_define_method(mSRTSocketKlass, "accept", rbsrt_socket_accept, 0);
    rb_define_method(mSRTSocketKlass, "bind", rbsrt_socket_bind, 2);
    rb_define_method(mSRTSocketKlass, "connect", rbsrt_socket_connect, 2);
    rb_define_method(mSRTSocketKlass, "listen", rbsrt_socket_listen, 1);



    // SRT::Server Class

    mSRTServerKlass = rb_define_class_under(mSRTModule, "Server", rb_cObject);

    rb_define_alloc_func(mSRTServerKlass, rbsrt_server_allocate);

    rb_define_method(mSRTServerKlass, "initialize", rbsrt_server_initialize, 2);

    rbsrt_socket_base_define_base_api(mSRTServerKlass);
    rbsrt_define_socket_state_api(mSRTServerKlass);
    
    rb_define_method(mSRTServerKlass, "close", rbsrt_server_close, 0);
    rb_define_method(mSRTServerKlass, "connection_count", rbsrt_server_connection_count, 0);
    rb_define_method(mSRTServerKlass, "start", rbsrt_server_start, 0);


    // SRT::Connection Class

    mSRTConnectionKlass = rb_define_class_under(mSRTModule, "Connection", rb_cObject);

    rb_define_alloc_func(mSRTConnectionKlass, rbsrt_connection_allocate);
    
    rbsrt_socket_base_define_basic_api(mSRTConnectionKlass);
    rbsrt_socket_base_define_base_api(mSRTConnectionKlass);
    rbsrt_socket_base_define_option_api(mSRTConnectionKlass);
    rbsrt_define_socket_state_api(mSRTConnectionKlass);

    rb_define_method(mSRTConnectionKlass, "sendmsg", rbsrt_socket_sendmsg, 1);
    rb_alias(mSRTConnectionKlass, rb_intern("write"), rb_intern("sendmsg"));


    // calbacks

    rb_define_method(mSRTConnectionKlass, "at_data", rbsrt_connection_set_at_data_block, 0);
    rb_define_method(mSRTConnectionKlass, "at_close", rbsrt_connection_set_at_close_block, 0);

    
    // SRT::Client Class

    mSRTClientKlass = rb_define_class_under(mSRTModule, "Client", rb_cObject);

    rb_define_alloc_func(mSRTClientKlass, rbsrt_client_allocate);

    // SRT::Client methods

    rb_define_method(mSRTClientKlass, "initialize", rbsrt_client_initialize, 0);

    rbsrt_socket_base_define_basic_api(mSRTClientKlass);
    rbsrt_socket_base_define_base_api(mSRTClientKlass);
    rbsrt_socket_base_define_connection_api(mSRTClientKlass);
    rbsrt_socket_base_define_option_api(mSRTClientKlass);
    rbsrt_socket_base_define_io_api(mSRTClientKlass);
    rbsrt_define_socket_state_api(mSRTClientKlass);


    // SRT::Poll

    mSRTPollKlass = rb_define_class_under(mSRTModule, "Poll", rb_cObject);

    rb_define_alloc_func(mSRTPollKlass, rbsrt_poll_allocate);

    rb_define_method(mSRTPollKlass, "initialize", rbsrt_poll_initialize, 0);
    rb_define_method(mSRTPollKlass, "add", rbsrt_poll_add_socket, -1);
    rb_define_method(mSRTPollKlass, "update", rbsrt_poll_update_socket, -1);
    rb_define_method(mSRTPollKlass, "remove", rbsrt_poll_remove_socket, 1);
    rb_define_method(mSRTPollKlass, "wait", rbsrt_poll_wait, -1);


    // Init Stats

    RBSRT_stat_init(mSRTModule);

    // Startup SRT

    rbsrt_srt_startup(NULL);

    ruby_vm_at_exit(rbsrt_srt_cleanup);
}
