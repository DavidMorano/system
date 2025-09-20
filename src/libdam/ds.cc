/* ds SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* low-level terminal-display manager */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-01-20, David A­D­ Morano
	This is a complete rewrite of the trash that performed this
	function previously.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	ds

	Description:
	This module implements the display related functions for a
	screen-oriented user-interface program.  This code is
	actually an API-independent front-end for one or more
	underlying terminal-display APIs.  The primary terminal-display
	API of the past used to be |curses(3)|  but that had way
	too many bugs for continued use.  The primary underlying
	terminal-display API is now |td(3dam)|.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>
#include	<cstring>
#include	<usystem.h>
#include	<termstr.h>
#include	<ascii.h>
#include	<uterm.h>
#include	<td.h>
#include	<localmisc.h>

#include	"ds.h"

import libutil ;			/* |memclear(3u)| */

/* local defines */

/* mask for graphic renditions */
#define	DS_GRMASK	(DS_GRBOLD| DS_GRUNDER| DS_GRBLINK| DS_GRREV)


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int ds_ctor(ds *op,Args ... args) noex {
    	DS		*hop = op ;
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->tdp = new(nothrow) td) != np) {
		rs = SR_OK ;
	    } /* end if (new-td) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ds_ctor) */

static int ds_dtor(ds *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->tdp) {
		delete op->tdp ;
		op->tdp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ds_dtor) */

