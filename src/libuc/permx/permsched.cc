/* permsched SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find a file according to rules */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	permsched

	Description:
	This subroutine will form a file name according to some
	rules.

	Synopsis:
	int permsched(mainv sched,vecstr *nsp,char *rbuf,int rlen,
			cc *fn,mode_t am) noex

	Arguments:
	sched		array of strings forming the search schedule
	nsp		string-list of substitution keys
	rbuf		user supplied buffer to hold resuling filepath
	rlen		length of user supplied buffer
	fn		filename to search for
	am		access-mode needed to get a file hit

	Returns:
	>=0	length of found filepath
	<0	error (system-error)

	Notes:
	A typical example is something like trying the following:

	programroot/etc/name/name.fname
	programroot/etc/name/fname
	programroot/etc/name.fname
	programroot/name.fname
	programroot/fname

	if (programroot != PWD) {
		etc/name/name.fname
		etc/name/fname
		etc/name.fname
		name.fname
		fname
	}

	%p/%e/%n/%n.%f
	%p/%e/%n/%f
	%p/%e/%n.%f
	%p/%n.%f
	%p/%f
	
	%e/%n/%n.%f
	%e/%n.%f
	%e/%n.%f
	%n.%f
	%f

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<sbuf.h>
#include	<ids.h>
#include	<vecstr.h>
#include	<vstrkeycmpx.h>
#include	<localmisc.h>

#include	"permx.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */


/* local typedefs */

typedef mainv		mv ;
typedef mode_t		pm ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	schedexpand(cchar *,vecstr *,char *,int,cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int permsched(mv sched,vecstr *nsp,char *rbuf,int rlen,cc *fn,pm am) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		sl = 0 ;
	if (sched && nsp && rbuf && fn) ylikely {
	    rs = SR_INVALID ;
	    if (fn[0]) {
	        if (ids id ; (rs = id.load) >= 0) ylikely {
	            rs = SR_NOEXIST ;
	            for (int i = 0 ; sched[i] ; i += 1) {
		        cchar	*sch = sched[i] ;
	                if ((rs = schedexpand(sch,nsp,rbuf,rlen,fn)) >= 0) {
	                    sl = rs ;
	                    if (ustat sb ; (rs = uc_stat(rbuf,&sb)) >= 0) {
	                        rs = permid(&id,&sb,am) ;
	                    }
	                } /* end if (schedexpand) */
	                if (rs >= 0) break ;
	            } /* end for */
	            rs1 = id.release ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (ids) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (permsched) */


/* local subroutines */

static int schedexpand(cc *fmt,vecstr *nsp,char *rbuf,int rlen,cc *fn) noex {
	cnullptr	np{} ;
	int		rs ;
	int		len = 0 ;
	rbuf[0] = '\0' ;
	if (sbuf b ; (rs = b.start(rbuf,rlen)) >= 0) ylikely {
	    cauto	vf = vecstr_finder ;
	    cauto	vs = vstrkeycmp ;
	    char	kb[2] = {} ;
	    for (cchar *fp = fmt ; *fp && (rs >= 0) ; fp += 1) {
	        if (*fp == '%') {
	            fp += 1 ;
	            if (! *fp) break ;
	            if (*fp == '%') {
	                rs = b.chr('%') ;
	            } else {
			kb[0] = *fp ;
	                if (cc *cp ; (rs = vf(nsp,kb,vs,&cp)) >= 0) {
	                    if (cc *tp ; (tp = strchr(cp,'=')) != np) {
	                        rs = b.strw((tp + 1)) ;
	                    } /* end if (it had a value) */
	                } else if (*fp == 'f') {
	                    rs = b.strw(fn) ;
	                } else {
	                    rs = SR_NOTFOUND ;
			}
	            } /* end if (tried to expand a key) */
	        } else {
	            rs = b.chr(*fp) ;
	        } /* end if (escape or regular character) */
	    } /* end for */
	    len = b.finish ;
	    if (rs >= 0) rs = len ;
	} /* end if (sbuf) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (schedexpand) */


