/* fifostr SUPPORT */
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

	This object manages a FIFO of strings.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<snwcpy.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"fifostr.h"


/* local defines */


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


/* exported subroutines */

int fifostr_start(fifostr *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->head = nullptr ;
	    op->tail = nullptr ;
	    op->ic = 0 ;
	    op->cc = 0 ;
	    op->magic = FIFOSTR_MAGIC ;
	}
	return rs ;
}
/* end subroutine (fifostr_start) */

int fifostr_finish(fifostr *op) noex {
	int		rs ;
	if ((rs = fifostr_magic(op)) >= 0) {
		while ((rs = fifostr_del(op,nullptr)) >= 0) ;
		if (rs == SR_NOTFOUND) rs = SR_OK ;
		op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fifostr_finish) */

int fifostr_add(fifostr *op,cchar *sp,int sl) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = fifostr_magic(op,sp)) >= 0) {
	        fifostr_ent	*ep = nullptr ;
	        cint		size = sizeof(fifostr_ent) + (sl + 1) ;
	    if (sl < 0) sl = strlen(sp) ;
	        if ((rs = uc_libmalloc(size,&ep)) >= 0) {
	            ep->slen = sl ;
	            {
	                char	*bp = (char *) ep ;
	                bp += sizeof(fifostr_ent) ;
	                strwcpy(bp,sp,sl) ;
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
	            op->cc += sl ;
	        } /* end if (memory-allocation) */
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
	                cchar	*sp = (cchar *) ep ;
	                sp += sizeof(fifostr_ent) ;
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
	            fifostr_ent		*ep = op->head ;
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
	                fifostr_ent	*ep = op->head ;
	                for (int i = 0 ; (i < n) && ep ; i += 1) {
	                    ep = ep->next ;
	                }
	                if (ep) {
	                    sl = ep->slen ;
	                    if (rbuf) {
	                        cchar	*sp = (cchar *) ep ;
	                        sp += sizeof(fifostr_ent) ;
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
	                fifostr_ent	*ep = op->head ;
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

int fifostr_remove(fifostr *op,char *rbuf,int rlen) noex {
	int		rs ;
	int		sl = 0 ;
	if ((rs = fifostr_magic(op)) >= 0) {
	        if (op->head != nullptr) {
	            fifostr_ent	*ep = op->head ;
	            sl = ep->slen ;
	            if (rbuf) {
	                cchar	*sp = (cchar *) ep ;
	                sp += sizeof(fifostr_ent) ;
	                rs = snwcpy(rbuf,rlen,sp,sl) ;
	            }
	            if (rs >= 0) {
	                op->head = ep->next ;
	                if (op->head == nullptr) {
	                    op->tail = nullptr ;
	                } else {
	                    (op->head)->prev = nullptr ;
	                }
	                rs = uc_libfree(ep) ;
	                op->ic -= 1 ;
	                op->cc -= sl ;
	            } /* end if (successful removal) */
	        } else {
	            rs = SR_NOTFOUND ;
	        }
	} /* end if (magic) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (fifostr_remove) */

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

int fifostr_enum(fifostr *op,fifostr_cur *curp,char *rbuf,int rlen) noex {
	int		rs ;
	int		sl = 0 ;
	if ((rs = fifostr_magic(op,curp)) >= 0) {
	        fifostr_ent	*ep ;
	        if ((curp == nullptr) || (curp->current == nullptr)) {
	            ep = op->head ;
	        } else {
	            if ((rs = fifostr_mat(op,curp->current)) >= 0) {
	                ep = (curp->current)->next ;
	            }
	        } /* end if */
	        if (rs >= 0) {
	            cchar	*sp = nullptr ;
	            if (curp != nullptr) curp->current = ep ;
	            if (ep != nullptr) {
	                sl = ep->slen ;
	                if (rbuf != nullptr) {
	                    sp = (cchar *) ep ;
	                    sp += sizeof(fifostr_ent) ;
	                    rs = snwcpy(rbuf,rlen,sp,sl) ;
	                }
	            } else {
	                rs = SR_NOTFOUND ;
	            }
	        } /* end if (ok) */
	} /* end if (magic) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (fifostr_enum) */

int fifostr_del(fifostr *op,fifostr_cur *curp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = fifostr_magic(op,curp)) >= 0) {
	        fifostr_ent	*ep ;
	        if ((curp == nullptr) || (curp->current == nullptr)) {
	            ep = op->head ;
	        } else {
	            ep = curp->current ;
	        }
	        if (ep != nullptr) {
	            int		sl = ep->slen ;
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
	            rs1 = uc_libfree(ep) ;
	            if (rs >= 0) rs = rs1 ;
	            c = --op->ic ;
	            op->cc -= sl ;
	        } else {
	            rs = SR_NOTFOUND ;
	        }
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (fifostr_del) */

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
	        fifostr_cur	cur ;
	        if ((rs = fifostr_curbegin(op,&cur)) >= 0) {
	            cchar	*rp ;
	            while ((rs = fifostr_enum(op,&cur,&rp)) >= 0) {
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


