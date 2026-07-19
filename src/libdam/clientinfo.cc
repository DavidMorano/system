/* clientinfo SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* manage client information (a little bit) */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-06-23, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	clientinfo

	Description:
	This module (not really an object) manages some of the
	client data.  This data is stored in a structure 'clientinfo'.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/socket.h>		/* POSIX */
#include	<netinet/in.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<new>			/* C++STD |nothrow(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<sockaddress.h>		/* LIBUC */
#include	<connection.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU |TIMEBUFLEN| */

#include	"clientinfo.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */


/* imported namespaces */

using libuc::mem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int clientinfo_ctor(clientinfo *op,Args ... args) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    clientinfo_head	*hop = cast_static<clientinfo_head *>(op) ;
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->sap = new(nothrow) sockaddress) != np) ylikely {
	        if ((op->nlp = new(nothrow) vecstr) != np) ylikely {
	            if ((op->slp = new(nothrow) vecstr) != np) ylikely {
			rs = SR_OK ;
	            } /* end if (new-vecstr) */
		    if (rs < 0) {
		        delete op->nlp ;
		        op->nlp = nullptr ;
		    } /* end if (error) */
	        } /* end if (new-vecstr) */
		if (rs < 0) {
		    delete op->sap ;
		    op->sap = nullptr ;
		} /* end if (error) */
	    } /* end if (new-sockaddress) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (clientinfo_ctor) */

local int clientinfo_dtor(clientinfo *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
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
} /* end subroutine (clientinfo_dtor) */

local int	clientinfo_load(clientinfo *,cchar *,vecstr *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int clientinfo_start(clientinfo *cip) noex {
	int		rs ;
	if ((rs = clientinfo_ctor(cip)) >= 0) ylikely {
	    cip->nnames = -1 ;
	    cip->fd_input = -1 ;
	    cip->fd_output = -1 ;
	    rs = vecstr_start(cip->slp,1,0) ;
	    if (rs < 0) {
		clientinfo_dtor(cip) ;
	    } /* end if (error) */
	} /* end if (clientinfo_ctor) */
	return rs ;
} /* end subroutine (clientinfo_start) */

int clientinfo_finish(clientinfo *cip) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (cip) ylikely {
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
} /* end subroutine (clientinfo_finish) */

int clientinfo_loadnames(clientinfo *cip,cchar *dname) noex {
	int		rs = SR_FAULT ;
	if (cip && dname) ylikely {
	    rs = SR_INVALID ;
	    if (dname[0]) ylikely {
	        rs = SR_OK ;
	        if (cip->nnames < 0) {
	            cint	vo = vecstrm.compact ;
	            cip->nnames = 0 ;
	            rs = vecstr_start(cip->nlp,5,vo) ;
	        }
	        if (rs >= 0) {
	            rs = clientinfo_load(cip,dname,cip->nlp) ;
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (clientinfo_loadnames) */


/* local subroutines */

local int clientinfo_load(clientinfo *cip,cchar *dname,vecstr *nlp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (cip && nlp) ylikely {
	    if (char *hnbuf ; (rs = mem.hostname(&hnbuf)) >= 0) ylikely {
		cint		hnlen = rs ;
	        connection	conn, *cnp = &conn ;
	        if ((rs = connection_start(cnp,dname)) >= 0) ylikely {
	            if (cip->salen > 0) {
	                sockaddress	*sap = cip->sap ;
	                int		sal = cip->salen ;
	                rs1 = connection_peername(cnp,sap,sal,hnbuf,hnlen) ;
	            } else {
	                cint		ifd = cip->fd_input ;
	                rs1 = connection_sockremname(cnp,hnbuf,hnlen,ifd) ;
	            }
	            if (rs1 >= 0) ylikely {
	                rs1 = connection_mknames(&conn,nlp) ;
	                if (rs1 >= 0) {
		            c += rs1 ;
		        }
	            } /* end if (ok) */
	            if (rs1 >= 0) ylikely {
	                rs1 = vecstr_adduniq(nlp,hnbuf,-1) ;
	                if ((rs1 >= 0) && (rs1 < INT_MAX)) {
		            c += 1 ;
		        }
	            } /* end if (ok) */
	            rs1 = connection_finish(&conn) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (connection_start) */
		rs1 = mem.free(hnbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (clientinfo_load) */


