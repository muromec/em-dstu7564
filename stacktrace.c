/*
 * Copyright (c) 2016 PrivatBank IT <acsk@privatbank.ua>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */

#include "stacktrace.h"

#include <string.h>
#include <stdlib.h>

/*#include "pthread_internal.h"*/

#undef FILE_MARKER
#define FILE_MARKER "cryptonite/stracktrace.c"

typedef struct StackTraceCtx_st {
    char *msg;
    unsigned long pid;
    ErrorCtx *head;
    ErrorCtx *tail;
} StackTraceCtx;

typedef struct ErrorsCtx_st {
    StackTraceCtx *stacktrace;
    struct ErrorsCtx_st *next;
} ErrorsCtx;

static char errors_mutex;
static ErrorsCtx *errors_list = NULL;

unsigned long pthread_id () { return 0; };
inline void pthread_mutex_lock(char* l) {};
inline void pthread_mutex_unlock(char* l) {};

static StackTraceCtx *errors_get_stacktrace_by_pid(unsigned long pid)
{
    if (errors_list) {
        ErrorsCtx *err_curr = errors_list;
        while (err_curr != NULL) {
            if ((err_curr->stacktrace != NULL) && (err_curr->stacktrace->pid == pid)) {
                return err_curr->stacktrace;
            } else {
                err_curr = err_curr->next;
            }
        }
    }

    return NULL;
}

static ErrorCtx *error_ctx_create(const char *file, const size_t line, const int error_code)
{
    ErrorCtx *new_ctx = NULL;

    new_ctx = malloc(sizeof(ErrorCtx));
    if (new_ctx == NULL) {
        return NULL;
    }

    if (file) {
        new_ctx->file = malloc(strlen(file) + 1);
        if (new_ctx->file == NULL) {
            free(new_ctx);
            return NULL;
        }
        strcpy(new_ctx->file, file);
    } else {
        new_ctx->file = NULL;
    }

    new_ctx->error_code = error_code;
    new_ctx->line = line;
    new_ctx->next = NULL;

    return new_ctx;
}

static void stacktrace_add_element(StackTraceCtx *errors_ctx, const char *file, const size_t line, const int error_code)
{
    ErrorCtx *new_ctx;

    new_ctx = error_ctx_create(file, line, error_code);
    if (new_ctx == NULL) {
        return;
    }

    if (errors_ctx->head == NULL) {
        errors_ctx->head = new_ctx;
    } else {
        errors_ctx->tail->next = new_ctx;
    }

    errors_ctx->tail = new_ctx;
}

static StackTraceCtx *errors_create_stacktrace(void)
{
    StackTraceCtx *stacktrace = malloc(sizeof(StackTraceCtx));

    if (stacktrace) {
        stacktrace->pid = pthread_id();
        stacktrace->head = NULL;
        stacktrace->tail = NULL;
        stacktrace->msg = NULL;

        ErrorsCtx *err = malloc(sizeof(ErrorsCtx));
        if (err) {
            err->stacktrace = stacktrace;
            err->next = errors_list;
        }

        errors_list = err;
    }

    return stacktrace;
}

void error_ctx_free(ErrorCtx *ctx)
{
    if (ctx) {
        ErrorCtx *error_curr = ctx;
        ErrorCtx *error_next;
        while (error_curr != NULL) {
            error_next = error_curr->next;
            free(error_curr->file);
            free(error_curr);
            error_curr = error_next;
        }
    }
}

static ErrorCtx *error_copy_with_alloc(ErrorCtx *ctx)
{
    ErrorCtx *out = NULL;

    if (ctx == NULL) {
        return NULL;
    }

    out = error_ctx_create(ctx->file, ctx->line, ctx->error_code);

    if (out) {
        ErrorCtx *error_next = ctx->next;
        ErrorCtx *out_curr = out;

        while (error_next) {
            out_curr->next = error_ctx_create(error_next->file, error_next->line, error_next->error_code);
            error_next = error_next->next;
            out_curr = out_curr->next;
        }
    }

    return out;
}

