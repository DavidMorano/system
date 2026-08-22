/* dialhttp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* dial out to the web */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dialhttp

	Descrption:
	This subroutine will dial out to an INET host using the
	HTTP protocol.

	Synopsis:
	int dialhttp(cc *hn,cc *ps,int af,cc *svc,mainv sargv,
			int to,int dot) noex

	Arguments:
	hn		host to dial to
	ps		port specification to use
	af		address family
	svc		service specification
	sargv		pointer to array of pointers to arguments
	to		timeout ('>0' means use one, '-1' means do not)
	dot		any dial options

	Returns:
	>=0		file descriptor
	<0		error in dialing (system-return)

	What is up with the 'timeout' argument?
	>0		use the timeout as it is
	==0             asynchronously span the connect and do not wait
			for it
	<0              use the system default timeout (xx minutes --
			whatever)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/socket.h>		/* POSIX® */
#include	<netinet/in.h>		/* POSIX® */
#include	<arpa/inet.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<csignal>		/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uinet.h>		/* LIBU |AF_{x}(3u)| */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucsig.h>		/* LIBUC */
#include	<getnodename.h>		/* LIBUC */
#include	<getnodedomain.h>	/* LIBUC |getinetdomain(3uc)| */
#include	<getprogpath.h>		/* LIBUC */
#include	<bufsizevar.hh>		/* LIBUC */
#include	<varnames.hh>		/* LIBUC */
#include	<estrings.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<permx.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<pathclean.h>		/* LIBUC */
#include	<mkpr.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<ctdec.h>		/* LIBUC */
#include	<strx.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"dialtcp.h"
#include	"dialprog.h"

#pragma		GCC dependency		"mod/uconstants.ccm"

import uconstants ;			/* |varname(3u)| */

/* local defines */

#define	PROG_WGET	"wget"
#define	BINDNAME	"bin"
#define	URLMULT		4		/* URL-buflen multiplier */
#define	TOBUFLEN	20

#define	VS		vecstr


/* imported namespaces */


/* local typedefs */

typedef mainv		mv ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	dialhttper(cc *,cc *,int,cc *,mainv,int,int) noex ;
local int	findprog(char *,cchar *) noex ;
local int	loadpath(vecstr *) noex ;
local int	findprprog(ids *,vecstr *,char *,cchar *) noex ;
local int	runprog(cchar *,cchar *,cchar *,cchar *) noex ;
local int	mkurl(char *,int,cchar *,cchar *,cchar *,mainv) noex ;
local int	mkvars() noex ;
local int	afvalid(int) noex ;


/* local variables */

static constexpr cpcchar	prnames[] = {
	"gnu",
	"extra",
	"usr",
	"local",
	nullptr
} ;

static bufsizevar		maxpathlen(bufsize_mp) ;
local int			urlbuflen ;


/* exported variables */


/* exported subroutines */

