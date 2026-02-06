/* pcstrustuser SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* is a user a PCS-trusted user */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This code module was completely rewritten to replace any
	original garbage that was here before.

	= 2013-06-04, David A­D­ Morano
	"Turn down the heat!"  That is what I used to say when one
	of my circuits that I was designing started to get a little
	too heavy in the power-consumption department!  This
	subroutine here was a little bit that way; namely, overly
	heavy!  I have turned down the heat a bit by rewriting this
	subroutine substantially and making it into a more simple
	process of finding whether a username can be trusted or
	not.  It is not like this subroutine is used everywhere, so
	let us not overdue this whole process (overly prioritizing
	performace over simplicity).

*/

/* Copyright © 1998,2013 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	pcstrustuser

  	Description:
	This subroutine checks to see if a user is a PCS-trusted user.

	Notes:
	There is not strict order needed to find the "correct" thing
	here.  We just search to see if the given username is
	supposed to be trusted or not (a binary decision).  So order
	of searching is flexible.  Since we can search in whatever
	order we want, what we are going to try to do here is to
	search in the order of increasing complexity (increasing
	time) of search.  Is going to the system name-server cache
	faster than going to a file in the file-system?  In theory
	the name-server cache could respond with an answer *without*
	ever going to any file-systems, so we are going to consider
	the name-server faster than a file-system file lookup.  You
	mileage may vary.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>		/* |strcmp(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<ucentpw.h>
#include	<ucentgr.h>
#include	<getbufsize.h>
#include	<getusername.h>
#include	<getax.h>
#include	<getpwx.h>
#include	<mkpathx.h>
#include	<strwcpy.h>
#include	<vecstr.h>
#include	<matstr.h>
#include	<permx.h>
#include	<isnot.h>		/* |isNotPresent(3uc)| */
#include	<localmisc.h>

#include	"pcstrustuser.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	TRUSTFNAME	"trusted"

#define	SI		subinfo


/* external subroutines */


/* external variables */


/* local structures */

struct subinfo_fl {
	uint		id_pr:1 ;
	uint		id_un:1 ;
} ; /* end struct (subinfo_fl) */

struct subinfo {
	cchar		*pr ;		/* passed argument */
	cchar		*un ;		/* passed argument */
	char		*unbuf ;
	subinfo_fl	fl ;
	uid_t		uid_pr, uid_un ;
	gid_t		gid_pr, gid_un ;
	int		unlen ;
} ; /* end struct (subinfo) */


/* local typedefs */


/* forward references */

local int	subinfo_start(SI *,cchar *,cchar *) noex ;
local int	subinfo_finish(SI *) noex ;
local int	subinfo_idpr(SI *) noex ;
local int	subinfo_idun(SI *) noex ;
local int	subinfo_listdb(SI *) noex ;
local int	subinfo_filedb(SI *) noex ;
local int	subinfo_filedbx(SI *,vecstr *,char *,int) noex ;
local int	subinfo_prgroup(SI *) noex ;

typedef int (*subinfo_f)(subinfo *) noex ;


/* local variables */

constexpr cpcchar	trustedusers[] = {
	"root",
	"uucp",
	"nuucp",
	"adm",
	"admin",
	"daemon",
	"listen",
	"pcs",
	"post",
	"genserv",
	"local",
	"ncmp",
	nullptr
} ; /* end array */

constexpr cpcchar	sched[] = {
	"%r/etc/pcs.%f",
	"%r/etc/%f",
	nullptr
} ; /* end array */

constexpr subinfo_f	tries[] = {
	subinfo_listdb,
	subinfo_prgroup,
	subinfo_filedb,
	nullptr
} ; /* end array (tries) */


/* exported variables */


/* exported subroutines */

