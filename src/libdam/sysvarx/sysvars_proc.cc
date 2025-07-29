/* sysvars_proc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Ssytem-Variable-Process */
/* version %I% last-modified %G% */

#define	CF_PROCVARFILE	0		/* compile in 'procvarfile' */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	sysvars_proc

	Description:
	Process a system variable. I have no idea what that means!

	Synopsis:
	int sysvars_procget(hdbstr *vlp,cchar *fname) noex

	Arguments:
	vlp		pointer to hash-string object
	fname		file to process

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<strings.h>		/* |strncasecmp(3c)| */
#include	<usystem.h>
#include	<linebuffer.h>
#include	<filer.h>
#include	<vecstr.h>
#include	<hdbstr.h>
#include	<field.h>
#include	<varmk.h>
#include	<strn.h>
#include	<matstr.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"sysvars.h"


/* local defines */

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN	2048
#endif
#endif

#ifndef	VBUFLEN
#define	VBUFLEN		(10 * MAXPATHLEN)
#endif

#define	BUFLEN		MAX((4 * MAXPATHLEN),LINEBUFLEN)

#define	WORDEXPORT	"export"

#define	TO_OPEN		5
#define	TO_READ		5
#define	TO_MKWAIT	20

#ifndef	DEFNVARS
#define	DEFNVARS	1000
#endif


/* external subroutines */

#if	defined(BSD) && (! defined(EXTERN_STRNCASECMP))
extern int	strncasecmp(cchar *,cchar *,int) noex ;
#endif


/* forward references */

static int	procaddvar(hdbstr *,cchar *,int) noex ;

#if	CF_PROCVARFILE
static int	procvarfile(hdbstr *,cchar *) noex ;
static bool	hasweird(cchar *,int) noex ;
#endif


/* local variables */

