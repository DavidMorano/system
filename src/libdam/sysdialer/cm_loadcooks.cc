/* cm_loadcooks SUPPORT (Communications-Connection Manager) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Connection Manager */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was adopted for use from the DWD program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	cm

	Description:
	This is a communications connection manager object.  This
	object abstracts the details of a particular connection
	from the calling program.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<algorithm>		/* CSTD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<getusername.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<fieldterms.h>		/* LIBUC */
#include	<field.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<buffer.h>		/* LIBUC */
#include	<expcook.h>		/* LIBUC */
#include	<ctdec.h>		/* LIBUC */
#include	<snx.h>			/* LIBUC */
#include	<systems.h>		/* LIBUC */
#include	<sysdialer.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU |TIMEBUFLEN| */

#include	"cm_defs.h"
#include	"cm_subinfo.hh"
#include	"cm_vars.hh"
#include	"cm_caller.hh"
#include	"cm_opener.hh"
#include	"cm.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine */
using std::max ;			/* subroutine */
using libuc::mem ;			/* variable */
using cm_ns::var ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

enum cooknames {
    cookname_searchname,
    cookname_progroot,
    cookname_rootname,
    cookname_nodename,
    cookname_domainname,
    cookname_inetname,
    cookname_username,
    cookname_hostname,
    cookname_servname,
    cookname_timeout,
    cookname_overlast
} ;  /* end enum (cooknames) */

namespace {
    struct namer {
	ccharp		n[cookname_overlast] ;
	namer() noex {
	    n[cookname_searchname]	= "S" ;
	    n[cookname_progroot]	= "R" ;
	    n[cookname_rootname]	= "RN" ;
	    n[cookname_nodename]	= "N" ;
	    n[cookname_domainname]	= "D" ;
	    n[cookname_inetname]	= "H" ;
	    n[cookname_username]	= "U" ;
	    n[cookname_hostname]	= "h" ;
	    n[cookname_servname]	= "s" ;
	    n[cookname_timeout]		= "t" ;
	} ; /* end ctor */
    } ; /* end struct (namer) */
} /* end namespace */

namespace {
    struct lener {
	operator int () noex ;
    } ; /* end struct (lener) */
    struct loader {
	cm_args		*argp ;
	expcook		*cookp ;
	cchar		*hn ;
	cchar		*sn ;
	mainv		av ;
	charp		mbuf ;
	int		mlen ;
	loader(cm_args *ap,expcook *ep,ccp ªhn,ccp ªsn,mainv ªav) noex {
	    argp	= ap ;
	    cookp	= ep ;
	    hn		= ªhn ;
	    sn		= ªsn ;
	    av		= ªav ;
	    mbuf	= nullptr ;
	    mlen	= 0 ;
	} ; /* end ctor */
	operator int () noex ;
	int argvals	() noex ;
	int argsz	() noex ;
	int username	() noex ;
	int digtime	() noex ;
	int inetname	() noex ;
	int argbufer	() noex ;
    } ; /* end struct (loader) */
    typedef int (loader::*loader_m)() noex ;
} /* end namespace */


/* forward references */


/* local variables */

static namer		names ;
static lener		varlen ;

constexpr loader_m	loadmems[] = {
    	&loader::argvals,
	&loader::inetname,
	&loader::username,
	&loader::digtime,
	&loader::argbufer
} ; /* end array */


/* exported variables */


/* exported subroutines */

namespace cm_ns {
    int opener::loadcooks() noex {
	loader lo(argp,&cooks,hn,sn,av) ;
	return lo ;
    } /* end methof (opener::loadcooks) */
} /* end namespace (cm_ns) */


/* local subroutines */

loader::operator int () noex {
    	int		rs ;
	int		rs1 ;
	if (static cint llen = varlen ; (rs = llen) >= 0) {
	    mlen = llen ;
	    if ((rs = mem.mall((mlen + 1),&mbuf)) >= 0) {
		for (cauto &m : loadmems) {
		    rs = (this->*m)() ;
		    if (rs < 0) break ;
		} /* end for */
	        rs1 = mem.free(mbuf) ;
	        if (rs >= 0) rs = rs1 ;
	        mbuf = nullptr ;
	        mlen = 0 ;
	    } /* end if (memory-acquire) */
	} /* end if (lener) */
    	return rs ;
} /* end method (loader::operator) */

