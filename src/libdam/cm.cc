/* cm SUPPORT */
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
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<getusername.h>
#include	<expcook.h>
#include	<vecstr.h>
#include	<field.h>
#include	<fieldterms.h>
#include	<sbuf.h>
#include	<buffer.h>
#include	<localmisc.h>		/* |TIMEBUFLEN| */

#include	"cm.h"


/* local defines */

#define	TI_FILECHECK	3

#define	EC		expcook
#define	SI		subinfo
#define	SY		systemsnt
#define	SY_ENT		systems_ent
#define	SD		sysdialer
#define	SD_ARGS		sysdialer_arg


/* imported namespaces */


/* local typedefs */

typedef mainv		mv ;


/* external subroutines */


/* external variables */


/* local structures */

struct subinfo {
	int		argslen ;
} ;

namespace {
    struct vars {
	int		maxnamelen ;
	int		maxpathlen ;
	int		usernamelen ;
	int		nodenamelen ;
	int		hostnamelen ;
	int		maxarg ;
	operator int () noex ;
    } ; /* end struct (vars) */
}


/* forward references */

template<typename ... Args>
static int cmsb_ctor(cmsb *op,Args ... args) noex {
    	CM		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cmsb_ctor) */

static int cmsb_dtor(cmsb *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cmsb_dtor) */

template<typename ... Args>
static inline int cmsb_magic(cmsb *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == CM_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (cmsb_magic) */

static int cm_loadcooks(CM *,SI *,EC *,cm_args *,cc *,cc *,mv) noex ;
static int cm_trysys(CM *,SI *,SD *,SD_ARGS *,
			EC *,SY_ENT *,cc *,cc *,mv) noex ;
static int cm_trysysargs(CM *,SI *,vecstr *,cc *,int) noex ;


/* local variables */

static char		terms[fieldterms_termsize] ;


/* exported variables */


/* exported subroutines */

int cm_open(cm *op,cm_args *ap,cc *hostname,cc *svcname,mainv av) noex {
	SUBINFO		si{} ;
	SYSTEMS_CUR	cur ;
	SY_ENT	*sep ;
	SD_ARGS	da{} ;
	expcook		cooks ;
	int		rs ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;
	if (hostname == nullptr) return SR_FAULT ;
	if (svcname == nullptr) return SR_FAULT ;
	if (ap == nullptr) return SR_FAULT ;

/* do we have the necessary helper objects */

	if ((ap->dp == nullptr) || (ap->sp == nullptr)) return SR_INVALID ;

	memclear(op) ;

/* setup dialer arguments */
	da.pr = ap->pr ;
	da.prn = ap->prn ;
	da.timeout = ap->timeout ;
	da.options = ap->options ;

/* setup or calculate the cookie values for later */

	if ((rs = expcook_start(&cooks)) >= 0) {
	    cchar	*hn = hostname ;
	    cchar	*sn = svcname ;
	    if ((rs = cm_loadcooks(op,&si,&cooks,ap,hn,sn,av)) >= 0) {

/* search the SYSTEMS file for our host */

	if ((rs = systems_curbegin(ap->sp,&cur)) >= 0) {
	    cchar	*hn = hostname ;
	    cchar	*sn = svcname ;

	    while ((rs = systems_fetch(ap->sp,hn,&cur,&sep)) >= 0) {
	        rs = cm_trysys(op,&si,ap->dp,&da,&cooks,sep,hn,sn,av) ;
	        if (rs >= 0) break ;
	    } /* end while */

	    systems_curend(ap->sp,&cur) ;
	} /* end if (cursor) */

/* save the dialer name */

	if (rs >= 0) {
	    cint	sl = sep->dialernamelen ;
	    cchar	*sp = sep->dialername ;
	    if (cchar *cp ; (rs = uc_mallocstrw(sp,sl,&cp)) >= 0) {
		op->magic = CM_MAGIC ;
	        op->dname = cp ;
	    }
	}

	    } /* end if (load-cooks) */
	    rs1 = expcook_finish(&cooks) ;
            if (rs >= 0) rs = rs1 ;
	} /* end if (expcook) */

	if (rs < 0) {
	    if (op->dobj != nullptr) {
	        if (op->c.close != nullptr) {
	            (*op->c.close)(op->dobj) ;
		}
	        uc_free(op->dobj) ;
	        op->dobj = nullptr ;
	    }
#ifdef	COMMENT
	    sysdialer_loadout(ap->dp,sep->dialername) ;
#endif
	    op->magic = 0 ;
	}

	return rs ;
}
/* end subroutine (cm_open) */

int cm_close(CM *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != CM_MAGIC) return SR_NOTOPEN ;

	if (op->dname != nullptr) {
	    rs1 = uc_free(op->dname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->dname = nullptr ;
	}

/* close the connection */

	if (op->c.close != nullptr) {
	    rs1 = (*op->c.close)(op->dobj) ;
	    if (rs >= 0) rs = rs1 ;
	    op->c.close = nullptr ;
	}

	if (op->dobj != nullptr) {
	    rs1 = uc_free(op->dobj) ;
	    if (rs >= 0) rs = rs1 ;
	    op->dobj = nullptr ;
	}

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (cm_close) */

int cm_getinfo(CM *op,char *nbuf,int nlen,CM_INFO *ip) noex {
	if (op == nullptr) return SR_FAULT ;
	if (ip == nullptr) return SR_FAULT ;

	if (op->magic != CM_MAGIC) return SR_NOTOPEN ;

	ip->itype = op->itype ;
	ip->dflags = op->dflags ;
	strwcpy(ip->dname,op->dname,MAXNAMELEN) ;

	return SR_OK ;
}
/* end subroutine (cm_info) */

int cm_reade(CM *op,char *buf,int buflen,int timeout,int opts) noex {
	int		rs = SR_NOTSUP ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != CM_MAGIC) return SR_NOTOPEN ;

	if (op->c.reade != nullptr) {
	    rs = (*op->c.reade)(op->dobj,buf,buflen,timeout,opts) ;
	}

	return rs ;
}
/* end subroutine (cm_reade) */

int cm_recve(CM *op,char *buf,int buflen,int flags,int timeout,int opts) noex {
	int		rs = SR_NOTSUP ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != CM_MAGIC) return SR_NOTOPEN ;

	if (op->c.recve != nullptr) {
	    rs = (*op->c.recve)(op->dobj,buf,buflen,flags,timeout,opts) ;
	}

	return rs ;
}
/* end subroutine (cm_recve) */

