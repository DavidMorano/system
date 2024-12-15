/* clientinfo SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* manage client information (a little bit) */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-06-23, David A­D­ Morano
	I updated this subroutine to just poll for machine status
	and write the Machine Status (MS) file.  This was a cheap
	excuse for not writing a whole new daemon program just to
	poll for machine status.  I hope this works out! :-)

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module (not really an object) manages some of the
	client data.  This data is stored in a structure 'clientinfo'.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<vecstr.h>
#include	<sockaddress.h>
#include	<connection.h>
#include	<localmisc.h>		/* |TIMEBUFLEN| */

#include	"clientinfo.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int clientinfo_ctor(clientinfo *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    clientinfo_head	*hop = static_cast<clientinfo_head *>(op) ;
	    cnullptr	np{} ;
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->sap = new(nothrow) sockaddress) != np) {
	        if ((op->nlp = new(nothrow) vecstr) != np) {
	            if ((op->slp = new(nothrow) vecstr) != np) {
			rs = SR_OK ;
	            } /* end if (new-vecstr) */
		    if (rs < 0) {
		        delete op->nlp ;
		        op->nlp = nullptr ;
		    }
	        } /* end if (new-vecstr) */
		if (rs < 0) {
		    delete op->sap ;
		    op->sap = nullptr ;
		}
	    } /* end if (new-sockaddress) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (clientinfo_ctor) */

static int clientinfo_dtor(clientinfo *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->slp) {
		delete op->slp ;
		op->slp = nullptr ;
	    }
	    if (op->nlp) {
		delete op->nlp ;
		op->nlp = nullptr ;
	    }
	    if (op->sap) {
		delete op->sap ;
		op->sap = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (clientinfo_dtor) */

static int	clientinfo_load(clientinfo *,cchar *,vecstr *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int clientinfo_start(clientinfo *cip) noex {
	int		rs ;
	if ((rs = clientinfo_ctor(cip)) >= 0) {
	    cip->nnames = -1 ;
	    cip->fd_input = -1 ;
	    cip->fd_output = -1 ;
	    rs = vecstr_start(cip->slp,1,0) ;
	    if (rs < 0) {
		clientinfo_dtor(cip) ;
	    }
	} /* end if (clientinfo_ctor) */
	return rs ;
}
/* end subroutine (clientinfo_start) */

int clientinfo_finish(clientinfo *cip) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (cip) {
	    rs = SR_OK ;
	    if (cip->fd_input >= 0) {
	        rs1 = u_close(cip->fd_input) ;
	        if (rs >= 0) rs = rs1 ;
	        cip->fd_input = -1 ;
	    }
	    if (cip->fd_output >= 0) {
	        rs1 = u_close(cip->fd_output) ;
	        if (rs >= 0) rs = rs1 ;
	        cip->fd_output = -1 ;
	    }
	    if (cip->nlp && (cip->nnames >= 0)) {
	        cip->nnames = 0 ;
	        rs1 = vecstr_finish(cip->nlp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (cip->slp) {
	        rs1 = vecstr_finish(cip->slp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = clientinfo_dtor(cip) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (clientinfo_finish) */

int clientinfo_loadnames(clientinfo *cip,cchar *dname) noex {
	int		rs = SR_FAULT ;
	if (cip && dname) {
	    rs = SR_INVALID ;
	    if (dname[0]) {
	        rs = SR_OK ;
	        if (cip->nnames < 0) {
	            cint	vo = VECSTR_OCOMPACT ;
	            cip->nnames = 0 ;
	            rs = vecstr_start(cip->nlp,5,vo) ;
	        }
	        if (rs >= 0) {
	            rs = clientinfo_load(cip,dname,cip->nlp) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (clientinfo_loadnames) */


/* local subroutines */

static int clientinfo_load(clientinfo *cip,cchar *dname,vecstr *nlp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (cip && nlp) {
	    char	*hnbuf{} ;
	    if ((rs = malloc_hn(&hnbuf)) >= 0) {
		cint		hnlen = rs ;
	        connection	conn, *cnp = &conn ;
	        if ((rs = connection_start(cnp,dname)) >= 0) {
	            if (cip->salen > 0) {
	                sockaddress	*sap = cip->sap ;
	                int		sal = cip->salen ;
	                rs1 = connection_peername(cnp,sap,sal,hnbuf,hnlen) ;
	            } else {
	                cint		ifd = cip->fd_input ;
	                rs1 = connection_sockremname(cnp,hnbuf,hnlen,ifd) ;
	            }
	            if (rs1 >= 0) {
	                rs1 = connection_mknames(&conn,nlp) ;
	                if (rs1 >= 0) {
		            c += rs1 ;
		        }
	            }
	            if (rs1 >= 0) {
	                rs1 = vecstr_adduniq(nlp,hnbuf,-1) ;
	                if ((rs1 >= 0) && (rs1 < INT_MAX)) {
		            c += 1 ;
		        }
	            }
	            rs1 = connection_finish(&conn) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (connection_start) */
		rs1 = uc_free(hnbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutines (clientinfo_load) */


