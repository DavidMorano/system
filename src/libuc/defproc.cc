/* defproc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* process a "def" (define) file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	NameL
	defproc

	Description:
	This subroutine will read (process) an "def" (define) file
	and put all of the define-variables into an VECSTR
	object (supplied).  New define-variables just get added
	to the list.  Old define-variables already on the list
	are deleted with a new definition is encountered.

	Synopsis:
	int defproc(vecstr *dlp,cchar **envv,expcook *clp,cchar *fname) noex

	Arguments:
	dlp		defines list pointer
	envv		process environment
	clp		cookies list pointer
	fname		file to process

	Returns:
	>=0		count of environment variables
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstddef>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<vecstr.h>
#include	<bfile.h>
#include	<field.h>
#include	<ascii.h>
#include	<buffer.h>
#include	<expcook.h>
#include	<strn.h>
#include	<sfx.h>
#include	<snwcpy.h>
#include	<char.h>
#include	<getev.h>
#include	<localmisc.h>		/* <- for |REALNAMELEN| */

#include	"defproc.h"


/* local defines */

#define	ENVNAMELEN	REALNAMELEN	/* should be sufficient? */


#define	SI		subinfo


/* imported namespaces */

using std::nullptr_t ;			/* type */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	vecstr_envadd(vecstr *,cchar *,cchar *,int) noex ;
    extern int	getev(mainv,cchar *,int,cchar **) noex ;
}


/* external variables */


/* local structures */

struct subinfo {
	mainv		envv ;
	expcook		*clp ;
	vecstr		*dlp ;
} ;


/* forward references */

static int	procline(SI *,cchar *,int) noex ;
static int	checkdeps(SI *,cchar *,int) noex ;
static int	procvalues(SI *,buffer *,cchar *,cchar *,int) noex ;
static int	procvalue(SI *,buffer *,cchar *,cchar *,int) noex ;
static int	procsubenv(SI *,buffer *,cchar *,int) noex ;


/* local variables */