int cm_write(CM *op,cchar *buf,int buflen) noex {
	int		rs = SR_NOTSUP ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != CM_MAGIC) return SR_NOTOPEN ;

	if (op->c.write != nullptr) {
	    rs = (*op->c.write)(op->dobj,buf,buflen) ;
	}

	return rs ;
}
/* end subroutine (cm_write) */

int cm_shutdown(CM *op,int cmd) noex {
	int		rs = SR_NOTSUP ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != CM_MAGIC) return SR_NOTOPEN ;

/* do the shutdown if the module supports it */

	if (op->c.shutdown != nullptr) {
	    rs = (*op->c.shutdown)(op->dobj,cmd) ;
	}

	return rs ;
}
/* end subroutine (cm_shutdown) */


/* private subroutines */

static int cm_loadcooks(cm *op,SI *sip,EC *cookp,cm_args *ap,
		cc *hostname,cc *svcname,mainv av) noex {
	int		rs = SR_OK ;
	int		size ;
	int		i ;
	int		cl ;
	char		*argsbuf = nullptr ;

	if (op == nullptr) return SR_FAULT ;

	if ((rs >= 0) && (ap->searchname != nullptr))
	    rs = expcook_add(cookp,"S",ap->searchname,-1) ;

	if ((rs >= 0) && (ap->pr != nullptr))
	    rs = expcook_add(cookp,"R",ap->pr,-1) ;

	if ((rs >= 0) && (ap->prn != nullptr))
	    rs = expcook_add(cookp,"RN",ap->prn,-1) ;

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

	        size = snsds(hbuf,MAXHOSTNAMELEN,nnp,dnp) ;

	        rs = expcook_add(cookp,"H",hbuf,size) ;

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

	if (rs >= 0)
	    rs = expcook_add(cookp,"h",hostname,-1) ;

	if (rs >= 0)
	    rs = expcook_add(cookp,"s",svcname,-1) ;

	size = 1 ;
	if ((rs >= 0) && (av != nullptr)) {
	    for (i = 0 ; av[i] != nullptr ; i += 1) {
		size += (strlen(av[i]) + 1) ;
	    }
	}

	sip->argslen = (size-1) ;
	if ((rs >= 0) && ((rs = uc_malloc(size,&argsbuf)) >= 0)) {
	    int		alen = 0 ;

	    argsbuf[0] = '\0' ;
	    if (av != nullptr) {
	        SBUF	ab ;
	        if ((rs = sbuf_start(&ab,argsbuf,sip->argslen)) >= 0) {
	            for (i = 0 ; av[i] != nullptr ; i += 1) {
	                if (i > 0) sbuf_chr(&ab,' ') ;
	                sbuf_strw(&ab,av[i],-1) ;
	            } /* end for */
	            alen = sbuf_finish(&ab) ;
		    if (rs >= 0) rs = alen ;
		} /* end if (sbuf) */
	    } /* end if (had some arguments) */

	    if (rs >= 0) {
	        rs = expcook_add(cookp,"a",argsbuf,alen) ;
	    }

	    uc_free(argsbuf) ;
	} /* end if (memory_allocation) */

	return rs ;
}
/* end subroutine (cm_loadcooks) */

