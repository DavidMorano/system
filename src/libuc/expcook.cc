/* expcook SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Expand-Cookie - creates the substitution variables for cookie escapes */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	expcook

	Description:
	This little object is used to set up the substitution
	variables and to do the substitution expansions for string
	buffers with cookie escapes in it.

	Notes:
	The left brace character is HEX '\x7b', the right brace
	character is HEX '\x7d'.  Deal with it!

	Cookies take the form:
	%<keychr>
	%{<keychr>}
	%{<keystr>}

	Where:
	<keychr>	is the key to look-up
	<keystr>	is the key to look-up

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>
#include	<usystem.h>
#include	<hdbstr.h>
#include	<sbuf.h>
#include	<buffer.h>
#include	<snwcpy.h>
#include	<strwcpy.h>
#include	<strn.h>
#include	<localmisc.h>

#include	"expcook.h"

#pragma		GCC dependency	"mod/libutil.ccm"

import libutil ;		/* |lenstr(3u)| */

/* local defines */

#define	EX		expcook


/* imported namespaces */

using std::nothrow ;			/* constant */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static inline int expcook_ctor(EX *op) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    if ((op->hlp = new(nothrow) hdbstr) != np) {
		rs = SR_OK ;
	    } /* end if (new-hdbstr) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (expcook_ctor) */

static int expcook_dtor(EX *op) noex {
	int		rs = SR_OK ;
	if (op->hlp) {
	    delete op->hlp ;
	    op->hlp = nullptr ;
	}
	return rs ;
}
/* end subroutine (expcook_dtor) */

