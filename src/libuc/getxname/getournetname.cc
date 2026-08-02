/* getournetname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the RPC net-name for a user */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-20, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getournetname

	Description:
	This little diddy retrieves the RPC "netname" of the caller.
	We first check whether the NIS server is running and if so,
	then perform the retrieval.  For extra utility, we will
	also create an RPC net-name for a user other than ourselves.
	In the case of getting an RPC net-name for another user,
	we assume the name is correct.  We do not really know if
	it is registered with the NIS server.

	Synopsis:
	int getournetname(nbuf,nlen,un) noex

	Arguments:
	nbuf		result buffer
	nlen		length of result buffer
	un		username

	Returns:
	>=0		length of result
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<netdb.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucgetnetname.h>	/* LIBUC */
#include	<getxid.h>		/* LIBUC |getuid_name(3uc)| */
#include	<getxname.h>		/* LIBUC */
#include	<getnisdomain.h>	/* LIBUC |getnisdomain(3uc)| */
#include	<sncpyx.h>		/* LIBUC */
#include	<ctdec.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU |DECBUFLEN| */

#include	"getournetname.h"


/* local defines */

#define	PROCNAME	"keyserv"

#ifndef	MAXNETNAMELEN
#define	MAXNETNAMELEN	255	/* maximum length of network user-name */
#endif


/* external subroutines */

extern "C" {
    extern int uc_procpid(cchar *,uid_t) noex ;
}


/* external variables */


/* local structures */


/* forward references */

local int	getothernetname(char *,int,cchar *) noex ;


/* local variables */

cint		netlen = MAXNETNAMELEN ;


/* exported variables */


/* exported subroutines */

int getournetname(char *nbuf,int nlen,cchar *un) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (nlen < 0) nlen = netlen ;
	if (nbuf) ylikely {
	    if ((un == nullptr) || (un[0] == '\0') || (un[0] == '-')) {
	        if (nlen >= netlen) {
	            rs = uc_getnetname(nbuf,nlen) ;
	            len = rs ;
	        }  else {
	            char	netbuf[netlen +1] ;
	            if ((rs = uc_getnetname(netbuf,netlen)) >= 0) {
	                rs = sncpy1(nbuf,nlen,netbuf) ;
	                len = rs ;
	            }
	        } /* end if */
	    } else {
	        rs = getothernetname(nbuf,nlen,un) ;
	        len = rs ;
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (getournetname) */


/* local subroutines */

local int getothernetname(char *nbuf,int nlen,cchar *un) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	cchar		*procname = PROCNAME ;
	if ((rs = uc_procpid(procname,0)) > 0) ylikely {
	    if (char *dbuf ; (rs = lm_hostname(&dbuf)) >= 0) ylikely {
		cint	dlen = rs ;
	        if ((rs = getnisdomain(dbuf,dlen)) >= 0) ylikely {
	            cint	dl = rs ;
		    if ((rs = getuid_user(un,-1)) >= 0) {
	                cint	dilen = DECBUFLEN ;
	                cint	v = rs ;
	                char	dibuf[DECBUFLEN+1] ;
	                if ((rs = ctdeci(dibuf,dilen,v)) >= 0) {
			    cchar	*u = "unix." ;
	                    rs = sncpy4w(nbuf,nlen,u,dibuf,"@",dbuf,dl) ;
	                    len = rs ;
	                }
	            } /* end if (getuid_user) */
	        } /* end if (getnisdomain) */
		rs1 = lm_free(dbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} else if (rs == 0) {
	    rs = SR_UNAVAIL ;
	}
	return (rs >= 0) ? len : rs ;
} /* end subroutine (getothernetname) */


