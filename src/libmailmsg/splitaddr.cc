/* splitaddr HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* mail address management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A�D� Morano
	Module was originally written.

*/

/* Copyright � 1889 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	splitaddr

	Description:
	These object manages a mail splitaddr.

	Matching:
	We do "prefix" matching.

	list		candidate	match
	----------------------------------------------------

	b.a		a		N
	b.a		b.a		Y
	b.a		c.b.a		Y
	c.b.a		b.a		N
	c.b.a		d.b.a		N
	c.b.a		c.b.a		Y
	d@c.b.a		c.b.a		N
	d@c.b.a		y@c.b.a		N
	d@c.b.a		d@c.b.a		Y
	e@c.b.a		e@d.c.b.a	Y

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<strings.h>		/* for |strcasecmp(3c)| */
#include	<usystem.h>
#include	<vechand.h>
#include	<strn.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"mailaddr.h"
#include	"splitaddr.h"


/* local defines */

#define	SPLITADDR_DEFENTS	4


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */

template<typename ... Args>
static int splitaddr_ctor(splitaddr *op,Args ... args) noex {
	SPLITADDR	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->comp = new(nothrow) vechand) != np) {
		rs = SR_OK ;
	    } /* end if (new-vechand) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (splitaddr_ctor) */

static int splitaddr_dtor(splitaddr *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->comp) {
		delete op->comp ;
		op->comp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (splitaddr_dtor) */

template<typename ... Args>
static inline int splitaddr_magic(splitaddr *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == SPLITADDR_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (splitaddr_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int splitaddr_start(splitaddr *op,cchar *ap) noex {
	cint		nents = SPLITADDR_DEFENTS ;
	int		rs ;
	int		n = 0 ;
	if ((rs = splitaddr_ctor(op,ap)) >= 0) {
	    if ((rs = vechand_start(op->comp,nents,0)) >= 0) {
	        int	al = strlen(ap) ;
	        while (al && (ap[al-1] == '.')) {
		    al -= 1 ;
	        }
	        if (char *bp ; (rs = uc_malloc((al+1),&bp)) >= 0) {
	            int		bl = al ;
	            bool	f = false ;
	            cchar	*tp ;
	            op->mailaddr = charp(bp) ;
		    strwcpy(bp,ap,al) ;
	            while ((tp = strnrpbrk(bp,bl,".@")) != nullptr) {
	                f = (*tp == '@') ;
	                n += 1 ;
	                rs = vechand_add(op->comp,(tp+1)) ;
	                bl = (tp - bp) ;
	                bp[bl] = '\0' ;
	                if (f) break ;
	                if (rs < 0) break ;
	            } /* end while */
	            if ((rs >= 0) && (bp[0] != '\0')) {
	                if (! f) {
	                    n += 1 ;
	                    rs = vechand_add(op->comp,bp) ;
	                } else {
	                    op->local = bp ;
		        }
	            } /* end if */
	            if (rs >= 0) {
		        op->nd = n ;
			op->magic = SPLITADDR_MAGIC ;
		    }
		    if (rs < 0) {
			uc_free(bp) ;
			op->mailaddr = nullptr ;
		    }
	        } /* end if (memory-allocation) */
	        if (rs < 0) {
	            vechand_finish(op->comp) ;
	        }
	    } /* end if (vechand_start) */
	    if (rs < 0) {
		splitaddr_dtor(op) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (splitaddr_start) */

int splitaddr_finish(splitaddr *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if ((rs = splitaddr_magic(op)) >= 0) {
	    if (op->mailaddr) {
	        rs1 = uc_free(op->mailaddr) ;
	        if (rs >= 0) rs = rs1 ;
	        op->mailaddr = nullptr ;
	    }
	    if (op->comp) {
	        rs1 = vechand_finish(op->comp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = splitaddr_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (splitaddr_finish) */

int splitaddr_count(splitaddr *op) noex {
	int		rs ;
	if ((rs = splitaddr_magic(op)) >= 0) {
	    rs = vechand_count(op->comp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (splitaddr_count) */

int splitaddr_prematch(splitaddr *op,splitaddr *tp) noex {
	int		rs ;
	int		f = false ;
	if ((rs = splitaddr_magic(op,tp)) >= 0) {
	    int		rs1 ;
	    int		rs2 ;
	    int		f_so = false ;
	    for (int i = 0 ; true ; i += 1) {
	        void	*v1p{} ;
	        void	*v2p{} ;
		{
	            rs1 = vechand_get(op->comp,i,&v1p) ;
	            if (rs1 < 0) break ;
    		}
		{
	            f_so = false ;
	            rs2 = vechand_get(tp->comp,i,&v2p) ;
	            if (rs2 < 0) break ;
		}
	        f_so = true ;
	        cchar	*c1p = charp(v1p) ;
	        cchar	*c2p = charp(v2p) ;
	        f = (strcasecmp(c1p,c2p) == 0) ;
	        if (! f) break ;
	    } /* end for */
	    /* handle non-matching related errors first */
	    if ((rs1 < 0) && (rs1 != SR_NOTFOUND)) {
	        rs = rs1 ;
	    }
	    if ((rs >= 0) && (rs2 < 0) && (rs2 != SR_NOTFOUND)) {
	        rs = rs2 ;
	    }
    	    /* candidate entry must be as long or longer than the list entry */
	    if ((rs >= 0) && f) {
	        f = f_so ;
	    }
    	    /* candidate entry must have local-name match if it is local */
	    if ((rs >= 0) && f && (op->local != nullptr)) {
	        f = false ;
	        if (tp->local != nullptr) {
	            f = (strcmp(op->local,tp->local) == 0) ;
	        }
	    } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (splitaddr_prematch) */

int splitaddr_audit(splitaddr *op) noex {
	int		rs ;
	if ((rs = splitaddr_magic(op)) >= 0) {
	    rs = vechand_audit(op->comp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (splitaddr_audit) */


