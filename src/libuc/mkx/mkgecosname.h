/* mkgecosname HEADER */
/* lang=C20 */

/* make UUID (also a specialized object) */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MKGECOSNAME_INCLUDE
#define	MKGECOSNAME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


EXTERNC_begin

extern int mkgecosname(char *,int,cchar *) noex ;
extern int getgecosname(cchar *,int,cchar **) noex ;

EXTERNC_end


#endif /* MKGECOSNAME_INCLUDE */


