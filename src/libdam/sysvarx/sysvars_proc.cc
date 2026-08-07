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
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIC® */
#include	<strings.h>		/* BSD |strncasecmp(3c)| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<linebuffer.h>		/* LIBUC */
#include	<filer.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<hdbstr.h>		/* LIBUC */
#include	<field.h>		/* LIBUC */
#include	<varmk.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<matstr.h>		/* LIBUC */
#include	<ischarx.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"sysvars.h"


/* local defines */

#define	BUFMULT		5		/* buffer size multiplier */

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


/* external subroutines */


/* local structures */

namespace {
    struct varer {
	int	buflen ;
	operator int () noex ;
    } ; /* end struct (varer) */
} /* end namespace */


/* forward references */

local int	procaddvar(hdbstr *,cchar *,int) noex ;

#if	CF_PROCVARFILE
local int	procvarfile(hdbstr *,cchar *) noex ;
local bool	hasweird(cchar *,int) noex ;
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
} ; /* end array */
#endif /* CF_PROCVARFILE */

constexpr cpcchar	wstrs[] = {
	"TZ",
	"LANG",
	"UMASK",
	"PATH",
	nullptr
} ; /* end array */

constexpr cpcchar	pstrs[] = {
	"LC_",
	nullptr
} ; /* end array */

static varer		vdata ;


/* exported variables */


/* exported subroutines */

