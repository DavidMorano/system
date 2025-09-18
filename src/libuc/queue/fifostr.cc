/* fifostr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* FIFO string operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-12-09, David A­D­ Morano
	Originally written for Rightcore Network Services.

	= 2017-09-12, David A­D­ Morano
	Small interface change to |fifostr_entread()|.

*/

/* Copyright © 1999,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	fifostr

	Description:
	This object manages a FIFO of strings.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<usupport.h>		/* |libu::snwcpy(3u)| */
#include	<uclibmem.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"fifostr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */
using libu::snwcpy ;			/* subroutine */


/* local typedefs */

typedef fifostr_ent *	entp ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int fifostr_magic(fifostr *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == FIFOSTR_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (fifostr_magic) */

static int	fifostr_mat(fifostr *,fifostr_ent *) noex ;

#ifdef	COMMENT
static int	cmpdefault() ;
#endif


/* local variables */


/* exported variables */


/* exported subroutines */

int fifostr_start(fifostr *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->head = nullptr ;
	    op->tail = nullptr ;
	    op->ic = 0 ;
	    op->cnt = 0 ;
	    op->magic = FIFOSTR_MAGIC ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fifostr_start) */

int fifostr_finish(fifostr *op) noex {
	int		rs ;
	if ((rs = fifostr_magic(op)) >= 0) {
	    while ((rs = fifostr_curdel(op,nullptr)) >= 0) ;
	    if (rs == SR_NOTFOUND) rs = SR_OK ;
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fifostr_finish) */

int fifostr_add(fifostr *op,cchar *sp,int µsl) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = fifostr_magic(op,sp)) >= 0) {
	    if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
	        cint	sz = szof(fifostr_ent) + (sl + 1) ;
		if (void *vp ; (rs = libmem.mall(sz,&vp)) >= 0) {
	    	    fifostr_ent		*ep = entp(vp) ;
	            ep->slen = sl ;
	            {
	                char	*bp = cast_reinterpret<charp>(ep) ;
	                bp += szof(fifostr_ent) ;
	                strwcpy(bp,sp,sl) ;	/* <- ok: see 'sz' above */
	            }
	            ep->next = nullptr ;
	            ep->prev = nullptr ;
	            if (op->head == nullptr) {
	                op->head = ep ;
	                op->tail = ep ;
	            } else {
	                ep->prev = op->tail ;
	                (op->tail)->next = ep ;
	                op->tail = ep ;
	            } /* end if */
	            c = ++op->ic ;
	            op->cnt += sl ;
	        } /* end if (memory-allocation) */
	    } /* end block */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (fifostr_add) */