int loader::argvals() noex {
    	int		rs = SR_OK ;
	for (int i = 0 ; i < cookname_overlast ; i += 1) {
	    cchar *valp = nullptr ;
	    switch (i) {
	    case cookname_searchname:
		valp = argp->searchname ;
		break ;
	    case cookname_progroot:
		valp = argp->pr ;
		break ;
	    case cookname_rootname:
		valp = argp->prn ;
		break ;
	    case cookname_nodename:
	    case cookname_domainname:
		break ;
	    case cookname_hostname:
		valp = hn ;
		break ;
	    case cookname_servname:
		valp = sn ;
	        break ;
	    } /* end switch */
	    if (valp) {
	        rs = cookp->add(names.n[i],valp) ;
	    } /* end */
	    if (rs < 0) break ;
	} /* end for */
	return rs ;
} /* end method (loader::argvals) */

int loader::inetname() noex {
    	cint		ilen = min(mlen,var.hostnamelen) ;
    	int		rs ;
	ccharp		cnp = names.n[cookname_inetname] ;
	if (ccharp inet = argp->hostname) {
		cchar *nnp = nullptr ;
		cchar *dnp = nullptr ;
	        if ((rs = snsds(mbuf,ilen,nnp,dnp)) >= 0) {
	            rs = cookp->add(cnp,mbuf,rs) ;
		} /* end */
	} else {
	        rs = cookp->add(cnp,inet) ;
	} /* end */
	return rs ;
} /* end method (loader::inetname) */

int loader::username() noex {
    	int		rs ;
	ccharp		cnp = names.n[cookname_username] ;
	if (ccharp un = argp->username) {
	    rs = cookp->add(cnp,un) ;
	} else {
    	    cint userlen = var.usernamelen ;
	    if ((rs = getusername(mbuf,userlen,-1)) >= 0) {
	        rs = cookp->add(cnp,mbuf) ;
	    } /* end */
	} /* end */
	return rs ;
} /* end method (loader::username) */

int loader::digtime() noex {
    	cint		tlen = min(mlen,TIMEBUFLEN) ; /* <- just for show */
	int		rs ;
	ccharp		cnp = names.n[cookname_timeout] ;
	if ((rs = ctdec(mbuf,tlen,argp->timeout)) >= 0) {
	    rs = cookp->add(cnp,mbuf,rs) ;
	} /* end if */
	return rs ;
} /* end method (loader::digtime) */

