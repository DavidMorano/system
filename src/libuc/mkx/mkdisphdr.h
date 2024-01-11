/* mkdisphdr HEADER */
/* lang=C20 */

/* make a c-string displayable */
/* version %I% last-modified %G% */


/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	MKDISPHDR_INCLUDE
#define	MKDISPHDR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


EXTERNC_begin

extern int mkdisphdr(char *,int,cchar *,int) noex ;
extern int mkaddrpart(char *,int,cchar *,int) noex ;
extern int mkdispaddr(char *,int,cchar *,int) noex ;

EXTERNC_end


#endif /* MKDISPHDR_INCLUDE */


