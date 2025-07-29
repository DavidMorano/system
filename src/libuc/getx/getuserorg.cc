/* getuserorg SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the organization name (string) for a specified user-name */
/* version %I% last-modified %G% */

#define	CF_ORGSYS	0		/* get from system? */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

	= 2018-10-19, David A-D- Morano
	Did some cleanup and error robustness, to bring it up to
	this century.  I do not even know if this subroutine is
	even used much any more, but whatever.  This was not actually
	as bad as it could have been!

*/

/* Copyright © 1998,2018 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getuserorg

	Descriptor:
	This subroutine retrieves the organization name (string)
	for a specified user-name.

	Synopsis:
	int getuserorg(char *rbuf,int rlen,cchar *username) noex

	Arguments:
	rbuf		user supplied buffer to hold result
	rlen		length of user supplied buffer
	username	username to look up

	Returns:
	>=0		length of return organization string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<ucpwcache.h>
#include	<getbufsize.h>
#include	<getax.h>
#include	<getpwx.h>
#include	<getusername.h>
#include	<getuserhome.h>
#include	<mallocxx.h>
#include	<varnames.hh>
#include	<syswords.hh>
#include	<gecos.h>
#include	<filereadln.h>
#include	<sncpyx.h>
#include	<mkpathx.h>
#include	<char.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"getuserorg.h"

import libutil ;

/* local defines */

#define	SI		subinfo

#undef	ORGCNAME
#define	ORGCNAME	"organization"


/* external subroutines */


/* external variables */


/* local structures */

struct subinfo {
	cchar		*ofp ;
	cchar		*un ;
	char		*rbuf ;		/* user-supplied buffer */
	int		rlen ;
} ;


/* forward references */

static int	subinfo_start(SI *,cchar *,char *,int,cchar *) noex ;
static int	subinfo_finish(SI *) noex ;

static int	getuserorg_var(SI *) noex ;
static int	getuserorg_home(SI *) noex ;
static int	getuserorg_passwd(SI *) noex ;
#if	CF_ORGSYS
static int	getuserorg_sys(SI *) noex ;
#endif


/* local variables */

constexpr cchar		orgname[] = ORGCNAME ;

#if	CF_ORGSYS
static cchar		*etcdir = sysword.w_etcdir ;
#endif


/* exported variables */


/* exported subroutines */

int getuserorg(char *rbuf,int rlen,cchar *un) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (rbuf && un) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (un[0]) {
	        subinfo		si, *sip = &si ;
		cchar		*on = orgname ;
	        if ((rs = subinfo_start(&si,on,rbuf,rlen,un)) >= 0) {
	            for (int i = 0 ; i < 3 ; i += 1) {
	                switch (i) {
	                case 0:
	                    rs = getuserorg_var(sip) ;
	                    break ;
	                case 1:
	                    rs = getuserorg_home(sip) ;
	                    break ;
	                case 2:
	                    rs = getuserorg_passwd(sip) ;
	                    break ;
	                } /* end switch */
	                len = rs ;
	                if (rs != 0) break ;
	            } /* end for */
	            rs1 = subinfo_finish(&si) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (subinfo) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getuserorg) */

int gethomeorg(char *rbuf,int rlen,cchar *hd) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (rbuf && hd) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (hd[0]) {
	        if (char *cbuf{} ; (rs = malloc_mn(&cbuf)) >= 0) {
		    cint	clen = rs ;
	            if ((rs = sncpy2(cbuf,clen,".",orgname)) >= 0) {
	                if (char *obuf{} ; (rs = malloc_mp(&obuf)) >= 0) {
	                    if ((rs = mkpath(obuf,hd,cbuf)) >= 0) {
	                        if ((rs = filereadln(obuf,rbuf,rlen)) >= 0) {
	                            len = rs ;
		                } else if (isNotAccess(rs)) {
		                    rs = SR_OK ;
		                }
	                    }
		            rs1 = uc_free(obuf) ;
		            if (rs >= 0) rs = rs1 ;
		        } /* end if (m-a-f) */
	            } /* end if (sncpy) */
		    rs1 = uc_free(cbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (gethomeorg) */


/* local subroutines */

static int subinfo_start(SI *sip,cc *ofp,char *rbuf,int rlen,cc *un) noex {
	int		rs = SR_FAULT ;
	if (sip) {
	    rs = memclear(sip) ;
	    sip->ofp = ofp ;
	    sip->rbuf = rbuf ;
	    sip->rlen = rlen ;
	    sip->un = un ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_start) */

static int subinfo_finish(SI *sip) noex {
	int		rs = SR_FAULT ;
	if (sip) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (subinfo_finish) */

static int getuserorg_var(SI *sip) noex {
	cchar		*vusername = varname.username ;
	cchar		*vorganization = varname.organization ;
	int		rs = SR_OK ;
	int		len = 0 ;
	cchar		*un = sip->un ;
	bool		f = (un[0] == '-') ;
	if (! f) {
	    static cchar	*vun = getenv(vusername) ;
	    if (vun && vun[0]) {
	        f = (strcmp(vun,un) == 0) ;
	    }
	}
	if (f) {
	    static cchar	*vorg = getenv(vorganization) ;
	    if (vorg && vorg[0]) {
	        rs = sncpy1(sip->rbuf,sip->rlen,vorg) ;
	        len = rs ;
	    }
	} /* end if */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getuserorg_var) */

static int getuserorg_home(SI *sip) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (char *hbuf{} ; (rs = malloc_mp(&hbuf)) >= 0) {
	    cint	hlen = rs ;
	    if ((rs = getuserhome(hbuf,hlen,sip->un)) >= 0) {
	        if ((rs = gethomeorg(sip->rbuf,sip->rlen,hbuf)) >= 0) {
		    len = rs ;
	        }
	    } /* end if (getuserhome) */
	    rs1 = uc_free(hbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getuserorg_home) */

/* this tries to retrieve from the PASSWD gecos field */
static int getuserorg_passwd(SI *sip) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (char *pwbuf{} ; (rs = malloc_pw(&pwbuf)) >= 0) {
	    ucentpw	pw ;
	    cint	pwlen = rs ;
	    if (sip->un[0] == '-') {
	        rs = getpwusername(&pw,pwbuf,pwlen,-1) ;
	    } else {
	        rs = getpwx_name(&pw,pwbuf,pwlen,sip->un) ;
	    }
	    if (rs >= 0) {
	        if (gecos g ; (rs = gecos_start(&g,pw.pw_gecos,-1)) >= 0) {
	            cint	gi = gecosval_organization ;
	            cchar	*vp{} ;
	            if (int vl ; (vl = gecos_getval(&g,gi,&vp)) > 0) {
	                rs = sncpy1w(sip->rbuf,sip->rlen,vp,vl) ;
	                len = rs ;
	            }
	            rs1 = gecos_finish(&g) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (gecos) */
	    } /* end if (get PW entry) */
	    rs1 = uc_free(pwbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getuserorg_passwd) */

#if	CF_ORGSYS

static int getuserorg_sys(SI *sip) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (char *ofname{} ; (rs = malloc_mp(&ofname)) >= 0) {
	    if ((rs = mkpath(ofname,etcdir,sip->ofp)) >= 0) {
	        rs = filereadln(ofname,sip->rbuf,sip->rlen) ;
	        len = rs ;
	    }
	    rs1 = uc_free(ofname) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getuserorg_sys) */

#endif /* CF_ORGSYS */


