/* sfxchr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* various string-find operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SFXCHR_INCLUDE
#define	SFXCHR_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int sfochr(cchar *,int,int,cchar **) noex ;
extern int sfrchr(cchar *,int,int,cchar **) noex ;

EXTERNC_end


#endif /* SFXCHR_INCLUDE */


