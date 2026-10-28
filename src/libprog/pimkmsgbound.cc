/* pimkmsgbound SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* create a mail message boundary for multipart MIME messages */
/* version %I% last-modified %G% */

#define	CF_MTIME	0		/* use MSG 'time' */

/* revision history:

	= 1998-05-01, David A­D­ Morano
	This subroutine is originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	pimkmsgbound

	Description:
	This subroutine creates a unique (?) string no more than
	MSGBOUND characters.  This string (called a "message-boundary"
	here) is used to separate sections within email messages.

	Synopsis:
	int pimkmsgbound(proginfo *pip,char *rbuf,int rlen) noex

	Arguments:
	pip		pointer to program information
	rbuf		caller-supplied buffer to receive result
	rlen		caller-supplied buffer length

	Returns:
	>=0		length of result
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<sbuf.h>		/* LIBUC */
#include	<ctdec.h>		/* LIBUC */
#include	<cthex.h>		/* LIBUC */
#include	<dater.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"proginfo.hh"

import proginfo ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

constexpr bool	f_mtime = CF_MTIME ;


/* exported variables */


/* exported subroutines */

int pimkmsgbound(proginfo *pip,char *mbuf,int mlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (mbuf) {
	    mbuf[0] = '\0' ;
	    if ((rs = pimkrand(pip)) >= 0) {
	        if (sbuf b ; (rs = sbuf_start(&b,mbuf,mlen)) >= 0) {
	            time_t	t ;
	            uint	hostid ;
		    /* nodename */
	            sbuf_strw(&b,pip->nodename,-1) ;
		    /* PID */
	            sbuf_deci(&b,(int) pip->pid) ;
	            sbuf_chr(&b,'.') ;
		    /* time-of-day */
		    if_constexpr (f_mtime) {
	                dater_gettime(&pip->mdate,&t) ;
		    } else {
	                t = pip->daytime ;
		    }
	            sbuf_hexi(&b,(uint) t) ;
		    /* hostid */
	            hostid = gethostid() ;
	            sbuf_hexui(&b,hostid) ;
		    /* random number */
	            sbuf_hexull(&b,pip->rand) ;
	            sbuf_chr(&b,'.') ;
	            sbuf_decui(&b,pip->serial) ;
	            sbuf_chr(&b,'@') ;
	            if (pip->domainname) {
	                sbuf_strw(&b,pip->domainname,-1) ;
		    }
	            len = sbuf_finish(&b) ;
	            if (rs >= 0) rs = len ;
	        } /* end if (sbuf) */
	    } /* end if (pimkrand) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (pimkmsgbound) */


