/* hostent HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* manipulate host entry structures */
/* version %I% last-modified %G% */


/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	HOSTENT_INCLUDE
#define	HOSTENT_INCLUDE 


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/socket.h>		/* POSIX® */
#include	<netinet/in.h>		/* POSIX® */
#include	<arpa/inet.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	HOSTENT		struct hostent
#define	HOSTENT_CUR	struct hostent_cursor


struct hostent_cursor {
	int		i ;
} ; /* end struct (hostent_cursor) */

typedef HOSTENT_CUR	hostent_cur ;

EXTERNC_begin

extern int hostent_getofficial(HOSTENT *,cchar **) noex ;
extern int hostent_getcanonical(HOSTENT *,cchar **) noex ;
extern int hostent_getaf(HOSTENT *) noex ;
extern int hostent_getalen(HOSTENT *) noex ;
extern int hostent_curbegin(HOSTENT *,hostent_cur *) noex ;
extern int hostent_curend(HOSTENT *,hostent_cur *) noex ;
extern int hostent_curenumname(HOSTENT *,hostent_cur *,cchar **) noex ;
extern int hostent_curenumaddr(HOSTENT *,hostent_cur *,const uchar **) noex ;
extern int hostent_size(HOSTENT *) noex ;
extern int hostent_load(HOSTENT *,char *,int,const HOSTENT *) noex ;

EXTERNC_end


#endif /* HOSTENT_INCLUDE */