template<typename ... Args>
static inline int ds_magic(ds *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == DS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (ds_magic) */

/* local variables */


/* exported variables */


/* exported subroutines */

int ds_start(DS *op,int tfd,cchar *termtype,int rows,int cols) noex {
	int		rs ;
	if ((rs = ds_ctor(op,termtype)) >= 0) {
	    rs = SR_INVALID ;
	    if (termtype[0]) {
		rs = SR_BADFD ;
		if (tfd >= 0) {
		    auto &tt = termtype ;
	            op->tfd = tfd ;
	            if (cchar *cp ; (rs = uc_mallocstrw(tt,-1,&cp)) >= 0) {
	                op->termtype = cp ;
	                rs = td_start(op->tdp,tfd,termtype,rows,cols) ;
	                if (rs >= 0) {
	                    op->magic = DS_MAGIC ;
	                }
	                if (rs < 0) {
	                    uc_free(op->termtype) ;
	                    op->termtype = NULL ;
	                }
	            } /* end if (m-a) */
	        } /* end if (good-FD) */
	    } /* end if (valid) */
	    if (rs < 0) {
		ds_dtor(op) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ds_start) */

int ds_finish(DS *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = ds_magic(op)) >= 0) {
	    {
	        rs1 = td_finish(op->tdp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->termtype) {
	        rs1 = uc_free(op->termtype) ;
	        if (rs >= 0) rs = rs1 ;
	        op->termtype = NULL ;
	    }
	    {
		rs1 = ds_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ds_finish) */

int ds_subnew(DS *op,int srow,int scol,int rows,int cols) noex {
	int		rs ;
	if ((rs = ds_magic(op)) >= 0) {
	    rs = td_subnew(op->tdp,srow,scol,rows,cols) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ds_subnew) */

int ds_subdel(DS *op,int w) noex {
	int		rs ;
	if ((rs = ds_magic(op)) >= 0) {
	    rs = td_subdel(op->tdp,w) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ds_subdel) */

int ds_getlines(DS *op,int w) noex {
	int		rs ;
	if ((rs = ds_magic(op)) >= 0) {
	    rs = td_getlines(op->tdp,w) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ds_getlines) */

int ds_setlines(DS *op,int w,int nl) noex {
	int		rs ;
	if ((rs = ds_magic(op)) >= 0) {
	    rs = td_setlines(op->tdp,w,nl) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ds_setlines) */

int ds_move(DS *op,int w,int r,int c) noex {
	int		rs ;
	if ((rs = ds_magic(op)) >= 0) {
	    rs = td_move(op->tdp,w,r,c) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ds_move) */

int ds_printf(DS *op,int w,cchar *fmt,...) noex {
	va_list		ap ;
	int		rs ;
	if ((rs = ds_magic(op,fmt)) >= 0) {
	    va_begin(ap,fmt) ;
	    rs = td_vpprintf(op->tdp,w,-1,-1,fmt,ap) ;
	    va_end(ap) ;
	} /* end if (magic) */
	return rs ;
}
/* end wubroutine (ds_printf) */

int ds_pprintf(DS *op,int w,int r,int c,cchar *fmt,...) noex {
	va_list		ap ;
	int		rs ;
	if ((rs = ds_magic(op,fmt)) >= 0) {
	    va_begin(ap,fmt) ;
	    rs = td_vpprintf(op->tdp,w,r,c,fmt,ap) ;
	    va_end(ap) ;
	} /* end if (magic) */
	return rs ;
}
/* end wubroutine (ds_pprintf) */

int ds_vprintf(DS *op,int w,cchar *fmt,va_list ap) noex {
	int		rs ;
	if ((rs = ds_magic(op,fmt,ap)) >= 0) {
	    rs = td_vpprintf(op->tdp,w,-1,-1,fmt,ap) ;
	} /* end if (magic) */
	return rs ;
}
/* end wubroutine (ds_vprintf) */

int ds_vpprintf(DS *op,int w,int r,int c,cchar *fmt,va_list ap) noex {
	int		rs ;
	if ((rs = ds_magic(op,fmt,ap)) >= 0) {
	    rs = td_vpprintf(op->tdp,w,r,c,fmt,ap) ;
	} /* end if (magic) */
	return rs ;
}
/* end wubroutine (ds_vpprintf) */

int ds_write(DS *op,int w,cchar *bp,int bl) noex {
	int		rs ;
	if ((rs = ds_magic(op,bp)) >= 0) {
	    rs = td_pwrite(op->tdp,w,-1,-1,bp,bl) ;
	} /* end if (magic) */
	return rs ;
}
/* end wubroutine (ds_write) */

int ds_pwrite(DS *op,int w,int r,int c,cchar *bp,int bl) noex {
	int		rs ;
	if ((rs = ds_magic(op,bp)) >= 0) {
	    rs = td_pwrite(op->tdp,w,r,c,bp,bl) ;
	} /* end if (magic) */
	return rs ;
}
/* end wubroutine (ds_pwrite) */

int ds_pwritegr(DS *op,int w,int r,int c,int gr,cchar *bp,int bl) noex {
	int		rs ;
	if ((rs = ds_magic(op,bp)) >= 0) {
	    int		tdgr = 0 ;
	    if (gr) {
	        if (gr & DS_GRBOLD) tdgr |= TD_GRBOLD ;
	        if (gr & DS_GRUNDER) tdgr |= TD_GRUNDER ;
	        if (gr & DS_GRBLINK) tdgr |= TD_GRBLINK ;
	        if (gr & DS_GRREV) tdgr |= TD_GRREV ;
	    }
	    rs = td_pwritegr(op->tdp,w,r,c,tdgr,bp,bl) ;
	} /* end if (magic) */
	return rs ;
}
/* end wubroutine (ds_pwritegr) */

/* erase window; type: 0=forward, 1=back, 2=whole */
int ds_ew(DS *op,int w,int r,int type) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = ds_magic(op)) >= 0) {
	    rs = td_ew(op->tdp,w,r,type) ;
	    len = rs ;
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end wubroutine (ds_ew) */

int ds_el(DS *op,int w,int type) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = ds_magic(op)) >= 0) {
	    rs = td_el(op->tdp,w,type) ;
	    len = rs ;
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end wubroutine (ds_el) */

int ds_ec(DS *op,int w,int n) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = ds_magic(op)) >= 0) {
	    rs = td_ec(op->tdp,w,n) ;
	    len = rs ;
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end wubroutine (ds_ec) */

int ds_scroll(DS *op,int w,int n) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = ds_magic(op)) >= 0) {
	    rs = td_scroll(op->tdp,w,n) ;
	    len = rs ;
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (ds_scroll) */

int ds_clear(DS *op,int w) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = ds_magic(op)) >= 0) {
	    rs = td_clear(op->tdp,w) ;
	    len = rs ;
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (ds_clear) */

int ds_flush(DS *op) noex {
	int		rs ;
	if ((rs = ds_magic(op)) >= 0) {
	    rs = td_flush(op->tdp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ds_flush) */

/* suspend the display (optionally leaving the cursor someplace) */
int ds_suspend(DS *op,int r,int c) noex {
	int		rs ;
	if ((rs = ds_magic(op)) >= 0) {
	    if ((rs = ds_flush(op)) >= 0) {
	        rs = td_suspend(op->tdp,r,c) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ds_suspend) */

int ds_done(DS *op) noex {
	int		rs ;
	if ((rs = ds_magic(op)) >= 0) {
	    if ((rs = ds_pwrite(op,0,(op->rows-1),0,"\v",1)) >= 0) {
	        if ((rs = ds_flush(op)) >= 0) {
	            rs = td_suspend(op->tdp,-1,-1) ;
	        }
	    }
	}
	return rs ;
}
/* end subroutine (ds_done) */


