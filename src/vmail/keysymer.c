/* keysymer SUPPORT */
/* lang=C++20 */

/* keysym name-value database */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-01-20, David A­D­ Morano
	This was written from scratch.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object provides access to a keysym name-value database.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<strings.h>		/* |strncasecmp(3c)| */
#include	<usystem.h>
#include	<estrings.h>
#include	<mapstrint.h>
#include	<bfile.h>
#include	<cfnum.h>
#include	<ascii.h>
#include	<sfx.h>
#include	<strwcpy.h>
#include	<char.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"keysymer.h"


/* local defines */

#define	KEYSYMER_INCDNAME	"include"
#define	KEYSYMER_KSFNAME	"keysym.h"


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int keysymer_parse(keysymer *,cchar *) noex ;
static int keysymer_parseline(keysymer *,cchar *,int) noex ;
static int keysymer_process(keysymer *,cchar *,int,int) noex ;
static int keysymer_finishthem(keysymer *) noex ;
static int keysymer_seen(keysymer *,cchar *,int,int *) noex ;

static int cfliteral(cchar *,int,int *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int keysymer_open(keysymer *op,cchar *pr) noex {
	int		rs = SR_OK ;
	char		tmpfname[MAXPATHLEN + 1] ;

	if (op == NULL) return SR_FAULT ;
	if (pr == NULL) return SR_FAULT ;

	if (pr[0] == '\0') return SR_INVALID ;

	memset(op,0,sizeof(KEYSYMER)) ;

	if ((rs = mapstrint_start(&op->map,20)) >= 0) {
	    USTAT	sb ;
	    if ((rs = u_stat(pr,&sb)) >= 0) {
		if (S_ISDIR(sb.st_mode)) {
		    cchar	*idn = KEYSYMER_INCDNAME ;
		    cchar	*kfn = KEYSYMER_KSFNAME ;
		    if ((rs = mkpath3(tmpfname,pr,idn,kfn)) >= 0) {
			if ((rs = keysymer_parse(op,tmpfname)) >= 0) {
			    op->magic = KEYSYMER_MAGIC ;
			}
		    }
		} else
	    	    rs = SR_NOTDIR ;
	    } /* end if (stat) */
	    if (rs < 0)
	        mapstrint_finish(&op->map) ;
	} /* end if (mapstrint_start) */

	return rs ;
}
/* end subroutine (keysymer_open) */

int keysymer_close(keysymer *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != KEYSYMER_MAGIC) return SR_NOTOPEN ;

	rs1 = keysymer_finishthem(op) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = mapstrint_finish(&op->map) ;
	if (rs >= 0) rs = rs1 ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (keysymer_close) */

int keysymer_count(keysymer *op) noex {
	int		rs = SR_OK ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != KEYSYMER_MAGIC) return SR_NOTOPEN ;

	rs = mapstrint_count(&op->map) ;

	return rs ;
}
/* end subroutine (keysymer_count) */

int keysymer_lookup(keysymer *op,cchar *kp,int kl) noex {
	int		rs = SR_OK ;
	int		v = 0 ;
	char		knbuf[KEYSYMER_NAMELEN + 1] ;

	if (op == NULL) return SR_FAULT ;
	if (kp == NULL) return SR_FAULT ;

	if (op->magic != KEYSYMER_MAGIC) return SR_NOTOPEN ;

	if (kp[0] == '\0') return SR_INVALID ;

	if (kl < 0) kl = strlen(kp) ;

	if (hasuc(kp,kl)) {
	    kl = strwcpylc(knbuf,kp,MIN(kl,KEYSYMER_NAMELEN)) - knbuf ;
	    kp = knbuf ;
	}

	rs = mapstrint_fetch(&op->map,kp,kl,NULL,&v) ;

	return (rs >= 0) ? v : rs ;
}
/* end subroutine (keysymer_lookup) */

int keysymer_curbegin(keysymer *op,KEYSYMER_CUR *curp) noex {
	int		rs ;

	if (op == NULL) return SR_FAULT ;
	if (curp == NULL) return SR_FAULT ;

	if (op->magic != KEYSYMER_MAGIC) return SR_NOTOPEN ;

	rs = mapstrint_curbegin(&op->map,&curp->c) ;

	return rs ;
}
/* end subroutine (keysymer_curbegin) */

int keysymer_curend(keysymer *op,KEYSYMER_CUR *curp) noex {
	int		rs ;

	if (op == NULL) return SR_FAULT ;
	if (curp == NULL) return SR_FAULT ;

	if (op->magic != KEYSYMER_MAGIC) return SR_NOTOPEN ;

	rs = mapstrint_curend(&op->map,&curp->c) ;

	return rs ;
}
/* end subroutine (keysymer_curend) */

int keysymer_enum(keysymer *op,KEYSYMER_CUR *curp,KEYSYMER_KE *rp) noex {
	int		rs ;
	int		nl = 0 ;
	int		v ;
	cchar	*np = NULL ;

	if (op == NULL) return SR_FAULT ;
	if (curp == NULL) return SR_FAULT ;
	if (rp == NULL) return SR_FAULT ;

	if (op->magic != KEYSYMER_MAGIC) return SR_NOTOPEN ;

	rp->keynum = 0 ;
	rp->keyname[0] = '\0' ;
	if ((rs = mapstrint_enum(&op->map,&curp->c,&np,&v)) >= 0) {
	    nl = rs ;
	    if (np != NULL) {
	        strwcpy(rp->keyname,np,MIN(nl,KEYSYMER_NAMELEN)) ;
	        rp->keynum = v ;
	    }
	}

	return (rs >= 0) ? nl : rs ;
}
/* end subroutine (keysymer_enum) */


