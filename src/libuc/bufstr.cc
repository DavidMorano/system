/* bufstr SUPPORT */
/* lang=C++20 */

/* string buffer object */
/* version %I% last-modified %G% */

#define	CF_FASTGROW	1		/* grow faster */

/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module can be used to construct strings or messages
	in buffers WITHOUT using the |snsprint(3c)| subroutine.  This
	module is useful when the user does NOT supply a buffer to
	be used as the working store.  Rather, an internally,
	dynamically grown and managed buffer is maintained.  This
	module uses an object, that must be initialized and eventually
	freed, to track the state of the dynamically used internal
	buffer.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"bufstr.h"


/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	bufstr_extend(bufstr *,int,char **) noex ;


/* exported variables */


/* exported subroutines */

int bufstr_start(bufstr *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->len = 0 ;
	    op->dlen = 0 ;
	    op->dbuf = nullptr ;
	    op->sbuf[0] = '\0' ;
	}
	return rs ;
}
/* end subroutine (bufstr_start) */

int bufstr_finish(bufstr *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->dbuf) {
	        rs1 = uc_free(op->dbuf) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dbuf = nullptr ;
	    }
	    if (rs >= 0) rs = op->len ;
	    op->len = 0 ;
	    op->dlen = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bufstr_finish) */

int bufstr_char(bufstr *op,int ch) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    char	buf[2] = { char(ch) } ;
	    rs = bufstr_strw(op,buf,1) ;
	}
	return rs ;
}
/* end subroutine (bufstr_char) */

int bufstr_strw(bufstr *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op) {
	    char	*bp ;
	    if (sl < 0) sl = strlen(sp) ;
	    if ((rs = bufstr_extend(op,sl,&bp)) >= 0) {
	        strwcpy(bp,sp,sl) ;
	        op->len += sl ;
	        len = op->len ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (bufstr_strw) */

int bufstr_buf(bufstr *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op) {
	    char	*bp ;
	    if (sl < 0) sl = strlen(sp) ;
	    if ((rs = bufstr_extend(op,sl,&bp)) >= 0) {
	        memcpy(bp,sp,sl) ;
	        op->len += sl ;
	        len = op->len ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (bufstr_buf) */

int bufstr_get(bufstr *op,cchar **spp) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op) {
	    rs = SR_OK ;
	    if (spp) {
	        *spp = (op->dbuf) ? op->dbuf : op->sbuf ;
	    }
	    len = op->len ;
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (bufstr_get) */


/* private subroutines */

static int bufstr_extend(bufstr *op,int nlen,char **rpp) noex {
	cint		slen = BUFSTR_LEN ;
	int		rs = SR_OK ;
	int		dlen ;
	int		rlen ;
	char		*dp ;
	if (op->dbuf) {
	    rlen = (slen-op->len) ;
	    if (nlen > rlen) {
	    	dlen = max((slen + nlen),(slen * 2)) ;
		if ((rs = uc_malloc((dlen+1),&dp)) >= 0) {
		    op->dlen = dlen ;
		    op->dbuf = dp ;
		    dp = strwcpy(dp,op->sbuf,op->len) ;
		}
	    } else {
		dp = (op->sbuf+op->len) ;
	    }
	} else {
	    rlen = (op->dlen-op->len) ;
	    if (nlen > rlen) {
		dlen = (op->dlen+nlen+slen) ;
		if ((rs = uc_realloc(op->dbuf,(dlen+1),&dp)) >= 0) {
		    op->dbuf = dp ;
		    op->dlen = dlen ;
		}
	    } else {
		dp = (op->dbuf+op->len) ;
	    }
	} /* end if (extension needed) */
	if (rpp) {
	    *rpp = (rs >= 0) ? dp : nullptr ;
	}
	return rs ;
}
/* end subroutine (bufstr_extend) */


