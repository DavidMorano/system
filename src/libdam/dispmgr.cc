/* dispmgr SUPPORT */
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
	dispmgr

	Description:
	This module implements the display related functions for a
	screen-oriented user-interface program.  This code is
	actually an API-independent front-end for one or more
	underlying terminal-display APIs.  The primary terminal-display
	API of the past used to be |curses(3)|  but that had way
	too many bugs for continued use.  The primary underlying
	terminal-display API is now |termdisp(3dam)|.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<termstr.h>		/* LIBUC */
#include	<uterm.h>		/* LIBUC */
#include	<termdisp.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"dispmgr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	DS		dispmgr
#define	DS_MAG		DISPMGR_MAGIC
/* mask for graphic renditions */
#define	DS_GRMASK	(DS_GRBOLD| DS_GRUNDER| DS_GRBLINK| DS_GRREV)
/* options */
#define	DS_GRBOLD	DISPMGR_GRBOLD
#define	DS_GRUNDER	DISPMGR_GRUNDER
#define	DS_GRBLINK	DISPMGR_GRBLINK
#define	DS_GRREV	DISPMGR_GRREV

#define	TD_GRBOLD	termdispgrm.bold
#define	TD_GRUNDER	termdispgrm.under
#define	TD_GRBLINK	termdispgrm.blink
#define	TD_GRREV	termdispgrm.rev


/* imported namespaces */

using libuc::mem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int dispmgr_ctor(DS *op,Args ... args) noex {
    	DS		*hop = op ;
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->tdp = new(nothrow) termdisp) != np) ylikely {
		rs = SR_OK ;
	    } /* end if (new-td) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (dispmgr_ctor) */

