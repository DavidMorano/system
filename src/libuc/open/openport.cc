/* openport SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* proxy-open a socket port for the calling program */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-20, David A­D­ Morano
	This code was originally written so that dangerous daemon
	programs do not have to be run as 'root' in order to bind
	to a priveledge port (for those transport providers that
	have priveledged ports).  A good example of a dangerous
	daemon program that this capability is especially useful
	for is the infamous SENDMAIL daemon!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	openport

	Description:
	This subroutine will open a socket port as a proxy for our
	calling process.

	Synopsis:
	int openport(int pf,int pt,int proto,sockaddress *sap) noex

	Arguments:
	pf		protocol family (PF_xxx)
	pt		protocol type (eg: SOCK_DGRAM)
	proto		protocol
	sap		pointer to socket address

	Returns:
	>=0		FD
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/socket.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<getusername.h>		/* LIBUC */
#include	<nulstr.h>		/* LIBUC */
#include	<sockaddress.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<spawnproc.h>		/* LIBUC */
#include	<permx.h>		/* LIBUC */
#include	<matxstr.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"openport.h"
#include	"openportmsg.h"

#pragma		GCC dependency	"mod/libutil.ccm"
#pragma		GCC dependency	"mod/uconstants.ccm"

import libutil ;			/* |memclear(3u)| */
import uconstants ;

/* local defines */

#define	NARGS		5
#define	NENVS		10

#define	SA		sockaddress
#define	SP_CON		spawnproc_con


/* imported namespaces */


/* local type-defs */


/* external subroutines */

extern "C" {
    extern int uc_writen(int,cvoid *,int) noex ;
} /* end extern */


/* external variables */


/* local structures */

namespace {
    struct openporter {
	mainv		envv ;
	char		*a{} ;
	char		*ubuf ;
	char		*pbuf ;
	sockaddress	*sap ;
	int		pf ;
	int		pt ;
	int		proto ;
	int		ulen ;
	int		plen ;
	openporter(int f,int t,int r,sockaddress *s) noex {
	    pf = f ;
	    pt = t ;
	    proto = r ;
	    sap = s ;
	} ; /* end ctor */
	operator int () noex ;
	int start	() noex ;
	int finish	() noex ;
	int opener	() noex ;
	int progproc	(cchar *,int) noex ;
    } ; /* end struct (openporter) */
} /* end namespace */


/* forward references */

local int procspawn(cchar *,cchar *,mainv,mainv,int,int,int,SA *) noex ;
local int procspawn_begin(SP_CON *,cchar *,mainv,mainv) noex ;
local int procspawn_end(pid_t,int *) noex ;

local int procexchange(cchar *,int,int,int,int,SA *) noex ;

local int	loadenvs(vecstr *,cchar *,cchar *,cchar *,int) noex ;
local int	progfind(char *) noex ;


/* local variables */

constexpr cpcchar	prs[] = {
	"/usr/extra",
	"/usr/preroot",
	nullptr
} ; /* end arry (prs) */

constexpr int		unlen = OPENPORTMSG_UNLEN ;


/* exported variables */


/* exported subroutines */

