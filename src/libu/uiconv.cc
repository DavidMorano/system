/* uiconv SUPPORT */
/* encoding=ISO8859-1 */
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
#include	<iconv.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<intsat.h>
#include	<localmisc.h>

#include	"uiconv.h"


/* local defines */

#define	UICONV_TOMEM	(1*60)


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef iconv_t		ic_t ;
typedef iconv_t	*	iconvp ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int uiconv_ctor(uiconv *op,Args ... args) noex {
	UICONV		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uiconv_ctor) */

static int uiconv_dtor(uiconv *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uiconv_dtor) */

template<typename ... Args>
static inline int uiconv_magic(uiconv *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == UICONV_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (uiconv_magic) */

static int uiconv_libopen(uiconv *,cchar *,cchar *) noex ;
static int uiconv_libclose(uiconv *) noex ;


/* local variables */

static const iconv_t	iconvbad = cast_reinterpret<iconv_t>(-1) ;

constexpr size_t	szbad = size_t(-1) ;


/* exported variables */


/* exported subroutines */

int uiconv_open(uiconv *op,cchar *tsp,cchar *fsp) noex {
    	cnullptr	np{} ;
	int		rs ;
	if ((rs = uiconv_ctor(op,tsp,fsp)) >= 0) {
	    rs = SR_INVALID ;
	    if (tsp[0] && fsp[0]) {
		rs = SR_NOMEM ;
	        if (ic_t *cdp ; (cdp = new(nothrow) iconv_t) != np) {
		    op->cdp = cdp ;
	            if ((rs = uiconv_libopen(op,tsp,fsp)) >= 0) {
	                op->magic = UICONV_MAGIC ;
	            }
	            if (rs < 0) {
			iconv_t	*cdp = iconvp(op->cdp) ;
		        delete cdp ;
		        op->cdp = nullptr ;
	            }
	        } /* end if (memory-allocation) */
	    } /* end if (valid) */
	    if (rs < 0) {
		uiconv_dtor(op) ;
	    }
	} /* end if (uiconv_ctor) */
	return rs ;
}
/* end subroutine (uiconv_open) */

int uiconv_close(uiconv *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = uiconv_magic(op)) >= 0) {
	    {
	        rs1 = uiconv_libclose(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->cdp) {
		iconv_t *cdp = iconvp(op->cdp) ;
		delete cdp ;
	        op->cdp = nullptr ;
	    }
	    {
		rs1 = uiconv_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (uiconv_close) */

int uiconv_trans(uiconv *op,cchar **ib,int *ilp,char **ob,int *olp) noex {
	int		rs ;
	if ((rs = uiconv_magic(op,ilp,olp)) >= 0) {
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
			rs = (- errno) ;
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
}
/* end subroutine (uiconv_trans) */


/* private subroutines */

static int uiconv_libopen(uiconv *op ,cchar *tsp,cchar *fsp) noex {
	int		rs = SR_BUGCHECK ;
	if (op->cdp) {
	    iconv_t	*cdp = iconvp(op->cdp) ;
	    int		to_mem = UICONV_TOMEM ;
	    bool	f_exit = false ;
	    repeat {
	        rs = SR_OK ;
		if ((*cdp = iconv_open(tsp,fsp)) == iconvbad) {
		    rs = (- errno) ;
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
}
/* end subroutine (uiconv_libopen) */

static int uiconv_libclose(uiconv *op) noex {
	iconv_t		*cdp = iconvp(op->cdp) ;
	int		rs ;
	repeat {
	    if ((rs = iconv_close(*cdp)) == -1) {
	        rs = (- errno) ;
	    }
	} until (rs != SR_INTR) ;
	return rs ;
}
/* end subroutine (uiconv_close) */


