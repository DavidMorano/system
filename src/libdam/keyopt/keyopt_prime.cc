/* keyopt_prime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* paramater option manipulations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This code module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	keyopt

	Description:
	This object is similar to the PARAMOPT object in some ways.
	This object does not provide (natually) any exponetial
	enumeration of the gathered options (like PARAMOPT does).
	Rather it allows several options (different options) to be
	specified together (or strung together) separated by commas.
	This is actually quite natural for most applications.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>		/* |strncmp(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<sfx.h>
#include	<strn.h>
#include	<strkeycmp.h>
#include	<strwcpy.h>
#include	<char.h>
#include	<localmisc.h>

#include	"keyopt.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	KEYBUFLEN	100

#define	KO		keyopt
#define	CUR		keyopt_cur
#define	VAL		keyopt_val
#define	NAM		keyopt_na


/* imported namespaces */

using std::min ;			/* subroutine */
using std::max ;			/* subroutine */
using keyopt_keyname::keyname_incri ;	/* subroutine */
using keyopt_keyname::keyname_findv ;	/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

extern "C" {
    int		keyopt_loads(keyopt *,cchar *,int) noex ;
    int		keyopt_load(keyopt *,cchar *,int) noex ;
    int		keyopt_loadvalue(keyopt *,cchar *,cchar *,int) noex ;
}

local int	keyopt_findkey(keyopt *,cchar *,int,NAM **) noex ;
local int	keyopt_loadpair(keyopt *,cchar *,int) noex ;


/* local variables */

cint		keybuflen = KEYBUFLEN ;


/* exported variables */


/* exported subroutines */

