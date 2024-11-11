/* openport SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* proxy-open a socket port for the calling program */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-20, David A�D� Morano
	This code was originally written so that dangerous
	daemon programs do not have to be run as 'root' in order
	to bind to a priveledge port (for those transport providers
	that have priveledged ports).  A good example of a dangerous
	daemon program that this capability is especially useful
	for is the infamous SENDMAIL daemon!

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

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
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<uvariables.hh>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<nulstr.h>
#include	<sockaddress.h>
#include	<sfx.h>
#include	<mkpathx.h>
#include	<strwcpy.h>
#include	<vecstr.h>
#include	<spawnproc.h>
#include	<getusername.h>
#include	<xperm.h>
#include	<matxstr.h>
#include	<isnot.h>
#include	<localmisc.h>		/* |MSGBUFLEN| */

#include	"openport.h"
#include	"openportmsg.h"


/* local defines */

#define	MBUFLEN		MSGBUFLEN

#define	NENVS		10

#define	SA		sockaddress
#define	SP_CON		spawnproc_con


/* imported namespaces */

using std::nullptr_t ;			/* type */


/* local type-defs */


/* external subroutines */


/* external variables */

extern cchar	**environ ;


/* local structures */

namespace {
    struct openporter {
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
	} ;
	operator int () noex ;
	int start() noex ;
	int finish() noex ;
	int opener() noex ;
	int progproc(cchar *,int) noex ;
    } ; /* end struct (openporter) */
}


/* forward references */

static int procspawn(cchar *,cchar *,mainv,mainv,int,int,int,SA *) noex ;
static int procspawn_begin(SP_CON *,cchar *,mainv,mainv) noex ;
static int procspawn_end(pid_t,int *) noex ;

static int procexchange(cchar *,int,int,int,int,SA *) noex ;

static int	loadenvs(vecstr *,cchar *,cchar *,cchar *,int) noex ;
static int	progfind(char *) noex ;


/* local variables */

static constexpr cpcchar	prs[] = {
	"/usr/extra",
	"/usr/preroot",
	nullptr
} ;

constexpr int			unlen = OPENPORTMSG_UNLEN ;


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
}
/* end subroutine (openport) */


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
}
/* end method (openporter::operator) */

int openporter::start() noex {
	int		rs ;
	if ((rs = getbufsize(getbufsize_un)) >= 0) {
	    ulen = rs ;
	    if ((rs = getbufsize(getbufsize_mp)) >= 0) {
		cint	sz = (ulen+1) + (rs+1) ;
		plen = rs ;
		if ((rs = uc_malloc(sz,&a)) >= 0) {
		    ubuf = a ;
		    pbuf = (ubuf + (ulen+1)) ;
		    rs = getusername(ubuf,ulen,-1) ;
		    if (rs < 0) {
			uc_free(a) ;
		    }
		} /* end if (memory-allocation) */
	    } /* end if (getbufsize) */
	} /* end if (getbufsize) */
	return rs ;
}
/* end method (openporter::start) */

int openporter::finish() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (a) {
	    rs1 = uc_free(a) ;
	    if (rs >= 0) rs = rs1 ;
	    a = nullptr ;
	    pbuf = nullptr ;
	    ubuf = nullptr ;
	}
	return rs ;
}
/* end method (openporter::finish) */

int openporter::opener() noex {
	int		rs ;
	int		fd = -1 ;
	if ((rs = progfind(pbuf)) >= 0) {
	    cint	pl = rs ;
	    int		bl ;
	    cchar	*bn ;
	    rs = SR_INVALID ;
	    if ((bl = sfbasename(pbuf,pl,&bn)) > 0) {
	        rs = progproc(bn,bl) ;
	        fd = rs ;
	    }
	} /* end if (progfind) */
	return (rs >= 0) ? fd : rs ;
}
/* end if (openporter::opener) */

