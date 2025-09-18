/* strpack SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* string pack object */
/* version %I% last-modified %G% */

#define	CF_PREALLOC	0		/* pre-allocate a chunk */

/* revision history:

	= 2011-01-12, David A­D­ Morano
	This object module was morphed from some previous one. I
	do not remember what the previous one was.

*/

/* Copyright © 2011 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strpack

	Description:
	This object module creates and manages a string storage
	object.  This object is sort of a write-only storage area
	for storing strings.  Strings cannot be deleted from the
	object once they are added (something not commonly needed
	anyway).  Since most string storage requirements do NOT
	need the ability to remove strings once they are entered,
	this object provides a nice optimized manager for handling
	that sort of situation.

	This object is very similar to a STRTAB object (in that
	strings cannot be deleted) but is somewhat more optimized.

	Arguments:
	op		pointer to the strpack object
	<others>

	Returns:
	>=0		the total length of the filled up strpack so far!
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<new>
#include	<usystem.h>
#include	<vechand.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"strpack.h"

import libutil ;

/* local defines */

#define	STRPACK_CHSIZE	512

#ifndef	CF_PREALLOC
#define	CF_PREALLOC	0		/* pre-allocate a chunk */
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef strpack_ch *	chunkp ;


/* external subroutines */


/* local structures */


/* forward references */

template<typename ... Args>
static int strpack_ctor(strpack *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    op->chp = nullptr ;
	    op->magic = 0 ;
	    op->chsize = 0 ;
	    op->totalsize = 0 ;
	    op->c = 0 ;
	    if ((op->clp = new(nothrow) vechand) != np) {
		rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (strpack_ctor) */

static int strpack_dtor(strpack *op) noex {
	int		rs = SR_OK ;
	if (op->clp) {
	    delete op->clp ;
	    op->clp = nullptr ;
	}
	return rs ;
}
/* end subroutine (strpack_dtor) */

static int	strpack_chunknew(strpack *,int) noex ;
static int	strpack_chunkfins(strpack *) noex ;

static int	chunk_start(strpack_ch *,int) noex ;
static int	chunk_adv(strpack_ch *) noex ;
static int	chunk_finish(strpack_ch *) noex ;


/* local variables */

constexpr bool		f_prealloc = CF_PREALLOC ;


/* exported variables */


/* exported subroutines */

int strpack_start(strpack *op,int chsize) noex {
	int		rs ;
	if (chsize < STRPACK_CHSIZE) chsize = STRPACK_CHSIZE ;
	if ((rs = strpack_ctor(op)) >= 0) {
	    cint	vo = VECHAND_OORDERED ;
	    op->chsize = chsize ;
	    if ((rs = vechand_start(op->clp,0,vo)) >= 0) {
		if_constexpr (f_prealloc) {
	    	    rs = strpack_chunknew(op,0) ;
		}
	        if (rs >= 0) {
		    op->magic = STRPACK_MAGIC ;
	        }
	        if (rs < 0) {
		    vechand_finish(op->clp) ;
		}
	    } /* end if (vechand_start) */
	    if (rs < 0) {
		strpack_dtor(op) ;
	    }
	} /* end if (strpack_ctor) */
	return rs ;
}
/* end subroutine (strpack_start) */

int strpack_finish(strpack *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = strpack_magic(op)) >= 0) {
	    {
		rs1 = strpack_chunkfins(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = vechand_finish(op->clp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = strpack_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->chp = nullptr ;
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strpack_finish) */

int strpack_store(strpack *op,cchar *sp,int sl,cchar **rpp) noex {
	int		rs ;
	if ((rs = strpack_magic(op,sp)) >= 0) {
	    if (sl < 0) sl = lenstr(sp) ;
	    {
	        strpack_ch	*chp = op->chp ;
	        int		amount = (sl + 1) ;
	        if ((chp == nullptr) || (amount > (chp->csz - chp->i))) {
	            rs = strpack_chunknew(op,amount) ;
	            chp = op->chp ;
	        }
	        if (rs >= 0) { /* chunk-add */
	            char	*bp = (chp->cdata + chp->i) ;
	            strwcpy(bp,sp,sl) ;
	            chp->i += amount ;
	            chp->c += 1 ;		/* count in chunk */
	            op->c += 1 ;		/* count in object */
	            op->totalsize += amount ;
	            if (rpp) *rpp = (rs >= 0) ? bp : nullptr ;
	        }
	    } /* end blck */
	} /* end if (magic) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (strpack_store) */

int strpack_count(strpack *op) noex {
	int		rs  ;
	if ((rs = strpack_magic(op)) >= 0) {
	    rs = op->c ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strpack_count) */

int strpack_size(strpack *op) noex {
	int		rs ;
	if ((rs = strpack_magic(op)) >= 0) {
		rs = op->totalsize ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strpack_size) */


/* private subroutines */

static int strpack_chunknew(strpack *op,int amount) noex {
	cint		csz = szof(strpack_ch) ;
	int		rs ;
	if (op->chsize > amount) amount = op->chsize ;
	if (void *vp ; (rs = lm_mall(csz,&vp)) >= 0) {
	    strpack_ch	*cep = chunkp(vp) ;
	    if ((rs = chunk_start(cep,(amount + 1))) >= 0) {
	        if ((rs = vechand_add(op->clp,cep)) >= 0) {
	            op->chp = cep ;
	            if (op->totalsize == 0) {
	                chunk_adv(cep) ;
	                op->totalsize = 1 ;
	            }
		} /* end if (vechand) */
	        if (rs < 0) {
	            chunk_finish(cep) ;
		}
	    } /* end if (chunk) */
	    if (rs < 0) {
	        lm_free(cep) ;
	    }
	} /* end if (memory-allocation) */
	return rs ;
}
/* end subroutine (strpack_chunknew) */

static int strpack_chunkfins(strpack *op) noex {
	vechand		*clp = op->clp ;
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; vechand_get(clp,i,&vp) >= 0 ; i += 1) {
	    strpack_ch	*chp = chunkp(vp) ;
	    if (vp) {
		{
	            rs1 = chunk_finish(chp) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = lm_free(chp) ;
	            if (rs >= 0) rs = rs1 ;
		}
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (strpack_chunkfins) */

static int chunk_start(strpack_ch *cnp,int csz) noex {
	int		rs = SR_INVALID ;
	memclear(cnp) ;
	if (csz > 0) {
	    cnp->csz = csz ;
	    if (void *vp ; (rs = lm_mall(csz,&vp)) >= 0) {
	        cnp->cdata = charp(vp) ;
	    }
	}
	return rs ;
}
/* end subroutine (chunk_start) */

static int chunk_adv(strpack_ch *cnp) noex {
	int		rs = SR_FAULT ;
	if (cnp && cnp->cdata) {
	    rs = SR_OK ;
	    cnp->cdata[0] = '\0' ;
	    cnp->i += 1 ;
	}
	return rs ;
}
/* end subroutine (chunk_adv) */

static int chunk_finish(strpack_ch *cnp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (cnp->cdata) {
	    rs1 = lm_free(cnp->cdata) ;
	    if (rs >= 0) rs = rs1 ;
	    cnp->cdata = nullptr ;
	}
	cnp->csz = 0 ;
	cnp->i = 0 ;
	cnp->c = 0 ;
	return rs ;
}
/* end subroutine (chunk_finish) */