int loader::argbufer() noex {
	int		rs ;
	int		rs1 ;
	if ((rs = argsz()) >= 0) {
    	    cint	sz = (rs + 1) ;
	    cint	argslen = rs ;
	    if (char *argsbuf ; (rs = mem_mall(sz,&argsbuf)) >= 0) {
	        int	al = 0 ;
	        argsbuf[0] = eos ;
	        if (av) {
	            if (sbuf ab ; (rs = ab.start(argsbuf,argslen)) >= 0) {
	                for (int i = 0 ; av[i] ; i += 1) {
	                    if (i > 0) ab.chr(' ') ;
	                    ab.strw(av[i],-1) ;
	                } /* end for */
	                al = ab.finish ;
		        if (rs >= 0) rs = al ;
		    } /* end if (sbuf) */
	        } /* end if (had some arguments) */
	        if (rs >= 0) {
	            rs = cookp->add("a",argsbuf,al) ;
	        } /* end if (ok) */
	        rs1 = mem_free(argsbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (memory_allocation) */
	} /* end if (argsz) */
	return rs ;
} /* end method (loader::argbufer) */

int loader::argsz() noex {
    	int		rs = SR_OK ;
	int		sz = 1 ;
	if ((rs >= 0) && av) {
	    for (int i = 0 ; av[i] ; i += 1) {
		sz += (lenstr(av[i]) + 1) ;
	    } /* end for */
	} /* end if */
	return (rs >= 0) ? sz : rs ;
} /* end method (loader::argsz) */


#ifdef	COMMENT
	int		rs = SR_OK ;
	int		sz ;
	int		cl ;
	char		*argsbuf = nullptr ;

	if ((rs >= 0) && argp->searchname) {
	    rs = cooks.add("S",argp->searchname) ;
	}

	if ((rs >= 0) && (argp->pr != nullptr)) {
	    rs = expcook_add(cookp,"R",argp->pr,-1) ;
	}

	if ((rs >= 0) && (argp->prn != nullptr)) {
	    rs = expcook_add(cookp,"RN",argp->prn,-1) ;
	}

	if (rs >= 0) {
	    cchar	*nnp, *dnp ;
	    char	domainname[MAXHOSTNAMELEN + 1] ;
	    char	nodename[NODENAMELEN + 1] ;

	    if ((argp->nodename == nullptr) || (argp->domainname == nullptr)) {
	        nnp = nodename ;
	        dnp = domainname ;
	        if ((rs = getnodedomain(nodename,domainname)) >= 0) {
	            if ((rs = expcook_add(cookp,"N",nodename,-1)) >= 0) {
	        	rs = expcook_add(cookp,"D",domainname,-1) ;
		    }
		} 
	    } else {
	        nnp = argp->nodename ;
	        dnp = argp->domainname ;
	        if ((rs = expcook_add(cookp,"N",argp->nodename,-1)) >= 0) {
	            rs = expcook_add(cookp,"D",argp->domainname,-1) ;
		}
	    } /* end if */

	    if ((rs >= 0) && (argp->hostname == nullptr)) {
	        char	hbuf[MAXHOSTNAMELEN + 1] ;
	        sz = snsds(hbuf,MAXHOSTNAMELEN,nnp,dnp) ;
	        rs = expcook_add(cookp,"H",hbuf,sz) ;
	    } else {
	        rs = expcook_add(cookp,"H",argp->hostname,-1) ;
	    }

	} /* end if */

	if ((rs >= 0) && (argp->username == nullptr)) {
	    char	ubuf[USERNAMELEN + 1] ;
	    if ((rs = getusername(ubuf,USERNAMELEN,-1)) >= 0) {
	        rs = expcook_add(cookp,"U",ubuf,-1) ;
	    }
	} else {
	    rs = expcook_add(cookp,"U",argp->username,-1) ;
	}

	if (rs >= 0) {
	    char	timebuf[TIMEBUFLEN + 1] ;
	    cl = ctdecui(timebuf,TIMEBUFLEN,argp->timeout) ;
	    rs = expcook_add(cookp,"t",timebuf,cl) ;
	} /* end block */

	if (rs >= 0) {
	    rs = expcook_add(cookp,"h",hostname,-1) ;
	}

	if (rs >= 0) {
	    rs = expcook_add(cookp,"s",svcname,-1) ;
	}

	sz = 1 ;
	if ((rs >= 0) && av) {
	    for (int i = 0 ; av[i] ; i += 1) {
		sz += (lenstr(av[i]) + 1) ;
	    } /* end for */
	} /* end if */

	sip->argslen = (sz - 1) ;
	if ((rs >= 0) && ((rs = mem_mall(sz,&argsbuf)) >= 0)) {
	    int		alen = 0 ;
	    argsbuf[0] = eos ;
	    if (av) {
	        sbuf ab ;
	        if ((rs = sbuf_start(&ab,argsbuf,sip->argslen)) >= 0) {
	            for (int i = 0 ; av[i] ; i += 1) {
	                if (i > 0) sbuf_chr(&ab,' ') ;
	                sbuf_strw(&ab,av[i],-1) ;
	            } /* end for */
	            alen = sbuf_finish(&ab) ;
		    if (rs >= 0) rs = alen ;
		} /* end if (sbuf) */
	    } /* end if (had some arguments) */
	    if (rs >= 0) {
	        rs = expcook_add(cookp,"a",argsbuf,alen) ;
	    } /* end if (ok) */
	    mem_free(argsbuf) ;
	} /* end if (memory_allocation) */

	return rs ;
    } /* end method (opener::loadcooks) */
} /* end namespace (cm_ns) */
#endif /* COMMENT */

#ifdef	COMMENT
namespace cm_ns {
    int opener::loadcooks() noex {
    	return SR_OK ;
    } /* end method (opener::loadcooks) */
} /* end namespce (cm_ns) */
#endif /* COMMENT */

lener::operator int () noex {
    	int		rs = 0 ;
	rs = max(rs,var.nodenamelen) ;
	rs = max(rs,var.hostnamelen) ;
	rs = max(rs,var.usernamelen) ;
	rs = max(rs,TIMEBUFLEN) ;
	return rs ;
} /* end method (lener::operator) */


