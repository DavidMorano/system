/* mkfingerquery SUPPORT */
/* lang=C20 */

/* make argument string for FINGER query */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkfingerquery

	Description:
	This subroutine creates a string that reqpresent a FINGER
	query string.  We handle the so-called "long" flag option
	and we also handle FINGER service-arguments if they are
	supplied.

	Synopsis:
	int mkfingerquery(char *qbuf,int qlen,int f_long,cc *up,mainv av) noex

	Arguments:
	qbuf		result buffer pointer
	qlen		result buffer length
	f_long		the "long" flag
	up		user-part
	av		service arguments (if any)


	>=0		length of used destination buffer from conversion
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<string.h>
#include	<usystem.h>
#include	<sbuf.h>
#include	<localmisc.h>


/* external subroutines */

extern int	sbuf_addquoted(sbuf *,const char *,int) ;


/* forward references */


/* local variables */


/* exported subroutines */

int mkfingerquery(char *qbuf,int qlen,int f_long,cchar *up,cchar **av) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (qbuf && up) {
	    rs = SR_INVALID ;
	    if (qlen > 0) {
	sbuf		b ;
	if ((rs = sbuf_start(&b,qbuf,qlen)) >= 0) {
	    rs = sbuf_strw(&b,up,-1) ;
	    if ((rs >= 0) && f_long) {
		sbuf_strw(&b," /W",3) ;
	    }
	    if (av) {
	        for (int i = 0 ; (rs >= 0) && av[i] ; i += 1) {
	            if ((rs = sbuf_char(&b,' ')) >= 0) {
			rs = sbuf_addquoted(&b,av[i],-1) ;
		    }
	        } /* end for */
	    } /* end if (argument-vector) */
	    if (rs >= 0) {
	        sbuf_strw(&b,"\n\r",2) ;
	    }
	    len = sbuf_finish(&b) ;
	    if (rs >= 0) rs = len ;
	} /* end if (sbuf) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mkfingerquery) */


