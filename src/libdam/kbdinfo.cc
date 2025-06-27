/* kbdinfo SUPPORT */
/* charset=ISO8859-1 */
/* version %I% last-modified %G% */

/* keyboard-information database access */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-01-20, David A­D­ Morano
	This was written from sratch.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	kbdinfo

	Description:
	This object provides access to a keyboard-information
	database.  This information is used in creating the name
	of a key pressed on a particular keyboard.

        Notes: 
	There is much to be desired with the implementation of this
	object.  We may have to revisit this object if we ever need
	to get some enhanced function out of function keys.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<vecobj.h>
#include	<bfile.h>
#include	<field.h>
#include	<strwcpyx.h>
#include	<sncpyx.h>
#include	<mkpathx.h>
#include	<sfx.h>
#include	<strwcpy.h>
#include	<matstr.h>
#include	<matxstr.h>
#include	<cfnum.h>
#include	<hasx.h>
#include	<ischarx.h>
#include	<localmisc.h>
#include	<termcmd.h>

#include	"keysymer.h"
#include	"kbdinfo.h"

import libutil ;

/* local defines */

#define	KI		kbdinfo
#define	KI_KEYNAMELEN	60
#define	KI_ENT		kbdinfo_ent
#define	KI_CUR		kbdinfo_cur

#undef	ENTRYINFO
#define	ENTRYINFO	entryinfo


/* imported namespaces */

using std::min ;			/* type */
using std::max ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct entryinfo {
	cchar		*fp ;
	int		fl ;
} ;


/* forward references */