int fifostr_headread(fifostr *op,char *rbuf,int rlen) noex {
	int		rs ;
	int		sl = 0 ;
	if ((rs = fifostr_magic(op)) >= 0) {
	    rs = SR_NOTFOUND ;
            if (op->head) {
                fifostr_ent	*ep = op->head ;
                sl = ep->slen ;
                if (rbuf) {
                    cchar	*sp = cast_reinterpret<charp>(ep) ;
                    sp += szof(fifostr_ent) ;
                    rs = snwcpy(rbuf,rlen,sp,sl) ;
                }
            }
	} /* end if (magic) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (fifostr_headread) */

int fifostr_headlen(fifostr *op) noex {
	int		rs ;
	if ((rs = fifostr_magic(op)) >= 0) {
	    rs = SR_NOTFOUND ;
	    if (op->head) {
	        fifostr_ent	*ep = op->head ;
	        rs = ep->slen ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fifostr_headlen) */

int fifostr_entread(fifostr *op,char *rbuf,int rlen,int n) noex {
	int		rs ;
	int		sl = 0 ;
	if ((rs = fifostr_magic(op)) >= 0) {
            rs = SR_INVALID ;
            if (n >= 0) {
                rs = SR_NOTFOUND ;
                if (op->head) {
                    fifostr_ent     *ep = op->head ;
                    for (int i = 0 ; (i < n) && ep ; i += 1) {
                        ep = ep->next ;
                    }
                    if (ep) {
                        sl = ep->slen ;
                        if (rbuf) {
                            cchar   *sp = cast_reinterpret<charp>(ep) ;
                            sp += szof(fifostr_ent) ;
                            rs = snwcpy(rbuf,rlen,sp,sl) ;
                        } else {
                            rs = SR_OK ;
                        }
                    }
                } /* end if (possible) */
            } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (fifostr_entread) */

int fifostr_entlen(fifostr *op,int n) noex {
	int		rs ;
	int		sl = 0 ;
	if ((rs = fifostr_magic(op)) >= 0) {
            rs = SR_INVALID ;
            if (n >= 0) {
                rs = SR_NOTFOUND ;
                if (op->head) {
                    fifostr_ent     *ep = op->head ;
                    for (int i = 0 ; (i < n) && ep ; i += 1) {
                        ep = ep->next ;
                    }
                    if (ep) {
                        rs = SR_OK ;
                        sl = ep->slen ;
                    }
                } /* end if (possible) */
            } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (fifostr_entlen) */

int fifostr_rem(fifostr *op,char *rbuf,int rlen) noex {
	int		rs ;
	int		sl = 0 ;
	if ((rs = fifostr_magic(op)) >= 0) {
            if (op->head != nullptr) {
                fifostr_ent *ep = op->head ;
                sl = ep->slen ;
                if (rbuf) {
                    cchar   *sp = cast_reinterpret<charp>(ep) ;
                    sp += szof(fifostr_ent) ;
                    rs = snwcpy(rbuf,rlen,sp,sl) ;
                }
                if (rs >= 0) {
                    op->head = ep->next ;
                    if (op->head == nullptr) {
                        op->tail = nullptr ;
                    } else {
                        (op->head)->prev = nullptr ;
                    }
                    rs = libmem.free(ep) ;
                    op->ic -= 1 ;
                    op->cnt -= sl ;
                } /* end if (successful removal) */
            } else {
	        rs = SR_NOTFOUND ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (fifostr_rem) */

int fifostr_curbegin(fifostr *op,fifostr_cur *curp) noex {
	int		rs ;
	if ((rs = fifostr_magic(op,curp)) >= 0) {
	    curp->current = nullptr ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fifostr_curbegin) */

int fifostr_curend(fifostr *op,fifostr_cur *curp) noex {
	int		rs ;
	if ((rs = fifostr_magic(op,curp)) >= 0) {
	    curp->current = nullptr ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fifostr_curend) */

int fifostr_curenum(fifostr *op,fifostr_cur *curp,char *rbuf,int rlen) noex {
	int		rs ;
	int		sl = 0 ;
	if ((rs = fifostr_magic(op,curp)) >= 0) {
            fifostr_ent     *ep ;
            if ((curp == nullptr) || (curp->current == nullptr)) {
                ep = op->head ;
            } else {
                if ((rs = fifostr_mat(op,curp->current)) >= 0) {
                    ep = (curp->current)->next ;
                }
            } /* end if */
            if (rs >= 0) {
                if (curp != nullptr) {
                    curp->current = ep ;
                }
                if (ep != nullptr) {
                    sl = ep->slen ;
                    if (rbuf != nullptr) {
                        cchar       *sp = cast_reinterpret<charp>(ep) ;
                        sp = charp(ep) ;
                        sp += szof(fifostr_ent) ;
                        rs = snwcpy(rbuf,rlen,sp,sl) ;
                    }
                } else {
                    rs = SR_NOTFOUND ;
                }
            } /* end if (ok) */
	} /* end if (magic) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (fifostr_curenum) */

int fifostr_curdel(fifostr *op,fifostr_cur *curp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = fifostr_magic(op,curp)) >= 0) {
            fifostr_ent     *ep ;
            if ((curp == nullptr) || (curp->current == nullptr)) {
                ep = op->head ;
            } else {
                ep = curp->current ;
            }
            if (ep != nullptr) {
                int         sl = ep->slen ;
                if (curp != nullptr) {
                    if (ep->prev == nullptr) {
                        op->head = ep->next ;
                    } else {
                        (ep->prev)->next = ep->next ;
                    }
                    if (ep->next == nullptr) {
                        op->tail = ep->prev ;
                    } else {
                        (ep->next)->prev = ep->prev ;
                    }
                    curp->current = ep->prev ;
                } else {
                    op->head = ep->next ;
                    if (op->head == nullptr) {
                        op->tail = nullptr ;
                    } else {
                        (op->head)->prev = nullptr ;
                    }
                } /* end if */
                rs1 = libmem.free(ep) ;
                if (rs >= 0) rs = rs1 ;
                c = --op->ic ;
                op->cnt -= sl ;
            } else {
                rs = SR_NOTFOUND ;
            }
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (fifostr_curdel) */

int fifostr_count(fifostr *op) noex {
	int		rs ;
	if ((rs = fifostr_magic(op)) >= 0) {
	    rs = op->ic ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fifostr_count) */

#ifdef	COMMENT
/* search for a string in the FIFO string object */
int fifostr_finder(fifostr *op,char *s,fifostr_cmp cmpfunc,char **rpp) noex {
	int		rs ;
	if (cmpfunc == nullptr) cmpfunc = cmpdefault ;
	if ((rs = fifostr_magic(op)) >= 0) {
	    fifostr_cur	cur{} ;
	    if ((rs = fifostr_curbegin(op,&cur)) >= 0) {
	        cchar	*rp{} ;
	        while ((rs = fifostr_curenum(op,&cur,&rp)) >= 0) {
	            if ((*cmpfunc)(s,rp) == 0) break ;
	        } /* end while */
	        if (rpp) *rpp = (rs >= 0) ? rp : nullptr ;
	        fifostr_curend(op,&cur) ;
	    } /* end if (fifostr-cur) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fifostr_finder) */
#endif /* COMMENT */


/* private subroutines */

static int fifostr_mat(fifostr *op,fifostr_ent *mep) noex {
	fifostr_ent	*ep = op->head ;
	int		rs = SR_NOTFOUND ;
	while (ep != nullptr) {
	    if (ep == mep) {
	        rs = SR_OK ;
	        break ;
	    }
	    ep = ep->next ;
	} /* end while */
	return rs ;
}
/* end subroutine (fifostr_mat) */

int fifostr::add(cchar *sp,int sl) noex {
	return fifostr_add(this,sp,sl) ;
}

int fifostr::headread(char *rbuf,int rlen) noex {
	return fifostr_headread(this,rbuf,rlen) ;
}

int fifostr::entread(char *rbuf,int rlen,int n) noex {
	return fifostr_entread(this,rbuf,rlen,n) ;
}

int fifostr::entlen(int n) noex {
	return fifostr_entlen(this,n) ;
}

int fifostr::rem(char *rbuf,int rlen) noex {
	return fifostr_rem(this,rbuf,rlen) ;
}

int fifostr::curbegin(fifostr_cur *curp) noex {
	return fifostr_curbegin(this,curp) ;
}

int fifostr::curend(fifostr_cur *curp) noex {
	return fifostr_curend(this,curp) ;
}

int fifostr::curenum(fifostr_cur *curp,char *rbuf,int rlen) noex {
	return fifostr_curenum(this,curp,rbuf,rlen) ;
}

int fifostr::curdel(fifostr_cur *curp) noex {
	return fifostr_curdel(this,curp) ;
}

void fifostr::dtor() noex {
	if (cint rs = int(finish) ; rs < 0) {
	    ulogerror("fifostr",rs,"fini-finish") ;
	}
} /* end method (dtor) */

fifostr_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case fifostrmem_start:
	        rs = fifostr_start(op) ;
	        break ;
	    case fifostrmem_headlen:
	        rs = fifostr_headlen(op) ;
	        break ;
	    case fifostrmem_count:
	        rs = fifostr_count(op) ;
	        break ;
	    case fifostrmem_finish:
	        rs = fifostr_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (fifostr_co::operator) */


