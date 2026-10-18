/* cm_prime SUPPORT (Communications-Connection Manager) */
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
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<getusername.h>		/* LIBUC */
#include	<expcook.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<fieldterms.h>		/* LIBUC */
#include	<field.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<buffer.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU |TIMEBUFLEN| */

#include	"cm_subinfo.h"
#include	"cm.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	TI_FILECHECK	3

#define	EC		expcook
#define	SI		cm_subinfo
#define	SY		systems
#define	SY_ENT		systems_ent
#define	ST_CUR		systems_cur
#define	SD		sysdialer
#define	SD_ARGS		sysdialer_args
#define	SD_ENT		sysdialer_ent


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */

typedef mainv		mv ;

extern "C" {
    typedef int (*open_f)	(void *) noex ;
    typedef int (*close_f)	(void *) noex ;
    typedef int (*shutdown_f)	(void *,int) noex ;
    typedef int (*reade_f)	(void *,void *,int,int,int) noex ;
    typedef int (*recve_f)	(void *,void *,int,int,int,int) noex ;
    typedef int (*write_f)	(void *,cvoid *,int) noex ;
} /* end extern (C) */


/* external subroutines */

namespace cm_ns {
    extern int cm_trysys(CM *,SI *,SD *,SD_ARGS *,
			EC *,SY_ENT *,cc *,cc *,mv) noex ;
    extern int cm_trysysargs(CM *,SI *,vecstr *,cc *,int) noex ;
} /* end namespace (cm_ns) */


/* external variables */


/* local structures */

