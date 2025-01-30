/* keyopt SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* paramater option manipulations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This code module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object is similar to the PARAMOPT object in some ways.
	This object does not provide (natually) any exponetial
	enumeration of the gather options (like PARAMOPT does).
	Rather it allows several options (different options) to be
	specified together (or strung together) separated by commas.
	This is actually quite natural for most applications.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>
#include	<usystem.h>
#include	<char.h>
#include	<sfx.h>
#include	<strn.h>
#include	<strkeycmp.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"keyopt.h"


/* local defines */

#define	KEYBUFLEN	100

#define	CUR		keyopt_cur
#define	VAL		keyopt_val
#define	NAM		keyopt_na


/* external subroutines */


/* external variables */


/* forward references */

extern "C" {
    int		keyopt_loads(keyopt *,cchar *,int) noex ;
    int		keyopt_load(keyopt *,cchar *,int) noex ;
    int		keyopt_loadvalue(keyopt *,cchar *,cchar *,int) noex ;
}

template<typename ... Args>
static inline int keyopt_magic(keyopt *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == KEYOPT_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (keyopt_magic) */

static int	keyopt_findkey(keyopt *,cchar *,int,NAM **) noex ;
static int	keyopt_loadpair(keyopt *,cchar *,int) noex ;

static int	keyname_incri(NAM *) noex ;
static int	keyname_findv(NAM *,cchar *,int,VAL **) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int keyopt_start(keyopt *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    op->head = nullptr ;
	    op->tail = nullptr ;
	    op->count = 0 ;
	    op->magic = KEYOPT_MAGIC ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (keyopt_start) */

int keyopt_finish(keyopt *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = keyopt_magic(op)) >= 0) {
	    NAM		*sp, *nnp ;
	    VAL		*vp, *nvp ;
	    for (sp = op->head ; sp != nullptr ; sp = nnp) {
	        for (vp = sp->head ; vp != nullptr ; vp = nvp) {
	            if (vp->value != nullptr) {
	                rs1 = uc_free(vp->value) ;
		        if (rs >= 0) rs = rs1 ;
	            }
	            nvp = vp->next ;
	            rs1 = uc_free(vp) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end for */
	        if (sp->name) {
	            rs1 = uc_free(sp->name) ;
		    if (rs >= 0) rs = rs1 ;
	        }
	        nnp = sp->next ;
	        rs1 = uc_free(sp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end for */
	    op->head = nullptr ;
	    op->tail = nullptr ;
	    op->count = 0 ;
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (keyopt_finish) */

int keyopt_loads(keyopt *op,cchar *sp,int sl) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = keyopt_magic(op,sp)) >= 0) {
	    cchar	*tp ;
	    cchar	*cp ;
	    int		cl ;
	    if (sl <= 0) sl = strlen(sp) ;
	    while ((tp = strnpbrk(sp,sl,",\t\n\r ")) != nullptr) {
	        cp = sp ;
	        cl = tp - sp ;
	        if (cl > 0) {
	            rs = keyopt_loadpair(op,cp,cl) ;
	            c += rs ;
	        }
	        sl -= ((tp + 1) - sp) ;
	        sp = (tp + 1) ;
	        if (rs < 0) break ;
	    } /* end while */
	    if ((rs >= 0) && (sl > 0)) {
	        rs = keyopt_loadpair(op,sp,sl) ;
	        c += rs ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (keyopt_loads) */

int keyopt_load(keyopt *op,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = keyopt_magic(op)) >= 0) {
	    rs = keyopt_loadpair(op,sp,sl) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (keyopt_load) */

int keyopt_loadvalue(keyopt *op,cchar *key,cchar *vbuf,int vlen) noex {
	int		rs ;
	if ((rs = keyopt_magic(op,key)) >= 0) {
	    VAL		*vp, *nvp ;
	    NAM		*pp, *tpp ;
	    cchar	*tp ;
	    cchar	*cp ;
	    int		klen ;
/* clean up the value a little */
	    if (vlen < 0) vlen = (vbuf) ? strlen(vbuf) : 0 ;
/* do we have one of these named keys already? */
	    klen = -1 ;
	    if ((tp = strchr(key,'=')) != nullptr) {
	        klen = (tp - key) ;
	    }
	    if (keyopt_findkey(op,key,klen,&pp) == SR_NOTFOUND) {
	        cint	nsize = sizeof(NAM) ;
/* make a new parameter header block */
	        if ((rs = uc_malloc(nsize,&pp)) >= 0) {
	            pp->count = 0 ;
	            pp->next = nullptr ;
	            pp->head = nullptr ;
	            pp->tail = nullptr ;
	            pp->current = nullptr ;
	            if ((rs = uc_mallocstrw(key,klen,&cp)) >= 0) {
	                pp->name = cp ;
	                tpp = op->tail ;
	                op->tail = pp ;
	                if (tpp != nullptr) {
	                    tpp->next = pp ;
		        }
	                if (op->head == nullptr) {
	                    op->head = pp ;
		        }
	            } else {
	                uc_free(pp) ;
	                pp = nullptr ;
	            }
	        } /* end if (memory allocation) */
	    } /* end if (adding a new parameter block on the list) */
    /* OK, now we have the parameter block that we are looking for in 'pp' */
	    if (rs >= 0) {
	        cint	vsize = sizeof(VAL) ;
	        if ((rs = uc_malloc(vsize,&nvp)) >= 0) {
	            nvp->next = nullptr ;
	            nvp->value = nullptr ;
	            if (vbuf != nullptr) {
	                if ((rs = uc_mallocstrw(vbuf,vlen,&cp)) >= 0) {
	                    nvp->value = cp ;
	                } else {
	                    uc_free(nvp) ;
		        }
	            } /* end if (new value) */
	            if (rs >= 0) {
	                if (pp->head != nullptr) {
	                    vp = pp->tail ;
	                    vp->next = nvp ;
	                    pp->tail = nvp ;
	                } else {
	                    pp->head = nvp ;
	                    pp->tail = nvp ;
	                }
	                pp->count += 1 ;
	            } /* end if */
	            if (rs >= 0) op->count += 1 ;
	        } /* end if */
	    } /* end if (ok) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (keyopt_loadvalue) */

int keyopt_count(keyopt *op) noex {
	int		rs ;
	if ((rs = keyopt_magic(op)) >= 0) {
	    rs = op->count ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (keyopt_count) */

int keyopt_enumkeys(keyopt *op,CUR *curp,cchar **rpp) noex {
	int		rs ;
	if ((rs = keyopt_magic(op,curp)) >= 0) {
	    NAM	*kp ;
	    rs = SR_NOTFOUND ;
	    if (rpp) *rpp = nullptr ;
	    if (curp->keyp == nullptr) {
	        kp = op->head ;
	        curp->valuep = nullptr ;
	    } else {
	        kp = curp->keyp ;
	        kp = kp->next ;
	    }
	    curp->keyp = kp ;
	    if (kp != nullptr) {
	        if (rpp != nullptr) *rpp = kp->name ;
	        rs = strlen(kp->name) ;
	    } /* end if */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (keyopt_enumkeys) */

int keyopt_fetch(keyopt *op,cchar *kname,CUR *curp,cchar **rpp) noex {
	int		rs ;
	if ((rs = keyopt_magic(op,kname)) >= 0) {
	    NAM		*kp ;
	    VAL		*vp ;
	    CUR		dcur ;
	    int		klen ;
	    cchar	*tp ;
	    if (curp == nullptr) {
	        curp = &dcur ;
	        curp->keyp = nullptr ;
	        curp->valuep = nullptr ;
	    }
	    if (curp->keyp == nullptr) {
/* do we have this key? */
	        klen = -1 ;
	        if ((tp = strchr(kname,'=')) != nullptr) {
	            klen = (tp - kname) ;
	        }
	        if ((rs = keyopt_findkey(op,kname,klen,&kp)) >= 0) {
	            curp->keyp = kp ;
	            vp = kp->head ;
	        }
	    } else {
	        kp = curp->keyp ;
	        vp = nullptr ;
	        if (curp->valuep != nullptr) {
	            vp = (curp->valuep)->next ;
	        }
	    } /* end if */
	    if (rpp) *rpp = nullptr ;
	    if (rs >= 0) {
	        if ((kp == nullptr) || (vp == nullptr)) {
	            rs = SR_NOENT ;
	        }
	        if (rs >= 0) {
	            if (rpp) *rpp = vp->value ;
	            rs = 0 ;
	            if (vp->value != nullptr) {
	                rs = strlen(vp->value) ;
	            }
	            curp->valuep = vp ;
	        } /* end if */
	    } /* end if (got one) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (keyopt_fetch) */

int keyopt_enumvalues(keyopt *op,cchar *key,CUR *curp,cchar **rpp) noex {
	int		rs ;
	if ((rs = keyopt_magic(op,key,curp)) >= 0) {
	    rs = keyopt_fetch(op,key,curp,rpp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (keyopt_enumvalues) */

int keyopt_incr(keyopt *op) noex {
	int		rs ;
	if ((rs = keyopt_magic(op)) >= 0) {
	    NAM		*pp = op->head ;
	    rs = SR_NOTFOUND ;
	    if (pp->next != nullptr) {
	        rs = keyname_incri(pp->next) ;
	    }
/* increment ourselves if we are at bottom or if previous guy carried */
	    if (rs == SR_NOTFOUND) {
	        VAL	*vp = pp->current ;
	        if (vp->next == nullptr) {
	            pp->current = pp->head ;
	            rs = -1 ;
	        } else {
	            pp->current = vp->next ;
	            rs = 0 ;
	        }
	    } /* end if (not found) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (keyopt_incr) */

int keyopt_curbegin(keyopt *op,CUR *curp) noex {
	int		rs ;
	if ((rs = keyopt_magic(op,curp)) >= 0) {
	    curp->keyp = nullptr ;
	    curp->valuep = nullptr ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (keyopt_curbegin) */

int keyopt_curend(keyopt *op,CUR *curp) noex {
	int		rs ;
	if ((rs = keyopt_magic(op)) >= 0) {
	    curp->keyp = nullptr ;
	    curp->valuep = nullptr ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (keyopt_curend) */

int keyopt_findvalue(keyopt *op,cc *key,cc *value,int vlen,VAL **rpp) noex {
	int		rs ;
	if ((rs = keyopt_magic(op,key,value)) >= 0) {
	    NAM		*kp ;
	    cchar	*tp ;
	    int		klen = -1 ;
	    if (vlen < 0) vlen = strlen(value) ;
/* do we have this key? */
	    if ((tp = strchr(key,'=')) != nullptr) {
	        klen = (tp - key) ;
	    }
	    if ((rs = keyopt_findkey(op,key,klen,&kp)) >= 0) {
	        if (vlen < 0) vlen = strlen(value) ;
	        rs = keyname_findv(kp,value,vlen,rpp) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (keyopt_findvalue) */


/* private subroutines */

static int keyopt_findkey(keyopt *op,cchar *key,int klen,NAM **rpp) noex {
	NAM		*pp ;
	for (pp = op->head ; pp != nullptr ; pp = pp->next) {
	    bool	f ;
	    if (klen < 0) {
	        f = (strkeycmp(pp->name,key) == 0) ;
	    } else {
		f = true ;
	        f = f && (strncmp(pp->name,key,klen) == 0) ;
		f = f && ((pp->name[klen] == '=') || (pp->name[klen] == '\0')) ;
	    }
	    if (f) break ;
	} /* end for */
	if (rpp) *rpp = pp ;
	return (pp != nullptr) ? SR_OK : SR_NOTFOUND ;
}
/* end subroutine (keyopt_findkey) */

static int keyopt_loadpair(keyopt *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (sp) {
	    cchar	*keyp{} ;
	    int		klen ;
	    rs = SR_OK ;
	    if ((klen = sfshrink(sp,sl,&keyp)) > 0) {
	        int	vlen = 0 ;
	        cchar	*valuep = nullptr ;
	        cchar	*tp ;
	        char	keybuf[KEYBUFLEN + 1] ;
	        if ((tp = strnchr(keyp,klen,'=')) != nullptr) {
	            valuep = (tp + 1) ;
	            vlen = (keyp + klen) - valuep ;
	            klen = (tp - keyp) ;
	            while ((klen > 0) && CHAR_ISWHITE(keyp[klen - 1])) {
	                klen -= 1 ;
		    }
	            while ((vlen > 0) && CHAR_ISWHITE(*valuep)) {
	                valuep += 1 ;
	                vlen -= 1 ;
	            }
	        } /* end if */
	        strwcpy(keybuf,keyp,MIN(klen,KEYBUFLEN)) ; /* cannot fail */
	        rs = keyopt_loadvalue(op,keybuf,valuep,vlen) ;
	    } /* end if (positive) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (keyopt_loadpair) */

/* KEYNAME begin */

static int keyname_incri(NAM *pp) noex {
	VAL		*vp ;
	int		rs = SR_NOTFOUND ;
	if (pp->next != nullptr) {
	    rs = keyname_incri(pp->next) ;
	}
/* increment ourselves if we are at bottom or if previous guy carried */
	if (rs == SR_NOTFOUND) {
	    vp = pp->current ;
	    if (vp->next == nullptr) {
	        pp->current = pp->head ;
	        rs = -1 ;
	    } else {
	        pp->current = vp->next ;
	        rs = 0 ;
	    } /* end if */
	} /* end if (not found) */
	return rs ;
}
/* end subroutine (keyname_incri) */

static int keyname_findv(NAM *pp,cchar *vbuf,int vlen,VAL **rp) noex {
	VAL		*vp ;
	bool		f = false ;
	if (vlen < 0) vlen = strlen(vbuf) ;
	for (vp = pp->head ; vp != nullptr ; vp = vp->next) {
	    f = (strncmp(vp->value,vbuf,vlen) == 0) ;
	    f = f && (vp->value[vlen] == '\0') ;
	    if (f) break ;
	} /* end for */
	if (rp) *rp = vp ;
	return (f) ? SR_OK : SR_NOTFOUND ;
}
/* end subroutine (keyname_findv) */

/* KEYNAME end */