void stacktrace_create(const char *file, const size_t line, const int error_code, const char *msg)
{
    pthread_mutex_lock(&errors_mutex);

    unsigned long pid = pthread_id();
    StackTraceCtx *stacktrace = errors_get_stacktrace_by_pid(pid);

    if (stacktrace != NULL) {
        error_ctx_free(stacktrace->head);
        stacktrace->head = NULL;
        stacktrace->tail = NULL;
        stacktrace->msg = NULL;
    } else {
        stacktrace = errors_create_stacktrace();
    }

    stacktrace_add_element(stacktrace, file, line, error_code);

    /* Copy msg. */
    free(stacktrace->msg);
    if (msg != NULL) {
        stacktrace->msg = malloc(strlen(msg) + 1);
        if (stacktrace->msg != NULL) {
            strcpy(stacktrace->msg, msg);
        }
    } else {
        stacktrace->msg = NULL;
    }

    pthread_mutex_unlock(&errors_mutex);
}

void stacktrace_add(const char *file, const size_t line, const int error_code)
{
    pthread_mutex_lock(&errors_mutex);

    unsigned long pid = pthread_id();
    StackTraceCtx *stacktrace = errors_get_stacktrace_by_pid(pid);

    if (stacktrace == NULL) {
        stacktrace = errors_create_stacktrace();
    }

    stacktrace_add_element(stacktrace, file, line, error_code);

    pthread_mutex_unlock(&errors_mutex);
}

const ErrorCtx *stacktrace_get_last(void)
{
    unsigned long tid = pthread_id();
    ErrorCtx *out;

    pthread_mutex_lock(&errors_mutex);

    StackTraceCtx *stracktrace = errors_get_stacktrace_by_pid(tid);
    out = (stracktrace != NULL) ? stracktrace->head : NULL;

    pthread_mutex_unlock(&errors_mutex);

    return out;
}

static void stacktrace_free(StackTraceCtx *stacktrace)
{
    if (stacktrace) {
        error_ctx_free(stacktrace->head);
        free(stacktrace->msg);
        free(stacktrace);
    }
}

static void errors_free(ErrorsCtx *ctx)
{
    if (ctx) {
        stacktrace_free(ctx->stacktrace);
        free(ctx);
    }
}

void stacktrace_free_current(void)
{
    unsigned long pid = pthread_id();

    pthread_mutex_lock(&errors_mutex);

    if (errors_list) {
        if (errors_list->stacktrace != NULL && errors_list->stacktrace->pid == pid) {
            if (errors_list->next == NULL) {
                errors_free(errors_list);
                errors_list = NULL;
            } else {
                ErrorsCtx *errors_list_ptr = errors_list;
                errors_list = errors_list->next;
                errors_free(errors_list_ptr);
            }
        } else {
            ErrorsCtx *errors_curr = errors_list;
            ErrorsCtx *errors_next;

            while (errors_curr != NULL) {
                errors_next = errors_curr->next;
                if (errors_next != NULL && errors_next->stacktrace != NULL
                        && errors_next->stacktrace->pid == pid) {
                    errors_curr->next = (errors_next->next != NULL) ? errors_next->next : NULL;
                    errors_free(errors_next);
                    break;
                }
                errors_curr = errors_next;
            }
        }
    }

    pthread_mutex_unlock(&errors_mutex);
}

ErrorCtx *stacktrace_get_last_with_alloc(void)
{
    ErrorCtx *error_ctx_new = NULL;

    pthread_mutex_lock(&errors_mutex);

    StackTraceCtx *stracktrace = errors_get_stacktrace_by_pid(pthread_id());
    if (stracktrace != NULL) {
        error_ctx_new = error_copy_with_alloc(stracktrace->head);
    }

    pthread_mutex_unlock(&errors_mutex);

    return error_ctx_new;
}

void stacktrace_finalize(void)
{
    pthread_mutex_lock(&errors_mutex);

    if (errors_list) {
        ErrorsCtx *errors_curr = errors_list;
        ErrorsCtx *errors_next;
        while (errors_curr != NULL) {
            errors_next = errors_curr->next;
            errors_free(errors_curr);
            errors_curr = errors_next;
        }
        errors_list = NULL;
    }

    pthread_mutex_unlock(&errors_mutex);
}
