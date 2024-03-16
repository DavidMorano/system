/* uiconv SUPPORT */
/* lang=C++20 */

/* UNIX® international conversion */
/* version %I% last-modified %G% */


/* revision history:

	= 2014-07-15, David A­D­ Morano
	This is a new knock-off for the "standard" ICONV subroutines.

*/

/* Copyright © 2014 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We create here a more reasonable wrapper around the "standard"
	ICONV subroutines.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<iconv.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"uiconv.h"


/* local defines */

#define	UICONV_TOMEM	(1*60)


/* external subroutines */


/* local structures */


/* forward references */

static int uiconv_libopen(UICONV *,cchar *,cchar *) noex ;
static int uiconv_libclose(UICONV *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int uiconv_open(UICONV *op ,cchar *tsp,cchar *fsp) noex {
	cint		isize = sizeof(iconv_t) ;
	int		rs ;
	char		*p ;

	if (op == NULL) return SR_FAULT ;
	if (tsp == NULL) return SR_FAULT ;
	if (fsp == NULL) return SR_FAULT ;

	if (tsp[0] == '\0') return SR_INVALID ;
	if (fsp[0] == '\0') return SR_INVALID ;

	memclear(op) ;			/* dangerous */

	if ((rs = uc_malloc(isize,&p)) >= 0) {
	    op->cdp = p ;
	    if ((rs = uiconv_libopen(op,tsp,fsp)) >= 0) {
	        op->magic = UICONV_MAGIC ;
	    }
	    if (rs < 0) {
		uc_free(op->cdp) ;
		op->cdp = NULL ;
	    }
	} /* end if (memory-allocation) */

	return rs ;
}
/* end subroutine (uiconv_open) */

int uiconv_close(UICONV *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == NULL) return SR_FAULT ;
	if (op->magic != UICONV_MAGIC) return SR_NOTOPEN ;

	rs1 = uiconv_libclose(op) ;
	if (rs >= 0) rs = rs1 ;

	if (op->cdp != NULL) {
	    rs1 = uc_free(op->cdp) ;
	    if (rs >= 0) rs = rs1 ;
	    op->cdp = NULL ;
	}

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (uiconv_close) */

int uiconv_trans(UICONV *op,cchar **ib,int *ilp,char **ob,int *olp) noex {
	size_t		isize ;
	int		rs = SR_OK ;

	if (op == NULL) return SR_FAULT ;
	if (ilp == NULL) return SR_FAULT ;
	if (olp == NULL) return SR_FAULT ;

	if (op->magic != UICONV_MAGIC) return SR_NOTOPEN ;

	{
	    iconv_t	*cdp = (iconv_t *) op->cdp ;
	    size_t	ileft, *ileftp = &ileft ;
	    size_t	oleft, *oleftp = &oleft ;
	    {
		ileft = (size_t) *ilp ;
		oleft = (size_t) *olp ;
	    }
	    {
		char	**ibp = const_cast<char **>(ib) ;
	        isize = iconv(*cdp,ibp,ileftp,ob,oleftp) ;
	        if (isize == ((size_t)-1)) rs = (-errno) ;
	    }
	    {
		*olp = (int) oleft ;
		*ilp = (int) ileft ;
	    }
	} /* end block */

	if (rs >= 0) rs = (isize & INT_MAX) ;

	return rs ;
}
/* end subroutine (uiconv_trans) */


/* private subroutines */

static int uiconv_libopen(UICONV *op ,cchar *tsp,cchar *fsp) noex {
	iconv_t		cd ;
	int		rs ;
	int		to_mem = UICONV_TOMEM ;
	int		f_exit = FALSE ;

	if (op->cdp == NULL) return SR_BUGCHECK ;

	repeat {
	    rs = SR_OK ;
	    cd = iconv_open(tsp,fsp) ;
	    if (cd == ((iconv_t)-1)) rs = (-errno) ;
	    if (rs < 0) {
	        switch (rs) {
		case SR_INTR:
		    break ;
	        case SR_NOMEM:
		    if (to_mem-- > 0) {
		        msleep(1000) ;
		    } else {
		        f_exit = TRUE ;
		    }
		    break ;
		default:
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	if (rs >= 0) {
	    memcpy(op->cdp,&cd,sizeof(iconv_t)) ;
	}

	return rs ;
}
/* end subroutine (uiconv_libopen) */

static int uiconv_libclose(UICONV *op) noex {
	iconv_t		*cdp = (iconv_t *) op->cdp ;
	int		rs ;
	repeat {
	    rs = iconv_close(*cdp) ;
	    if (rs == -1) rs = (-errno) ;
	} until (rs != SR_INTR) ;
	return rs ;
}
/* end subroutine (uiconv_close) */


