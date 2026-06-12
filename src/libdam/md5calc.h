/* md5calc HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* calculate a MD5 digest */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MD5CALC_INCLUDE
#define	MD5CALC_INCLUDE

#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */


EXTERNC_begin

extern int md5calc(ulong *,cchar *,int) noex ;

EXTERNC_end


#endif /* MD5CALC_INCLUDE */


