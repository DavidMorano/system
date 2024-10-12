/* stackaddr SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* stack-address management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	stackaddr

	Description:
	We manage stack-addresses.  We needed something like this
	because SBUF and BUFFER do not have the back-up facility
	that we need.

	Synopsis:
	stackaddr_start(stackaddr *op,char *abuf,int alen) noex

	Arguments:
	op		pointer to the STACKADDR object
	abuf		result buffer to stack-address
	alen		length of result buffer

	Returns:
	>=0		accummlated length
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<storebuf.h>
#include	<nulstr.h>
#include	<strwcmp.h>
#include	<localmisc.h>

#include	"stackaddr.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int stackaddr_start(stackaddr *op,char *dbuf,int dlen) noex {
	int		rs = SR_FAULT ;
	if (op && dbuf) {
	    stackaddr_head	*hp = static_cast<stackaddr_head *>(op) ;
	    dbuf[0] = '\0' ;
	    rs = memclear(hp) ;
	    op->dbuf = dbuf ;
	    op->dlen = dlen ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (stackaddr_start) */

int stackaddr_finish(stackaddr *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = op->i ;
	}
	return rs ;
}
/* end subroutine (stackaddr_finish) */

int stackaddr_add(stackaddr *op,cchar *hp,int hl,cchar *up,int ul) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (op && up) {
	    if ((rs = op->i) >= 0) {
		cnullptr	np{} ;
                if ((rs >= 0) && (op->i > 0)) {
                    op->i = op->ri ;
                }
                if ((rs >= 0) && (hp != np) && (hp[0] != '\0')) {
                    nulstr      h ;
                    cchar       *nhp ;
                    if (hl < 0) hl = strlen(hp) ;
                    if ((rs = nulstr_start(&h,hp,hl,&nhp)) >= 0) {
                        cchar   *lhp = op->lhp ;
                        int     lhl = op->lhl ;
                        if ((lhp == np) || (strwcmp(nhp,lhp,lhl) != 0)) {
                            if (rs >= 0) {
				cint	dl = op->dlen ;
				char	*dp = op->dbuf ;
                                op->lhp = (op->dbuf + op->i) ;
                                op->lhl = hl ;
                                rs = storebuf_strw(dp,dl,op->i,hp,hl) ;
                                op->i += rs ;
                                len += rs ;
                            }
                            if (rs >= 0) {
				cint	dl = op->dlen ;
				char	*dp = op->dbuf ;
                                rs = storebuf_chr(dp,dl,op->i,'!') ;
                                op->i += rs ;
                                len += rs ;
                                op->ri = op->i ;
                            }
                        } /* end if (a new different host) */
                        rs1 = nulstr_finish(&h) ;
                        if (rs >= 0) rs = rs1 ;
                    } /* end if (nulstr) */
                } /* end if (had a host) */
                if (rs >= 0) {
		    cint	dl = op->dlen ;
		    char	*dp = op->dbuf ;
                    rs = storebuf_strw(dp,dl,op->i,up,ul) ;
                    op->i += rs ;
                    len += rs ;
                }
                if (rs < 0) op->i = rs ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (stackaddr_add) */


