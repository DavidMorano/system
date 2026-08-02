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
#include	<sys/types.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucpwcache.h>		/* LIBUC */
#include	<ucpwcache.h>		/* LIBUC */
#include	<getax.h>		/* LIBUC */
#include	<getpwx.h>		/* LIBUC */
#include	<getusername.h>		/* LIBUC */
#include	<getuserhome.h>		/* LIBUC */
#include	<gecos.h>		/* LIBU */
#include	<filereadln.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"getuserorg.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;
import uconstants ;			/* |varname(3u)| */

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
} ; /* end sruct (subinfo) */


/* forward references */

local int	subinfo_start		(SI *,cchar *,char *,int,cchar *) noex ;
local int	subinfo_finish		(SI *) noex ;

local int	getuserorg_var		(SI *) noex ;
local int	getuserorg_home		(SI *) noex ;
local int	getuserorg_passwd	(SI *) noex ;
#if	CF_ORGSYS
local int	getuserorg_sys		(SI *) noex ;
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
	if (rbuf && un) ylikely {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (un[0]) ylikely {
	        subinfo		si, *sip = &si ;
		cchar		*on = orgname ;
	        if ((rs = subinfo_start(&si,on,rbuf,rlen,un)) >= 0) ylikely {
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
} /* end subroutine (getuserorg) */

int gethomeorg(char *rbuf,int rlen,cchar *hd) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (rbuf && hd) ylikely {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (hd[0]) ylikely {
	        if (char *cbuf ; (rs = lm_mn(&cbuf)) >= 0) ylikely {
		    cint	clen = rs ;
	            if ((rs = sncpy2(cbuf,clen,".",orgname)) >= 0) ylikely {
	                if (char *obuf ; (rs = lm_mp(&obuf)) >= 0) {
	                    if ((rs = mkpath(obuf,hd,cbuf)) >= 0) {
	                        if ((rs = filereadln(obuf,rbuf,rlen)) >= 0) {
	                            len = rs ;
		                } else if (isNotAccess(rs)) {
		                    rs = SR_OK ;
		                }
	                    }
		            rs1 = lm_free(obuf) ;
		            if (rs >= 0) rs = rs1 ;
		        } /* end if (m-a-f) */
	            } /* end if (sncpy) */
		    rs1 = lm_free(cbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (gethomeorg) */


/* local subroutines */

local int subinfo_start(SI *sip,cc *ofp,char *rbuf,int rlen,cc *un) noex {
	int		rs = SR_FAULT ;
	if (sip) ylikely {
	    rs = memclear(sip) ;
	    sip->ofp = ofp ;
	    sip->rbuf = rbuf ;
	    sip->rlen = rlen ;
	    sip->un = un ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (subinfo_start) */

local int subinfo_finish(SI *sip) noex {
	int		rs = SR_FAULT ;
	if (sip) ylikely {
	    rs = SR_OK ;
	}
	return rs ;
} /* end subroutine (subinfo_finish) */

local int getuserorg_var(SI *sip) noex {
	cchar		*vusername = varname.username ;
	cchar		*vorganization = varname.organization ;
	int		rs = SR_OK ;
	int		len = 0 ;
	cchar		*un = sip->un ;
	bool		f = (un[0] == '-') ;
	if (! f) {
	    static cchar	*vun = getenver(vusername) ;
	    if (vun && vun[0]) {
	        f = (strcmp(vun,un) == 0) ;
	    }
	}
	if (f) {
	    static cchar	*vorg = getenver(vorganization) ;
	    if (vorg && vorg[0]) {
	        rs = sncpy1(sip->rbuf,sip->rlen,vorg) ;
	        len = rs ;
	    }
	} /* end if */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (getuserorg_var) */

local int getuserorg_home(SI *sip) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (char *hbuf ; (rs = lm_mp(&hbuf)) >= 0) ylikely { 
	    cint	hlen = rs ;
	    if ((rs = getuserhome(hbuf,hlen,sip->un)) >= 0) ylikely {
	        if ((rs = gethomeorg(sip->rbuf,sip->rlen,hbuf)) >= 0) {
		    len = rs ;
	        }
	    } /* end if (getuserhome) */
	    rs1 = lm_free(hbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (getuserorg_home) */

/* this tries to retrieve from the PASSWD gecos field */
local int getuserorg_passwd(SI *sip) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (char *pwbuf ; (rs = lm_pw(&pwbuf)) >= 0) ylikely {
	    ucentpw	pw ;
	    cint	pwlen = rs ;
	    if (sip->un[0] == '-') {
	        rs = getpwusername(&pw,pwbuf,pwlen,-1) ;
	    } else {
	        rs = getpwx_name(&pw,pwbuf,pwlen,sip->un) ;
	    }
	    if (rs >= 0) ylikely {
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
	    rs1 = lm_free(pwbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (getuserorg_passwd) */

#if	CF_ORGSYS

local int getuserorg_sys(SI *sip) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (char *ofname ; (rs = lm_mp(&ofname)) >= 0) ylikely {
	    if ((rs = mkpath(ofname,etcdir,sip->ofp)) >= 0) ylikely {
	        rs = filereadln(ofname,sip->rbuf,sip->rlen) ;
	        len = rs ;
	    }
	    rs1 = lm_free(ofname) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (getuserorg_sys) */

#endif /* CF_ORGSYS */