int dialhttp(cc *hn,cc *ps,int af,cc *svc,mainv sargv,int to,int) noex {
	int		rs = SR_FAULT ;
	int		fd = -1 ;
	if (hn) ylikely {
	    rs = SR_INVALID ;
	    if (hn[0]) ylikely {
		if ((rs = afvalid(af)) >= 0) ylikely {
		    if (static cint rsv = mkvars() ; (rs = rsv) >= 0) {
			rs = dialhttper(hn,ps,af,svc,sargv,to,0) ;
			fd = rs ;
		    } /* end if (mkvars) */
	        } /* end if (af-valid) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (dialhttp) */


/* local subroutines */

local int dialhttper(cc *hn,cc *ps,int,cc *svc,mv sargv,int to,int) noex {
	cint		ulen = urlbuflen ;
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	if (char *ubuf ; (rs = lm_mall((ulen+1),&ubuf)) >= 0) {
	    if ((rs = mkurl(ubuf,ulen,hn,ps,svc,sargv)) >= 0) {
		cchar	*pn = PROG_WGET ;
		if (char *ebuf ; (rs = lm_mp(&ebuf)) >= 0) {
		    if ((rs = findprog(ebuf,pn)) >= 0) {
			char	tobuf[TOBUFLEN + 1] = {} ;
	                if (to >= 0) {
	                    if (to == 0) to = 1 ;
	                    rs = ctdeci(tobuf,TOBUFLEN,to) ;
	                }
	                if (rs >= 0) {
	                    rs = runprog(ebuf,ubuf,tobuf,pn) ;
	                    fd = rs ;
			} /* end if (ok) */
		    } /* end if (findprog) */
		    rs1 = lm_free(ebuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (mkurl) */
	    rs1 = lm_free(ubuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	if ((rs < 0) && (fd >= 0)) {
	    u_close(fd) ;
	} /* end if (error) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (dialhttper) */

local int findprog(char *execfname,cchar *pn) noex {
	int		rs  ;
	int		rs1 ;
	int		pl = 0 ;
	execfname[0] = '\0' ;
	if (ids id ; (rs = ids_load(&id)) >= 0) ylikely {
	    if (vecstr path ; (rs = vecstr_start(&path,20,0)) >= 0) {
	        if ((rs = loadpath(&path)) >= 0) {
	            rs = getprogpath(&id,&path,execfname,pn,-1) ;
	            pl = rs ;
	            if ((rs == SR_NOENT) || (rs == SR_ACCESS)) {
	                rs = findprprog(&id,&path,execfname,pn) ;
	                pl = rs ;
	            }
	        } /* end if (loadpath) */
	        rs1 = vecstr_finish(&path) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (vecstr) */
	    rs1 = ids_release(&id) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ids) */
	return (rs >= 0) ? pl : rs ;
} /* end subroutine (findprog) */

local int findprprog(ids *idp,vecstr *plp,char *rbuf,cchar *pn) noex {
	cint		rsn = SR_NOTFOUND ;
	cint		perms = (R_OK | X_OK) ;
	int		rs ;
	int		rs1 ;
	int		rl = 0 ;
	rbuf[0] = '\0' ;
	if (char *dn ; (rs = lm_hostname(&dn)) >= 0) ylikely {
	    if ((rs = getnodedomain(nullptr,dn)) >= 0) ylikely {
	        cint	sz = ((maxpathlen + 1) * 2) ;
	        cchar	*bdname = BINDNAME ;
	        if (char *prbuf ; (rs = lm_mall((sz+1),&prbuf)) >= 0) {
		    cint	prlen = maxpathlen ;
	            char	*dbuf = (prbuf + (maxpathlen+1)) ;
	            for (int i = 0 ; prnames[i] ; i += 1) {
	                rbuf[0] = '\0' ;
	                if ((rs = mkpr(prbuf,prlen,prnames[i],dn)) >= 0) {
	                    if ((rs = mkpath2(dbuf,prbuf,bdname)) >= 0) {
	                        if ((rs = vecstr_find(plp,dbuf)) == rsn) {
	                            if ((rs = mkpath2(rbuf,dbuf,pn)) >= 0) {
	    			        USTAT	sb ;
	                                rl = rs ;
	                                if ((rs = u_stat(rbuf,&sb)) >= 0) {
	                                    if (S_ISREG(sb.st_mode)) {
	                                        rs = permids(idp,&sb,perms) ;
	                                        if (isNotAccess(rs)) {
	                                            rl = 0 ;
	                                        }
	                                    } else {
	                                        rs = SR_ISDIR ;
	                                    }
	                                } else if (isNotPresent(rs)) {
	                                    rl = 0 ;
	                                    rs = SR_OK ;
	                                }
	                            } /* end if (mkpath) */
	                        } /* end if (not-found) */
	                    } /* end if (mkpath) */
	                } /* end if (mkpr) */
	                if (rl > 0) break ;
	                if (rs < 0) break ;
	            } /* end for */
		    rs1 = lm_free(prbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (getnodedomain) */
	    rs1 = lm_free(dn) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (findprprog) */

local int runprog(cchar *efname,cchar *ubuf,cchar *tobuf,cchar *pn) noex {
	SIGACTION	nsh{} ;
	SIGACTION	osh ;
	sigset_t	smask ;
	cint		of = O_NOCTTY ;
	cint		sig = SIGPIPE ;
	int		rs ;
	int		fd = -1 ;
	uc_sigsetempty(&smask) ;
	nsh.sa_handler = SIG_IGN ;
	nsh.sa_mask = smask ;
	nsh.sa_flags = 0 ;
	if ((rs = u_sigaction(sig,&nsh,&osh)) >= 0) {
	    int		i = 0 ;
	    cchar	*av[10] ;
	    av[i++] = pn ;
	    av[i++] = "-q" ;
	    av[i++] = "-O" ;
	    av[i++] = "-" ;
	    if (tobuf[0] != '\0') {
	        av[i++] = "-T" ;
	        av[i++] = tobuf ;
	    }
	    av[i++] = ubuf ;
	    av[i] = nullptr ;
	    rs = dialprog(efname,of,av,nullptr,nullptr) ;
	    fd = rs ;
	    u_sigaction(sig,&osh,nullptr) ;
	} /* end if (sigaction) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (runprog) */

local int loadpath(vecstr *plp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	cchar		*vn = varname.path ;
	if (cchar *pp ; (pp = getenver(vn)) != nullptr) {
	    if (char *tbuf ; (rs = lm_mp(&tbuf)) >= 0) {
	        cchar	*tp ;
	        while ((tp = strbrk(pp,":;")) != nullptr) {
		    cint	pl = intconv(tp - pp) ;
	            if ((rs = pathclean(tbuf,pp,pl)) >= 0) {
			rs = vecstr_adduniq(plp,tbuf,rs) ;
			c += (rs < INT_MAX) ;
		    }
	            pp = (tp + 1) ;
	            if (rs < 0) break ;
	        } /* end while */
	        if ((rs >= 0) && (pp[0] != '\0')) {
	            if ((rs = pathclean(tbuf,pp,-1)) >= 0) {
			rs = vecstr_adduniq(plp,tbuf,rs) ;
			c += (rs < INT_MAX) ;
	            }
	        } /* end if (trailing one) */
		rs1 = lm_free(tbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (getenver) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (loadpath) */

local int mkurl(char *ubuf,int ulen,cc *hn,cc *ps,cc *svc,mainv sargv) noex {
	int		rs ;
	int		rs1 ;
	if (sbuf url ; (rs = sbuf_start(&url,ubuf,ulen)) >= 0) {
	    sbuf_strw(&url,"http://",-1) ;
	    sbuf_strw(&url,hn,-1) ;
	    if ((ps != nullptr) && (ps[0] != '\0')) {
	        sbuf_chr(&url,':') ;
	        sbuf_strw(&url,ps,-1) ;
	    }
	    sbuf_chr(&url,'/') ;
	    if ((svc != nullptr) && (svc[0] != '\0')) {
	        if (svc[0] == '/') svc += 1 ;
	        sbuf_strw(&url,svc,-1) ;
	    } /* end if */
	    if (sargv != nullptr) {
	        for (int i = 0 ; sargv[i] ; i += 1) {
	            sbuf_chr(&url,'?') ;
	            sbuf_buf(&url,sargv[i],-1) ;
	        } /* end for */
	    } /* end if */
	    rs1 = sbuf_finish(&url) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sbuf) */
	return rs ;
} /* end subroutine (mkurl) */

local int mkvars() noex {
	int		rs ;
	if ((rs = maxpathlen) >= 0) {
	    urlbuflen = (rs * URLMULT) ;
	}
	return rs ;
} /* end subroutine (mkvars) */

local int afvalid(int af) noex {
	bool		f = false ;
	f = f || (af == AF_UNSPEC) ;
	f = f || (af == AF_INET4) ;
	f = f || (af == AF_INET6) ;
	return (f) ? SR_OK : SR_AFNOSUPPORT ;
} /* end subroutine (afvalid) */


