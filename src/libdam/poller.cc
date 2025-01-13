/* poller SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* poll manager */
/* version %I% last-modified %G% */


/* revision history:

	= 2006-09-10, David A­D­ Morano
	I created this from hacking something that was similar that
	was originally created for a PCS program.

*/

/* Copyright © 2006 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	poller

	Description:
	This object manages poll events.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<poll.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<vecobj.h>
#include	<localmisc.h>

#include	"poller.h"


/* local defines */

#define	POLLER_NDEF	10


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int poller_ctor(poller *op,Args ... args) noex {
    	POLLER		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->rlp = new(nothrow) vecobj) != np) {
		rs = SR_OK ;
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (poller_ctor) */

static int poller_dtor(poller *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->rlp) {
		delete op->rlp ;
		op->rlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (poller_dtor) */

template<typename ... Args>
static inline int poller_magic(poller *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == POLLER_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (poller_magic) */

static int	poller_extend(poller *,int) noex ;
static int	poller_del(poller *,poller_spec *) noex ;

static bool	ismatch(poller_spec *,poller_spec *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int poller_start(poller *op) noex {
	int		rs ;
	if ((rs = poller_ctor(op)) >= 0) {
	    cint	esz = szof(poller_spec) ;
	    cint	vn = POLLER_NDEF ;
	    cint	vo = 0 ;
	    if ((rs = vecobj_start(op->rlp,esz,vn,vo)) >= 0) {
	        op->magic = POLLER_MAGIC ;
	    }
	    if (rs < 0) {
		poller_dtor(op) ;
	    }
	} /* end if (poller_ctor) */
	return rs ;
}
/* end subroutine (poller_start) */

int poller_finish(poller *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = poller_magic(op)) >= 0) {
	    if (op->rlp) {
	        rs1 = vecobj_finish(op->rlp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->pa) {
	        rs1 = uc_free(op->pa) ;
	        if (rs >= 0) rs = rs1 ;
	        op->pa = nullptr ;
	    }
	    {
	        rs = poller_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (poller_finish) */

/* register an event */
int poller_reg(poller *op,poller_spec *rep) noex {
	int		rs ;
	if ((rs = poller_magic(op,rep)) >= 0) {
	   rs = vecobj_add(op->rlp,rep) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (poller_reg) */

/* cancel a previously registered event */
int poller_cancel(poller *op,poller_spec *cep) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = poller_magic(op,cep)) >= 0) {
	    vecobj	*rlp = op->rlp ;
	    void	*vp{} ;
	    for (int i = 0 ; rlp->get(i,&vp) >= 0 ; i += 1) {
	        poller_spec	*ep = (poller_spec *) vp ;
	        if (vp) {
	            if (ismatch(ep,cep)) {
	                c += 1 ;
	                rs = rlp->del(i--) ;
	            }
	        }
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (poller_cancel) */

/* cancel all previously registered events on an FD */
int poller_cancelfd(poller *op,int fd) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = poller_magic(op)) >= 0) {
	    vecobj	*rlp = op->rlp ;
	    void	*vp{} ;
	    for (int i = 0 ; rlp->get(i,&vp) >= 0 ; i += 1) {
	        poller_spec	*ep = (poller_spec *) vp ;
	        if (vp) {
	            if (ep->fd == fd) {
	                c += 1 ;
	                rs = rlp->del(i--) ;
		    }
	        }
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (poller_cancelfd) */

/* wait for an event to become ready */
int poller_wait(poller *op,poller_spec *rp,int mto) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = poller_magic(op,rp)) >= 0) {
	    vecobj	*rlp = op->rlp ;
	    if (op->nready > 0) {
	        rs = poller_get(op,rp) ;
	        c = rs ;
	    } else if ((rs = rlp->count) >= 0) {
                cint        n = rs ;
                if ((n > op->e) || (op->pa == nullptr)) {
                    rs = poller_extend(op,n) ;
                }
                if (rs >= 0) {
                    int     j = 0 ;
                    void    *vp{} ;
                    op->n = n ;
                    for (int i = 0 ; rlp->get(i,&vp) >= 0 ; i += 1) {
                        poller_spec *ep = (poller_spec *) vp ;
                        if (vp) {
                            op->pa[j].fd = ep->fd ;
                            op->pa[j].events = ep->events ;
                                op->pa[j].revents = 0 ;
                            j += 1 ;
                        }
                    } /* end for */
                    if ((rs = u_poll(op->pa,n,mto)) > 0) {
                        op->nready = rs ;
                        rs = poller_get(op,rp) ;
                        c = rs ;
                    } else if (rs == 0) {
                        op->nready = 0 ;
                    } else if (rs == SR_INTR) {
                        op->nready = 0 ;
                        rs = SR_OK ;
                    }
                } /* end if (ok) */
	    } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (poller_wait) */

int poller_get(poller *op,poller_spec *rp) noex {
    	int		rs ;
	int		c = 0 ;
	if ((rs = poller_magic(op,rp)) >= 0) {
	    int		i ; /* used-afterwards */
	    for (i = 0 ; i < op->n ; i += 1) {
	        if (op->pa[i].revents != 0) break ;
	    } /* end for */
	    if (i < op->n) {
	        *rp = op->pa[i] ;
	        rs = poller_del(op,(op->pa + i)) ;
	        op->pa[i].fd = -1 ;
	        op->pa[i].events = 0 ;
	        op->pa[i].revents = 0 ;
	        c = op->nready ;
	        op->nready -= 1 ;
	    } /* end if (result) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (poller_get) */

int poller_count(poller *op) noex {
	int		rs ;
	if ((rs = poller_magic(op)) >= 0) {
	    rs = vecobj_count(op->rlp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (poller_count) */

int poller_curbegin(poller *op,poller_cur *curp) noex {
    	int		rs ;
	if ((rs = poller_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (poller_curbegin) */

int poller_curend(poller *op,poller_cur *curp) noex {
    	int		rs ;
	if ((rs = poller_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (poller_curend) */

int poller_enum(poller *op,poller_cur *curp,poller_spec *rp) noex {
	int		rs ;
	if ((rs = poller_magic(op,curp,rp)) >= 0) {
	    int		i = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	    poller_spec	*ep = nullptr ;
	    void	*vp{} ;
	    while ((rs = vecobj_get(op->rlp,i,&vp)) >= 0) {
	        ep = (poller_spec *) vp ;
	        if (vp) break ;
	        i += 1 ;
	    } /* end while */
	    if ((rs >= 0) && ep) {
	        *rp = *ep ;
	        curp->i = i ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (poller_enum) */


/* private subroutines */

static int poller_extend(poller *op,int n) noex {
	int		rs = SR_OK ;
	cint		ne = max(n,1) ;
	if ((ne > op->e) || (op->pa == nullptr)) {
	    cint	sz = ne * szof(poller_spec) ;
	    if (op->pa) {
	        uc_free(op->pa) ;
	        op->pa = nullptr ;
	    }
	    if (POLLFD *a ; (rs = uc_malloc(sz,&a)) >= 0) {
	        op->pa = a ;
	        op->e = ne ;
	    } else {
	        op->pa = nullptr ;
	    }
	} /* end if (needed) */
	return (rs >= 0) ? ne : rs ;
}
/* end subroutine (poller_extend) */

static int poller_del(poller *op,poller_spec *cep) noex {
	vecobj		*rlp = op->rlp ;
	int		rs = SR_OK ;
	int		i ; /* used-afterwards */
	void	*vp{} ;
	for (i = 0 ; rlp->get(i,&vp) >= 0 ; i += 1) {
	    poller_spec	*ep = (poller_spec *) vp ;
	    if (vp) {
	        if (ismatch(ep,cep)) {
	            rs = rlp->del(i) ;
	            break ;
		}
	    }
	} /* end for */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (poller_del) */

static bool ismatch(poller_spec *e1p,poller_spec *e2p) noex {
	bool		f = true ;
	f = f && (e1p->fd == e2p->fd) ;
	f = f && (e1p->events == e2p->events) ;
	return f ;
}
/* end subroutine (ismatch) */


