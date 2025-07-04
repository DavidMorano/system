/* keysymer SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* keysym name-value database */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-01-20, David A­D­ Morano
	This was written from scratch.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	keysymer

	Description:
	This object provides access to a KEY-SYM name-value database.
	The database is stored as a plain-text file, with a single
	record (make up of fields) on each single line.  Each valid
	line starts with the '#' character in column 1 and the word
	(sequence of characters) "define" following the initial '#'
	character.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<strings.h>		/* |strncasecmp(3c)| + |lenstr(3c)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<estrings.h>
#include	<mapstrint.h>
#include	<bfile.h>
#include	<bfliner.h>
#include	<cfnum.h>
#include	<ascii.h>
#include	<sfx.h>			/* |sfnext(3uc)| */
#include	<strwcpy.h>
#include	<rmx.h>
#include	<mkchar.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"keysymer.h"


/* local defines */

#define	KEYSYMER_INCDNAME	"include"
#define	KEYSYMER_KSFNAME	"keysym.h"

#define	KS		keysymer


/* local namespaces */

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
static int keysymer_ctor(keysymer *op,Args ... args) noex {
	KEYSYMER	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->mlp = new(nothrow) mapstrint) != np) {
		rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (keysymer_ctor) */

static int keysymer_dtor(keysymer *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->mlp) {
		delete op->mlp ;
		op->mlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (keysymer_dtor) */

template<typename ... Args>
static inline int keysymer_magic(keysymer *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == KEYSYMER_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (keysymer_magic) */

static int keysymer_parse(keysymer *,cchar *) noex ;
static int keysymer_parseln(keysymer *,cchar *,int) noex ;
static int keysymer_ks(KS *,cc *,int,cc *,int) noex ;
static int keysymer_process(keysymer *,cchar *,int,int) noex ;
static int keysymer_finishthem(keysymer *) noex ;
static int keysymer_seen(keysymer *,cchar *,int,int *) noex ;

static int cfliteral(cchar *,int,int *) noex ;


/* local variables */

constexpr int		keylen = KEYSYMER_NAMELEN ;


/* exported variables */


/* exported subroutines */

int keysymer_open(keysymer *op,cchar *pr) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = keysymer_ctor(op,pr)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
		cint	ne = 20 ;
	        if ((rs = mapstrint_start(op->mlp,ne)) >= 0) {
	            if (USTAT sb ; (rs = u_stat(pr,&sb)) >= 0) {
		        if (S_ISDIR(sb.st_mode)) {
	        	    if (char *tbuf{} ; (rs = malloc_mp(&tbuf)) >= 0) {
		                cchar	*idn = KEYSYMER_INCDNAME ;
		                cchar	*kfn = KEYSYMER_KSFNAME ;
		                if ((rs = mkpath(tbuf,pr,idn,kfn)) >= 0) {
			            if ((rs = keysymer_parse(op,tbuf)) >= 0) {
			                op->magic = KEYSYMER_MAGIC ;
			            }
		                } /* end if (mkpath) */
				rs1 = uc_free(tbuf) ;
				if (rs >= 0) rs = rs1 ;
			    } /* end if (m-a-f) */
		        } else {
	    	            rs = SR_NOTDIR ;
			}
	            } /* end if (stat) */
	            if (rs < 0) {
	                mapstrint_finish(op->mlp) ;
	            }
	        } /* end if (mapstrint_start) */
	    } /* end if (valid) */
	    if (rs < 0) {
		keysymer_dtor(op) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (keysymer_open) */

int keysymer_close(keysymer *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = keysymer_magic(op)) >= 0) {
	    {
	        rs1 = keysymer_finishthem(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = mapstrint_finish(op->mlp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = keysymer_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (keysymer_close) */

int keysymer_count(keysymer *op) noex {
	int		rs ;
	if ((rs = keysymer_magic(op)) >= 0) {
	    rs = mapstrint_count(op->mlp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (keysymer_count) */

int keysymer_lookup(keysymer *op,cchar *kp,int kl) noex {
	int		rs ;
	int		v = 0 ;
	if ((rs = keysymer_magic(op,kp)) >= 0) {
	    rs = SR_INVALID ;
	    if (kp[0]) {
	        char	knbuf[keylen + 1] ;
	        if (kl < 0) kl = lenstr(kp) ;
	        if (hasuc(kp,kl)) {
	            kl = strwcpylc(knbuf,kp,min(kl,keylen)) - knbuf ;
	            kp = knbuf ;
	        }
	        rs = mapstrint_fetch(op->mlp,kp,kl,nullptr,&v) ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? v : rs ;
}
/* end subroutine (keysymer_lookup) */

int keysymer_curbegin(keysymer *op,keysymer_cur *curp) noex {
	int		rs ;
	if ((rs = keysymer_magic(op,curp)) >= 0) {
	    rs = mapstrint_curbegin(op->mlp,&curp->c) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (keysymer_curbegin) */

int keysymer_curend(keysymer *op,keysymer_cur *curp) noex {
	int		rs ;
	if ((rs = keysymer_magic(op,curp)) >= 0) {
	    rs = mapstrint_curend(op->mlp,&curp->c) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (keysymer_curend) */

int keysymer_enum(keysymer *op,keysymer_cur *curp,keysymer_ke *rp) noex {
	int		rs ;
	int		sl = 0 ;
	if ((rs = keysymer_magic(op,curp,rp)) >= 0) {
	    int		v ;
	    cchar	*sp = nullptr ;
	    rp->keynum = 0 ;
	    rp->keyname[0] = '\0' ;
	    if ((rs = mapstrint_curenum(op->mlp,&curp->c,&sp,&v)) >= 0) {
	        sl = rs ;
	        if (sp) {
	            strwcpy(rp->keyname,sp,min(sl,keylen)) ;
	            rp->keynum = v ;
	        }
	    }
	} /* end if (magic) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (keysymer_enum) */


/* private subroutines */

static int keysymer_parse(keysymer *op,cchar *fname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (fname) {
	    bfile	dfile, *dfp = &dfile ;
	    if ((rs = bopen(dfp,fname,"r",0666)) >= 0) {
		if (bfliner bl ; (rs = bl.start(dfp)) >= 0) {
		    cchar	*lp ;
	            while ((rs = bl.getlns(&lp)) > 0) {
	                cint	len = rmeol(lp,rs) ;
	                cchar	*sp{} ;
		        if (int sl ; (sl = sfshrink(lp,len,&sp)) > 0) {
	                     if (sp[0] == '#') { /* look for '#define' */
	        	         if ((rs = keysymer_parseln(op,sp,sl)) >= 0) {
	        	             c += 1 ;
			         }
		             }
		         }
	                 if (rs < 0) break ;
	            } /* end while */
		    rs1 = bl.finish ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (bfliner) */
	        rs1 = bclose(dfp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (open-file) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (keysymer_parse) */

static int keysymer_parseln(keysymer *op,cchar *lp,int ll) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	if (ll < 0) ll = lenstr(lp) ;
	if ((ll > 1) && (lp[0] == '#')) {
	    int		sl = (ll-1) ;
	    cchar	*sp = (lp+1) ;
	    cchar	*cp{} ;
	    if (int cl ; (cl = sfnext(sp,sl,&cp)) > 0) {
		if (strncmp("define",cp,cl) == 0) {
		    sl -= ((cp+cl) - sp) ;
		    sp = (cp+cl) ;
		    if ((cl = sfnext(sp,sl,&cp)) > 0) {
			cnullptr	np{} ;
		        if (cchar *tp ; (tp = strnchr(cp,cl,'_')) != np) {
		            if (strncmp("KEYSYM",cp,(tp-cp)) == 0) {
				cint	kl = ((cp+cl) - (tp+1)) ;
				cchar	*kp = (tp+1) ;
				if (strncasecmp("include",cp,cl) != 0) {
				    sl -= ((cp + cl) - sp) ;
				    sp = (cp + cl) ;
				    rs = keysymer_ks(op,kp,kl,sp,sl) ;
				    c = rs ;
			 	} /* end if (not "INCLUDE") */
	                    } /* end if (KEYSYM) */
	                } /* end if (marker) */
	            } /* end if (positive) */
	        } /* end if (define) */
	    } /* end if (deine-key) */
	} /* end if (pound) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (keysymer_parseln) */

/* store a key-value string pair */
static int keysymer_ks(KS *op,cc *kp,int kl,cc *sp,int sl) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	cchar		*ip{} ;
	if (int il ; (il = sfnext(sp,sl,&ip)) > 0) {
	    int		kn ;
	    if (ip[0] == CH_SQUOTE) {
	        rs1 = cfliteral(ip,il,&kn) ;
	    } else if (ip[0] == 'K') {
	        rs1 = keysymer_seen(op,ip,il,&kn) ;
	    } else {
	        rs1 = cfnumi(ip,il,&kn) ;
	    }
	    if (rs1 >= 0) {
	        rs = keysymer_process(op,kp,kl,kn) ;
	        c = rs ;
	    }
	} /* end if (postive) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (keysymer_ks) */

static int keysymer_process(keysymer *op,cchar *kp,int kl,int kn) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	{
	    char	knbuf[keylen + 1] ;
	    if (hasuc(kp,kl)) {
	        kl = strwcpylc(knbuf,kp,min(kl,keylen)) - knbuf ;
	        kp = knbuf ;
	    }
	    c = 1 ;
	    rs = mapstrint_add(op->mlp,kp,kl,kn) ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (keysymer_process) */

static int keysymer_finishthem(keysymer *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (keysymer_finishthem) */

static int keysymer_seen(keysymer *op,cchar *sp,int sl,int *rp) noex {
	int		rs = SR_INVALID ;
	int		v = 0 ;
	if (sl < 0) sl = lenstr(sp) ;
	if (cchar *tp ; (tp = strnchr(sp,sl,'_')) != nullptr) {
	    int		kl = ((sp + sl) - (tp + 1)) ;
	    cchar	*kp = (tp + 1) ;
	    char	knbuf[keylen + 1] ;
	    if (hasuc(kp,kl)) {
	        cint	ml = min(kl,keylen) ;
	        kl = strwcpylc(knbuf,kp,ml) - knbuf ;
	        kp = knbuf ;
	    }
	    rs = mapstrint_fetch(op->mlp,kp,kl,nullptr,&v) ;
	} /* end if */
	if (rp) {
	    *rp = (rs >= 0) ? v : 0 ;
	}
	return rs ;
}
/* end subroutine (keysymer_seen) */

/* Convert-From-Literal */
static int cfliteral(cchar *sp,int sl,int *rp) noex {
	cint		sch = CH_SQUOTE ;
	int		rs = SR_INVALID ;
	int		v = 0 ;
	if (sl < 0) sl = lenstr(sp) ;
	if ((sl > 1) && (sp[0] == sch)) {
	    sp += 1 ;
	    sl -= 1 ;
	    if (cchar *tp ; (tp = strnchr(sp,sl,sch)) != nullptr) {
	        if ((tp - sp) > 0) {
	            if (rp) v = mkchar(sp[1]) ;
	            rs = SR_OK ;
	        }
	    }
	} /* end if (found start) */
	if (rp) {
	    *rp = (rs >= 0) ? v : 0 ;
	}
	return rs ;
}
/* end subroutine (cfliteral) */