template<typename ... Args>
static inline int expcook_magic(EX *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == EXPCOOK_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (expcook_magic) */

static int	expcook_prockey(EX *,int,buffer *,cchar *,int) noex ;

static int	buffer_keydef(buffer *,int,cchar *,int) noex ;

static int	mkcomp(char *,int,cchar *,int,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int expcook_start(EX *op) noex {
	int		rs ;
	if ((rs = expcook_ctor(op)) >= 0) {
	    hdbstr	*slp = op->hlp ;
	    cint	ne = 10 ;
	    op->magic = 0 ;
	    if ((rs = slp->start(ne)) >= 0) {
	        op->magic = EXPCOOK_MAGIC ;
	    }
	    if (rs < 0) {
		expcook_dtor(op) ;
	    }
	} /* end if (expcook_ctor) */
	return rs ;
}
/* end subroutine (expcook_start) */

int expcook_finish(EX *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = expcook_magic(op)) >= 0) {
	    {
	        hdbstr	*slp = op->hlp ;
		rs1 = slp->finish ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = expcook_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (expcook_finish) */

int expcook_add(EX *op,cchar *kbuf,cchar *vbuf,int vlen) noex {
	cnullptr	np{} ;
	int		rs ;
	if ((rs = expcook_magic(op,kbuf)) >= 0) {
	    hdbstr	*slp = op->hlp ;
	    cint	kl = lenstr(kbuf) ;
	    if ((rs = slp->fetch(kbuf,kl,np,np)) >= 0) {
	        rs = slp->delkey(kbuf,kl) ;
	    } else if (rs == SR_NOTFOUND) {
	        rs = SR_OK ;
	    }
	    if ((rs >= 0) && vbuf) {
	        rs = slp->add(kbuf,kl,vbuf,vlen) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (expcook_add) */

int expcook_curbegin(EX *op,expcook_cur *curp) noex {
	cnullptr	np{} ;
	int		rs ;
	if ((rs = expcook_magic(op,curp)) >= 0) {
	    rs = SR_NOMEM ;
	    if ((curp->clp = new(nothrow) hdbstr_cur) != np) {
	        hdbstr	*slp = op->hlp ;
	        rs = slp->curbegin(curp->clp) ;
		if (rs < 0) {
		    delete curp->clp ;
		    curp->clp = nullptr ;
		}
	    } /* end if (new-hdbstr) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (expcook_curbegin) */

int expcook_curend(EX *op,expcook_cur *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = expcook_magic(op,curp)) >= 0) {
	    {
	        hdbstr	*slp = op->hlp ;
	        rs1 = slp->curend(curp->clp) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    if (curp->clp) {
		delete curp->clp ;
		curp->clp = nullptr ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (expcook_curend) */

int expcook_enum(EX *op,expcook_cur *curp,char *rbuf,int rlen) noex {
	int		rs ;
	int		bl = 0 ;
	if ((rs = expcook_magic(op,curp,rbuf)) >= 0) {
	    hdbstr	*slp = op->hlp ;
	    int		vl{} ;
	    cchar	*kp{} ;
	    cchar	*vp{} ;
	    if ((rs = slp->curenum(curp->clp,&kp,&vp,&vl)) >= 0) {
	        cint	kl = rs ;
	        rs = mkcomp(rbuf,rlen,kp,kl,vp,vl) ;
	        bl = rs ;
	    } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? bl : rs ;
}
/* end subroutine (expcook_enum) */

int expcook_findkey(EX *op,cchar *kp,int kl,cchar **rpp) noex {
	int		rs ;
	if ((rs = expcook_magic(op,kp,rpp)) >= 0) {
	    hdbstr	*slp = op->hlp ;
	    rs = slp->fetch(kp,kl,nullptr,rpp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (expcook_findkey) */

int expcook_delkey(EX *op,cchar *key) noex {
	int		rs ;
	if ((rs = expcook_magic(op,key)) >= 0) {
	    hdbstr	*slp = op->hlp ;
	    rs = slp->delkey(key,-1) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (expcook_delkey) */

int expcook_exp(EX *op,int wch,char *rbuf,int rlen,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	int		bl = 0 ;
	if ((rs = expcook_magic(op,rbuf,sp)) >= 0) {
	    if (sl < 0) sl = lenstr(sp) ;
	    rbuf[0] = '\0' ;
	    if (rlen > 0) {
	        if (buffer bo ; (rs = bo.start(rlen)) >= 0) {
	            if ((rs = expcook_expbuf(op,wch,&bo,sp,sl)) >= 0) {
	                bl = rs ;
		        if (cchar *bp{} ; (rs = bo.get(&bp)) >= 0) {
	                    if (bp) {
	                        rs = snwcpy(rbuf,rlen,bp,bl) ;
			    }
	                }
	            }
	            rs1 = bo.finish ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (buffer) */
	    } else {
	        rs = SR_OVERFLOW ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? bl : rs ;
}
/* end subroutine (expcook_exp) */

int expcook_expbuf(EX *op,int wch,buffer *bufp,cchar *sp,int sl) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = expcook_magic(op,bufp,sp)) >= 0) {
	    cint	sch = '%' ;
	    cchar	*ss = "{}" ;
	    if (sl < 0) sl = lenstr(sp) ;
	    for (cchar *tp ; (tp = strnchr(sp,sl,sch)) != nullptr ; ) {
		cint tl = intconv(tp - sp) ;
	        if ((rs = bufp->strw(sp,tl)) >= 0) {
	            int		kl = -1 ;
	            cchar	*kp = nullptr ;
		    len += rs ;
	            sl -= intconv((tp + 1) - sp) ;
	            sp = (tp+1) ;
	            if (sl > 0) {
	                if (sp[0] == sch) {
	                    rs = bufp->chr(sch) ;
		  	    len += rs ;
	                    sl -= 1 ;
	                    sp += 1 ;
	                } else if (sp[0] == ss[0]) {
	                    sl -= 1 ;
	                    sp += 1 ;
	                    if ((tp = strnchr(sp,sl,ss[1])) != nullptr) {
	                        kp = sp ;
	                        kl = intconv(tp - sp) ;
	                        sl -= intconv((tp + 1) - sp) ;
	                        sp = (tp + 1) ;
	                    } else {
	                        kp = sp ;
	                        kl = 0 ;
	                        sl -= intconv((tp + 1) - sp) ;
	                        sp = (tp + 1) ;
	                    } /* end if */
	                } else {
	                    kp = sp ;
	                    kl = 1 ;
	                    sl -= 1 ;
	                    sp += 1 ;
	                } /* end if (extracting the key) */
	            } else {
		        kp = sp ;
		        kl = 0 ;
		    }
	            if ((rs >= 0) && kp) {
		        rs = expcook_prockey(op,wch,bufp,kp,kl) ;
		        len += rs ;
		    }
	        } /* end if (buf-add leading part) */
	        if (rs < 0) break ;
	    } /* end for (expanding) */
	    /* copy over any remainder (trailing part) */
	    if ((rs >= 0) && (sl > 0)) {
	        rs = bufp->strw(sp,sl) ;
	        len += rs ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (expcook_expbuf) */


/* private subroutines */

static int expcook_prockey(EX *op,int wch,buffer *bufp,cchar *kp,int kl) noex {
	int		rs = SR_NOTFOUND ;
	int		len = 0 ;
	if (kl < 0) kl = lenstr(kp) ;
	if (kl > 0) {
	    hdbstr	*slp = op->hlp ;
	    if (cchar *vp{} ; (rs = slp->fetch(kp,kl,nullptr,&vp)) >= 0) {
	        cint	vl = rs ;
	        if (vl > 0) {
	            rs = bufp->strw(vp,vl) ;
		    len += rs ;
	        }
	    } else if (rs == SR_NOTFOUND) {
	        if (wch > 0) {
		    rs = buffer_keydef(bufp,wch,kp,kl) ;
		    len += rs ;
		} else if (wch == 0) {
		    rs = SR_OK ;
		}
	    } /* end if (specified delimiter) */
	} /* end if (non-zero key) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (expcook_prockey) */

static int buffer_keydef(buffer *bufp,int wch,cchar *kp,int kl) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = bufp->chr(wch)) >= 0) {
	    len = rs ;
	    if ((rs = bufp->strw(kp,kl)) >= 0) {
		len += rs ;
	        rs = bufp->chr(wch) ;
		len += rs ;
	    } /* end if */
	} /* end if */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (buffer_keydef) */

static int mkcomp(char *rp,int rl,cchar *kp,int kl,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (rp && kp) {
	    if (sbuf b ; (rs = b.start(rp,rl)) >= 0) {
	        {
	            if (rs >= 0) rs = b.strw(kp,kl) ;
	            if (rs >= 0) rs = b.chr('=') ;
	            if (vp) {
		        if (rs >= 0) rs = b.strw(vp,vl) ;
		    }
	        }
	        rs1 = b.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sbuf) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mkcomp) */


