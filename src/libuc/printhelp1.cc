/* printhelp SUPPORT */
/* lang=C++20 */

/* print out a help file if we have one */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	The subroutine was written to get some common code for the
	printing of help files.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	printhelp

	Description:
	This subroutine will search for a program helpfile and print
	it out (by default to STDOUT).  A root filename is supplied
	(usually 'help') but along with a program root.  The
	"standard" places within the program root directory tree
	are scanned for the help file.

	Synopsis:
	int printhelp(void *fp,cchar *pr,cchar *sn,cchar *hfname) noex

	Arguments:
	fp		open file pointer (BFILE or SFIO)
	pr		program root directory path
	sn		program search name
	hfname		program help filename

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<ostream>
#include	<usystem.h>
#include	<syswords.hh>
#include	<bufsizevar.hh>
#include	<mallocxx.h>
#include	<vecstr.h>
#include	<bfile.h>
#include	<expcook.h>
#include	<snx.h>
#include	<sfx.h>
#include	<strwcpy.h>
#include	<mkpathx.h>
#include	<mkchar.h>
#include	<xperm.h>
#include	<getnodename.h>
#include	<rmx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"printhelp.hh"


/* local defines */

#ifndef	KBUFLEN
#define	KBUFLEN		40
#endif

#define	ELENMULT	2		/* multiply factor cookie expands */

#ifndef	HELPSCHEDFNAME
#define	HELPSCHEDFNAME	"etc/printhelp.filesched"
#endif


/* imported namespaces */

using std::ostream ;			/* type */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	vecstr_envadd(vecstr *,cchar *,cchar *,int) noex ;
    extern int	vecstr_loadfile(vecstr *,int,cchar *) noex ;
}


/* external variables */


/* local structures */

struct vars {
	int		maxnodelen ;
	int		maxhostlen ;
	int		maxcombolen ;
} ;

namespace {
    struct helper ;
    typedef int (helper::*helper_m)() noex ;
    struct helper {
	ostream		*osp ;
	cchar		*pr ;
	cchar		*sn ;
	cchar		*fn ;
	char		*tbuf ;
	vecstr		svars ;
	int		tlen ;
	helper(ostream *sp,cc *p,cc *s,cc *f) noex : osp(sp), pr(p), sn(s) { 
	    fn = f ;
	} ; /* end ctor */
 	int start(char *,int) noex ;
	int finish() noex ;
	operator int () noex ;
	int sched(mainv) noex ;
	int schedfile() noex ;
	int schedlocal() noex ;
    } ; /* end struct (helper) */
}


/* forward references */

static int	printhelper(ostream *,cc *,cc *,cc *) noex ;
static int	printproc(ostream *,cchar *,cchar *,cchar *) noex ;
static int	printout(ostream *,expcook *,cc *) noex ;

static int	loadscheds(vecstr *,cchar *,cchar *) noex ;

static int	expcook_load(expcook *,cc *,cc *) noex ;

static int	mkvars() noex ;


/* local variables */

static constexpr cchar *const schedule[] = {
	"%r/%l/%n/%n.%f",
	"%r/%l/%n/%f",
	"%r/share/help/%n.%f",
	"%r/share/help/%n",
	"/usr/extra/share/help/%n.%f",
	"/usr/extra/share/help/%n",
	"%r/%l/help/%n.%f",
	"%r/%l/help/%n",
	"%r/%l/%n.%f",
	"%r/%n.%f",
	"%n.%f",
	nullptr
} ;

enum expkeys {
	expkey_sn,
	expkey_ss,
	expkey_pr,
	expkey_rn,
	expkey_overlast
} ;

static constexpr cchar	*expkeys[] = {
	"SN",
	"SS",
	"PR",
	"RN",
	nullptr
} ;

static bufsizevar	maxpathlen(getbufsize_mp) ;
static vars		var ;

constexpr helper_m	mems[] = {
	&helper::schedfile,
	&helper::schedlocal,
	nullptr
} ;


/* exported variables */


/* export subroutines */