static constexpr cchar	fterms[32] = {
	0x00, 0x1A, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x04,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;

static constexpr cchar	dterms[32] = {
	0x00, 0x02, 0x00, 0x00,
	0x01, 0x10, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;

static cchar	ssb[] = {
	CH_LBRACE, CH_RBRACE, 0
} ;

#ifdef	COMMENT
static cchar	ssp[] = {
	CH_LPAREN, CH_RPAREN, 0
} ;
#endif /* COMMENT */


/* exported variables */


/* exported subroutines */

int defproc(vecstr *dlp,mainv envv,expcook *clp,cchar *fname) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if (dlp && clp && fname) ylikely {
	    rs = SR_INVALID ;
	    if (fname[0]) ylikely {
		SI	li, *lip = &li ;
		lip->envv = envv ;
		lip->clp = clp ;
		lip->dlp = dlp ;
		if (char *lbuf ; (rs = malloc_ml(&lbuf)) >= 0) ylikely {
		    cint	llen = rs ;
		    bfile	loadfile, *lfp = &loadfile ;
		    if ((rs = bopen(lfp,fname,"r",0666)) >= 0) ylikely {
	    		while ((rs = breadlns(lfp,lbuf,llen,-1,np)) > 0) {
			    cchar	*lp{} ;
			    if (int ll ; (ll = sfcontent(lbuf,rs,&lp)) > 0) {
	        		rs = procline(lip,lp,ll) ;
				c += rs ;
	        		if (rs < 0) break ;
			    } /* end if (sfcontent) */
	    		} /* end while (reading lines) */
	    		rs1 = bclose(lfp) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (file) */
		    rs1 = uc_free(lbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (defproc) */


/* local subroutines */

static int procline(SI *lip,cchar *lbuf,int llen) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		sl = llen ;
	int		cl ;
	int		len = 0 ;
	int		f_done = false ;
	cchar		*tp ;
	cchar		*cp ;
	cchar		*sp = lbuf ;
	cchar		*enp ;
	while (sl && CHAR_ISWHITE(*sp)) {
	    sp += 1 ;
	    sl -= 1 ;
	}
/* extract any dependencies (if we have any) */
	tp = strnbrk(sp,sl," \t?+=#") ;
	if ((tp != nullptr) && (*tp == '?')) {
	    cint tl = intconv(tp - sp) ;
	    if ((rs1 = checkdeps(lip,sp,tl)) > 0) {
	        sl -= intconv((tp + 1) - sp) ;
	        sp = (tp + 1) ;
	        while (sl && CHAR_ISWHITE(*sp)) {
	            sp += 1 ;
	            sl -= 1 ;
	        }
	        tp = strnbrk(sp,sl," \t+=#") ;
	    } else {
		f_done = true ;
	    }
	} /* end if (getting dependencies) */
	if (! f_done) {
	    cl = 0 ;
	    if ((tp != nullptr) && ((*tp == '=') || CHAR_ISWHITE(*tp))) {
		cint tl = intconv(tp - sp) ;
	        cl = sfshrink(sp,tl,&cp) ;
	        sl -= intconv((tp + 1) - sp) ;
	        sp = (tp + 1) ;
	    } /* end if (delimiter) */
	    if (cl > 0) {
	        cint	nrs = SR_NOTFOUND ;
	        if ((rs = vecstr_findn(lip->dlp,cp,cl)) == nrs) {
	            char	envname[ENVNAMELEN + 1] ;
	            enp = envname ;
	            snwcpy(envname,ENVNAMELEN,cp,cl) ;
	            buffer	b ;
	            if ((sl >= 0) && ((rs = b.start(sl)) >= 0)) {
	                if ((rs = procvalues(lip,&b,ssb,sp,sl)) >= 0) {
	                    if ((cl = b.get(&cp)) > 0) {
	                        rs = vecstr_envadd(lip->dlp,enp,cp,cl) ;
	                    }
	                }
	                len = b.finish ;
	                if (rs >= 0) rs = len ;
	            } /* end if (buffer) */
	        } /* end if (not-already-present) */
	    } /* end if (non-zero) */
	} /* end if (not-done) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (procline) */

static int checkdeps(SI *lip,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 = 0 ;
	if (field fsb ; (rs = fsb.start(sp,sl)) >= 0) ylikely {
	    cchar	*fp{} ;
	    for (int fl ; (fl = fsb.get(dterms,&fp)) >= 0 ; ) {
	        if (fl > 0) {
	            rs1 = getev(lip->envv,fp,fl,nullptr) ;
		}
	        if (rs1 < 0) break ;
	    } /* end for */
	    rs1 = fsb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (field) */
	return (rs >= 0) ? (rs1 >= 0) : rs ;
}
/* end subroutine (checkdeps) */

static int procvalues(SI *lip,buffer *bp,cchar *ss,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (char *fbuf ; (rs = malloc_mp(&fbuf)) >= 0) ylikely {
	    cint	flen = rs ;
	    if (field fsb ; (rs = fsb.start(sp,sl)) >= 0) ylikely {
	        int	c = 0 ;
	        cchar	*fp = fbuf ;
	        for (int fl ; (fl = fsb.sharg(fterms,fbuf,flen)) >= 0; ) {
	            if (fl > 0) {
	                if (c++ > 0) {
	                    rs = bp->chr(' ') ;
	                    len += rs ;
	                }
	                if (rs >= 0) {
	                    rs = procvalue(lip,bp,ss,fp,fl) ;
	                    len += rs ;
	                }
	            } /* end if (had a value) */
	            if (fsb.term == '#') break ;
	            if (rs < 0) break ;
	        } /* end while (looping over values) */
	        rs1 = fsb.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (fields) */
	    rs1 = uc_free(fbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (procvalues) */

static int procvalue(SI *lip,buffer *bp,cchar *ss,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	cchar		*kp ;
	for (int kl ; (kl = sfthing(sp,sl,ss,&kp)) >= 0 ; ) {
	    cchar	*cp = sp ;
	    cint	cl = intconv((kp - 2) - sp) ;
	    if (cl > 0) {
	        rs = bp->strw(cp,cl) ;
	        len += rs ;
	    }
	    if ((rs >= 0) && (kl > 0)) {
	        rs = procsubenv(lip,bp,kp,kl) ;
	        len += rs ;
	    }
	    sl -= intconv((kp + kl + 1) - sp) ;
	    sp = (kp + kl + 1) ;
	    if (rs < 0) break ;
	} /* end for */
	if ((rs >= 0) && (sl > 0)) {
	    rs = bp->strw(sp,sl) ;
	    len += rs ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (procvalue) */

static int procsubenv(SI *lip,buffer *bp,cchar *kp,int kl) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if (kl >= 0) ylikely {
	    int		al = 0 ;
	    cchar	*ap = nullptr ;
	    cchar	*cp{} ;
	    if (cchar *tp ; (tp = strnchr(kp,kl,'=')) != nullptr) {
	        ap = (tp + 1) ;
	        al = intconv((kp + kl) - (tp + 1)) ;
	        kl = intconv(tp - kp) ;
	    }
	    if (int cl ; (cl = getev(lip->envv,kp,kl,&cp)) > 0) {
	        rs = bp->strw(cp,cl) ;
	        len += rs ;
	    } else if (al > 0) {
	        rs = bp->strw(ap,al) ;
	        len += rs ;
	    }
	} /* end if (non-zero) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (procsubenv) */


