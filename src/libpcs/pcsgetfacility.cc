/* pcsgetfacility SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* get the "facility" name of the for the PCS distribution */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code was originally written.

	= 2017-11-22, David A­D­ Morano
	I changed this to use the PCSNS (PCS Name-Server) object.
	This allows for both system-wide and local process caching.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	pcsgetfacility

	Description:
	This subroutine retrieves the facility string for PCS.

	Synopsis:
	int pcsgetfacility(cchar *pr,char *rbuf,int rlen) noex

	Arguments:
	pr		PCS system program root (if available)
	rbuf		result buffer pointer
	rlen		result buffer length

	Returns:
	>=0		OK
	<0		error (system-return)

	Notes:

	Q. What is a "facility string"?
	A. It is an identifying string to represent the PCS facility.

	Q. Where do we get such a "string"?
	A. We simply use the "fullname" of the user 'pcs' (or whatever the
	   PCS username is) as the "facility string."

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<strdcpy.h>
#include	<strwcpy.h>		/* |strwcpylc(3uc)| */
#include	<pcsns.h>
#include	<isoneof.h>
#include	<localmisc.h>

#include	"pcsgetfacility.h"


/* local defines */

#ifndef	VARPRPCS
#define	VARPRPCS	"PCS"
#endif

#ifndef	PCSFACILITY
#define	PCSFACILITY	"Personal Communication Services" ;
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int		rsnots[] = {
	SR_DOM,
	SR_ACCESS,
	SR_NOTFOUND,
	SR_OVERFLOW,
	SR_NAMETOOLONG,
	SR_RANGE,
	0
} ; /* end array (rsnots) */

cchar		prname[]	= VARPRPCS ;
cchar		facility[]	= PCSFACILITY ;


/* exported variables */


/* exported subroutines */

int pcsgetfacility(cchar *pr,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rl = 0 ; /* return-value */
	if (pr && rbuf) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (pr[0]) {
	        if (char *ubuf ; (rs = lm_un(&ubuf)) >= 0) {
	            cint ulen = rs ;
	            strwcpylc(ubuf,prname,ulen) ; /* get lower-case */
	            if (PCSNS ns ; (rs = pcsns_open(&ns,pr)) >= 0) {
	                cint	w = pcsnsreq_fullname ;
	                if ((rs = pcsns_get(&ns,rbuf,rlen,ubuf,w)) >= 0) {
		            rl = rs ;
	                } else if (isOneOf(rsnots,rs)) {
			    cchar *p = strdcpy1(rbuf,rlen,facility) ;
		            rs = intconv(p - rbuf) ;
		            rl = rs ;
	                }
	                rs1 = pcsns_close(&ns) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (pcsns) */
		    rs1 = lm_free(ubuf) ;
	            if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (pcsgetfacility) */