int pcstrustuser(cchar *pr,cchar *un) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		f = false ; /* return-value */
	if (pr && un) {
	    rs = SR_INVALID ;
	    if (pr[0] && un[0]) {
	        if (SI si ; (rs = subinfo_start(&si,pr,un)) >= 0) {
	            for (int i = 0 ; (rs >= 0) && tries[i] ; i += 1) {
	                rs = (*tries[i])(&si) ;
			f = rs ;
	                if (rs != 0) break ;
	            } /* end for */
	            rs1 = subinfo_finish(&si) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (subinfo) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (pcstrustuser) */


/* local subroutines */

local int subinfo_start(SI *sip,cchar *pr,cchar *un) noex {
	int		rs = SR_OK ;
	memclear(sip) ;
	sip->pr = pr ;
	sip->un = un ;
	sip->uid_pr = -1 ;
	sip->gid_pr = -1 ;
	sip->uid_un = -1 ;
	sip->gid_un = -1 ;
	if (char *p ; (rs = lm_un(&p)) >= 0) {
	    sip->unbuf = p ;
	    sip->unlen = rs ;
	}
	return rs ;
}
/* end subroutine (subinfo_start) */

local int subinfo_finish(SI *sip) noex {
    	int		rs = SR_FAULT ;
	int		rs1 ;
	if (sip) {
	    rs = SR_OK ;
	    if (sip->unbuf) {
		rs1 = lm_free(sip->unbuf) ;
	        if (rs >= 0) rs = rs1 ;
		sip->unbuf = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_finish) */

local int subinfo_listdb(SI *sip) noex {
	int		rs = SR_OK ;
	int		f = false ;
	cchar		*un = sip->un ;
	if (un[0] == '-') {
	    rs = subinfo_idun(sip) ;
	    un = sip->un ;
	}
	if (rs >= 0) {
	    f = (matstr(trustedusers,un,-1) >= 0) ;
	} /* end if (ok) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subinfo_listdb) */

local int subinfo_filedb(SI *sip) noex {
	int		rs ;
	int		rs1 ;
	int		f = false ;
	cchar	*pr = sip->pr ;
	if (vecstr svs ; (rs = svs.start(6,0)) >= 0) {
	    if ((rs = svs.envset("r",pr,-1)) >= 0) {
		if (char *tbuf ; (rs = lm_mp(&tbuf)) >= 0) {
		    cint tlen = rs ;
		    rs = subinfo_filedbx(sip,&svs,tbuf,tlen) ;
		    f = rs ;
	        } /* end if (successful permsched) */
	    } /* end if (vecstr_envset) */
	    rs1 = svs.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (svs) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subinfo_filedb) */

local int subinfo_filedbx(SI *sip,vecstr *svp,char *tbuf,int tlen) noex {
    	cint		rsn = SR_NOTFOUND ;
    	int		rs ;
	int		rs1 ;
	int		f = false ; /* return-value */
	cchar		*tfname = TRUSTFNAME ;
	cchar		*un = sip->un ;
	if ((rs = permsched(sched,svp,tbuf,tlen,tfname,R_OK)) >= 0) {
            if (un[0] == '-') {
                rs = subinfo_idun(sip) ;
                un = sip->un ;
            }
            if (rs >= 0) {
                if (vecstr tu ; (rs = tu.start(10,0)) >= 0) {
                    if ((rs = tu.loadfile(0,tbuf)) >= 0) {
                        rs1 = tu.find(un) ;
                        f = (rs1 >= 0) ;
                    }
                    rs1 = tu.finish ;
		    if (rs >= 0) rs = rs1 ;
                } /* end if (vecstr) */
            } /* end if (ok) */
	} else if (rs == rsn) {
	    rs = SR_OK ;
	} /* end if (permsched) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subinfo_filedbx) */

local int subinfo_idpr(SI *sip) noex {
	int		rs = SR_OK ;
	if (! sip->fl.id_pr) {
	    cchar	*pr = sip->pr ;
	    sip->fl.id_pr = true ;
	    if (ustat sb ; (rs = u_stat(pr,&sb)) >= 0) {
	        sip->uid_pr = sb.st_uid ;
	        sip->gid_pr = sb.st_gid ;
	    }
	}
	return rs ;
}
/* end subroutine (subinfo_idpr) */

local int subinfo_idun(SI *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ;
	if (! sip->fl.id_un) {
	    cchar	*un = sip->un ;
	    sip->fl.id_un = true ;
	    if (char *pwbuf ; (rs = lm_pw(&pwbuf)) >= 0) {
	        ucentpw		pw ;		/* used-multiple */
		cint 		pwlen = rs ;	/* used-multiple */
	        if (un[0] == '-') {
	            if ((rs = getpwusername(&pw,pwbuf,pwlen,-1)) >= 0) {
			cint	ulen = sip->unlen ;
			un = pw.pw_name ;
	                len = int(strwcpy(sip->unbuf,un,ulen) - sip->unbuf) ;
	                sip->un = sip->unbuf ;
		    }
	        } else {
	            rs = getpwx_name(&pw,pwbuf,pwlen,un) ;
	        }
	        if (rs >= 0) {
	            sip->uid_un = pw.pw_uid ;
	            sip->gid_un = pw.pw_gid ;
	        } else if (isNotPresent(rs)) {
		    rs = SR_OK ;
		}
		rs1 = lm_free(pwbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a) */
	} else if (sip->un != nullptr) {
	    len = lenstr(sip->un) ;
	} /* end if (needed) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_idun) */

local int subinfo_prgroup(SI *sip) noex {
    	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = false ;
	if ((rs >= 0) && (! sip->fl.id_pr)) {
	    rs = subinfo_idpr(sip) ;
	}
	if ((rs >= 0) && (! sip->fl.id_un)) {
	    rs = subinfo_idun(sip) ;
	}
	if ((rs >= 0) && (! f)) {
	    f = (sip->gid_un == sip->gid_pr) ;
	}
	/* check if username is in the PCS group */
	if ((rs >= 0) && (! f)) {
	    if (char *grbuf ; (rs = lm_gr(&grbuf)) >= 0) {
		const gid_t gid = sip->gid_pr ;
		cint grlen = rs ;
	        if (ucentgr gr ; (rs = gr.getgid(grbuf,grlen,gid)) >= 0) {
	            for (int i = 0 ; gr.gr_mem[i] ; i += 1) {
	                f = (strcmp(gr.gr_mem[i],sip->un) == 0) ;
	                if (f) break ;
	            } /* end for */
		} else if (rs == rsn) {
		    rs = SR_OK ;
	        } /* end if (got a group for PR) */
		rs1 = lm_free(grbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a) */
	} /* end if */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (pcstrustuser) */