int printhelp(ostream *osp,cchar *pr,cchar *sn,cchar *fn) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if ((fn == nullptr) || (fn[0] == '\0')) {
	    fn = sysword.w_help ;
	}
	if (osp && pr && sn) {
	    rs = SR_INVALID ;
	    if (pr[0] && sn[0]) {
		static int	rsv = mkvars() ;
		if ((rs = rsv) >= 0) {
	            if (fn[0] != '/') {
		        rs = printhelper(osp,pr,sn,fn) ;
		        len = rs ;
		    } else {
		        rs = printproc(osp,pr,sn,fn) ;
		        len = rs ;
		    }
		} /* end if (mkvars) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (printhelp) */


/* local subroutines */

static int printhelper(ostream *osp,cc *pr,cc *sn,cc *fn) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	char		*tbuf{} ;
	if ((rs = malloc_mp(&tbuf)) >= 0) {
	    cint	tlen = rs ;
	    if (strchr(fn,'/') != nullptr) {
	        if ((rs = mkpath(tbuf,pr,fn)) >= 0) {
	            if ((rs = u_access(tbuf,R_OK)) >= 0) {
		        rs = printproc(osp,pr,sn,tbuf) ;
			len = rs ;
		    }
	        } /* end if */
	    } else {
		helper	ho(osp,pr,sn,fn) ;
		if ((rs = ho.start(tbuf,tlen)) >= 0) {
		    {
			rs = ho ;
			len = rs ;
		    }
		    rs1 = ho.finish() ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (helper) */
	    } /* end if (searching for file) */
	    rs1 = uc_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (printhelper) */

static int printproc(ostream *osp,cc *pr,cc *sn,cc *fn) noex {
	expcook		ck ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = expcook_start(&ck)) >= 0) {
	    if ((rs = expcook_load(&ck,pr,sn)) >= 0) {
	        rs = printout(osp,&ck,fn) ;
	        wlen += rs ;
	    } /* end if (expcook_load) */
	    rs1 = expcook_finish(&ck) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (expcook) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (printproc) */

static int printout(ostream *osp,expcook *ecp,cc *fn) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	char		*lbuf{} ;
	if ((rs = malloc_ml(&lbuf)) >= 0) {
	    char	*ebuf{} ;
	    cint	llen = rs ;
	    cint	elen = (ELENMULT * rs) ;
	    if ((rs = uc_malloc((elen+1),&ebuf)) >= 0) {
	        bfile	helpfile, *hfp = &helpfile ;
	        if ((rs = bopen(hfp,fn,"r",0666)) >= 0) {
	            while ((rs = breadln(hfp,lbuf,llen)) > 0) {
	                cint	len = rmeol(lbuf,rs) ;
		        if ((rs = expcook_exp(ecp,0,ebuf,elen,lbuf,len)) > 0) {
			    (*osp) << ebuf << eol ;
	                    wlen += (rs + 1) ;
		        } /* end if (expansion) */
	                if (rs < 0) break ;
	            } /* end while */
	            rs1 = bclose(hfp) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (opened helpfile) */
		rs1 = uc_free(ebuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	    rs1 = uc_free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (printout) */

int helper::start(char *b,int l) noex {
	vecstr		*svp = &svars ;
	int		rs ;
	tbuf = b ;
	tlen = l ;
	if ((rs = vecstr_start(svp,6,0)) >= 0) {
	    rs = loadscheds(svp,pr,sn) ;
	}
	return rs ;
}

int helper::finish() noex {
	vecstr		*svp = &svars ;
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = vecstr_finish(svp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}

helper::operator int () noex {
	int		rs = SR_OK ;
	for (int i = 0 ; (rs == SR_OK) && mems[i] ; i += 1) {
	    helper_m	m = mems[i] ;
	    rs = (this->*m)() ;
	} /* end for */
	return rs ;
}

int helper::schedfile() noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if ((rs = mkpath2(tbuf,pr,HELPSCHEDFNAME)) >= 0) {
	    if ((rs = perm(tbuf,-1,-1,nullptr,R_OK)) >= 0) {
		vecstr	hs ;
		cint	vo = VECSTR_OCOMPACT ;
	        if ((rs = vecstr_start(&hs,15,vo)) >= 0) {
	            if ((rs = vecstr_loadfile(&hs,false,tbuf)) >= 0) {
			mainv	spp{} ;
	                if ((rs = vecstr_getvec(&hs,&spp)) >= 0) {
			    rs = sched(spp) ;
			    len = rs ;
			}
		    } else if (isNotPresent(rs)) {
			rs = SR_OK ;
		    }
	    	    rs1 = vecstr_finish(&hs) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (vecstr) */
	    } else if (isNotPresent(rs)) {
	        rs = SR_OK ;
	    }
	} /* end if (mkpath) */
	return (rs >= 0) ? len : rs ;
}
/* end method (helper::schedfile) */

int helper::schedlocal() noex {
	return sched(schedule) ;
}
/* end method (helper::schedlocal) */

int helper::sched(mainv spp) noex {
	vecstr		*svp = &svars ;
	int		rs ;
	int		len = 0 ;
	if ((rs = permsched(spp,svp,tbuf,tlen,fn,R_OK)) >= 0) {
	    rs = printproc(osp,pr,sn,tbuf) ;
	    len = rs ;
	} else if (isNotAccess(rs)) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end method (helper::sched) */

static int loadscheds(vecstr *slp,cchar *pr,cchar *sn) noex {
	int		rs = SR_OK ;
	if (pr != nullptr) {
	    rs = vecstr_envadd(slp,"r",pr,-1) ;
	}
	if (rs >= 0) {
	    cchar	*w_lib = sysword.w_lib ;
	    rs = vecstr_envadd(slp,"l",w_lib,-1) ;
	}
	if (rs >= 0) {
	    if (sn == nullptr) sn = "prog" ;
	    rs = vecstr_envadd(slp,"n",sn,-1) ;
	}
	return rs ;
}
/* end subroutine (loadscheds) */

static int expcook_load(expcook *ecp,cc *pr,cc *sn) noex {
	int		rs ;
	int		rs1 ;
	char		*nn{} ;
	if ((rs = uc_malloc(var.maxcombolen,&nn)) >= 0) {
	    char	*dn = (nn + (var.maxnodelen + 1)) ;
	    if ((rs = getnodedomain(nn,dn)) >= 0) {
	        char	*hbuf{} ;
	        if ((rs = malloc_hn(&hbuf)) >= 0) {
		    cint	hlen = rs ;
	            cchar	*ks = "SNDHPR" ;
	            char	kbuf[KBUFLEN+1] ;
	            kbuf[1] = '\0' ;
	            for (int i = 0 ; (rs >= 0) && (ks[i] != '\0') ; i += 1) {
	                cint	kch = mkchar(ks[i]) ;
		        int	vl = -1 ;
		        cchar	*vp = nullptr ;
		        switch (kch) {
	                case 'S':
		            vp = sn ;
		            break ;
	                case 'N':
		            vp = nn ;
		            break ;
	                case 'D':
		            vp = dn ;
		            break ;
	                case 'H':
		            if ((rs = snsds(hbuf,hlen,nn,dn)) >= 0) {
			        vl = rs ;
		                vp = hbuf ;
		            }
		            break ;
	                case 'P':
		            vp = pr ;
		            break ;
	                case 'R':
		            vl = sfbasename(pr,-1,&vp) ;
		            break ;
	                } /* end switch */
	                if ((rs >= 0) && vp) {
		            kbuf[0] = kch ;
		            rs = expcook_add(ecp,kbuf,vp,vl) ;
	                }
	            } /* end for */
	            if (rs >= 0) {
		        cint	n = expkey_overlast ;
		        for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
		            int		vl = -1 ;
		            cchar	*vp = nullptr ;
		            switch (i) {
		            case expkey_sn:
			        vp = sn ;
			        break ;
		            case expkey_ss:
			        vp = hbuf ;
			        vl = strwcpyuc(hbuf,sn,-1) - hbuf ;
			        break ;
		            case expkey_pr:
			        vp = pr ;
			        break ;
		            case expkey_rn:
		                vl = sfbasename(pr,-1,&vp) ;
			        break ;
		            } /* end switch */
	                    if ((rs >= 0) && vp) {
		                rs = expcook_add(ecp,expkeys[i],vp,vl) ;
	                    }
		        } /* end for */
	            } /* end if (ok) */
		    rs1 = uc_free(hbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (getnodedomain) */
	    rs1 = uc_free(nn) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (expcook_load) */

static int mkvars() noex {
	int		rs ;
	if ((rs = getbufsize(getbufsize_hn)) >= 0) {
	    var.maxhostlen = rs ;
	    if ((rs = getbufsize(getbufsize_nn)) >= 0) {
	        var.maxnodelen = rs ;
		var.maxcombolen = ((var.maxhostlen+1) + rs) ;
	    }
	}
	return rs ;
}
/* end subroutine (mkvars) */