/* private subroutines */

static int keysymer_parse(keysymer *op,cchar *fname) noex {
	bfile		dfile, *dfp = &dfile ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;

	if (fname == NULL) return SR_FAULT ;

	if ((rs = bopen(dfp,fname,"r",0666)) >= 0) {
	    cint	llen = LINEBUFLEN ;
	    int		len ;
	    int		sl ;
	    cchar	*sp ;
	    char	lbuf[LINEBUFLEN + 1] ;

	    while ((rs = breadln(dfp,lbuf,llen)) > 0) {
	        len = rs ;

		if (lbuf[len-1] == '\n') len -= 1 ;

		if ((sl = sfshrink(lbuf,len,&sp)) > 0) {
	            if (sp[0] == '#') { /* look for '#define' */
	        	if ((rs = keysymer_parseline(op,sp,sl)) >= 0) {
	        	    c += 1 ;
			}
		    }
		}

	        if (rs < 0) break ;
	    } /* end while */

	    rs1 = bclose(dfp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (open-file) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (keysymer_parse) */

static int keysymer_parseline(keysymer *op,cchar *lp,int ll) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;

	if (ll < 0) ll = strlen(lp) ;

	if ((ll > 1) && (lp[0] == '#')) {
	    int		sl = (ll-1) ;
	    int		cl ;
	    cchar	*sp = (lp+1) ;
	    cchar	*cp ;
	    if ((cl = nextfield(sp,sl,&cp)) > 0) {
		if (strncmp("define",cp,cl) == 0) {
		    sl -= ((cp+cl) - sp) ;
		    sp = (cp+cl) ;
		    if ((cl = nextfield(sp,sl,&cp)) > 0) {
			cchar	*tp ;
		        if ((tp = strnchr(cp,cl,'_')) != NULL) {
		            if (strncmp("KEYSYM",cp,(tp-cp)) == 0) {
				cint	kl = ((cp+cl) - (tp+1)) ;
			        int		nl ;
				cchar		*kp = (tp+1) ;
				cchar		*np ;
				if (strncasecmp("include",cp,cl) != 0) {

	sl -= ((cp + cl) - sp) ;
	sp = (cp + cl) ;
	if ((nl = nextfield(sp,sl,&np)) > 0) {
		int		kn ;

	if (np[0] == CH_SQUOTE) {
	    rs1 = cfliteral(np,nl,&kn) ;
	} else if (np[0] == 'K') {
	    rs1 = keysymer_seen(op,np,nl,&kn) ;
	} else {
	    rs1 = cfnumi(np,nl,&kn) ;
	}

	if (rs1 >= 0) {
	    rs = keysymer_process(op,kp,kl,kn) ;
	    c = rs ;
	}

	} /* end if (postive) */

			 	} /* end if (not "INCLUDE") */
	                    } /* end if (KEYSYM) */
	                } /* end if (marker) */
	            } /* end if (positive) */
	        } /* end if (define) */
	    } /* end if (deine-key) */
	} /* end if (pound) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (keysymer_parseline) */

static int keysymer_process(keysymer *op,cchar *kp,int kl,int kn) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	char		knbuf[KEYSYMER_NAMELEN + 1] ;

	if (hasuc(kp,kl)) {
	    kl = strwcpylc(knbuf,kp,MIN(kl,KEYSYMER_NAMELEN)) - knbuf ;
	    kp = knbuf ;
	}

	c = 1 ;
	rs = mapstrint_add(&op->map,kp,kl,kn) ;

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (keysymer_process) */

static int keysymer_finishthem(keysymer *op) noex {
	if (op == NULL) return SR_FAULT ;
	return SR_OK ; /* nothing to do */
}
/* end subroutine (keysymer_finishthem) */

static int keysymer_seen(keysymer *op,cchar *np,int nl,int *rp) noex {
	int		rs = SR_INVALID ;
	int		v = 0 ;
	cchar	*tp ;

	if (nl < 0) nl = strlen(np) ;

	if ((tp = strnchr(np,nl,'_')) != NULL) {
	    int		kl = ((np + nl) - (tp + 1)) ;
	    cchar	*kp = (tp + 1) ;
	    char	knbuf[KEYSYMER_NAMELEN + 1] ;
	    if (hasuc(kp,kl)) {
	        cint	ml = MIN(kl,KEYSYMER_NAMELEN) ;
	        kl = strwcpylc(knbuf,kp,ml) - knbuf ;
	        kp = knbuf ;
	    }
	    rs = mapstrint_fetch(&op->map,kp,kl,NULL,&v) ;
	}

	if (rp) {
	    *rp = (rs >= 0) ? v : 0 ;
	}

	return rs ;
}
/* end subroutine (keysymer_seen) */

static int cfliteral(cchar *np,int nl,int *rp) noex {
	int		rs = SR_INVALID ;
	int		v = 0 ;

	if (nl < 0) nl = strlen(np) ;

	if ((nl > 1) && (np[0] == CH_SQUOTE)) {
	    cchar	*tp ;
	    np += 1 ;
	    nl -= 1 ;
	    if ((tp = strnchr(np,nl,CH_SQUOTE)) != NULL) {
	        if ((tp - np) > 0) {
	            if (rp != NULL) v = (np[1] & 0xff) ;
	            rs = SR_OK ;
	        }
	    }
	}

	if (rp) {
	    *rp = (rs >= 0) ? v : 0 ;
	}

	return rs ;
}
/* end subroutine (cfliternal) */


