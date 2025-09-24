/* nss_parse HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Name-Server-Switch (NSS) database parsing support */
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
 * Copyright 2006 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */


#ifndef	_NSS_H
#define	_NSS_H


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#ifdef	COMMENT /* modification by David A-D- Morano */
#include <sys/socket.h>
#include <netinet/in.h>
#include <nss_common.h>
#include <netdb.h>
#endif /* COMMENT */

EXTERNC_begin

#ifdef	COMMENT /* modification by David A-D- Morano */
extern struct hostent	
*_switch_gethostbyname_r(cchar *, struct hostent *,
    char *, int, int *) noex ;
extern struct hostent	
*_switch_gethostbyaddr_r(cchar *, int, int,
    struct hostent *, char *, int, int *) noex ;
extern struct hostent	
*_switch_getipnodebyname_r(cchar *,
    struct hostent *, char *, int, int, int, int *) noex ;
extern struct hostent	
*_switch_getipnodebyaddr_r(cchar *, int, int,
    struct hostent *, char *, int, int *) noex ;
extern struct hostent	
*_door_gethostbyname_r(cchar *, struct hostent *,
    char *, int, int *) noex ;
extern struct hostent	
*_door_gethostbyaddr_r(cchar *, int, int,
    struct hostent *, char *, int, int *) noex ;
extern struct hostent	
*_door_getipnodebyname_r(cchar *, struct hostent *,
    char *, int, int, int, int *) noex ;
extern struct hostent	
*_door_getipnodebyaddr_r(cchar *, int, int,
    struct hostent *, char *, int, int *) nox ;
extern struct hostent	*__mappedtov4(struct hostent *, int *) noex ;
extern int	str2hostent(cchar *, int, void *, char *, int) noex ;
extern int	str2hostent6(cchar *, int, void *, char *, int) noex ;
extern int	__str2hostent(int, cchar *, int, void *, char *, int) noex ;
extern int	str2servent(cchar *, int, void *, char *, int) noex ;
extern void	_nss_initf_hosts(nss_db_params_t *) noex ;
extern void	_nss_initf_ipnodes(nss_db_params_t *) noex ;
extern void	order_haddrlist_af(sa_family_t, char **) noex ;
extern int	nss_ioctl(int, int, void *) noex ;
#endif /* COMMENT */

/* parse.c */
extern char *_strtok_escape(char *, char *, char **) noex ;
extern char *_strpbrk_escape(char *, char *) noex ;
extern char *_escape(char *, char *) noex ;
extern char *_unescape(char *, char *) noex ;
extern char *_strdup_null(char *) noex ;

#ifdef	COMMENT /* modification by David A-D- Morano */
extern int _readbufline(char *,	int, char *, int, int *) noex ;
#endif /* COMMENT */

EXTERNC_end


#endif	/* _NSS_H */


