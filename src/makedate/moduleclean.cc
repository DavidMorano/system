/* makedate_moduleclean SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* clean up a software module name */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* run-time debugging */

/* revision history:

	= 1986-03-01, David A­D­ Morano
	This code was originally written for use for LPPI firmware
	development.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Here we do some processing on the (?) module name.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstring>
#include	<usystem.h>
#include	<sbuf.h>
#include	<strx.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */


/* external subroutines */


/* exported subroutines */

int moduleclean(proginfo *pip,char *rbuf,int rlen,cchar *inname) noex {
	sbuf		ubuf ;
	int		rs ;
	int		rs1 ;
	int		f = FALSE ;

	if (pip == NULL) return SR_FAULT ;
	if (inname == NULL) return SR_FAULT ;

	if (inname[0] == '\0')
	    return SR_OK ;

	if (rbuf == NULL) return SR_FAULT ;

	if ((rs = sbuf_start(&ubuf,rbuf,rlen)) >= 0) {
	     cchar	*np = inname ;
	     cchar	*tp ;

	    while ((tp = strbrk(np,"+-/")) != NULL) {
	        sbuf_buf(&ubuf,np,(tp - np)) ;
	        sbuf_chr(&ubuf,'_') ;
	        np = (tp + 1) ;
	        f = TRUE ;
	    } /* end while */
	    sbuf_buf(&ubuf,np,-1) ;

	    rs1 = sbuf_finish(&ubuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sbuf) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (moduleclean) */


