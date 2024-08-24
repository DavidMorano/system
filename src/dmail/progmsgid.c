/* progmsgid SUPPORT */
/* lang=C++20 */

/* create a mail message ID */
/* version %I% last-modified %G% */

#define	CF_HOSTID	0		/* use host-id? */

/* revision history:

	= 1998-05-01, David A­D­ Morano
	This subroutine is originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	progmsgid

	Description:
	This subroutine is used to create a mail message ID for
	certain PCS programs.

	Synopsis:
	int progmsgid(proginfo *pip,char *mbuf,int mlen,int serial) noex

	Arguments:
	pip		pointer to program information
	mbuf		caller supplied buffer to place result in
	mlen		length of caller supplied buffer
	serial		a serial number of some kind

	Returns:
	>=0		length of returned ID
	<0		error in process of creating ID (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<unistd.h>		/* |gethostid(3c)| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<sbuf.h>
#include	<mkpathx.h>
#include	<localmisc.h>

#include	"progmsgid.h"


/* local defines */

#ifndef	CF_HOSTID
#define	CF_HOSTID	0		/* use host-id? */
#endif


/* external subroutines */


/* external variables */


/* local structures */

struct vars {
	int		usernamelen ;
} ;


/* forward references */

static int	mkvars() noex ;


/* local variables */

static vars		var ;

constexpr bool		f_hostid = CF_HOSTID ;


/* exported variables */


/* exported subroutines */

int progmsgid(proginfo *pip,char *mbuf,int mlen,int serial) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (pip && mbuf) {
	    static int	rsv = mkvars() ;
	    if ((rs = rsv) >= 0) {
	        sbuf	ub ;
	        if ((rs = ub.start(mbuf,mlen)) >= 0) {
	            const time_t	dt = pip->daytime ;
	            const pid_t		pid = pip->pid ;
	            cchar		*nn = pip->nodename ;
	            cchar		*dn = pip->domainname ;
		    {
			uint	uv ;
	                if_constexpr (f_hostid) {
		            cint	nl = strlen(nn) ;
	                    if (nl > var.usernamelen) {
		                uv = pip->hwserial ;
	                        ub.hex(uv) ;
	                        ub.chr('-') ;
	                    } else {
	                        ub.strw(nn,nl) ;
		            }
	                } else {
	                    ub.str(nn) ;
	                }
		        {
	                    uv = uint(pid) ;
	                    ub.dec(uv) ;
    		        }
	                ub.chr('.') ;
    	                {
	                    uv = uint(dt) ;
	                    ub.hex(uv) ;
    		        }
	                ub.chr('.') ;
	                ub.dec(pip->pserial) ;
	                ub.chr('.') ;
	                ub.dec(serial) ;
	                ub.chr('@') ;
	                ub.strw(dn,-1) ;
    		    } /* end block */
	            len = ub.finish ;
	            if (rs >= 0) rs = len ;
	        } /* end if (sbuf) */
	    } /* end if (mkvars) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (progmsgid) */


/* local subroutines */

static int mkvars() noex {
	int		rs ;
	if ((rs = getbufsize(getbufsize_un)) >= 0) {
	    var.usernamelen = rs ;
	}
	return rs ;
}
/* end subroutine (mkvars) */