int sysvars_procget(hdbstr *vlp,cchar *fn) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (vlp && fn) ylikely {
	    rs = SR_INVALID ;
	    if (fn[0]) ylikely {
		if (static varer rsv = vdata ; (rs = rsv) >= 0) ylikely {
	            cint	vn = 10 ;
	            cint	vo = 0 ;
		    (void) vdata.buflen ; /* <- currently unused */
	            if (vecstr lvars ; (rs = lvars.start(vn,vo)) >= 0) ylikely {
	                if ((rs = lvars.envfile(fn)) >= 0) {
	                    bool	f ;
			    cchar	*cp{} ;
	                    for (int i = 0 ; lvars.get(i,&cp) >= 0 ; i += 1) {
	                        if (cp) {
	                            if (cc *tp = strchr(cp,'=') ; tp) {
			               cint tl = intconv(tp - cp) ;
	                               f = (matstr(wstrs,cp,tl) >= 0) ;
	                               f = f || (matpstr(pstrs,10,cp,tl) >= 0) ;
	                               if (f) {
	                                   rs = procaddvar(vlp,cp,-1) ;
	                               } /* end if */
			            } /* end if (had equal sign) */
			        } /* end if (non-null) */
	                        if (rs < 0) break ;
	                    } /* end for */
	                } /* end if (vecstr_envfile) */
	                rs1 = lvars.finish ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (lvars) */
		} /* end if (varer) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (sysvars_procget) */

int sysvars_procset(hdbstr *vlp,cchar *dbn,mode_t om) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rs2 ;
	int		c = 0 ;
	if (vlp && dbn) ylikely {
	    rs = SR_INVALID ;
	    if (dbn[0]) ylikely {
	        cint		of = O_CREAT ;
	        cint		vn = DEFNVARS ;
	        if (varmk svars ; (rs = varmk_open(&svars,dbn,of,om,vn)) >= 0) {
		    cauto hcb = hdbstr_curbegin ;
	            if (hdbstr_cur cur ; (rs2 = hcb(vlp,&cur)) >= 0) ylikely {
	            int		val ;
	            cchar	*kp ;
	            cchar	*vap ;
	                while (rs >= 0) {
	                    rs1 = hdbstr_curenum(vlp,&cur,&kp,&vap,&val) ;
	                    if (rs1 == SR_NOTFOUND) break ;
	                    rs = rs1 ;
	                    if (rs >= 0) {
	                        c += 1 ;
	                        rs = varmk_addvar(&svars,kp,vap,val) ;
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
} /* end subroutine (sysvars_procset) */


/* local subroutines */

#if	CF_PROCVARFILE

local int procvarln(hdbstr *vlp,cchar *cp,int cl) noex {
    	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if (field fsb ; (rs = fsb.start(cp,cl)) >= 0) ylikely {
	    int kl = field_get(&fsb,fterms,&kp) ;
            if (kl == 6) {
                if (strncasecmp(WORDEXPORT,kp,kl) == 0) {
                    kl = field_get(&fsb,fterms,&kp) ;
                }
            } /* end if (elimination of 'export') */
            if ((kl > 0) && (! hasweird(kp,kl))) {
                cvoid       *n = nullptr ;
                if ((rs = hdbstr_fetch(vlp,kp,kl,n,n)) == rsn) {
                    cchar *vap = vbuf ;
                    int val = 0 ;
                    if (fsb.term != '#') {
                        val = field_sharg(&fsb,fterms,vbuf,vlen) ;
                    }
                    if (val >= 0) {
                        c += 1 ;
                        rs = hdbstr_add(vlp,kp,kl,vap,val) ;
                    }
                } /* end if (didn't have it already) */
            } /* end if (have a variable keyname) */
            rs1 = field_finish(&fsb) ;
            if (rs >= 0) rs = rs1 ;
	} /* end if (fields) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (procvarln) */

local int procvarfile(hdbstr *vlp,cchar *fname) noex {
	cint		rsn = SR_NOTFOUND ;
	cint		to = TO_READ ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if (char *lbuf ; (rs = mem.ml(&lbuf)) >= 0) ylikely {
	    cint of = O_RDONLY ;
	    cint llen = rs ;
	    if ((rs = u_open(fname,of,0)) >= 0) ylikely {
	        cint	bsz = (llen * 2) ;
	        cint	fd = rs ;
	        if (filer fb ; (rs = fb.start(fd,0z,bsz,0)) >= 0) ylikely {
	            while ((rs = fb.readln(lbuf,llen,to)) > 0) {
		        cchar *cp ;
		        if (int cl = (sl = sfcontent(lbuf,rs,&cp)) > 0) {
			    rs = procvarln(blp,cp,cl) ;
			    c += rs 
		        } /* end if (sfcontent) */
	                if (rs < 0) break ;
	            } /* end while (reading lines) */
	            rs1 = fb.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (filer) */
	        rs1 = u_close(fd) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (file) */
	    rs1 = mem.free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (procvarfile) */

local bool hasweird(cchar *sp,int sl) noex {
	bool		f = false ;
	for (int i = 0 ; (i != sl) && (sp[i] != '\0') ; i += 1) {
	    cint	ch = mkchar(sp[i]) ;
	    f = ((! isalnumlatin(ch)) && (ch != '_')) ;
	    if (f) break ;
	} /* end if */
	return f ;
} /* end subroutine (hasweird) */

#endif /* CF_PROCVARFILE */

local int procaddvar(hdbstr *vlp,cchar *sp,int sl) noex {
    	cnullptr	np{} ;
    	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		kl = sl ;
	int		val = 0 ;
	int		c = 0 ;
	cchar		*kp = sp ;
	cchar		*vap = nullptr ;
	if (cchar *tp = strnchr(sp,sl,'=') ; tp) {
	    vap = (tp + 1) ;
	    val = -1 ;
	    kl = intconv(tp - sp) ;
	} /* end if (had equal sign) */
	if ((rs = hdbstr_fetch(vlp,kp,kl,np,np)) == rsn) {
	    c += 1 ;
	    rs = hdbstr_add(vlp,kp,kl,vap,val) ;
	} /* end if */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (procaddvar) */

varer::operator int () noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_mp)) >= 0) ylikely {
	    buflen = (BUFMULT * rs) ;
	} /* end if */
    	return rs ;
} /* end method (varer::operator) */


