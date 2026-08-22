/* uiconv SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® international conversion */
/* version %I% last-modified %G% */


/* revision history:

	= 2014-07-15, David A­D­ Morano
	This is a new knock-off for the "standard" ICONV subroutines.

*/

/* Copyright © 2014 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	uiconv

	Description:
	We create here a more reasonable wrapper around the "standard"
	ICONV subroutines.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<iconv.h>		/* ?? */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<intsat.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"uiconv.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	UICONV_TOMEM	(1*60)


/* imported namespaces */

using std::nothrow ;			/* constant */


/* local typedefs */

typedef iconv_t		ic_t ;
typedef iconv_t	*	iconvp ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int uiconv_ctor(uiconv *op,Args ... args) noex {
	UICONV		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uiconv_ctor) */

local int uiconv_dtor(uiconv *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uiconv_dtor) */

template<typename ... Args>
local inline int uiconv_magic(uiconv *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == UICONV_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (uiconv_magic) */

local int uiconv_libopen(uiconv *,cchar *,cchar *) noex ;
local int uiconv_libclose(uiconv *) noex ;


/* local variables */

local const iconv_t	iconvbad = cast_reinterpret<iconv_t>(-1) ;

constexpr size_t	szbad = size_t(-1) ;


/* exported variables */


/* exported subroutines */

int uiconv_open(uiconv *op,cchar *tsp,cchar *fsp) noex {
    	cnullptr	np{} ;
    	cnothrow	nt{} ;
	int		rs ;
	if ((rs = uiconv_ctor(op,tsp,fsp)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (tsp[0] && fsp[0]) ylikely {
		rs = SR_NOMEM ;
	        if (ic_t *cdp ; (cdp = new(nt) iconv_t) != np) ylikely {
		    op->cdp = cdp ;
	            if ((rs = uiconv_libopen(op,tsp,fsp)) >= 0) ylikely {
	                op->magval = UICONV_MAGIC ;
	            }
	            if (rs < 0) {
			cdp = iconvp(op->cdp) ;
		        delete cdp ;
		        op->cdp = nullptr ;
	            } /* end if (error - memory-release) */
	        } /* end if (memory-acquire) */
	    } /* end if (valid) */
	    if (rs < 0) {
		uiconv_dtor(op) ;
	    } /* end if (error) */
	} /* end if (uiconv_ctor) */
	return rs ;
} /* end subroutine (uiconv_open) */

int uiconv_close(uiconv *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = uiconv_magic(op)) >= 0) ylikely {
	    {
	        rs1 = uiconv_libclose(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->cdp) ylikely {
		iconv_t *cdp = iconvp(op->cdp) ;
		delete cdp ;
	        op->cdp = nullptr ;
	    } /* end if (memory-release) */
	    {
		rs1 = uiconv_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (uiconv_close) */

int uiconv_trans(uiconv *op,cchar **ib,int *ilp,char **ob,int *olp) noex {
	int		rs ;
	if ((rs = uiconv_magic(op,ilp,olp)) >= 0) ylikely {
	    size_t	isize ;
	    size_t	ileft = size_t(*ilp) ;
	    size_t	oleft = size_t(*olp) ;
	    {
	        iconv_t	*cdp = iconvp(op->cdp) ;
	        size_t	*ileftp = &ileft ;
	        size_t	*oleftp = &oleft ;
	        {
		    char	**ibp = cast_const<char **>(ib) ;
	            if ((isize = iconv(*cdp,ibp,ileftp,ob,oleftp)) == szbad) {
			rs = (neg errno) ;
		    }
	        }
	        {
		    *olp = (int) oleft ;
		    *ilp = (int) ileft ;
	        }
	    } /* end block */
	    if (rs >= 0) {
	        rs = intsat(isize) ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (uiconv_trans) */


/* private subroutines */

local int uiconv_libopen(uiconv *op ,cchar *tsp,cchar *fsp) noex {
	int		rs = SR_BUGCHECK ;
	if (op->cdp) ylikely {
	    iconv_t	*cdp = iconvp(op->cdp) ;
	    int		to_mem = UICONV_TOMEM ;
	    bool	f_exit = false ;
	    repeat {
	        rs = SR_OK ;
		if ((*cdp = iconv_open(tsp,fsp)) == iconvbad) {
		    rs = (neg errno) ;
		}
	        if (rs < 0) {
	            switch (rs) {
		    case SR_INTR:
		        break ;
	            case SR_NOMEM:
		        if (to_mem-- > 0) {
		            msleep(1000) ;
		        } else {
		            f_exit = true ;
		        }
		        break ;
		    default:
		        f_exit = true ;
		        break ;
	            } /* end switch */
	        } /* end if (error) */
	    } until ((rs >= 0) || f_exit) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uiconv_libopen) */

local int uiconv_libclose(uiconv *op) noex {
	iconv_t		*cdp = iconvp(op->cdp) ;
	int		rs ;
	repeat {
	    if ((rs = iconv_close(*cdp)) == -1) {
	        rs = (neg errno) ;
	    }
	} until (rs != SR_INTR) ;
	return rs ;
} /* end subroutine (uiconv_close) */


