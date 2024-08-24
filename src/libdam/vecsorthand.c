/* vecsorthand */

/* vector of sorted handles */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 2010-12-01,  David A.D. Morano
	Module was originally written.

	= 2019-01-17, David A.D. Morano
	I added a new method |vecsorthand_delall(3dam)|. A use for this
	has come up.

*/

/* Copyright � 2010,2019 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	This container object is used when the caller wants to store an item by
	its handle (pointer) alone in a sorted list, but always in sorted 
	order. This object (unlike some other vector-hand objects that have 
	sorted behavior) strictly maintains the sort on each addition.
	Additions are made in O(log-N) time, and searching is also O(log-N)
	time.


*******************************************************************************/


#define	VECSORTHAND_MASTER	1


#include	<envstandards.h>
#include	<sys/types.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<localmisc.h>
#include	"vecsorthand.h"


/* local defines */

#define	VECSORTHAND_DEFENTS	10


/* external subroutines */


/* forward references */

static int	vecsorthand_extend(vecsorthand *) ;

static int	defcmpfunc(const void *,const void *) ;


/* exported subroutines */

int vecsorthand_start(vecsorthand *op,int n,vecentcmp_t cmpfunc) noex {
	int		rs  = SR_FAULT ;
	int		size = 0 ;
	void		*p ;
	if (op != NULL) {
	    if (n <= 1) n = VECSORTHAND_DEFENTS ;
	    memset(op,0,sizeof(vecsorthand)) ;
	    op->i = 0 ;
	    op->c = 0 ;
	    size += (sizeof(void **) * (n+1)) ;
	    if ((rs = uc_libmalloc(size,&p)) >= 0) {
	        op->va = p ;
	        op->e = n ;
	        {
	            op->va[0] = NULL ;
	            op->cmpfunc = (cmpfunc != NULL) ? cmpfunc : defcmpfunc ;
	        }
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecsorthand_start) */

int vecsorthand_finish(vecsorthand *op) noex {
	int	rs = SR_FAULT ;
	if (op != NULL) {
	    rs = SR_OK ;
	    if (op->va != NULL) {
	        uc_libfree(op->va) ;
	        op->va = NULL ;
	    }
	    op->i = 0 ;
	    op->e = 0 ;
	} /* end if (non-null) */
	return rs;
}
/* end subroutine (vecsorthand_finish) */

int vecsorthand_add(vecsorthand *op,const void *buf) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
#if	CF_DEBUGS
	debugprintf("vecsorthand_add: ent\n") ;
#endif
	if ((op != NULL) && (buf == NULL)) {
#if	CF_DEBUGS
	debugprintf("vecsorthand_add: i=%d\n",op->i) ;
#endif
	if ((rs = vecsorthand_extend(op)) >= 0) {
	    int		bot = 0 ;
	    int		top = MAX((op->i - 1),0) ;
	    int		j ;
/* find the place in the existing list where this new item should be added */
	    i = (bot + top) / 2 ;
#if	CF_DEBUGS
	    debugprintf("vecsorthand_add: bot=%d top=%d i=%d\n",bot,top,i) ;
#endif
	    while ((top - bot) > 0) {
	        if ((rs = (*op->cmpfunc)(&buf,(op->va+i))) < 0) {
	            top = i - 1 ;
	        } else if (rs > 0) {
	            bot = i + 1 ;
	        } else
	            break ;
	        i = (bot + top) / 2 ;
	    } /* end while */
#if	CF_DEBUGS
	    debugprintf("vecsorthand_add: found bot=%d top=%d i=%d\n",
		bot,top,i) ;
#endif
	    if (i < op->i) {
	        if ((*op->cmpfunc)(&buf,&op->va[i]) > 0) {
	            i += 1 ;
	        }
/* move all entries from "i" through "op->i - 1" up one */
	        for (j = (op->i - 1) ; j >= i ; j -= 1) {
	            (op->va)[j + 1] = (op->va)[j] ;
	        }
	    } /* end if */
/* load the new entry */
	    (op->va)[i] = buf ;
	    op->i += 1 ;
	    (op->va)[op->i] = NULL ;
	    op->c += 1 ;			/* increment list count */
	} /* end if (vecsorthand_extend) */
	} /* end if (non-null) */
#if	CF_DEBUGS
	debugprintf("vecsorthand_add: ret rs=%d i=%u\n",rs,i) ;
#endif
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecsorthand_add) */

int vecsorthand_get(vecsorthand *op,int i,void *vp) noex {
	int		rs = SR_FAULT ;
#if	CF_DEBUGS
	debugprintf("vecsorthand_get: ent i=%d\n",i) ;
	debugprintf("vecsorthand_get: ii=%d\n",op->i) ;
#endif
	if (op != NULL) {
	    rs = SR_NOTFOUND ;
	    if (op->va != NULL) {
		const void	*rval = NULL ;
	        if ((i >= 0) && (i < op->i)) {
	            rval = (op->va)[i] ;
	            rs = i ;
	        }
	        if (vp != NULL) {
	            const void	**rpp = (const void **) vp ;
	            *rpp = rval ;
	        }
	    } /* end if (non-null) */
	} /* end if (non-null) */
#if	CF_DEBUGS
	debugprintf("vecsorthand_get: ret rs=%d\n",rs) ;
#endif
	return rs ;
}
/* end subroutine (vecsorthand_get) */

int vecsorthand_del(vecsorthand *op,int i) noex {
	int		rs = SR_NOTFOUND ;

	if (op == NULL) return SR_FAULT ;

	if (op->va == NULL) return SR_NOTFOUND ;

	if ((i >= 0) && (i < op->i)) {
	    int		j ;
	    op->i -= 1 ;
	    for (j = i ; j < op->i ; j += 1) {
	        (op->va)[j] = (op->va)[j + 1] ;
	    }
	    (op->va)[op->i] = NULL ;
	    op->c -= 1 ;		/* decrement list count */
	    rs = op->c ;
	} /* end if */

	return rs ;
}
/* end subroutine (vecsorthand_del) */

int vecsorthand_delall(vecsorthand *op) noex {
	int		rs = SR_OK ;

	if (op == NULL) return SR_FAULT ;

	if (op->va != NULL) {
	    rs = op->c ;
	    if (op->i > 0) {
		op->va[0] = NULL ;
	        op->i = 0 ;
	        op->c = 0 ;
	    }
	}

	return rs ;
}
/* end subroutine (vecsorthand_delall) */

int vecsorthand_delhand(vecsorthand *op,const void *ep) noex {
	const int	n = op->i ;
	int		rs = SR_NOTFOUND ;
	int		i ;
	int		f = FALSE ;
	for (i = 0 ; i < n ; i += 1) {
	    f = ((op->va)[i] == ep) ;
	    if (f) break ;
	} /* end for */
	if (f) {
	    rs = vecsorthand_del(op,i) ;
	}
	return rs ;
}
/* end subroutine (vecsorthand_delhand) */

int vecsorthand_count(vecsorthand *op) noex {
{
	if (op == NULL) return SR_FAULT ;
	return op->c ;
}
/* end subroutine (vecsorthand_count) */

int vecsorthand_search(vecsorthand *op,const void *ep,void *vp) noex {
	const int	esize = sizeof(void *) ;
	int		rs = SR_NOTFOUND ;
	int		i ;
	const void	**rpp2 ;

	if (op == NULL) return SR_FAULT ;
	if (ep == NULL) return SR_FAULT ;

	if (op->va == NULL) return SR_NOTFOUND ;

	rpp2 = (const void **) bsearch(&ep,op->va,op->i,esize,op->cmpfunc) ;

	if (rpp2 != NULL) {
	    i = (rpp2 - op->va) ;
	    rs = SR_OK ;
	}

	if (vp != NULL) {
	    const void	**rpp = (const void **) vp ;
	    *rpp = ((rs >= 0) ? op->va[i] : NULL) ;
	}

	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecsorthand_search) */


/* private subroutines */

static int vecsorthand_extend(vecsorthand *op) noex {
{
	int		rs = SR_OK ;
	if ((op->i + 1) > op->e) {
	    const int	ndef = VECSORTHAND_DEFENTS ;
	    int		size ;
	    int		ne ;
	    const void	**np = NULL ;
	    if (op->e == 0) {
	        ne = ndef ;
	        size = (ndef*sizeof(char **)) ;
	        rs = uc_libmalloc(size,&np) ;
	    } else {
	        const int	ne = (op->e * 2) ;
	        size = (ne*sizeof(cchar **)) ;
	        rs = uc_librealloc(op->va,size,&np) ;
	    }
	    if (rs >= 0) {
	        op->va = np ;
	        op->e = ne ;
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (vecsorthand_extend) */

static int defcmpfunc(const void *app,const void *bpp) noex {
	cchar	**e1pp = (cchar **) app ;
	cchar	**e2pp = (cchar **) bpp ;
	return strcmp(*e1pp,*e2pp) ;
}
/* end subroutine (defcmpfunc) */


