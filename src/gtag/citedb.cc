/* citedb SUPPORT (Citætion Data-base) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* maintain a DB of encountered citations */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debug print-outs */

/* revision history:

	= 1987-09-10, David A­D­ Morano
	This code module was originally written.

	= 1998-09-10, David A­D­ Morano
	This module was changed to serve in the REFERM program.

*/

/* Copyright © 1987,1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Object:
	citedb

	Description:
	This code module (object) maintains a citation database.
	It stores the citation keys, and a count for each, that are
	found within the document text.  No emumeration is required
	since only lookups by key are needed.

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<new>			/* C++STD placement-new */
#include	<memory>		/* C++STD |destroy_at(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<hdb.h>			/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"citedb.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	CD		citedb
#define	CD_ST		citedb_store
#define	CD_OFF		citedb_off
#define	CD_ENT		citedb_ent
#define	CD_CUR		citedb_cur
#define	CD_MAGIC	CITEDB_MAGIC
#define	CD_DEFENTS	200
#define	CD_STRLEN	CITEDB_CITESTRLEN
#define	CD_KEYLEN	CITEDB_CITEKEYLEN

#define	STORE		CD_STORE

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* compile-time debug print-outs */
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local inline int citedb_ctor(citedb *op,Args ... args) noex {
    	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    op->slp = nullptr ;
	    op->elp = nullptr ;
	    op->magval = 0 ;
	    op->citestrindex = 0 ;
	    if (op->elp = new(nt) vecobj ; op->elp) {
	        if (op->slp = new(nt) hdb ; op->slp) {
		    rs = SR_OK ;
	        } /* end if (new-vecobj) */
		if (rs < 0) {
		    delete op->elp ;
		    op->elp = nullptr ;
		} /* end if (error) */
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (citedb_ctor) */

local int citedb_dtor(citedb *op) noex {
	int		rs = SR_OK ;
	if (op->slp) ylikely {
	    delete op->slp ;
	    op->slp = nullptr ;
	}
	if (op->elp) ylikely {
	    delete op->elp ;
	    op->elp = nullptr ;
	}
	return rs ;
} /* end subroutine (citedb_dtor) */

template<typename ... Args>
local inline int citedb_magic(citedb *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == CD_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (citedb_magic) */

local int	mkcitestr(char *,int) noex ;

local int	store_start	(CD_ST *,cchar *,int) noex ;
local int	store_update	(CD_ST *,int) noex ;
local int	store_finish	(CD_ST *) noex ;

local int	entry_load(CD_ENT *,CD_ST *,CD_OFF *) noex ;


/* local variables */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int citedb_start(CD *op) noex {
    	cnullptr	np{} ;
	int		rs ;
	DEBUGPRINTF("ent \n",op) ;
	if ((rs = citedb_ctor(op)) >= 0) {
	    cint	n = CD_DEFENTS ;
	    cint	sz = szof(CD_OFF) ;
	    op->citestrindex = 0 ;
	    if ((rs = vecobj_start(op->elp,sz,n,0)) >= 0) {
	        if ((rs = hdb_start(op->slp,n,1,np,np)) >= 0) {
		    op->magval = CD_MAGIC ;
	        }
	        if (rs < 0) {
		    vecobj_finish(op->elp) ;
	        } /* end if (error) */
	    } /* end if */
	    if (rs < 0) {
		citedb_dtor(op) ;
	    } /* end if (error) */
	} /* end if (magic) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (citedb_start) */

int citedb_finish(CD *op) noex {
	int		rs ;
	int		rs1 ;
	DEBUGPRINTF("ent \n") ;
	if ((rs = citedb_magic(op)) >= 0) {
	    /* clean out the "store" DB */
	    if (hdb_cur kcur ; (rs = hdb_curbegin(op->slp,&kcur)) >= 0) {
	        hdb_dat	key ;
	        hdb_dat	value ;
	        while (hdb_curenum(op->slp,&kcur,&key,&value) >= 0) {
	            CD_ST *sp = (CD_ST *) value.buf ;
		    {
	                rs1 = store_finish(sp) ;
	                if (rs >= 0) rs = rs1 ;
		    } 
		    {
		        rs1 = mem.free(sp) ;
	                if (rs >= 0) rs = rs1 ;
		    }
	        } /* end while */
	        rs1 = hdb_curend(op->slp,&kcur) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (cursor) */
	    /* ready to drop the whole object */
	    {
	        rs1 = hdb_finish(op->slp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    /* clean out the "offset" DB list */
	    {
	        rs1 = vecobj_finish(op->elp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = citedb_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (citedb_magic) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (citedb_finish) */

int citedb_adds(CD *op,int fi,uint fo,cchar *kp,int kl) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = citedb_magic(op,kp)) >= 0) {
	        rs = SR_INVALID ;
	        if (kp[0]) {
		    if (kl < 0) kl = lenstr(kp) ;
		    for (cchar *tp ; (tp = strnchr(kp,kl,',')) != np ; ) {
			cchar *cp ;
			cint tl = conv<int>(tp - kp) ;
	    		if (int cl ; (cl = sfshrink(kp,tl,&cp)) > 0) {
			    c += 1 ;
	        	    rs = citedb_add(op,fi,fo,cp,cl) ;
	    		}
	    		kl -= conv<int>((tp + 1) - kp) ;
	    		kp = (tp + 1) ;
	    		if (rs < 0) break ;
		    } /* end for */
		    if ((rs >= 0) && (kl > 0)) {
			cchar *cp ;
	    		if (int cl ; (cl = sfshrink(kp,kl,&cp)) > 0) {
			    c += 1 ;
	      		    rs = citedb_add(op,fi,fo,cp,cl) ;
			}
		    } /* end if */
		} /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (citedb_adds) */

/* load a string parameter into the DB */
int citedb_add(CD *op,int fi,uint fo,cchar *kp,int kl) noex {
	cnullptr	np{} ;
	int		rs ;
	DEBUGPRINTF("ent fo=%u\n",fo) ;
	if ((rs = citedb_magic(op,kp)) >= 0) {
	        rs = SR_INVALID ;
	        if (kp[0]) {
		    CD_ST	*sp ;
		    hdb_dat	key ;
		    hdb_dat	value ;
	            if (kl < 0) kl = lenstr(kp) ;
	            DEBUGPRINTF("kp=%r\n",kp,kl) ;
	            DEBUGPRINTF("continuing\n") ;
	            /* check if this citation is in the general "store" DB */
	            key.buf = (void *) kp ;
	            key.len = (kl >= 0) ? kl : lenstr(kp) ;
	            if ((rs = hdb_fetch(op->slp,key,np,&value)) >= 0) {
	                sp = (CD_ST *) value.buf ;
	                rs = store_update(sp,op->citestrindex) ;
	                if (sp->n == 2) {
	    	            op->citestrindex += 1 ;
	                }
	            } else if (rs == SR_NOTFOUND) {
	                cint	sz = szof(CD_ST) ;
	                if ((rs = mem.mall(sz,&sp)) >= 0) {
	                    rs = store_start(sp,kp,kl) ;
	                    DEBUGPRINTF("store_start() rs=%d\n",rs) ;
	                    key.buf = cvoidp(sp->citekey) ;
	                    value.buf = sp ;
	        	    value.len = sz ;
	                    if (rs >= 0) {
	                        rs = hdb_store(op->slp,key,value) ;
	                        if (rs < 0) {
	                            store_finish(sp) ;
		                } /* end if (error) */
	                    } /* end if (ok) */
	                    if (rs < 0) {
	                        mem.free(sp) ;
		            } /* end if (error) */
	                } /* end if (memory-allocation) */
	            } /* end if */
		    /* now check if it is in the "offset" DB */
	            if (rs >= 0) {
	                CD_OFF	offe{} ;
	                DEBUGPRINTF("store cn=%u\n",sp->n) ;
	                offe.sp = sp ;
	                offe.off = fo ;
	                offe.fi = fi ;
	                offe.ci = sp->n ;		/* index at time seen */
	                DEBUGPRINTF("vecobj_add() ci=%u \n", offe.ci) ;
	                rs = vecobj_add(op->elp,&offe) ;
	                DEBUGPRINTF("vecobj_add() rs=%d\n",rs) ;
	            } /* end if (offset DB) */
	            DEBUGPRINTF("store cn=%u\n",sp->n) ;
	        } /* end if (valid) */
	} /* end if (citedb_magic) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (citedb_add) */

int citedb_curbegin(CD *op,CD_CUR *curp) noex {
    	int		rs  ;
	DEBUGPRINTF("ent \n") ;
	if ((rs = citedb_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (citedb_curbegin) */

int citedb_curend(CD *op,CD_CUR *curp) noex {
    	int		rs ;
	DEBUGPRINTF("ent \n") ;
	if ((rs = citedb_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	} /* end if (citedb_magic) */
	return rs ;
} /* end subroutine (citedb_curend) */

int citedb_curenum(CD *op,CD_CUR *curp,CD_ENT *ep) noex {
	int		rs ;
	int		i = 0 ; /* return-value */
	DEBUGPRINTF("ent\n") ;
	if ((rs = citedb_magic(op,curp,ep)) >= 0) {
	    CD_OFF	*fop = nullptr ;
	    CD_ST	*sp ;
	    i = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	    /* do the lookup */
	    void *vp ;
	    while ((rs = vecobj_get(op->elp,i,&vp)) >= 0) {
	        if (fop = resumelife<CD_OFF>(vp) ; fop) break ;
	        i += 1 ;
	    } /* end while */
	    DEBUGPRINTF("vecobj_get() rs=%d\n",rs) ;
	    DEBUGPRINTF("ci=%u\n",fop->ci) ;
	    if ((rs >= 0) && fop) {
	        sp = fop->sp ;
	        if (ep) {
	            rs = entry_load(ep,sp,fop) ;
	        }
	        curp->i = i ;
	    } /* end if */
	} /* end if (citedb_magic) */
	DEBUGPRINTF("ret rs=%d i=%u\n",rs,i) ;
	return (rs >= 0) ? i : rs ;
} /* end subroutine (citedb_curenum) */

int citedb_fetch(CD *op,cchar *citekey,CD_CUR *curp,CD_ENT *ep) noex {
	int		rs ;
	int		i = 0 ; /* return-value */
	if ((rs = citedb_magic(op,curp,citekey,ep)) >= 0) {
	    rs = SR_INVALID ;
	    if (citekey[0]) {
		CD_CUR	cur{} ;
		CD_OFF *fop = nullptr ;
	        CD_ST	*sp ;
	        if (curp == nullptr) {
	            curp = &cur ;
	            curp->i = -1 ;
	        } /* end if */
	        i = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	        /* do the lookup */
		void *vp ;
	        while ((rs = vecobj_get(op->elp,i,&vp)) >= 0) {
	            if (fop = resumelife<CD_OFF>(vp) ; fop) {
	                sp = fop->sp ;
	                if (strcmp(sp->citekey,citekey) == 0) break ;
	                i += 1 ;
	            }
	        } /* end while */
	        if ((rs >= 0) && fop) {
	            sp = fop->sp ;
	            if (ep) {
	                rs = entry_load(ep,sp,fop) ;
	            }
	            curp->i = i ;
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (citedb_magic) */
	return (rs >= 0) ? i : rs ;
} /* end subroutine (citedb_fetch) */

int citedb_audit(CD *op) noex {
	int		rs ;
	if ((rs = citedb_magic(op)) >= 0) {
	    rs = vecobj_audit(op->elp) ;
	} /* end if (citedb_magic) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (citedb_audit) */


/* private subroutines */

local int store_start(CD_ST *sp,cchar *kp,int kl) noex {
	int		rs = SR_BUGCHECK ;
	if (sp && kp) {
	    memclear(sp) ;
	    if (cchar *cp ; (rs = mem.strw(kp,kl,&cp)) >= 0) {
	        sp->citekey = cp ;
	        sp->n = 1 ;
	    } /* end if (memory-acquire) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (store_start) */

local int store_update(CD_ST *sp,int n) noex {
	int		rs = SR_OK ;
	if (sp == nullptr) return SR_FAULT ;
	if (sp->n == 1) {
	    rs = mkcitestr(sp->citestr,n) ;
	}
	if (rs >= 0) {
	    sp->n += 1 ;
	}
	return (rs >= 0) ? sp->n : rs ;
} /* end subroutine (store_update) */

local int store_finish(CD_ST *sp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (sp) {
	    rs = SR_OK ;
	    if (sp->citekey) {
	        voidp vp = voidp(sp->citekey) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        sp->citekey = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (store_finish) */

local int entry_load(CD_ENT *ep,CD_ST *sp,CD_OFF *fop) noex {
	int		rs ;
	ep->off = 0 ;
	ep->fi = 0 ;
	ep->ci = 0 ;
	if ((rs = sncpy1(ep->citekey,CD_KEYLEN,sp->citekey)) >= 0) {
	    ep->n = sp->n ;
	    strwcpy(ep->citestr,sp->citestr,CD_STRLEN) ;
	    if (fop) {
	        ep->off = fop->off ;
	        ep->fi = fop->fi ;
	        ep->ci = fop->ci ;
	    }
	}
	return rs ;
} /* end subroutine (entry_load) */

/* make a TROFF citation reference string */
local int mkcitestr(char *buf,int index) noex {
    	int		rs = SR_FAULT ;
	if (buf) {
    	    rs = SR_OVERFLOW ;
	    buf[0] = '\0' ;
	    if (index <= (2 * 26)) {
	        rs = SR_OK ;
	        buf[0] = 'r' ;
	        buf[1] = char((index <= 26) ? ('a' + index) : ('A' + index)) ;
	        buf[2] = '\0' ;
	        buf[3] = '\0' ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (mkcitestr) */


