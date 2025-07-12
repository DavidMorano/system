/* secdb HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Security Data-Base (SecDB) management support */
/* version %I% last-modified %G% */

/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*
 * Copyright (c) 1999, 2010, Oracle and/or its affiliates. All rights reserved.
 */

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */


#ifndef	_SECDB_H
#define	_SECDB_H


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	DEFAULT_POLICY		"solaris"
#define	SUSER_POLICY		"suser"		/* fallback: old policy */

#define	KV_ACTION		"act"
#define	KV_COMMAND		"cmd"
#define	KV_JAVA_CLASS		"java_class"
#define	KV_JAVA_METHOD		"java_method"

#define	KV_ASSIGN		"="
#define	KV_DELIMITER		";"
#define	KV_EMPTY		""
#define	KV_ESCAPE		'\\'
#define	KV_ADD_KEYS		16    /* number of key value pairs to realloc */
#define	KV_SPECIAL		"=;:\\"
#define	KV_TOKEN_DELIMIT	":"
#define	KV_WILDCARD		"*"
#define	KV_WILDCHAR		'*'
#define	KV_ACTION_WILDCARD	"*;*;*;*;*"
#define	KV_SEPCHAR		','
#define	KV_SEPSTR		","
#define	KV_OBJECTCHAR		'/'
#define	KV_OBJECT		"/"
#define	KV_AUDIT_DELIMIT	":"

#define	KV_FLAG_NONE		0x0000
#define	KV_FLAG_REQUIRED	0x0001

/*
 * return status macros for all attribute databases
 */
#define	ATTR_FOUND		0	/* Authoritative found */
#define	ATTR_NOT_FOUND		-1	/* Authoritative not found */
#define	ATTR_NO_RECOVERY	-2	/* Non-recoverable errors */


typedef struct kv_s {
	char   *key;
	char   *value;
} kv_t ;				/* A key-value pair */

typedef struct kva_s {
	int	length;			/* array length */
	kv_t    *data;			/* array of key value pairs */
} kva_t ;				/* Key-value array */

EXTERNC_begin

extern char *kva_match(kva_t *, char *) noex ;
extern int _auth_match(cchar *, cchar *) noex ;
extern char *_argv_to_csl(char **strings) noex ;
extern char **_csl_to_argv(char *csl) noex ;
extern char *_do_unescape(char *src) noex ;
extern void _free_argv(char **p_argv) noex ;
extern int _insert2kva(kva_t *, char *, char *) noex ;
extern int _kva2str(kva_t *, char *, int, char *, char *) noex ;
extern kva_t *_kva_dup(kva_t *) noex ;
extern void _kva_free(kva_t *) noex ;
extern void _kva_free_value(kva_t *, char *) noex ;
extern kva_t *_new_kva(int size) noex ;
extern kva_t *_str2kva(char *, char *, char *) noex ;
extern int _enum_auths(cchar *, int (*)(cchar *, void *, void *),
    void *ctxt, void *pres) noex ;
extern int _enum_profs(cchar *,
    int (*)(cchar *, kva_t *, void *, void *), void *ctxt, void *pres) noex ;
extern int _enum_attrs(cchar *,
    int (*)(cchar *, kva_t *, void *, void *), void *ctxt, void *pres) noex ;

EXTERNC_end


#endif	/* _SECDB_H */


