/* fifostr */
/* lang=C20 */

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
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<snwcpy.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"fifostr.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */

int		fifostr_curbegin(FIFOSTR *,FIFOSTR_CUR *) noex ;
int		fifostr_curend(FIFOSTR *,FIFOSTR_CUR *) noex ;
int		fifostr_del(FIFOSTR *,FIFOSTR_CUR *) noex ;

static int	fifostr_mat(FIFOSTR *,FIFOSTR_ENT *) noex ;

#ifdef	COMMENT
static int	cmpdefault() ;
#endif


/* exported subroutines */

int fifostr_start(FIFOSTR *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->head = op->tail = nullptr ;
	    op->ic = 0 ;
	    op->cc = 0 ;
	    op->magic = FIFOSTR_MAGIC ;
	}
	return rs ;
}
/* end subroutine (fifostr_start) */

int fifostr_finish(FIFOSTR *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->magic == FIFOSTR_MAGIC) {
		while ((rs = fifostr_del(op,nullptr)) >= 0) ;
		if (rs == SR_NOTFOUND) rs = SR_OK ;
		op->magic = 0 ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fifostr_finish) */

int fifostr_add(FIFOSTR *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	if (op && sp) {
	    rs = SR_NOTOPEN ;
	    if (sl < 0) sl = strlen(sp) ;
	    if (op->magic == FIFOSTR_MAGIC) {
	        FIFOSTR_ENT	*ep = nullptr ;
	        cint		size = sizeof(FIFOSTR_ENT) + (sl + 1) ;
	        if ((rs = uc_libmalloc(size,&ep)) >= 0) {
	            ep->slen = sl ;
	            {
	                char	*bp = (char *) ep ;
	                bp += sizeof(FIFOSTR_ENT) ;
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
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (fifostr_add) */

int fifostr_headread(FIFOSTR *op,char *rbuf,int rlen) noex {
	int		rs = SR_OK ;
	int		sl = 0 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->magic == FIFOSTR_MAGIC) {
		rs = SR_NOTFOUND ;
	        if (op->head) {
	            FIFOSTR_ENT	*ep = op->head ;
	            sl = ep->slen ;
	            if (rbuf) {
	                cchar	*sp = (cchar *) ep ;
	                sp += sizeof(FIFOSTR_ENT) ;
	                rs = snwcpy(rbuf,rlen,sp,sl) ;
	            }
	        }
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (fifostr_headread) */

int fifostr_headlen(FIFOSTR *op) noex {
	int		rs = SR_OK ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->magic == FIFOSTR_MAGIC) {
		rs = SR_NOTFOUND ;
	        if (op->head) {
	            FIFOSTR_ENT		*ep = op->head ;
	            rs = ep->slen ;
	        }
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fifostr_headlen) */

int fifostr_entread(FIFOSTR *op,char *rbuf,int rlen,int n) noex {
	int		rs = SR_OK ;
	int		sl = 0 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->magic == FIFOSTR_MAGIC) {
		rs = SR_INVALID ;
	        if (n >= 0) {
		    rs = SR_NOTFOUND ;
		    if (op->head) {
	                FIFOSTR_ENT	*ep = op->head ;
	                for (int i = 0 ; (i < n) && ep ; i += 1) {
	                    ep = ep->next ;
	                }
	                if (ep) {
	                    sl = ep->slen ;
	                    if (rbuf) {
	                        cchar	*sp = (cchar *) ep ;
	                        sp += sizeof(FIFOSTR_ENT) ;
	                        rs = snwcpy(rbuf,rlen,sp,sl) ;
			    } else {
				rs = SR_OK ;
	                    }
	                }
	            } /* end if (possible) */
	        } /* end if (valid) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (fifostr_entread) */

int fifostr_entlen(FIFOSTR *op,int n) noex {
	int		rs = SR_FAULT ;
	int		sl = 0 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->magic == FIFOSTR_MAGIC) {
		rs = SR_INVALID ;
		if (n >= 0) {
		    rs = SR_NOTFOUND ;
	            if (op->head) {
	                FIFOSTR_ENT	*ep = op->head ;
			for (int i = 0 ; (i < n) && ep ; i += 1) {
	    		    ep = ep->next ;
			}
			if (ep) {
			    rs = SR_OK ;
	    		    sl = ep->slen ;
			}
	            } /* end if (possible) */
	        } /* end if (valid) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (fifostr_entlen) */

int fifostr_remove(FIFOSTR *op,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		sl = 0 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->magic == FIFOSTR_MAGIC) {
		rs = SR_OK ;
	        if (op->head != nullptr) {
	            FIFOSTR_ENT	*ep = op->head ;
	            sl = ep->slen ;
	            if (rbuf != nullptr) {
	                cchar	*sp = (cchar *) ep ;
	                sp += sizeof(FIFOSTR_ENT) ;
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
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (fifostr_remove) */

int fifostr_curbegin(FIFOSTR *op,FIFOSTR_CUR *curp) noex {
	int		rs = SR_FAULT ;
	if (op && curp) {
	    rs = SR_NOTOPEN ;
	    if (op->magic == FIFOSTR_MAGIC) {
		curp->current = nullptr ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fifostr_curbegin) */

int fifostr_curend(FIFOSTR *op,FIFOSTR_CUR *curp) noex {
	int		rs = SR_FAULT ;
	if (op && curp) {
	    rs = SR_NOTOPEN ;
	    if (op->magic == FIFOSTR_MAGIC) {
		curp->current = nullptr ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fifostr_curend) */

int fifostr_enum(FIFOSTR *op,FIFOSTR_CUR *curp,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		sl = 0 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->magic == FIFOSTR_MAGIC) {
	        FIFOSTR_ENT	*ep ;
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
	                    sp += sizeof(FIFOSTR_ENT) ;
	                    rs = snwcpy(rbuf,rlen,sp,sl) ;
	                }
	            } else {
	                rs = SR_NOTFOUND ;
	            }
	        } /* end if (ok) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (fifostr_enum) */

int fifostr_del(FIFOSTR *op,FIFOSTR_CUR *curp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->magic == FIFOSTR_MAGIC) {
	        FIFOSTR_ENT	*ep ;
		rs = SR_OK ;
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
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (fifostr_del) */

int fifostr_count(FIFOSTR *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->magic == FIFOSTR_MAGIC) {
		rs = op->ic ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fifostr_count) */

#ifdef	COMMENT
/* search for a string in the FIFO string object */
int fifostr_finder(FIFOSTR *op,char *s,fifostr_cmp cmpfunc,char **rpp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    [if (cmpfunc == nullptr) cmpfunc = cmpdefault ;
	    if (op->magic == FIFOSTR_MAGIC) {
	        fifostr_cur	cur ;
	        if ((rs = fifostr_curbegin(op,&cur)) >= 0) {
	            cchar	*rp ;
	            while ((rs = fifostr_enum(op,&cur,&rp)) >= 0) {
	                if ((*cmpfunc)(s,rp) == 0) break ;
	            } /* end while */
	            if (rpp) *rpp = (rs >= 0) ? rp : nullptr ;
	            fifostr_curend(op,&cur) ;
	        } /* end if (fifostr-cur) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fifostr_finder) */
#endif /* COMMENT */


/* private subroutines */

static int fifostr_mat(FIFOSTR *op,FIFOSTR_ENT *mep) noex {
	FIFOSTR_ENT	*ep = op->head ;
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