template<typename ... Args>
static int kbdinfo_ctor(kbdinfo *op,Args ... args) noex {
    	KBDINFO		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (kbdinfo_ctor) */

static int kbdinfo_dtor(kbdinfo *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (kbdinfo_dtor) */

template<typename ... Args>
static inline int kbdinfo_magic(kbdinfo *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == KBDINFO_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (kbdinfo_magic) */

static int kbdinfo_parse(KI *,cchar *) noex ;
static int kbdinfo_parseline(KI *,cchar *,int) noex ;
static int kbdinfo_process(KI *,ENTRYINFO *,int) noex ;
static int kbdinfo_store(KI *,int,cchar *,int,ENTRYINFO *,int) noex ;
static int kbdinfo_kefins(KI *) noex ;

static int ke_start(KI_ENT *,int,cchar *,int,ENTRYINFO *,int) noex ;
static int ke_finish(KI_ENT *) noex ;

static int vcmpfind(cvoid **,cvoid **) noex ;


/* local variables */

constexpr cpcchar	keytypes[KBDINFO_TOVERLAST + 1] = {
	"reg",
	"esc",
	"csi",
	"dcs",
	"pf",
	"fkey",
	nullptr
} ; /* end array (keytypes) */

constexpr char		kterms[] = {
	0x00, 0x3E, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ; /* end array (kterms) */


/* exported variables */


/* exported subroutines */

int kbdinfo_open(KI *op,keysymer *ksp,cchar *fname) noex {
	int		rs ;
	if ((rs = kbdinfo_ctor(op,ksp,fname)) >= 0) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        cint	vsz = szof(KI_ENT) ;
	        cint	vo = VECOBJ_OSORTED ;
	        cint	ve = 4 ;
	        cint	n = KBDINFO_TOVERLAST ;
		int	i ; /* used-afterwards */
	        op->ksp = ksp ;
	        for (i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
		    vecobj *tlp = (op->types + i) ;
	            rs = tlp->start(vsz,ve,vo) ;
	        } /* end for */
	        if (rs >= 0) {
	            if ((rs = kbdinfo_parse(op,fname)) >= 0) {
		        op->magic = KBDINFO_MAGIC ;
	            }
	            if (rs < 0) {
		        kbdinfo_kefins(op) ;
		        for (i = 0 ; i < n ; i += 1) {
			    vecobj *tlp = (op->types + i) ;
	    	            tlp->finish() ;
		        }
	            } /* end if (error) */
	        } else {
		    /* variable 'i' from above */
	            for (int j = 0 ; j < i ; j += 1) {
			vecobj *tlp = (op->types + j) ;
	                tlp->finish() ;
	            }
	        } /* end if */
	    } /* end if (valid) */
	    if (rs < 0) {
		kbdinfo_dtor(op) ;
	    }
	} /* end if (kbdinfo_ctor) */
	return rs ;
}
/* end subroutine (kbdinfo_open) */

int kbdinfo_close(KI *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = kbdinfo_magic(op)) >= 0) {
	    {
	        rs1 = kbdinfo_kefins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    for (int i = 0 ; i < KBDINFO_TOVERLAST ; i += 1) {
	        vecobj *tlp = (op->types + i) ;
	        rs1 = tlp->finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end for f */
	    {
		rs1 = kbdinfo_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (kbdinfo_close) */

int kbdinfo_count(KI *op) noex {
	int		rs ;
	int		count = 0 ; /* return-value */
	if ((rs = kbdinfo_magic(op)) >= 0) {
    	    cint	n = KBDINFO_TOVERLAST ;
	    for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
	        vecobj *tlp = (op->types + i) ;
	        rs = tlp->count ;
	        count += rs ;
	    } /* end for */
	} /* end if (kbdinfo_magic) */
	return (rs >= 0) ? count : rs ;
}
/* end subroutine (kbdinfo_count) */

int kbdinfo_lookup(KI *op,char *ksbuf,int kslen,termcmd *cmdp) noex {
	cint		nps = 4 ;
	int		rs ;
	int		rs1 ;
	int		keynum = 0 ; /* return-value */
	if ((rs = kbdinfo_magic(op,ksbuf,cmdp)) >= 0) {
	    int		ktype = kbdinfot_fkey ;
	    short	params[nps + 1] ;
	    if (cmdp->name != '~') {
	        ktype = cmdp->type ;
	    }
	    KI_ENT	te{} ;
	    te.type = ktype ;
	    te.name = cmdp->name ;
	    te.p = params ;
	    if (cmdp->p[0] >= 0) {
	        cint	n = MIN(nps,TERMCMD_NP) ;
	        int	i ; /* used-afterwards */
	        for (i = 0 ; i < n ; i += 1) {
	            te.p[i] = cmdp->p[i] ;
		    if (cmdp->p[i] < 0) break ;
	        }
	        te.nparams = i ;
	    } /* end if */
	    void *vp ;
	    vecobj *tlp = (op->types + ktype) ;
	    if ((rs = tlp->search(&te,vcmpfind,&vp)) >= 0) {
	        KI_ENT	*ep = (KI_ENT *) vp ;
	        if (vp) {
	            if (ep->keynum < 0) {
	                if (op->ksp != nullptr) {
	                    rs1 = keysymer_lookup(op->ksp,ep->keyname,-1) ;
        
	                    if (rs1 >= 0) {
	                        ep->keynum = rs1 ;
	                        keynum = rs1 ;
	                    }
	                }
	            } else {
	                keynum = ep->keynum ;
	            }
	            if (ksbuf) {
		        rs = sncpy1(ksbuf,kslen,ep->keyname) ;
	            }
	        } /* end if (non-null) */
	    } else {
	        if (ksbuf) ksbuf[0] = '\0' ;
	    } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? keynum : rs ;
}
/* end subroutine (kbdinfo_lookup) */

int kbdinfo_curbegin(KI *op,KI_CUR *curp) noex {
    	int		rs ;
	if ((rs = kbdinfo_magic(op,curp)) >= 0) {
	    rs = memclear(curp) ;
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (kbdinfo_curbegin) */

int kbdinfo_curend(KI *op,KI_CUR *curp) noex {
    	int		rs ;
	if ((rs = kbdinfo_magic(op,curp)) >= 0) {
	    rs = memclear(curp) ;
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (kbdinfo_curend) */

int kbdinfo_curenum(KI *op,KI_CUR *curp,KI_ENT **rpp) noex {
	int		rs ;
	int		nl = 0 ; /* return-value */
	if ((rs = kbdinfo_magic(op,curp,rpp)) >= 0) {
	    cint	n  = KBDINFO_TOVERLAST ;
	    int	i = curp->i ;
	    int j ;
	    if (i < 0) {
	        i = 0 ;
	        j = 0 ;
	    } else {
	        j = (curp->j + 1) ;
	    }
	    if (i < n) {
	        void	*vp ;
	        if ((rs = vecobj_get((op->types + i),j,&vp)) >= 0) {
		    *rpp = (KI_ENT *) vp ;
	        } else if ((rs == SR_NOTFOUND) && (i < n)) {
	            i += 1 ;
	            j = 0 ;
		    vecobj	*tlp = (op->types + i) ;
	            if ((rs = tlp->get(j,&vp)) >= 0) {
		        *rpp = (KI_ENT *) vp ;
		    }
	        }
	    } /* end if (within) */
	    if ((rs >= 0) && (*rpp != nullptr)) {
	        curp->i = i ;
	        curp->j = j ;
	        if ((*rpp)->keyname != nullptr) {
	            nl = lenstr((*rpp)->keyname) ;
	        }
	    }
	} /* end if (magic) */
	return (rs >= 0) ? nl : rs ;
}
/* end subroutine (kbdinfo_curenum) */


/* private subroutines */

static int kbdinfo_parse(KI *op,cchar *fname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (fname) {
	    if (char *lbuf ; (rs = malloc_ml(&lbuf)) >= 0) {
		cint	llen = rs ;
	        if (bfile df ; (rs = df.open(fname,"r",0)) >= 0) {
	            while ((rs = df.readln(lbuf,llen)) > 0) {
			cchar	*sp ;
			if (int sl ; (sl = sfcontent(lbuf,rs,&sp)) > 0) {
	                    if ((rs = kbdinfo_parseline(op,sp,sl)) > 0) {
	                        c += 1 ;
			    }
			}
	                if (rs < 0) break ;
		    } /* end while */
	            rs1 = df.close ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (open-file) */
	        rs = rsfree(rs,lbuf) ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (kbdinfo_parse) */

static int kbdinfo_parseline(KI *op,cchar *lp,int ll) noex {
	ENTRYINFO	eis[20] = {} ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if (field fsb ; (rs = fsb.start(lp,ll)) >= 0) {
	    cint	n = nelem(eis) ;
	    int		i = 0 ; /* used-afterwards */
	    cchar	*fp ;
	    for (int fl ; (i < n) && ((fl = fsb.get(kterms,&fp)) > 0) ; ) {
	        eis[i].fp = fp ;
	        eis[i].fl = fl ;
		i += 1 ;
	        if (fsb.term == '#') break ;
	    } /* end for */
	    if (i >= 3) {
	        rs = kbdinfo_process(op,eis,i) ;
	        c = rs ;
	    }
	    rs1 = field_finish(&fsb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (field) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (kbdinfo_parseline) */

static int kbdinfo_process(KI *op,ENTRYINFO *eis,int n) noex {
	int		rs = SR_OK ;
	int		ktl = eis[1].fl ;
	int		c = 0 ; /* return-value */
	cchar		*ktp = eis[1].fp ;
	if (int ktype ; (ktype = matocasestr(keytypes,2,ktp,ktl)) >= 0) {
	    int		knl = eis[0].fl ;
	    cchar	*knp = eis[0].fp ;
	    c = 1 ;
	    rs = kbdinfo_store(op,ktype,knp,knl,eis,n) ;
	} /* end if (match) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (kbdinfo_process) */

static int kbdinfo_store(KI *op,int ktype,cchar *knp,int knl,
		ENTRYINFO *eis,int n) noex {
	int		rs = SR_OK ;
	char		keybuf[KI_KEYNAMELEN + 1] ;
	if (knl < 0) knl = lenstr(knp) ;
	if (hasuc(knp,knl)) {
	    cint	ml = MIN(knl,KI_KEYNAMELEN) ;
	    knl = intconv(strwcpylc(keybuf,knp,ml) - keybuf) ;
	    knp = keybuf ;
	}
	if (KI_ENT e ; (rs = ke_start(&e,ktype,knp,knl,eis,n)) >= 0) {
	    rs = vecobj_add((op->types + ktype),&e) ;
	    if (rs < 0) {
	        ke_finish(&e) ;
	    }
	}
	return rs ;
}
/* end subroutine (kbdinfo_store) */

static int kbdinfo_kefins(KI *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	for (int i = 0 ; i < KBDINFO_TOVERLAST ; i += 1) {
	    vecobj	*tlp = (op->types + i) ;
	    void	*vp ;
	    for (int j = 0 ; tlp->get(j,&vp) >= 0 ; j += 1) {
		KI_ENT	*ep = (KI_ENT *) vp ;
	        if (vp) {
	            rs1 = ke_finish(ep) ;
	            if (rs >= 0) rs = rs1 ;
		}
	    } /* end for */
	} /* end for */
	return rs ;
}
/* end subroutine (kbdinfo_kefins) */

#ifdef	COMMENT
struct kbdinfo_e {
	cchar	*a		/* the memory allocation */
	cchar	*keyname ;	/* keysym-name */
	cchar	*istr ;
	cchar	*dstr ;
	short		*p ;		/* parameters */
	int		type ;		/* key type */
	int		name ;		/* key name */
	int		keynum ;	/* key number */
	int		np ;		/* number of paramters */
} ;
# keysym	type	final	inter	param(s)
F6              FKEY	-	-	17
F7              FKEY	-	-	18
#endif /* COMMENT */

static int ke_start(KI_ENT *kep,int ktype,cchar *knp,int knl,
		ENTRYINFO *eis,int n) noex {
	cint		oi = 4 ;
	int		rs = SR_FAULT ;
	int		nparams = 0 ;
	int		sz = 0 ;
	if (kep && knp) {
	    rs = SR_INVALID ;
	    if (knp[0]) {
	        rs = memclear(kep) ;
	kep->type = ktype ;
	kep->keynum = -1 ;
	{
	    int	name = (eis[2].fp[0] & 0xff) ;
	    if (ktype == KBDINFO_TFKEY) name = '~' ;
	    kep->name = name ;
	}
	sz += (xstrnlen(knp,knl) + 1) ;
	if (n > 3) {
	    sz += (xstrnlen(eis[3].fp,eis[3].fl) + 1) ;
	} else {
	    sz += 1 ;
	}
	if (n > oi) {
	    for (int i = oi ; i < n ; i += 1) {
	        if ((eis[i].fl > 0) && (eis[i].fp[0] != '-')) {
		    nparams = (i+1-oi) ;
	        }
	    }
	    sz += (nparams * szof(short)) ;
	}

	if (char *bp ; (rs = uc_malloc(sz,&bp)) >= 0) {
	    int		pi ;
	    int		fl ;
	    int		v ;
	    short	*pp ;
	    cchar	*fp ;

	    kep->a = bp ;
	    kep->nparams = nparams ;
	    kep->p = (short *) bp ;
	    pp = (short *) bp ;
	    for (pi = 0 ; pi < nparams ; pi += 1) {
		v = 0 ;
		fl = eis[pi+oi].fl ;
		fp = eis[pi+oi].fp ;
		if (fl > 0) {
		    if ((fp != nullptr) && (fp[0] != '-')) {
		        rs = cfnumi(fp,fl,&v) ;
		    }
		}
		pp[pi] = (v & SHORT_MAX) ;
		if (rs < 0) break ;
	    } /* end for */
	    bp += (nparams * szof(short)) ;

	    if (rs >= 0) {
	        kep->keyname = bp ;
		bp = (strwcpy(bp,knp,knl) + 1) ;
	    }

	    if (rs >= 0) {
	        kep->istr = bp ;
		fp = eis[3].fp ;
		fl = eis[3].fl ;
		if ((fl > 0) && (fp[0] != '-')) {
		    bp = (strwcpy(bp,fp,fl) + 1) ;
		} else {
		    *bp++ = '\0' ;
		}
	    }

	    if (rs < 0) {
		uc_free(kep->a) ;
		kep->a = nullptr ;
	    }
	} /* end if (memory-allocation) */

	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ke_start) */

static int ke_finish(KI_ENT *kep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (kep) {
	    rs = SR_OK ;
	    if (kep->a) {
	        rs1 = uc_free(kep->a) ;
	        if (rs >= 0) rs = rs1 ;
	        kep->a = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ke_finish) */

static int cmpent(KI_ENT *e1p,KI_ENT *e2p) noex {
    	int		rc ;
	if ((rc = (e1p->name - e2p->name)) == 0) {
	    if ((e1p->nparams > 0) && (e2p->nparams > 0)) {
		rc = (e1p->p[0] - e2p->p[0]) ;
	    } else if (e1p->nparams > 0) {
		rc = +1 ;
	    } else if (e2p->nparams > 0) {
		rc = -1 ;
	    }
	}
	return rc ;
} /* end subroutine (cmpent) */

static int vcmpfind(cvoid **v1pp,cvoid **v2pp) noex {
	KI_ENT		**e1pp = (KI_ENT **) v1pp ;
	KI_ENT		**e2pp = (KI_ENT **) v2pp ;
	int		rc = 0 ;
	{
	    KI_ENT	*e1p = (KI_ENT *) *e1pp ;
	    KI_ENT	*e2p = (KI_ENT *) *e2pp ;
	    if (e1p || e2p) {
	        if (e1p) {
	            if (e2p) {
		        rc = cmpent(e1p,e2p) ;
	            } else {
	                rc = -1 ;
		    }
	        } else {
	            rc = +1 ;
	        }
	    }
	} /* end block */
	return rc ;
}
/* end subroutine (vcmpfind) */


