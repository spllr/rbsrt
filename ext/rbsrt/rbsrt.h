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

#ifndef RBSRT_HEADER
#define RBSRT_HEADER

#include <stdatomic.h>

#include <ruby/ruby.h>
#include <srt/srt.h>


// MARK: - Utils

// MARK: - Status

#define RBSRT_SUCCESS 0
#define RBSRT_FAILURE 1


// MARK: Debug

// #define RBSRT_HAVE_DEBUG

#ifdef RBSRT_HAVE_DEBUG
#define RBSRT_DEBUG 1
#endif


#if RBSRT_DEBUG
#define RBSRT_DEBUG_MSG_MAX 1024
#define RBSRT_DEBUG_PRINT(...)                                \
{                                                       \
    char debugmsg__[RBSRT_DEBUG_MSG_MAX];               \
    sprintf(debugmsg__, ## __VA_ARGS__);                \
    fprintf(stderr, "rbsrt - %s\n", debugmsg__);        \
}                                                       \

#define DEBUG_ERROR_PRINT(...)                          \
{                                                       \
    char debugmsg__[RBSRT_DEBUG_MSG_MAX];               \
    sprintf(debugmsg__, ## __VA_ARGS__);                \
    fprintf(stderr, "rbsrt error - %s\n", debugmsg__);  \
}  
#else
#define RBSRT_DEBUG_PRINT(...)
#define DEBUG_ERROR_PRINT(...)
#endif


// MARK: Constants

#define RBSRT_PAYLOAD_SIZE 1316


// MARK: - Structs

typedef struct RBSRTSocketBase
{
    SRTSOCKET socket;
} rbsrt_socket_base_t;

typedef struct RBSRTSocket
{
    SRTSOCKET socket;
} rbsrt_socket_t;

typedef struct RBSRTConnection
{
    SRTSOCKET socket;
    VALUE at_data_block;
    VALUE at_close_block;
} rbsrt_connection_t;

typedef struct RBSRTServer
{
    SRTSOCKET socket;
    int flags; // TODO: Deprecate flags
    atomic_size_t num_connections;
    SRT_EPOLL_T epollid;
    VALUE acceptor_block;
} rbsrt_server_t;

typedef struct RBSRTClient
{
    SRTSOCKET socket;
    int flags; // TODO: Deprecate flags
} rbsrt_client_t;

typedef struct RBSRTPoll
{
    SRT_EPOLL_T epollid;
    VALUE sockets_by_id;
} rbsrt_poll_t;

typedef struct RBSRTStats
{
  SRT_TRACEBSTATS perf;
} rbsrt_stats_t;


// MARK: - Ruby Struct Headers

// MARK: SRT::Socket Class

size_t rbsrt_socket_dsize(const void *socket);
void rbsrt_socket_dmark(void *data);
void rbsrt_socket_deallocate(rbsrt_socket_t *socket);

// MARK: SRT::Connection Class

size_t rbsrt_connection_dsize(const void *connection);
void rbsrt_connection_dmark(void *data);
void rbsrt_connection_deallocate(rbsrt_connection_t *connection);

// MARK: SRT::Server Class

size_t rbsrt_server_dsize(const void *server);
void rbsrt_server_dmark(void *data);
void rbsrt_server_deallocate(rbsrt_server_t *server);

// MARK: SRT::Client Class

size_t rbsrt_client_dsize(const void *client);
void rbsrt_client_dmark(void *data);
void rbsrt_client_deallocate(rbsrt_client_t *client);

// MARK: SRT::Poll Class

size_t rbsrt_poll_dsize(const void *poll);
void rbsrt_poll_dmark(void *data);
void rbsrt_poll_deallocate(rbsrt_poll_t *poll);


// MARK: SRT::Stats Class

size_t rbsrt_stats_dsize(const void *stats);
void rbsrt_stats_dmark(void *data);
void rbsrt_stats_deallocate(rbsrt_stats_t *stats);


// MARK: - Ruby Structs

static const rb_data_type_t rbsrt_socket_rbtype = {
	.wrap_struct_name = "rbsrt_socket",
	.function = {
		.dfree = (void *)rbsrt_socket_deallocate,
        .dsize = rbsrt_socket_dsize,
        .dmark = rbsrt_socket_dmark
	},
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

static const rb_data_type_t rbsrt_connection_rbtype = {
	.wrap_struct_name = "rbsrt_connection",
	.function = {
		.dfree = (void *)rbsrt_connection_deallocate,
        .dsize = rbsrt_connection_dsize,
        .dmark = rbsrt_connection_dmark,
	},
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

static const rb_data_type_t rbsrt_server_rbtype = {
	.wrap_struct_name = "rbsrt_server",
	.function = {
		.dfree = (void *)rbsrt_server_deallocate,
        .dsize = rbsrt_server_dsize,
        .dmark = rbsrt_server_dmark
	},
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

static const rb_data_type_t rbsrt_client_rbtype = {
	.wrap_struct_name = "rbsrt_client",
	.function = {
		.dfree = (void *)rbsrt_client_deallocate,
        .dsize = rbsrt_client_dsize,
        .dmark = rbsrt_client_dmark
	},
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

static const rb_data_type_t rbsrt_poll_rbtype = {
	.wrap_struct_name = "rbsrt_poll",
	.function = {
		.dfree = (void *)rbsrt_poll_deallocate,
        .dsize = rbsrt_poll_dsize,
        .dmark = rbsrt_poll_dmark
	},
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

static const rb_data_type_t rbsrt_stats_rbtype = {
	.wrap_struct_name = "rbsrt_stats",
	.function = {
		.dfree = (void *)rbsrt_stats_deallocate,
    .dsize = rbsrt_stats_dsize,
    .dmark = rbsrt_stats_dmark
	},
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};


// MARK: Unwraps

#define RBSRT_IS_SOCKET(socket) (srt_getsockstate(socket->socket) != SRTS_NONEXIST)

#define RBSRT_CHECK_SOCKET(input, socket)                                           \
if (!RBSRT_IS_SOCKET(socket))                                                       \
{                                                                                   \
    rb_raise(rb_eTypeError,                                                         \
            "wrong type %"PRIsVALUE" is not a SRT Socket Type",                     \
            rb_obj_class(input));                                                   \
}                                                                                   \

// NOTE: Bypassing some safty guards allowing us to use same function for server, 
//       connection, socket, etc.
#define RBSRT_SOCKET_BASE_UNWRAP(input, output)                                     \
rbsrt_socket_base_t *output = (rbsrt_socket_base_t *)DATA_PTR(input);               \
RBSRT_CHECK_SOCKET(input, output);                                                  \

#define RBSRT_SOCKET_UNWRAP(input, output)                                          \
rbsrt_socket_t *output;                                                             \
TypedData_Get_Struct(input, rbsrt_socket_t, &rbsrt_socket_rbtype, output);          \

#define RBSRT_CONNECTION_UNWRAP(input, output)                                      \
rbsrt_connection_t *output;                                                         \
TypedData_Get_Struct(input, rbsrt_connection_t, &rbsrt_connection_rbtype, output);  \

#define RBSRT_SERVER_UNWRAP(input, output)                                          \
rbsrt_server_t *output;                                                             \
TypedData_Get_Struct(input, rbsrt_server_t, &rbsrt_server_rbtype, output);          \

#define RBSRT_CLIENT_UNWRAP(input, output)                                          \
rbsrt_client_t *output;                                                             \
TypedData_Get_Struct(input, rbsrt_client_t, &rbsrt_client_rbtype, output);          \

#define RBSRT_POLL_UNWRAP(input, output)                                            \
rbsrt_poll_t *output;                                                               \
TypedData_Get_Struct(input, rbsrt_poll_t, &rbsrt_poll_rbtype, output);              \

#define RBSRT_STATS_UNWRAP(input, output)                                           \
rbsrt_stats_t *output;                                                              \
TypedData_Get_Struct(input, rbsrt_stats_t, &rbsrt_stats_rbtype, output);            \


// MARK: - Errors

_Noreturn void rbsrt_raise_last_srt_error(void);

#endif