int openporter::progproc(cchar *bn,int bl) noex {
	vecstr		envs ;
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	if ((rs = vecstr_start(&envs,5,0)) >= 0) {
	    if ((rs = loadenvs(&envs,ubuf,pbuf,bn,bl)) >= 0) {
		nulstr	n ;
		cchar	*name ;
		if ((rs = nulstr_start(&n,bn,bl,&name)) >= 0) {
		    cchar	*vnhome = varname.home ;
	            int 	i = 0 ;
	            int		j ;
	            cchar	*sargv[5] ;
	            cchar	*senvv[NENVS +2+ 1] ;
	            cchar	*ep ;
	            sargv[i++] = name ;
	            sargv[i++] = "-b" ;
	            sargv[i] = nullptr ;
	            i = 0 ;
	            if ((j = matkeystr(environ,vnhome,-1)) >= 0) {
	                senvv[i++] = environ[j] ;
	            }
	            for (j = 0 ; vecstr_get(&envs,j,&ep) >= 0 ; j += 1) {
	                if (ep) {
	                    if (i >= NENVS) {
	                        rs = SR_NOANODE ;
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
		    rs1 = nulstr_finish(&n) ;
	            if (rs >= 0) rs = rs1 ;
		} /* end if (nulstr) */
	    } /* end if (loadenvs) */
	    rs1 = vecstr_finish(&envs) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && (fd >= 0)) uc_close(fd) ;
	} /* end if (vecstr) */
	return (rs >= 0) ? fd : rs ;
}
/* end method (openporter::progproc) */

static int procspawn(cc *un,cc *prog,mainv sargv,mainv senvv,
		int pf,int pt,int proto,SA *sap) noex {
	SP_CON		psa ;
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	if ((rs = procspawn_begin(&psa,prog,sargv,senvv)) >= 0) {
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
}
/* end subroutine (procspawn) */

static int procspawn_begin(SP_CON *psp,cc *prog,mainv sargv,mainv senvv) noex {
	int		rs = SR_FAULT ;
	if (psp && prog && sargv && senvv) {
	    memclear(psp) ;
	    psp->opts |= SPAWNPROC_OIGNINTR ;
	    psp->opts |= SPAWNPROC_OSETPGRP ;
	    psp->disp[0] = SPAWNPROC_DOPEN ;
	    psp->disp[1] = SPAWNPROC_DCLOSE ;
	    psp->disp[2] = SPAWNPROC_DCLOSE ;
	    rs = spawnproc(psp,prog,sargv,senvv) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (procspawn_begin) */

static int procspawn_end(pid_t pid,int *csp) noex {
	return u_waitpid(pid,csp,0) ;
}
/* end subroutine (procspawn_end) */

static int procexchange(cc *un,int cfd,int pf,int pt,int proto,SA *sap) noex {
	cint		mlen = MBUFLEN ;
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	char		mbuf[MBUFLEN+1] ;
	if ((rs = uc_malloc((mlen+1),&mbuf)) >= 0) {
	    openportmsg_req	m0{} ;
	    openportmsg_res	m1{} ;
	    int		ml ;
	    m0.msgtype = openportmsgtype_request ;
	    m0.pf = pf ;
	    m0.ptype = pt ;
	    m0.proto = proto ;
	    m0.sa = *sap ;
	    strwcpy(m0.username,un,unlen) ;
    
	    rs = openportmsg_msgrequest(&m0,0,mbuf,mlen) ;
	    ml = rs ;
    
	    if (rs >= 0)
	        rs = uc_writen(cfd,mbuf,ml) ;
    
	    if (rs >= 0) {
	        cint	mt = openportmsgtype_response ;
    
	        rs1 = u_read(cfd,mbuf,mlen) ;
	        ml = rs1 ;
	        if (rs1 >= 0) {
	            rs1 = openportmsg_msgresponse(&m1,1,mbuf,ml) ;
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
	    rs1 = uc_free(mbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (procexchange) */

static int loadenvs(vecstr *elp,cchar *un,cchar *prog,cchar *bn,int bl) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	if (rs >= 0) {
	    rs = vecstr_envadd(elp,"_",prog,-1) ;
	    if (rs < INT_MAX) c += 1 ;
	}
	if (rs >= 0) {
	    rs = vecstr_envadd(elp,"_EF",prog,-1) ;
	    if (rs < INT_MAX) c += 1 ;
	}
	if (rs >= 0) {
	    rs = vecstr_envadd(elp,"_A0",bn,bl) ;
	    if (rs < INT_MAX) c += 1 ;
	}
	if (rs >= 0) {
	    cchar	*key = varname.username ;
	    rs = vecstr_envadd(elp,key,un,-1) ;
	    if (rs < INT_MAX) c += 1 ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (loadenvs) */

static int progfind(char *progfname) noex {
	cnullptr	np{} ;
	USTAT		sb ;
	int		rs = SR_LIBACC ;
	int		rl = 0 ;
	int		len = 0 ;
	cchar		*bin = "sbin" ;
	cchar		*pn = OPERPORT_PROGNAME ;
	for (int i = 0 ; prs[i] != nullptr ; i += 1) {
	    if ((rs = mkpath3(progfname,prs[i],bin,pn)) >= 0) {
	        rl = rs ;
	        if ((rs = u_stat(progfname,&sb)) >= 0) {
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
}
/* end subroutine (progfind) */


