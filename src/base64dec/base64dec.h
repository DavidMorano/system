/* base64dec HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* perform BASE64DEC encoding and decoding */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	BASE64DEC_INCLUDE
#define	BASE64DEC_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<stdio.h>		/* |FILE(3stdio)| */
#include	<clanguage.h>
#include	<usysbase.h>


struct proginfo_fl {
    	uint		text:1 ;
} ; /* end struct (proginfo_fl) */

struct proginfo {
	FILE		*ofp{} ;
	mainv		argv ;
	mainv		envv ;
    	proginfo_fl	fl{} ;
	int		argc ;
	proginfo(int c,mainv a,mainv e) noex : argc(c), argv(a), envv(e) { } ;
} ; /* end struct (proginfo) */

EXTERNC_begin

extern int base64dec_dec(proginfo *,cchar *) noex ;

EXTERNC_end


#endif /* BASE64DEC_INCLUDE */


