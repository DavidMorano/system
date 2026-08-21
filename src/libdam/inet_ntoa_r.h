/* inet_ntoa_r HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* convert network-format INET address to base-256 d.d.d.d representation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-26, David A­D­ Morano
        I wrote this to get a reentrant subroutine for this function. Maybe
        someday POSIX© will make this subroutine instead but until, you can use
        this! It's dirty, but POSIX© should have already done it. What planet do
        those POSIX© guys live on?

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	INETNTOAR_INCLUDE
#define	INETNTOAR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecstr.h>		/* LIBUC */


EXTERNC_begin

extern char *inet_ntoa_r(INADDR,char *,int) noex ;

EXTERNC_end


#endif /* INETNTOAR_INCLUDE */


