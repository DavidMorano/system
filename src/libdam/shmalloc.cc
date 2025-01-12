/* shmalloc SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Shared-Memory-Allocation management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name
	shmalloc

	Description:
	This object manages shared-memory allocation in a shared-memory
	segment.

	Synopsis:
	int shmalloc_init(shmalloc *op,char *strp,int sz) noex

	Arguments:
	- op		object pointer
	- strp		pointer to string table (free-memory area)
	- sz		size of free-memory area

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<intceil.h>
#include	<intfloor.h>
#include	<localmisc.h>

#include	"shmalloc.h"


/* local defines */

#define	SHMALLOC_VER	1		/* implementation version */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int shmalloc_init(shmalloc *op,char *strp,int ssz) noex {
	int		rs = SR_FAULT ;
	if (op && strp) {
	    ulong		strv = (ulong) strp ;
	    cint		asz = SHMALLOC_ALIGNSIZE ;
	    char		*rp = charp(op) ;
	    op->used = 0 ;
	    if ((strv&(asz-1)) != 0) {
	        cint	diff = (strv - (strv & (asz - 1))) ;
	        strp += diff ;
	        ssz -= diff ;
	    }
	    if (int flsz ; (flsz = ifloor(ssz,asz)) >= 0) {
	        op->str = (strp - rp) ;
	        op->b.bsz = flsz ;
	        op->b.next = 0 ;
	        {
	            shmalloc_blk	*fsp = (shmalloc_blk *) strp ;
	            fsp->bsz = flsz ;
	            fsp->next = -1 ;
	        }
	    } else {
	        rs = SR_INVALID ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (shmalloc_init) */

int shmalloc_fini(shmalloc *op) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (shmalloc_finish) */

/* returns <0 (SR_NOMEM) on NOMEM */
int shmalloc_alloc(shmalloc *op,int rsz) noex {
    	int		rs = SR_FAULT ;
	int		roff = 0 ;
	if (op) {
	    char	*rp = charp(op) ;
	    rs = SR_INVALID ;
	    if (rsz > 0) {
	        shmalloc_blk	*psp = &op->b ;
	        shmalloc_blk	*fsp ;
	        shmalloc_blk	*nsp ;
	        cint		asz = SHMALLOC_ALIGNSIZE ; /* w/ overhead */
	        int		bsz = iceil(rsz,SHMALLOC_ALIGNSIZE) ;
	        int		next ;
	        char		*strp = charp(rp + op->str) ;
	        rs = SR_NOMEM ;
	        bsz += asz ; /* add the amount for our overhead */
	        next = psp->next ;
	        while ((next >= 0) && (next < op->b.bsz)) {
	            fsp = (shmalloc_blk *) (strp+next) ;
	            if (fsp->bsz >= bsz) {
			rs = SR_OK ;
	                roff = (next+asz) ;
	                if ((fsp->bsz - bsz) >= SHMALLOC_BSZ) {
	                    nsp = (shmalloc_blk *) (strp+next+bsz) ;
	                    nsp->bsz = (fsp->bsz-bsz) ;
	                    nsp->next = fsp->next ;
	                    psp->next = (next+bsz) ; /* update previous */
	                    fsp->bsz = bsz ;		/* update ourself */
	                    fsp->next = -1 ;
	                } else {
	                    psp->next = fsp->next ;
	                    fsp->next = -1 ;
	                } /* end if */
	                op->used += fsp->bsz ;
	                break ;
	            } else {
	                psp = fsp ;
	                next = fsp->next ;
	            } /* end if */
	        } /* end while (searching for free block) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? roff : rs ;
}
/* end subroutine (shmalloc_alloc) */

int shmalloc_free(shmalloc *op,int uoff) noex {
	cint		asz = SHMALLOC_ALIGNSIZE ;
	int		rs = SR_FAULT ;
	int		rused = 0 ;
	if (op) {
	    char	*rp = charp(op) ;
	    rs = SR_INVALID ;
	    if (uoff >= asz) {
	        shmalloc_blk	*psp = &op->b ;
	        shmalloc_blk	*fsp ;
	        shmalloc_blk	*csp ;
	        shmalloc_blk	*nsp ;
	        int		fsz ;
	        int		foff ;
	        int		comoff ;
	        char		*strp = charp(rp + op->str) ;
	        foff = (uoff-asz) ;
	        fsp = (shmalloc_blk *) (strp+foff) ;
	        comoff = psp->next ;
	        fsz = fsp->bsz ;
	        if (fsz <= op->used) {
	            while ((comoff >= 0) && (comoff < op->b.bsz)) {
	                csp = (shmalloc_blk *) (strp+comoff) ;
	                if (foff < comoff) {
	                    if ((foff+fsp->bsz) == comoff) { /* merge forward */
	                        psp->next = foff ;
	                        fsp->bsz += csp->bsz ;
	                        fsp->next = csp->next ;
	                    } else { /* free alone */
	                        psp->next = foff ;
	                        fsp->next = comoff ;
	                    }
	                    op->used -= fsz ;
	                    foff = -1 ;
	                } else if (foff == (comoff+csp->bsz)) { /* backward */
	                    csp->bsz += fsp->bsz ;
	                    if ((comoff+csp->bsz) == csp->next) { /* forward */
	                        nsp = (shmalloc_blk *) (strp+csp->next) ;
	                        csp->bsz += nsp->bsz ;
	                        csp->next = nsp->next ;
	                    }
	                    op->used -= fsz ;
	                    foff = -1 ;
	                } /* end if */
	                if (foff >= 0) {
	                    psp = csp ;
	                    comoff = csp->next ;
	                }
	                if (foff < 0) break ;
	            } /* end while */
	            if (foff >= 0) {
	                psp->next = foff ;
	                op->used -= fsz ;
	            }
	        } else {
	            rs = SR_NOMSG ;
	        }
		rused = op->used ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rused : rs ;
}
/* end subroutine (shmalloc_free) */

int shmalloc_already(shmalloc *op,int uoff) noex {
	cint		asz = SHMALLOC_ALIGNSIZE ;
	int		rs = SR_FAULT ;
	int		f = false ;
	if (op) {
	    char	*rp = charp(op) ;
	    rs = SR_INVALID ;
	    if (uoff >= asz) {
	        shmalloc_blk	*psp = &op->b ;
	        shmalloc_blk	*fsp ;
	        shmalloc_blk	*csp ;
	        int		foff ;
	        int		fsz ;
	        int		comoff ;
	        char		*strp = charp(rp + op->str) ;
	        foff = (uoff-asz) ;
	        fsp = (shmalloc_blk *) (strp+foff) ;
	        fsz = fsp->bsz ;
	        comoff = psp->next ;
	        if (comoff < fsz) {
	            while ((comoff >= 0) && (comoff < op->b.bsz)) {
	                csp = (shmalloc_blk *) (strp+comoff) ;
	                f = ((foff >= comoff) && (foff < (comoff+csp->bsz))) ;
	                if (f) break ;
	                comoff = csp->next ;
	            } /* end while */
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (shmalloc_already) */

int shmalloc_used(shmalloc *op) noex {
	return op->used ;
}
/* end subroutine (shmalloc_used) */

int shmalloc_avail(shmalloc *op) noex {
	int		rs = SR_FAULT ;
	int		avail = 0 ;
	char		*rp = charp(op) ;
	if (op) {
	    shmalloc_blk	*psp = &op->b ;
	    shmalloc_blk	*csp ;
	    int		comoff ;
	    int		comsz ;
	    char	*strp = charp(rp + op->str) ;
	    comoff = psp->next ;
	    while (comoff >= 0) {
	        csp = (shmalloc_blk *) (strp+comoff) ;
	        comsz = csp->bsz ;
	        avail += comsz ;
	        comoff = csp->next ;
	        psp = csp ;
	    } /* end while */
	} /* end if (non-null) */
	return (rs >= 0) ? avail : rs ;
}
/* end subroutine (shmalloc_avail) */

int shmalloc_audit(shmalloc *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = shmalloc_avail(op)) >= 0) {
	        if ((op->used + rs) != op->b.bsz) {
		    rs = SR_BADFMT ;
	        }
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (shmalloc_audit) */