local int dispmgr_dtor(DS *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->tdp) ylikely {
		delete op->tdp ;
		op->tdp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (dispmgr_dtor) */

template<typename ... Args>
local inline int dispmgr_magic(DS *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == DS_MAG) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (dispmgr_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int dispmgr_start(DS *op,int tfd,cchar *termtype,int rows,int cols) noex {
	int		rs ;
	if ((rs = dispmgr_ctor(op,termtype)) >= 0) {
	    rs = SR_INVALID ;
	    if (termtype[0]) {
		rs = SR_BADFD ;
		if (tfd >= 0) {
		    auto &tt = termtype ;
	            op->tfd = tfd ;
	            if (cchar *cp ; (rs = mem.strw(tt,-1,&cp)) >= 0) {
	                op->termtype = cp ;
	                rs = termdisp_start(op->tdp,tfd,termtype,rows,cols) ;
	                if (rs >= 0) {
	                    op->magval = DS_MAG ;
	                }
	                if (rs < 0) {
	                    voidp vp = voidp(op->termtype) ;
	                    mem.free(vp) ;
	                    op->termtype = nullptr ;
	                } /* end if (error) */
	            } /* end if (memory-acquire) */
	        } /* end if (good-FD) */
	    } /* end if (valid) */
	    if (rs < 0) {
		dispmgr_dtor(op) ;
	    } /* end if (error) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (dispmgr_start) */

int dispmgr_finish(DS *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = dispmgr_magic(op)) >= 0) {
	    {
	        rs1 = termdisp_finish(op->tdp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->termtype) {
	        voidp vp = voidp(op->termtype) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->termtype = nullptr ;
	    } /* end if (memory-release) */
	    {
		rs1 = dispmgr_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (dispmgr_finish) */

int dispmgr_subnew(DS *op,int srow,int scol,int rows,int cols) noex {
	int		rs ;
	if ((rs = dispmgr_magic(op)) >= 0) {
	    rs = termdisp_subnew(op->tdp,srow,scol,rows,cols) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (dispmgr_subnew) */

int dispmgr_subdel(DS *op,int w) noex {
	int		rs ;
	if ((rs = dispmgr_magic(op)) >= 0) {
	    rs = termdisp_subdel(op->tdp,w) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (dispmgr_subdel) */

int dispmgr_getlines(DS *op,int w) noex {
	int		rs ;
	if ((rs = dispmgr_magic(op)) >= 0) {
	    rs = termdisp_getlines(op->tdp,w) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (dispmgr_getlines) */

int dispmgr_setlines(DS *op,int w,int nl) noex {
	int		rs ;
	if ((rs = dispmgr_magic(op)) >= 0) {
	    rs = termdisp_setlines(op->tdp,w,nl) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (dispmgr_setlines) */

int dispmgr_move(DS *op,int w,int r,int c) noex {
	int		rs ;
	if ((rs = dispmgr_magic(op)) >= 0) {
	    rs = termdisp_move(op->tdp,w,r,c) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (dispmgr_move) */

int dispmgr_printf(DS *op,int w,cchar *fmt,...) noex {
	va_list		ap ;
	int		rs ;
	if ((rs = dispmgr_magic(op,fmt)) >= 0) {
	    va_begin(ap,fmt) ;
	    rs = termdisp_vpprintf(op->tdp,w,-1,-1,fmt,ap) ;
	    va_end(ap) ;
	} /* end if (magic) */
	return rs ;
} /* end wubroutine (dispmgr_printf) */

int dispmgr_pprintf(DS *op,int w,int r,int c,cchar *fmt,...) noex {
	va_list		ap ;
	int		rs ;
	if ((rs = dispmgr_magic(op,fmt)) >= 0) {
	    va_begin(ap,fmt) ;
	    rs = termdisp_vpprintf(op->tdp,w,r,c,fmt,ap) ;
	    va_end(ap) ;
	} /* end if (magic) */
	return rs ;
} /* end wubroutine (dispmgr_pprintf) */

int dispmgr_vprintf(DS *op,int w,cchar *fmt,va_list ap) noex {
	int		rs ;
	if ((rs = dispmgr_magic(op,fmt,ap)) >= 0) {
	    rs = termdisp_vpprintf(op->tdp,w,-1,-1,fmt,ap) ;
	} /* end if (magic) */
	return rs ;
} /* end wubroutine (dispmgr_vprintf) */

int dispmgr_vpprintf(DS *op,int w,int r,int c,cchar *fmt,va_list ap) noex {
	int		rs ;
	if ((rs = dispmgr_magic(op,fmt,ap)) >= 0) {
	    rs = termdisp_vpprintf(op->tdp,w,r,c,fmt,ap) ;
	} /* end if (magic) */
	return rs ;
} /* end wubroutine (dispmgr_vpprintf) */

int dispmgr_write(DS *op,int w,cchar *bp,int bl) noex {
	int		rs ;
	if ((rs = dispmgr_magic(op,bp)) >= 0) {
	    rs = termdisp_pwrite(op->tdp,w,-1,-1,bp,bl) ;
	} /* end if (magic) */
	return rs ;
} /* end wubroutine (dispmgr_write) */

int dispmgr_pwrite(DS *op,int w,int r,int c,cchar *bp,int bl) noex {
	int		rs ;
	if ((rs = dispmgr_magic(op,bp)) >= 0) {
	    rs = termdisp_pwrite(op->tdp,w,r,c,bp,bl) ;
	} /* end if (magic) */
	return rs ;
} /* end wubroutine (dispmgr_pwrite) */

int dispmgr_pwritegr(DS *op,int w,int r,int c,int gr,cchar *bp,int bl) noex {
	int		rs ;
	if ((rs = dispmgr_magic(op,bp)) >= 0) {
	    int		tdgr = 0 ;
	    if (gr) {
	        if (gr & DS_GRBOLD)	tdgr |= TD_GRBOLD ;
	        if (gr & DS_GRUNDER)	tdgr |= TD_GRUNDER ;
	        if (gr & DS_GRBLINK)	tdgr |= TD_GRBLINK ;
	        if (gr & DS_GRREV)	tdgr |= TD_GRREV ;
	    }
	    rs = termdisp_pwritegr(op->tdp,w,r,c,tdgr,bp,bl) ;
	} /* end if (magic) */
	return rs ;
} /* end wubroutine (dispmgr_pwritegr) */

/* erase window; type: 0=forward, 1=back, 2=whole */
int dispmgr_ew(DS *op,int w,int r,int type) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = dispmgr_magic(op)) >= 0) {
	    rs = termdisp_ew(op->tdp,w,r,type) ;
	    len = rs ;
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
} /* end wubroutine (dispmgr_ew) */

int dispmgr_el(DS *op,int w,int type) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = dispmgr_magic(op)) >= 0) {
	    rs = termdisp_el(op->tdp,w,type) ;
	    len = rs ;
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
} /* end wubroutine (dispmgr_el) */

int dispmgr_ec(DS *op,int w,int n) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = dispmgr_magic(op)) >= 0) {
	    rs = termdisp_ec(op->tdp,w,n) ;
	    len = rs ;
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
} /* end wubroutine (dispmgr_ec) */

int dispmgr_scroll(DS *op,int w,int n) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = dispmgr_magic(op)) >= 0) {
	    rs = termdisp_scroll(op->tdp,w,n) ;
	    len = rs ;
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (dispmgr_scroll) */

int dispmgr_clear(DS *op,int w) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = dispmgr_magic(op)) >= 0) {
	    rs = termdisp_clear(op->tdp,w) ;
	    len = rs ;
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (dispmgr_clear) */

int dispmgr_flush(DS *op) noex {
	int		rs ;
	if ((rs = dispmgr_magic(op)) >= 0) {
	    rs = termdisp_flush(op->tdp) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (dispmgr_flush) */

/* suspend the display (optionally leaving the cursor someplace) */
int dispmgr_suspend(DS *op,int r,int c) noex {
	int		rs ;
	if ((rs = dispmgr_magic(op)) >= 0) {
	    if ((rs = dispmgr_flush(op)) >= 0) {
	        rs = termdisp_suspend(op->tdp,r,c) ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (dispmgr_suspend) */

int dispmgr_done(DS *op) noex {
	int		rs ;
	if ((rs = dispmgr_magic(op)) >= 0) {
	    if ((rs = dispmgr_pwrite(op,0,(op->rows-1),0,"\v",1)) >= 0) {
	        if ((rs = dispmgr_flush(op)) >= 0) {
	            rs = termdisp_suspend(op->tdp,-1,-1) ;
	        }
	    }
	}
	return rs ;
} /* end subroutine (dispmgr_done) */


