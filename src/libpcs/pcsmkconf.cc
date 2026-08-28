/* pcsmkconf SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make the PCS CONF index file */
/* version %I% last-modified %G% */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	pcsmkconf

	Description:
	This subroutine makes the PCS CONF index file.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<mktmp.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<paramfile.h>		/* LIBUC */
#include	<varmk.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	SI		subinfo
#define	SI_FL		subinfo_flags

#define	PRCONF		"conf"

#ifndef	PARAMBUFLEN
#define	PARAMBUFLEN	256
#endif


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	mainv	envv ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */

struct subinfo_flags {
	uint		localconf:1 ;
} ; /* end struct */

struct subinfo {
	SI_FL		fl ;
	paramfile	pf ;
	varmk		v ;
	mainv		envv ;
	cchar		*prconf ;
	cchar		*pr ;
	cchar		*cfname ;
} ; /* end struct */


/* forward references */

local int	subinfo_start(SI *,cchar *,cchar **,cchar *) noex ;
local int	subinfo_finish(SI *) noex ;
local int	subinfo_startsubs(SI *,vecstr *) noex ;
local int	subinfo_confglobal(SI *,char *) noex ;
local int	subinfo_conflocal(SI *,char *) noex ;
local int	subinfo_proc(SI *) noex ;


/* external variables */


/* local variables */

constexpr cpcchar	schedconf[] = {
	"%p/etc/%n.%f",
	"%p/etc/%f",
	"%p/%n.%f",
	nullptr
} ; /* end array (scedconf) */

static vars		var ;


/* exported variables */


/* exported subroutines */

int pcsmkconf(cchar *pr,mainv envv,cc *cfname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if (pr && cfname) ylikely {
	    rs = SR_INVALID ;
	    if (pr[0] && cfname[0]) ylikely {
		if (static cint rsv = var ; (rs = rsv) >= 0) ylikely {
	            SI	si ;
	            if ((rs = subinfo_start(&si,pr,envv,cfname)) >= 0) {
	                {
	                    rs = subinfo_proc(&si) ;
		            rv = rs ;
	                }
	                rs1 = subinfo_finish(&si) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (subinfo) */
	            if (rs == SR_EXIST) rs = SR_OK ;
		} /* end if (vars) */
	    /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (pcsmkconf) */


/* local subroutines */

local int subinfo_start(SI *sip,cc *pr,mainv envv,cc *cfname) noex {
	cint		of = O_CREAT ;
	cint		n = 20 ;
	int		rs = SR_OK ;
	int		rs1 ;
	cmode		vm = 0444 ;
	cbool		f_global = (cfname == nullptr) ;
	char		dbname[MAXPATHLEN+1] ;
	if (envv == nullptr) envv = var.envv ;
	memclear(sip) ;
	sip->envv = envv ;
	sip->prconf = PRCONF ;
	sip->pr = pr ;
	sip->cfname = cfname ;
	/* open the PCS-configuration file (if it exists) */
	if (vecstr subs ; (rs = vecstr_start(&subs,4,0)) >= 0) ylikely {
	    char	tmpfname[MAXPATHLEN+1] ;
	    rs = subinfo_startsubs(sip,&subs) ;
	    if ((rs >= 0) && (cfname == nullptr)) {
	        int	tlen = MAXPATHLEN ;
	        cmode	m = R_OK ;
	        char	*tbuf = tmpfname ;
	        cfname = tmpfname ;
	        rs = permsched(schedconf,&subs,tbuf,tlen,sip->prconf,m) ;
	    }
	    if (rs >= 0) ylikely {
	        if (f_global) {
	            rs = subinfo_confglobal(sip,dbname) ;
	        } else {
	            rs = subinfo_conflocal(sip,dbname) ;
	        }
		/* see if we can create a new VAR DB */
	        if (rs >= 0) ylikely {
		    rs = varmk_open(&sip->v,dbname,of,vm,n) ;
	        } /* end if (ok) */
	    } /* end if (ok) */
	    rs1 = vecstr_finish(&subs) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (subs) */
	return rs ;
} /* end subroutine (subinfo_start) */

local int subinfo_finish(SI *sip) noex )
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = varmk_close(&sip->v) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (subinfo_finish) */

local int subinfo_startsubs(SI *sip,vecstr *slp) noex {
	int		rs = SR_OK ;
	int		cl ;
	cchar		*cp ;
	{
	cl = sfbasename(sip->pr,-1,&cp) ;
	if (cl <= 0) rs = SR_INVALID ;
	}
	if (rs >= 0) {
	    rs = vecstr_envadd(slp,"p",sip->pr,-1) ;
	}
	if (rs >= 0) {
	    rs = vecstr_envadd(slp,"n",cp,cl) ;
	}
	return rs ;
} /* end subroutine (subinfo_startsubs) */

local int subinfo_confglobal(SI *sip,char *dbname) noex {
	int		rs ;
	cmode		dm = 0777 ;
	cchar		*cdname = "pcsconf" ;
	char		tmpdname[MAXPATHLEN+1] ;
	if ((rs = prmktmpdir(sip->pr,tmpdname,cdname,dm)) >= 0) {
	    rs = mkpath2(dbname,tmpdname,sip->prconf) ;
	}
	return rs ;
} /* end subroutine (subinfo_confglobal) */

local int subinfo_conflocal(SI *sip,char *dbname) noex {
	int		rs ;
	cmode		dm = 0775 ;
	cchar		*cdname = "pcsconf" ;
	char		tmpdname[MAXPATHLEN+1] ;
	if ((rs = mktmpuserdir(tmpdname,"-",cdname,dm)) >= 0) ylikely {
	    rs = mkpath2(dbname,tmpdname,sip->prconf) ;
	}
	return rs ;
} /* end subroutine (subinfo_conflocal) */

local int subinfo_proc(SI *sip) noex {
	paramfile	*pfp = &sip->pf ;
	int		rs ;
	int		rs1 ;
	if ((rs = paramfile_open(pfp,sip->envv,sip->cfname)) >= 0) ylikely {
	    paramfile_cur	cur ;
	    paramfile_ent	pe ;
	    if ((rs = paramfile_curbegin(pfp,&cur)) >= 0) ylikely {
	        cint	plen = PARAMBUFLEN ;
	        int	kl ;
	        char	pbuf[PARAMBUFLEN+1] ;
	        while (rs >= 0) {
	            kl = paramfile_curenum(pfp,&cur,&pe,pbuf,plen) ;
	            if (kl == SR_NOTFOUND) break ;
	            rs = kl ;
		    if (rs >= 0) ylikely {
	                rs = varmk_addvar(&sip->v,pe.key,pe.value,pe.vlen) ;
		    } /* end if (ok) */
	        } /* end while (reading parameters) */
	        rs1 = paramfile_curend(pfp,&cur) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (cursor) */
	    rs1 = paramfile_close(pfp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (paramfile) */
	return rs ;
} /* end subroutine (subinfo_proc) */

vars::operator int () noex {
	return u_getenviron(&envv) ;
} /* end method (vars::operator) */