static int cm_trysys(cm *op,SI *sip,SD *dp,SD_ARGS *dap,
	    EC *cookp,SY_ENT *sep,cc *sysname,cc *svcname,mainv av) noex {
	SYSDIALER_ENT	*dep ;
	vecstr		args ;
	buffer		barg ;
	int		rs ;
	int		f_loaded = false ;
	int		f_args = false ;

	op->dobj = nullptr ;
	rs = sysdialer_loadin(dp,sep->dialername,&dep) ;
	f_loaded = (rs >= 0) ;

/* does this module have our attributes */

	if ((rs >= 0) && (dap->options != 0)) {
	    if ((dap->options & op->dflags) != op->dflags) {
	        rs = SR_NOTSUP ;
	    }
	}

/* OK, allocate and open the module */

	if (rs >= 0) {
	    op->dsize = dep->size ;
	    rs = uc_malloc(dep->size,&op->dobj) ;
	}

	if (rs >= 0) {
	    rs = vecstr_start(&args,5,0) ;
	    f_args = (rs >= 0) ? true : false ;
	}

	if ((rs >= 0) && ((rs = buffer_start(&barg,sip->argslen)) >= 0)) {
	    cchar	*abuf = nullptr ;
	    int		alen = 0 ;

	    memset(op->dobj,0,op->dsize) ;

	    op->c = dep->c ;

	    rs = vecstr_add(&args,sep->dialername,-1) ;

/* expand out the dialer arguments */

	    if ((rs >= 0) && (sep->dialerargs != nullptr)) {
		cint	dal = sep->dialerargslen ;
		cchar		*dap = sep->dialerargs ;

	    	if ((rs = expcook_expbuf(cookp,0,&barg,dap,dal)) >= 0) {
		    rs = buffer_get(&barg,&abuf) ;
		    alen = rs ;
		}

	    } /* end if */

/* separate the individual arguments out */

	    if ((rs >= 0) && (alen > 0)) {
		rs = cm_trysysargs(op,sip,&args,abuf,alen) ;
	    }

	    if (rs >= 0) {
		if (op->c.open != nullptr) {
 	            if (mainv tav ; (rs = vecstr_getvec(&args,&tav)) >= 0) {
	                dap->argv = tav ;
	                rs = (op->c.open)(op->dobj,dap,sysname,svcname,av) ;
		    }
		} else {
	            rs = SR_LIBACC ;
		}
	    }

	    buffer_finish(&barg) ;
	} /* end if (open the module) */

	if (f_args) {
	    vecstr_finish(&args) ;
	}

	if (rs < 0) {
	    if (op->dobj != nullptr) {
	        uc_free(op->dobj) ;
		op->dobj = nullptr ;
	    }
	    if (f_loaded) {
	        sysdialer_loadout(dp,sysname) ;
	    }
	} /* end if */

	return rs ;
}
/* end subroutine (cm_trysys) */

static int cm_trysysargs(cm *op,SI *sip,vecstr *alp,
		char *abuf,int alen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op && sip) {
	    if (char *fbuf ; (rs = malloc_mn(&fbuf)) >= 0) {
		cint	flen = rs ;
	        if (field fsb ; (rs = fsb.start(abuf,alen)) >= 0) {
	            for (int fl ; (fl = fsb.sharg(terms,fbuf,flen)) > 0 ; ) {
		        if (fl < 0) break ;
		        rs = alp->add(fbuf,fl) ;
		        if (rs < 0) break ;
	            } /* end for */
	            rs1 = fsb.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (field) */
	        rs = rsfree(rs,fbuf) ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cm_trysysargs) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = getbufsize(getbufsize_mn)) >= 0) {
	    maxnamelen = rs ;
	    if ((rs = getbufsize(getbufsize_mp)) >= 0) {
		maxpathlen = rs ;
	    	if ((rs = getbufsize(getbufsize_un)) >= 0) {
		    usernamelen = rs ;
	    	    if ((rs = getbufsize(getbufsize_nn)) >= 0) {
			nodenamelen = rs ;
	    	    	if ((rs = getbufsize(getbufsize_hn)) >= 0) {
			    hostnamelen = rs ;
	    	    	    if ((rs = getbufsize(getbufsize_ma)) >= 0) {
				maxarg = rs ;
				rs = fieldterms(terms,0," \t") ;
			    }
			}
		    }
		}
	    }
	}
	return rs ;
}
/* end method (vars::operator) */