#if	CF_PROCVARFILE
constexpr char		fterms[] = {
	0x00, 0x3A, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;
#endif /* CF_PROCVARFILE */

constexpr cpcchar	wstrs[] = {
	"TZ",
	"LANG",
	"UMASK",
	"PATH",
	nullptr
} ;

constexpr cpcchar	pstrs[] = {
	"LC_",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int sysvars_procget(hdbstr *vlp,cchar *fn) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (vlp && fn) {
	    rs = SR_INVALID ;
	    if (fn[0]) {
	        cint	vn = 10 ;
	        cint	vo = 0 ;
	        if (vecstr lvars ; (rs = lvars.start(vn,vo)) >= 0) {
	            if ((rs = lvars.envfile(fn)) >= 0) {
	                bool	f ;
	                cchar	*tp ;
			cchar	*cp{} ;
	                for (int i = 0 ; lvars.get(i,&cp) >= 0 ; i += 1) {
	                    if (cp == nullptr) continue ;
	                    if ((tp = strchr(cp,'=')) == nullptr) continue ;
			    cint tl = intconv(tp - cp) ;
	                    f = (matstr(wstrs,cp,tl) >= 0) ;
	                    f = f || (matpstr(pstrs,10,cp,tl) >= 0) ;
	                    if (f) {
	                        rs = procaddvar(vlp,cp,-1) ;
	                    } /* end if */
	                    if (rs < 0) break ;
	                } /* end for */
	            } /* end if (vecstr_envfile) */
	            rs1 = lvars.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (lvars) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sysvars_procget) */

int sysvars_procset(hdbstr *vlp,cchar *dbn,mode_t om) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rs2 ;
	int		c = 0 ;
	if (vlp && dbn) {
	    rs = SR_INVALID ;
	    if (dbn[0]) {
	        cint		of = O_CREAT ;
	        cint		vn = DEFNVARS ;
	        if (varmk svars ; (rs = varmk_open(&svars,dbn,of,om,vn)) >= 0) {
	            int		vl ;
	            cchar	*kp ;
	            cchar	*vp ;
		    auto hcb = hdbstr_curbegin ;
	            if (hdbstr_cur cur ; (rs2 = hcb(vlp,&cur)) >= 0) {
	                while (rs >= 0) {
	                    rs1 = hdbstr_curenum(vlp,&cur,&kp,&vp,&vl) ;
	                    if (rs1 == SR_NOTFOUND) break ;
	                    rs = rs1 ;
	                    if (rs >= 0) {
	                        c += 1 ;
	                        rs = varmk_addvar(&svars,kp,vp,vl) ;
	                    }
	                } /* end while */
	                rs1 = hdbstr_curend(vlp,&cur) ;
	                if (rs >= 0) rs = rs1 ;
		        if ((rs >= 0) && (rs2 != rsn)) rs = rs2 ;
	            } /* end if (hdbstr_cursor) */
	            rs1 = varmk_close(&svars) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (varmk) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (sysvars_procset) */


/* local subroutines */

#if	CF_PROCVARFILE

static int procvarfile(hdbstr *vlp,cchar *fname) noex {
	cint		to = TO_READ ;
	int		rs ;
	int		cl ;
	int		kl ;
	int		c = 0 ;
	cchar	*tp, *kp ;
	cchar	*cp ;

	if (fname == nullptr) return SR_FAULT ;

	if (fname[0] == '\0') return SR_INVALID ;

	if ((rs = u_open(fname,O_RDONLY,0666)) >= 0) {
	    filer	dfile, *dfp = &dfile ;
	    cint	fd = rs ;
	    if ((rs = filer_start(dfp,fd,0z,BUFLEN,0)) >= 0) {
		field		fsb ;
	        cint		llen = LINEBUFLEN ;
		cint		vlen = VBUFLEN ;
	        int		len ;
		int		vl ;
		cchar		*vp ;
	        char		lbuf[LINEBUFLEN + 1] ;
		char		vbuf[VBUFLEN + 1] ;

	        while ((rs = filer_readln(dfp,lbuf,llen,to)) > 0) {
	            len = rs ;

	            if (lbuf[len - 1] == '\n') len -= 1 ;
	            lbuf[len] = '\0' ;

	            cp = lbuf ;
	            cl = len ;
	            while ((cl > 0) && CHAR_ISWHITE(*cp)) {
	                cp += 1 ;
	                cl -= 1 ;
	            }

	            if ((cp[0] == '\0') || (cp[0] == '#'))
	                continue ;

	            if ((rs = field_start(&fsb,cp,cl)) >= 0) {

	                kl = field_get(&fsb,fterms,&kp) ;

	                if (kl == 6) {
	                    if (strncasecmp(WORDEXPORT,kp,kl) == 0) {
	                        kl = field_get(&fsb,fterms,&kp) ;
			    }
	                } /* end if (elimination of 'export') */

	                if ((kl > 0) && (! hasweird(kp,kl))) {
	                    cint	nrs = SR_NOTFOUND ;
	                    cvoid	*n = nullptr ;

	                    if ((rs = hdbstr_fetch(vlp,kp,kl,n,n)) == nrs) {

	                        vp = vbuf ;
	                        vl = 0 ;
	                        if (fsb.term != '#')
	                            vl = field_sharg(&fsb,fterms,vbuf,vlen) ;

	                        if (vl >= 0) {
	                            c += 1 ;
	                            rs = hdbstr_add(vlp,kp,kl,vp,vl) ;
	                        }

	                    } /* end if (didn't have it already) */

	                } /* end if (have a variable keyname) */
	                rs1 = field_finish(&fsb) ;
	    		if (rs >= 0) rs = rs1 ;
	            } /* end if (fields) */
	            if (rs < 0) break ;
	        } /* end while (reading lines) */
	        rs1 = filer_finish(dfp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (filer) */
	    rs1 = u_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (file) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procvarfile) */

static bool hasweird(cchar *sp,int sl) noex {
	bool		f = false ;
	for (int i = 0 ; (i != sl) && (sp[i] != '\0') ; i += 1) {
	    cint	ch = mkchar(sp[i]) ;
	    f = ((! isalnumlatin(ch)) && (ch != '_')) ;
	    if (f) break ;
	} /* end if */
	return f ;
}
/* end subroutine (hasweird) */

#endif /* CF_PROCVARFILE */

static int procaddvar(hdbstr *vlp,cchar *sp,int sl) noex {
    	cnullptr	np{} ;
    	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		kl = sl ;
	int		vl = 0 ;
	int		c = 0 ;
	cchar		*kp = sp ;
	cchar		*vp = nullptr ;
	if (cchar *tp ; (tp = strnchr(sp,sl,'=')) != np) {
	    vp = (tp + 1) ;
	    vl = -1 ;
	    kl = intconv(tp - sp) ;
	}
	if ((rs = hdbstr_fetch(vlp,kp,kl,np,np)) == rsn) {
	    c += 1 ;
	    rs = hdbstr_add(vlp,kp,kl,vp,vl) ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procaddvar) */