int openport(int pf,int pt ,int proto,SA *sap) noex {
	int		rs = SR_FAULT ;
	int		fd = -1 ;
	if (sap) {
	    rs = SR_INVALID ;
	    if ((pf >= 0) && (pt >= 0) && (proto >= 0)) {
		openporter	op(pf,pt,proto,sap) ;
		rs = op ;
		fd = rs ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (openport) */


/* local subroutines */

openporter::operator int () noex {
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	if ((rs = start()) >= 0) {
	    {
		rs = opener() ;
		fd = rs ;
	    }
	    rs1 = finish() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */
	if ((rs < 0) && (fd >= 0)) uc_close(fd) ;
	return (rs >= 0) ? fd : rs ;
} /* end method (openporter::operator) */

int openporter::start() noex {
	int		rs ;
	if ((rs = u_getenviron(&envv)) >= 0) {
	    if ((rs = bufsizeget(bufsize_un)) >= 0) {
	        ulen = rs ;
	        if ((rs = bufsizeget(bufsize_mp)) >= 0) {
		    cint	sz = (ulen+1) + (rs+1) ;
		    plen = rs ;
		    if ((rs = lm_mall(sz,&a)) >= 0) {
		        ubuf = a ;
		        pbuf = (ubuf + (ulen+1)) ;
		        rs = getusername(ubuf,ulen,-1) ;
		        if (rs < 0) {
			    lm_free(a) ;
			    ubuf = nullptr ;
			    pbuf = nullptr ;
		        } /* end if (error) */
		    } /* end if (memory-acquire) */
	        } /* end if (bufsizeget) */
	        if (rs < 0) {
		    plen = 0 ;
		    ulen = 0 ;
	        } /* end if (error) */
	    } /* end if (bufsizeget) */
	} /* end if (u_getenviron) */
	return rs ;
} /* end method (openporter::start) */

int openporter::finish() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (a) {
	    rs1 = lm_free(a) ;
	    if (rs >= 0) rs = rs1 ;
	    a = nullptr ;
	    pbuf = nullptr ;
	    ubuf = nullptr ;
	} /* end if (non-null) */
	return rs ;
} /* end method (openporter::finish) */

int openporter::opener() noex {
	int		rs ;
	int		fd = -1 ;
	if ((rs = progfind(pbuf)) >= 0) {
	    cint	pl = rs ;
	    cchar	*bn ;
	    rs = SR_INVALID ;
	    if (int bl ; (bl = sfbasename(pbuf,pl,&bn)) > 0) {
	        rs = progproc(bn,bl) ;
	        fd = rs ;
	    }
	} /* end if (progfind) */
	return (rs >= 0) ? fd : rs ;
} /* end method (openporter::opener) */

int openporter::progproc(cchar *bn,int bl) noex {
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	if (vecstr envs ; (rs = envs.start(5,0)) >= 0) {
	    if ((rs = loadenvs(&envs,ubuf,pbuf,bn,bl)) >= 0) {
		cchar	*name ;
		if (nulstr n ; (rs = n.start(bn,bl,&name)) >= 0) {
		    cchar	*vnhome = varname.home ;
	            cchar	*sargv[NARGS + 1] ;
	            cchar	*senvv[NENVS + 2 + 1] ;
	            cchar	*ep ;
		    {
	                int 	ai = 0 ;
	                sargv[ai++] = name ;
	                sargv[ai++] = "-b" ;
	                sargv[ai] = nullptr ;
		    }
	            int i = 0 ; /* used-multiple for env-var array */
	            if (int j ; (j = matkeystr(envv,vnhome,-1)) >= 0) {
	                senvv[i++] = envv[j] ;
	            }
	            for (int j = 0 ; envs.get(j,&ep) >= 0 ; j += 1) {
	                if (ep) {
	                    if (i >= NENVS) {
	                        rs = SR_BUGCHECK ;
	                        break ;
	                    }
	                    senvv[i++] = ep ;
		        }
	            } /* end for */
	            senvv[i] = nullptr ;
	            if (rs >= 0) {
			cchar	*un = ubuf ;
			cchar	*pn = pbuf ;
		        rs = procspawn(un,pn,sargv,senvv,pf,pt,proto,sap) ;
		        fd = rs ;
	            } /* end if (ok) */
		    rs1 = n.finish ;
	            if (rs >= 0) rs = rs1 ;
		} /* end if (nulstr) */
	    } /* end if (loadenvs) */
	    rs1 = envs.finish ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && (fd >= 0)) uc_close(fd) ;
	} /* end if (vecstr) */
	return (rs >= 0) ? fd : rs ;
} /* end method (openporter::progproc) */

local int procspawn(cc *un,cc *prog,mainv sargv,mainv senvv,
		int pf,int pt,int proto,SA *sap) noex {
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	if (SP_CON psa ; (rs = procspawn_begin(&psa,prog,sargv,senvv)) >= 0) {
	    const pid_t	pid = rs ;
	    cint	cfd = psa.fd[0] ;
	    int		cs{} ;
	    {
	        rs = procexchange(un,cfd,pf,pt,proto,sap) ;
	        fd = rs ;
	        uc_close(cfd) ;
	    }
	    rs1 = procspawn_end(pid,&cs) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs >= 0) && (cs != 0)) rs = SR_IO ;
	    if ((rs < 0) && (fd >= 0)) uc_close(fd) ;
	} /* end if (spawned program) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (procspawn) */

local int procspawn_begin(SP_CON *psp,cc *prog,mainv sargv,mainv senvv) noex {
	int		rs = SR_FAULT ;
	if (psp && prog && sargv && senvv) {
	    int i = 0 ;
	    memclear(psp) ;
	    psp->opts |= SPAWNPROC_OIGNINTR ;
	    psp->opts |= SPAWNPROC_OSETPGRP ;
	    psp->disp[i++] = SPAWNPROC_DOPEN ;
	    psp->disp[i++] = SPAWNPROC_DCLOSE ;
	    psp->disp[i++] = SPAWNPROC_DCLOSE ;
	    rs = spawnproc(psp,prog,sargv,senvv) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (procspawn_begin) */

local int procspawn_end(pid_t pid,int *csp) noex {
	return u_waitpid(pid,csp,0) ;
} /* end subroutine (procspawn_end) */

local int procexchange(cc *un,int cfd,int pf,int pt,int proto,SA *sap) noex {
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	if (char *mbuf ; (rs = lm_mm(&mbuf)) >= 0) {
	    cint	mlen = rs ;
	    openportmsg_req	m0{} ;
	    openportmsg_res	m1{} ;
	    int		ml ;
	    m0.msgtype = openportmsgtype_request ;
	    m0.pf = pf ;
	    m0.ptype = pt ;
	    m0.proto = proto ;
	    m0.sa = *sap ; /* <- copy object */
	    strwcpy(m0.username,un,unlen) ;
	    if ((rs = openportmsg_msgrequest(&m0,1,mbuf,mlen)) >= 0) {
	        ml = rs ;
	        if ((rs = uc_writen(cfd,mbuf,ml)) >= 0) {
	            cint	mt = openportmsgtype_response ;
    
	            rs1 = u_read(cfd,mbuf,mlen) ;
	            ml = rs1 ;
	            if (rs1 >= 0) {
	                rs1 = openportmsg_msgresponse(&m1,0,mbuf,ml) ;
	            }
    
	            if ((rs1 > 0) && (m1.msgtype == mt)) {
	                if (m1.rs >= 0) {
		            STRRECVFD	fds{} ;
		            cint	cmd = I_RECVFD ;
	                    rs = u_ioctl(cfd,cmd,&fds) ;
	                    fd = fds.fd ;
	                } else {
	                    rs = m1.rs ;
		        }
	            } else {
	                rs = SR_PROTO ;
	            }
	        } /* end if (write was successful) */
	    } /* end if (openportmsg) */
	    rs1 = lm_free(mbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (procexchange) */

local int loadenvs(vecstr *elp,cchar *un,cchar *prog,cchar *bn,int bl) noex {
	constexpr cpcchar penvnames[] = { "_", "_EF" } ;
	int		rs = SR_OK ;
	int		c = 0 ;
	for (cauto &s : penvnames) {
	    rs = elp->envadd(s,prog,-1) ;
	    if (rs < INT_MAX) c += 1 ;
	    if (rs < 0) break ;
	} /* end for */
	if (rs >= 0) {
	    rs = elp->envadd("_A0",bn,bl) ;
	    if (rs < INT_MAX) c += 1 ;
	}
	if (rs >= 0) {
	    cchar	*key = varname.username ;
	    rs = elp->envadd(key,un,-1) ;
	    if (rs < INT_MAX) c += 1 ;
	}
	return (rs >= 0) ? c : rs ;
} /* end subroutine (loadenvs) */

local int progfind(char *progfname) noex {
	cnullptr	np{} ;
	int		rs = SR_LIBACC ;
	int		len = 0 ; /* return-value */
	cchar		sbin[] = "sbin" ;
	cchar		pn[] = OPERPORT_PROGNAME ;
	for (int i = 0 ; prs[i] != nullptr ; i += 1) {
	    if ((rs = mkpath(progfname,prs[i],sbin,pn)) >= 0) {
	        cint	rl = rs ;
		if (ustat sb ; (rs = u_stat(progfname,&sb)) >= 0) {
	            if (S_ISREG(sb.st_mode)) {
			cint	am = (R_OK|X_OK) ;
	                if ((rs = perm(progfname,-1,-1,np,am)) >= 0) {
			    len = rl ;
			} else if (isNotAccess(rs)) {
			    rs = SR_OK ;
			}
		    } /* end if (is-reg-file) */
		} else if (isNotPresent(rs)) {
		    rs = SR_OK ;
	        } /* end if (stat) */
	    } /* end if (mkpath) */
	    if (len > 0) break ;
	    if (rs < 0) break ;
	} /* end for (prs) */
	if ((rs >= 0) && (len == 0)) {
	    rs = SR_LIBACC ;
	}
	return (rs >= 0) ? len : rs ;
} /* end subroutine (progfind) */