int keyopt_start(keyopt *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    op->head = nullptr ;
	    op->tail = nullptr ;
	    op->cnt = 0 ;
	    op->magval = KEYOPT_MAGIC ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (keyopt_start) */

int keyopt_finish(keyopt *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = keyopt_magic(op)) >= 0) {
	    for (NAM *nnp, *sp = op->head ; sp ; sp = nnp) {
	        for (VAL *nvp, *vp = sp->head ; vp ; vp = nvp) {
	            if (vp->value) {
			void *p = voidp(vp->value) ;
	                rs1 = lm_free(p) ;
		        if (rs >= 0) rs = rs1 ;
			vp->value = nullptr ;
	            }
	            nvp = vp->next ;
	            rs1 = lm_free(vp) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end for */
	        if (sp->name) {
		    void *p = voidp(sp->name) ;
	            rs1 = lm_free(p) ;
		    if (rs >= 0) rs = rs1 ;
		    sp->name = nullptr ;
	        }
	        nnp = sp->next ;
	        rs1 = lm_free(sp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end for */
	    op->head = nullptr ;
	    op->tail = nullptr ;
	    op->cnt = 0 ;
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (keyopt_finish) */

int keyopt_loads(keyopt *op,cchar *sp,int sl) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = keyopt_magic(op,sp)) >= 0) {
	    if (sl <= 0) sl = lenstr(sp) ;
	    for (cchar *tp ; (tp = strnbrk(sp,sl,",\t\n\r ")) != np ; ) {
	        cchar	*cp = sp ;
	        if (cint cl = intconv(tp - sp) ; cl > 0) {
	            rs = keyopt_loadpair(op,cp,cl) ;
	            c += rs ;
	        }
	        sl -= intconv((tp + 1) - sp) ;
	        sp = (tp + 1) ;
	        if (rs < 0) break ;
	    } /* end for */
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
	    NAM		*pp = nullptr ; /* used-afterwards */
	    cchar	*cp ;
	    int		klen = -1 ;
	    /* clean up the value a little */
	    if (vlen < 0) vlen = (vbuf) ? lenstr(vbuf) : 0 ;
	    /* do we have one of these named keys already? */
	    if (cchar *tp ; (tp = strchr(key,'=')) != nullptr) {
	        klen = intconv(tp - key) ;
	    }
	    if ((rs = keyopt_findkey(op,key,klen,&pp)) == SR_NOTFOUND) {
	        cint	nsz = szof(NAM) ;
	        /* make a new parameter header block */
	        if ((rs = lm_mall(nsz,&pp)) >= 0) {
	            pp->cnt = 0 ;
	            pp->next = nullptr ;
	            pp->head = nullptr ;
	            pp->tail = nullptr ;
	            pp->current = nullptr ;
	            if ((rs = lm_strw(key,klen,&cp)) >= 0) {
	                NAM *tpp = op->tail ;
	                pp->name = cp ;
	                op->tail = pp ;
	                if (tpp) {
	                    tpp->next = pp ;
		        }
	                if (op->head == nullptr) {
	                    op->head = pp ;
		        }
	            } else {
	                lm_free(pp) ;
	                pp = nullptr ;
	            }
	        } /* end if (memory allocation) */
	    } /* end if (adding a new parameter block on the list) */
    /* OK, now we have the parameter block that we are looking for in 'pp' */
	    if ((rs >= 0) && pp) {
	        cint	vsz = szof(VAL) ;
	        if (VAL *nvp ; (rs = lm_mall(vsz,&nvp)) >= 0) {
	            nvp->next = nullptr ;
	            nvp->value = nullptr ;
	            if (vbuf) {
	                if ((rs = lm_strw(vbuf,vlen,&cp)) >= 0) {
	                    nvp->value = cp ;
	                } else {
	                    lm_free(nvp) ;
		        }
	            } /* end if (new value) */
	            if (rs >= 0) {
	                if (pp->head) {
	    		    VAL *vp = pp->tail ;
	                    vp->next = nvp ;
	                    pp->tail = nvp ;
	                } else {
	                    pp->head = nvp ;
	                    pp->tail = nvp ;
	                }
	                pp->cnt += 1 ;
	            } /* end if (ok) */
	            if (rs >= 0) op->cnt += 1 ;
	        } /* end if (memory-allocation) */
	    } /* end if (ok) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (keyopt_loadvalue) */

int keyopt_count(keyopt *op) noex {
	int		rs ;
	if ((rs = keyopt_magic(op)) >= 0) {
	    rs = op->cnt ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (keyopt_count) */

int keyopt_enumkeys(keyopt *op,CUR *curp,cchar **rpp) noex {
	int		rs ;
	if ((rs = keyopt_magic(op,curp)) >= 0) {
	    NAM	*kp ; /* used-afterwards */
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
	    if (kp) {
	        if (rpp) *rpp = kp->name ;
	        rs = lenstr(kp->name) ;
	    } /* end if */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (keyopt_enumkeys) */

int keyopt_fetch(keyopt *op,cchar *kname,CUR *curp,cchar **rpp) noex {
	int		rs ;
	if ((rs = keyopt_magic(op,kname)) >= 0) {
	    NAM		*kp = nullptr ;
	    VAL		*vp = nullptr ;
	    CUR		dcur ;
	    if (curp == nullptr) {
	        curp = &dcur ;
	        curp->keyp = nullptr ;
	        curp->valuep = nullptr ;
	    }
	    if (curp->keyp == nullptr) {
		/* do we have this key? */
	        int klen = -1 ;
	        if (cchar *tp ; (tp = strchr(kname,'=')) != nullptr) {
	            klen = intconv(tp - kname) ;
	        }
	        if ((rs = keyopt_findkey(op,kname,klen,&kp)) >= 0) {
	            curp->keyp = kp ;
	            vp = kp->head ;
	        }
	    } else {
	        kp = curp->keyp ;
	        vp = nullptr ;
	        if (curp->valuep) {
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
	            if (vp->value) {
	                rs = lenstr(vp->value) ;
	            }
	            curp->valuep = vp ;
	        } /* end if (ok) */
	    } /* end if (ok) */
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
	    if (pp->next) {
	        rs = keyname_incri(pp->next) ;
	    }
	    /* increment ourselves if at bottom or if previous carried */
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

int keyopt_findvalue(keyopt *op,cc *key,cc *valp,int µvall,VAL **rpp) noex {
	int		rs ;
	if ((rs = keyopt_magic(op,key,valp)) >= 0) {
	    if (int vall ; (vall = getlenstr(valp,µvall)) >= 0) {
	        int	klen = -1 ;
	        /* do we have this key? */
	        if (cchar *tp ; (tp = strchr(key,'=')) != nullptr) {
	            klen = intconv(tp - key) ;
	        }
	        if (NAM *kp ; (rs = keyopt_findkey(op,key,klen,&kp)) >= 0) {
	            rs = keyname_findv(kp,valp,vall,rpp) ;
	        }
	    } /* end if (getlenstr) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (keyopt_findvalue) */


/* private subroutines */

local int keyopt_findkey(keyopt *op,cchar *keyp,int keyl,NAM **rpp) noex {
	NAM		*pp ; /* used-afterwards */
	for (pp = op->head ; pp ; pp = pp->next) {
	    cchar *namp = pp->name ;
	    bool f ; /* used-afterwards */
	    if (keyl < 0) {
	        f = (strkeycmp(namp,keyp) == 0) ;
	    } else {
		f = true ;
	        f = f && (strncmp(namp,keyp,keyl) == 0) ;
		f = f && ((namp[keyl] == '=') || (namp[keyl] == '\0')) ;
	    }
	    if (f) break ;
	} /* end for */
	if (rpp) *rpp = pp ;
	return (pp) ? SR_OK : SR_NOTFOUND ;
}
/* end subroutine (keyopt_findkey) */

local int keyopt_loadpair(keyopt *op,cchar *sp,int sl) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (sp) ylikely {
	    cchar	*keyp{} ;
	    rs = SR_OK ;
	    if (int klen ; (klen = sfshrink(sp,sl,&keyp)) > 0) {
	        int	vlen = 0 ; /* used-afterwards */
	        cchar	*valuep = nullptr ;
	        char	keybuf[keybuflen + 1] ;
	        if (cchar *tp ; (tp = strnchr(keyp,klen,'=')) != np) {
	            valuep = (tp + 1) ;
	            vlen = intconv((keyp + klen) - valuep) ;
	            klen = intconv(tp - keyp) ;
	            while ((klen > 0) && CHAR_ISWHITE(keyp[klen - 1])) {
	                klen -= 1 ;
		    }
	            while ((vlen > 0) && CHAR_ISWHITE(*valuep)) {
	                valuep += 1 ;
	                vlen -= 1 ;
	            }
	        } /* end if */
		cint kl = min(klen,keybuflen) ;
	        strwcpy(keybuf,keyp,kl) ; /* cannot fail */
	        rs = keyopt_loadvalue(op,keybuf,valuep,vlen) ;
	    } /* end if (positive) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (keyopt_loadpair) */

int keyopt::loads(cchar *sp,int sl) noex {
	return keyopt_loads(this,sp,sl) ;
}

int keyopt::load(cchar *sp,int sl) noex {
	return keyopt_load(this,sp,sl) ;
}

int keyopt::loadvalue(cchar *key,cchar *vbuf,int vlen) noex {
	return keyopt_loadvalue(this,key,vbuf,vlen) ;
}

int keyopt::enumkeys(CUR *curp,cchar **rpp) noex {
	return keyopt_enumkeys(this,curp,rpp) ;
}

int keyopt::fetch(cchar *kname,CUR *curp,cchar **rpp) noex {
	return keyopt_fetch(this,kname,curp,rpp) ;
}

int keyopt::enumvalues(cchar *key,CUR *curp,cchar **rpp) noex {
	return keyopt_enumvalues(this,key,curp,rpp) ;
}

int keyopt::curbegin(CUR *curp) noex {
	return keyopt_curbegin(this,curp) ;
}

int keyopt::curend(CUR *curp) noex {
	return keyopt_curend(this,curp) ;
}

int keyopt::findvalue(cc *key,cc *valp,int vall,VAL **rpp) noex {
	return keyopt_findvalue(this,key,valp,vall,rpp) ;
}

void keyopt::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("keyopt",rs,"fini-finish") ;
	}
} /* end method (keyopt::dtor) */

keyopt::operator int () noex {
    	int		rs = SR_NOTOPEN ;
	if (magval == KEYOPT_MAGIC) {
	    rs = cnt ;
	}
	return rs ;
} /* end method (keyopt::operator) */

keyopt_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case keyoptmem_start:
	        rs = keyopt_start(op) ;
	        break ;
	    case keyoptmem_incr:
		rs = keyopt_incr(op) ;
	        break ;
	    case keyoptmem_count:
	        rs = keyopt_count(op) ;
	        break ;
	    case keyoptmem_finish:
	        rs = keyopt_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (keyopt_co::operator) */