namespace {
    struct caller {
    	open_f		open ;
    	close_f		close ;
    	shutdown_f	shutdown ;
    	reade_f		reade ;
    	recve_f		recve ;
    	write_f		write ;
    } ; /* end struct (caller) */
    struct vars {
	int		maxnamelen ;
	int		maxpathlen ;
	int		usernamelen ;
	int		nodenamelen ;
	int		hostnamelen ;
	int		maxarg ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

template<typename ... Args>
local int cm_ctor(cm *op,Args ... args) noex {
    	CM		*hop = op ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if (caller *callp = new(nt) caller) ylikely {
		op->callp = callp ;
		if (cm_args *cap = new(nt) cm_args) ylikely {
		    op->argp = cap ;
		    rs = SR_OK ;
		} /* end if (new-cm_args) */
		if (rs < 0) {
		    delete callp ;
		    op->callp = nullptr ;
		} /* end if (error) */
	    } /* emd if (new-caller) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (cm_ctor) */

local int cm_dtor(cm *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->callp) {
		delete op->callp ;
		op->callp = nullptr ;
	    } /* end if */
	    if (op->argp) {
		delete op->argp ;
		op->argp = nullptr ;
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (cm_dtor) */

template<typename ... Args>
local inline int cm_magic(cm *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == CM_MAGIC) ? SR_OK : SR_NOTOPEN ;
	} /* end */
	return rs ;
} /* end subroutine (cm_magic) */

#ifdef	COMMENT
local int cm_loadcooks(CM *,SI *,EC *,cm_args *,cc *,cc *,mv) noex ;
#endif


/* local variables */

namespace cm_ns {
    char		terms[fieldterms_termsize] ;
} /* end namespace (cm_ns) */


/* exported variables */


/* exported subroutines */

namespace {
    struct opener {
	cm		*op ;
	cm_args		*argp ;
	cchar		*hn ;
	cchar		*sn ;
	mainv		av ;
	SD_ARGS		da{} ;
	expcook		cooks ;
	cm_subinfo	si ;
	int		arglen ;
	opener(cm *ªop,cm_args *ªap,ccp ªhn,ccp ªsn,mainv ªav) noex {
	    op		= ªop ;
	    argp	= ªap ;
	    hn		= ªhn ;
	    sn		= ªsn ;
	    av		= ªav ;
	} ; /* end ctor */
	operator int () noex ;
	int procargs	() noex ;
	int loadcooks	() noex ;
	int procargs	() noex ;
	int tryfind	() noex ;
	int trysave	() noex ;
    } ; /* end struct (opener) */
} /* end namespace */

int cm_open(cm *op,cm_args *ap,cc *hn,cc *sn,con mainv av) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if ((rs = cm_ctor(op,ap,hn,sn,av)) >= 0)  ylikely {
	    rs = SR_INVALID ;
	    /* do we have the necessary helper objects */
	    if (ap->dp && ap->sp) ylikely {
		opener oo(this,ap,hn,sn,av) ;
		rs = oo ;
	    } /* end if (valid) */
	    if (rs < 0) {
		cm_dtor(op) ;
	    } /* end if (error) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (cm_open) */

opener::operator int () noex {
    	int		rs ;
	int		rs1 ;
	if ((rs = procargs()) >= 0) ylikely {
	    {
	        if ((rs = cooks.start) >= 0) ylikely {
	    	    if ((rs = loadcooks()) >= 0) ylikely {
			if ((rs tryfind()) >= 0) ylikely {
			    rs = trysave()  ;
			}
		    } /* end if (cm_loadcooks) */
	        } /* end if (load-cooks) */
	        rs1 = cools.finish ;
                if (rs >= 0) rs = rs1 ;
	    } /* end block */
	    if (rs < 0) {
	        if (op->dobj) {
	            if (op->c.close) {
	                (*op->c.close)(op->dobj) ;
		    } /* end if */
		    {
	                lm_free(op->dobj) ;
	                op->dobj = nullptr ;
		    } /* end (memory-release) */
	        } /* end if */
	        op->magval = 0 ;
	    } /* end if (error) */
	} /* end if (procargs) */
    	return rs ;
} /* end method (opener::operator) */

int opener::procargs() noex {
    	int		rs = SR_OK ;
	{
	    da.pr	= ap->pr ;
	    da.prn	= ap->prn ;
	    da.timeout	= ap->timeout ;
	    da.options	= ap->options ;
	}
	return rs ;
} /* end method (opener::procrgs) */

int opener::tryfind() noex {
	int		rs ;
	if (SY_CUR cur ; (rs = systems_curbegin(ap->sp,&cur)) >= 0) {
	    SY_ENT *sep ;
	    while ((rs = systems_fetch(ap->sp,hn,&cur,&sep)) >= 0) {
	        rs = cm_trysys(op,&si,ap->dp,&da,&cooks,sep,hn,sn,av) ;
	        if (rs != 0) break ;
	    } /* end while */
	    systems_curend(ap->sp,&cur) ;
	} /* end if (cursor) */
	return rs ;
} /* end method (opener::tryfind) */

int opener::trysave() noex {
    	int		rs ;
	cint		sl = sep->dialernamelen ;
	ccharp		sp = sep->dialername ;
	if (cchar *cp ; (rs = lm_strw(sp,sl,&cp)) >= 0) {
	    op->magval = CM_MAGIC ;
	    op->dname = cp ;
	} /* end if */
	return rs ;
} /* end method (opener::trysave) */

int cm_close(CM *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = cm_magic(op)) >= 0) ylikely {
	    if (op->dname) {
	        rs1 = lm_free(op->dname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dname = nullptr ;
	    } /* end if (memory-release) */
	    /* close the connection */
	    if (op->c.close != nullptr) {
	        rs1 = (*op->c.close)(op->dobj) ;
	        if (rs >= 0) rs = rs1 ;
	        op->c.close = nullptr ;
	    } /* end */
	    if (op->dobj) {
	        rs1 = lm_free(op->dobj) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dobj = nullptr ;
	    } /* end if (memory-release) */
	    {
	        rs1 = cm_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end */
	    op->magval = 0 ;
	} /* end if (cm_magic) */
	return rs ;
} /* end subroutine (cm_close) */

int cm_getinfo(CM *op,char *nbuf,int nlen,CM_INFO *ip) noex {
    	int		rs ;
	if ((rs = cm_magic(op,nbuf,ip)) >= 0) ylikely {
	    ip->itype = op->itype ;
	    ip->dflags = op->dflags ;
	    strwcpy(ip->dname,op->dname,MAXNAMELEN) ;
	} /* end if (cm_magic) */
	return rs ;
} /* end subroutine (cm_info) */

int cm_reade(CM *op,char *bufp,int bufl,int to,int ro) noex {
	int		rs ;
	if ((rs = cm_magic(op,bufp)) >= 0) ylikely {
	    if (op->c.reade) {
	        rs = (*op->c.reade)(op->dobj,bufp,bufl,to,ro) ;
	    } /* end */
	} /* end if (cm_magic) */
	return rs ;
} /* end subroutine (cm_reade) */

int cm_recve(CM *op,char *bufp,int bufl,int fl,int to,int ro) noex {
	int		rs ;
	if ((rs = cm_magic(op,bufp)) >= 0) ylikely {
	    if (op->c.recve) {
	        rs = (*op->c.recve)(op->dobj,bufp,bufl,fl,to,ro) ;
	    } /* end */
	} /* end if (cm_magic) */
	return rs ;
} /* end subroutine (cm_recve) */

int cm_write(CM *op,cchar *bufp,int bufl) noex {
	int		rs ;
	if ((rs = cm_magic(op)) >= 0) ylikely {
	    if (op->c.write) {
	        rs = (*op->c.write)(op->dobj,bufp,bufl) ;
	    }
	} /* end if (cm_magic) */
	return rs ;
} /* end subroutine (cm_write) */

int cm_shutdown(CM *op,int cmd) noex {
	int		rs ;
	if ((rs = cm_magic(op)) >= 0) ylikely {
	    /* do the shutdown if the module supports it */
	    if (op->c.shutdown) {
	        rs = (*op->c.shutdown)(op->dobj,cmd) ;
	    } /* end */
	} /* end if (cm_magic) */
	return rs ;
} /* end subroutine (cm_shutdown) */


/* private subroutines */

#ifdef	COMMENT
int opener::loadcooks() noex {
	int		rs = SR_OK ;
	int		sz ;
	int		cl ;
	char		*argsbuf = nullptr ;

	if ((rs >= 0) && ap->searchname) {
	    rs = cools.add("S",ap->searchname) ;
	}

	if ((rs >= 0) && (ap->pr != nullptr)) {
	    rs = expcook_add(cookp,"R",ap->pr,-1) ;
	}

	if ((rs >= 0) && (ap->prn != nullptr)) {
	    rs = expcook_add(cookp,"RN",ap->prn,-1) ;
	}

	if (rs >= 0) {
	    cchar	*nnp, *dnp ;
	    char	domainname[MAXHOSTNAMELEN + 1] ;
	    char	nodename[NODENAMELEN + 1] ;

	    if ((ap->nodename == nullptr) || (ap->domainname == nullptr)) {
	        nnp = nodename ;
	        dnp = domainname ;
	        if ((rs = getnodedomain(nodename,domainname)) >= 0) {
	            if ((rs = expcook_add(cookp,"N",nodename,-1)) >= 0) {
	        	rs = expcook_add(cookp,"D",domainname,-1) ;
		    }
		} 
	    } else {
	        nnp = ap->nodename ;
	        dnp = ap->domainname ;
	        if ((rs = expcook_add(cookp,"N",ap->nodename,-1)) >= 0) {
	            rs = expcook_add(cookp,"D",ap->domainname,-1) ;
		}
	    } /* end if */

	    if ((rs >= 0) && (ap->hostname == nullptr)) {
	        char	hbuf[MAXHOSTNAMELEN + 1] ;
	        sz = snsds(hbuf,MAXHOSTNAMELEN,nnp,dnp) ;
	        rs = expcook_add(cookp,"H",hbuf,sz) ;
	    } else {
	        rs = expcook_add(cookp,"H",ap->hostname,-1) ;
	    }

	} /* end if */

	if ((rs >= 0) && (ap->username == nullptr)) {
	    char	ubuf[USERNAMELEN + 1] ;
	    if ((rs = getusername(ubuf,USERNAMELEN,-1)) >= 0) {
	        rs = expcook_add(cookp,"U",ubuf,-1) ;
	    }
	} else {
	    rs = expcook_add(cookp,"U",ap->username,-1) ;
	}

	if (rs >= 0) {
	    char	timebuf[TIMEBUFLEN + 1] ;
	    cl = ctdecui(timebuf,TIMEBUFLEN,ap->timeout) ;
	    rs = expcook_add(cookp,"t",timebuf,cl) ;
	} /* end block */

	if (rs >= 0) {
	    rs = expcook_add(cookp,"h",hostname,-1) ;
	}

	if (rs >= 0) {
	    rs = expcook_add(cookp,"s",svcname,-1) ;
	}

	sz = 1 ;
	if ((rs >= 0) && (av != nullptr)) {
	    for (int i = 0 ; av[i] != nullptr ; i += 1) {
		sz += (lenstr(av[i]) + 1) ;
	    } /* end for */
	}

	sip->argslen = (sz - 1) ;
	if ((rs >= 0) && ((rs = lm_mall(sz,&argsbuf)) >= 0)) {
	    int		alen = 0 ;

	    argsbuf[0] = '\0' ;
	    if (av != nullptr) {
	        sbuf ab ;
	        if ((rs = sbuf_start(&ab,argsbuf,sip->argslen)) >= 0) {
	            for (int i = 0 ; av[i] != nullptr ; i += 1) {
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

	    lm_free(argsbuf) ;
	} /* end if (memory_allocation) */

	return rs ;
} /* end method (opener::loadcooks) */
#endif /* COMMENT */

int opener::loadcooks() noex {
    	return SR_OK ;
} /* end method (opener::loadcooks) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_mn)) >= 0) ylikely {
	    maxnamelen = rs ;
	    if ((rs = bufsizeget(bufsize_mp)) >= 0) ylikely {
		maxpathlen = rs ;
	    	if ((rs = bufsizeget(bufsize_un)) >= 0) ylikely {
		    usernamelen = rs ;
	    	    if ((rs = bufsizeget(bufsize_nn)) >= 0) ylikely {
			nodenamelen = rs ;
	    	    	if ((rs = bufsizeget(bufsize_hostname)) >= 0) ylikely {
			    hostnamelen = rs ;
	    	    	    if ((rs = bufsizeget(bufsize_ma)) >= 0) ylikely {
				using cm_ns::terms ;
				maxarg = rs ;
				rs = fieldterms(terms,0," \t") ;
			    } /* end if */
			}
		    }
		}
	    }
	} /* end if */
	return rs ;
} /* end method (vars::operator) */


