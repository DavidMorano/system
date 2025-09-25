/* sreq SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Service-Request manager */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sreq

	Description:
	This module is responsible for providing means to store a
	job and to retrieve it later by its PID.  This is the lower
	half of a pair.  The SREQDB calls on this object for support.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<estrings.h>
#include	<mktmp.h>
#include	<ctdec.h>
#include	<upt.h>
#include	<opentmp.h>
#include	<toxc.h>
#include	<localmisc.h>

#include	"sreq.h"
#include	"mfslocinfo.h"


/* local defines */

#ifndef	TMPDNAME
#define	TMPDNAME	"/tmp"
#endif

#ifndef	VARTMPDNAME
#define	VARTMPDNAME	"TMPDIR"
#endif


/* imported namespaces */


/* local typedefs */

typedef int	(*objstart_t)(void *,cchar *,SREQ *,cchar **,cchar **) noex ;
typedef int	(*objcheck_t)(void *) noex ;
typedef int	(*objabort_t)(void *) noex ;
typedef int	(*objfinish_t)(void *) noex ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	sreq_fdfins(sreq *) noex ;
static int	sreq_builtdone(sreq *) noex ;

static int	mkfile(cchar *,cchar **) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int sreq_start(sreq *op,cchar *tpat,cchar *jobid,int ifd,int ofd) noex {
	const time_t	dt = time(nullptr) ;
	int		rs ;
	cchar		*cp ;

	if (op == nullptr) return SR_FAULT ;
	if (tpat == nullptr) return SR_FAULT ;
	if (jobid == nullptr) return SR_FAULT ;

	memclear(op) ;
	op->ifd = ifd ;
	op->ofd = ofd ;
	op->efd = -1 ;
	op->pid = -1 ;
	op->stime = dt ;

	strwcpy(op->logid,jobid,SREQ_JOBIDLEN) ;

	if ((rs = mkfile(tpat,&cp)) >= 0) {
	    op->efname = (char *) cp ;
	    op->magic = SREQ_MAGIC ;
	} /* end if */

	return rs ;
}
/* end subroutine (sreq_start) */

int sreq_finish(sreq *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;
	if (op->magic != SREQ_MAGIC) return SR_NOTFOUND ;

	rs1 = sreq_builtdone(op) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = sreq_thrdone(op) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = sreq_fdfins(op) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = sreq_stderrend(op) ;
	if (rs >= 0) rs = rs1 ;

	if (op->av != nullptr) {
	    rs1 = uc_free(op->av) ;
	    if (rs >= 0) rs = rs1 ;
	    op->av = nullptr ;
	}

	if (op->argstab != nullptr) {
	    rs1 = uc_free(op->argstab) ;
	    if (rs >= 0) rs = rs1 ;
	    op->argstab = nullptr ;
	}

	if (op->open.namesvcs) {
	    rs1 = sreq_sndestroy(op) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (op->fl.ss) {
	    rs1 = sreq_svcentend(op) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (op->svcbuf != nullptr) {
	    rs1 = uc_free(op->svcbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    op->svcbuf = nullptr ;
	}

	if (op->efname != nullptr) {
	    if (op->efname[0] != '\0') {
	        u_unlink(op->efname) ;
	        op->efname[0] = '\0' ;
	    }
	    rs1 = uc_free(op->efname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->efname = nullptr ;
	}

	if (op->svc != nullptr) {
	    rs1 = uc_free(op->svc) ;
	    if (rs >= 0) rs = rs1 ;
	    op->svc = nullptr ;
	}

	op->pid = -1 ;
	op->logid[0] = '\0' ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (sreq_finish) */

int sreq_typeset(sreq *op,int jt,int st) noex {
	op->jtype = jt ;
	op->stype = st ;
	return SR_OK ;
}
/* end subroutine (sreq_typeset) */

int sreq_getfd(sreq *op) noex {
	return op->ifd ;
}
/* end subroutine (sreq_getfd) */

int sreq_havefd(sreq *op,int fd) noex {
	int		f = false ;
	f = f || ((op->ifd >= 0) && (op->ifd == fd)) ;
	f = f || ((op->ofd >= 0) && (op->ofd == fd)) ;
	return f ;
}
/* end subroutine (sreq_havefd) */

int sreq_svcaccum(sreq *op,cchar *sp,int sl) noex {
	int		rs ;
	char		*bp ;
	if (sl < 0) sl = strlen(sp) ;
	if ((op->svclen + sl) < SREQ_SVCBUFLEN) {
	    if (op->svclen == 0) {
	        if ((rs = uc_malloc((sl+1),&bp)) >= 0) {
	            strwcpy(bp,sp,sl) ;
	            op->svcbuf = bp ;
	            op->svclen = sl ;
	        }
	    } else {
	        cint	nlen = (op->svclen+sl) ;
	        if ((rs = uc_realloc(op->svcbuf,(nlen+1),&bp)) >= 0) {
	            strwcpy((bp+op->svclen),sp,sl) ;
	            op->svcbuf = bp ;
	            op->svclen = nlen ;
	        }
	    }
	} else {
	    rs = SR_PROTO ;
	}
	return rs ;
}
/* end subroutine (sreq_svcaccun) */

int sreq_svcmunge(sreq *op) noex {
	vecstr		sa ; /* server arguments */
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = vecstr_start(&sa,1,0)) >= 0) {
	    int	f_long = false ;
	    if ((rs = vecstr_svcargs(&sa,&f_long,op->svcbuf)) >= 0) {
	        c = rs ;
	        if ((rs = sreq_svcparse(op,f_long)) >= 0) {
	            cint	ts = vecstr_strsize(&sa) ;
	            int		as ;
	            cchar	**av = nullptr ;

	            as = ((c + 2) * sizeof(cchar *)) ;
	            if ((rs = uc_malloc(as,&av)) >= 0) {
	                char	*at = nullptr ;
	                if ((rs = uc_malloc(ts,&at)) >= 0) {

	                    if ((rs = vecstr_avmkstr(&sa,av,as,at,ts)) >= 0) {
	                        op->nav = (c+1) ;
	                        op->av = av ;
	                        op->argstab = at ;
	                    }

	                    if (rs < 0) uc_free(at) ;
	                } /* end if (memory-allocation) */

	                if (rs < 0) {
	                    uc_free(av) ;
			}
	            } /* end if (memory-allocation) */

	        } /* end if (sreq_argparse) */
	    } /* end if (vecstr_svcargs) */
	    rs1 = vecstr_finish(&sa) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecstr) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (sreq_svcmunge) */

/* extract the service from the "service-buffer" and alloc-store in 'svc' */
int sreq_svcparse(sreq *op,int f_long) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if (op->svc == nullptr) {
	    int		sl ;
	    cchar	*sp ;
	    op->fl.longopt = f_long ;
	    if ((sl = sfnext(op->svcbuf,op->svclen,&sp)) >= 0) {
	        cchar	*tp ;
	        char	*bp ;
	        if ((tp = strnchr(sp,sl,'/')) != nullptr) {
	            if (! f_long) {
	                if (((sp+sl)-tp) > 1) {
	                    op->fl.longopt = (tolc(tp[1]) == 'w') ;
	                }
	            }
	            sl = (tp-sp) ;
	        }
	        if ((rs = uc_malloc((sl+1),&bp)) >= 0) {
	            len = sl ;
	            op->svc = bp ;
	            op->subsvc = strwcpy(bp,sp,sl) ;
	            if ((tp = strnchr(sp,sl,'+')) != nullptr) {
	                len = (tp-sp) ;
	                bp[tp-sp] = '\0' ;
	                op->subsvc = bp+((tp+1)-sp) ;
	            }
	        } /* end if (m-a) */
	    } /* end if (sfnext) */
	} else {
	    rs = SR_BUGCHECK ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (sreq_svcparse) */

int sreq_setlong(sreq *op,int f) noex {
	op->fl.longopt = MKBOOL(f) ;
	return SR_OK ;
}
/* end subroutine (sreq_setlong) */

int sreq_setstate(sreq *op,int state) noex {
	op->state = state ;
	if (state == sreqstate_done) {
	    op->etime = time(nullptr) ;
	    op->f_exiting = true ;
	}
	return SR_OK ;
}
/* end subroutine (sreq_setstate) */

int sreq_getjsn(sreq *op) noex {
	return op->jsn ;
}
/* end subroutine (sreq_getjsn) */

int sreq_getsvc(sreq *op,cchar **rpp) noex {
	int		sl = strlen(op->svc) ;
	if (rpp != nullptr) *rpp = op->svc ;
	return sl ;
}
/* end subroutine (sreq_getsvc) */

int sreq_getsubsvc(sreq *op,cchar **rpp) noex {
	int		sl ;
	if (rpp != nullptr) *rpp = op->subsvc ;
	sl = strlen(op->subsvc) ;
	return sl ;
}
/* end subroutine (sreq_getsubsvc) */

int sreq_getstate(sreq *op) noex {
	return op->state ;
}
/* end subroutine (sreq_getstate) */

int sreq_getav(sreq *op,cchar ***avp) noex {
	cint	nav = op->nav ;
	if (avp != nullptr) *avp = op->av ;
	return  nav ;
}
/* end subroutine (sreq_getav) */

int sreq_ofd(sreq *op) noex {
	if (op->ofd < 0) op->ofd = op->ifd ;
	return op->ofd ;
}
/* end subroutine (sreq_ofd) */

int sreq_getstdin(sreq *op) noex {
	return op->ifd ;
}
/* end subroutine (sreq_getstdin) */

int sreq_getstdout(sreq *op) noex {
	if (op->ofd < 0) op->ofd = op->ifd ;
	return op->ofd ;
}
/* end subroutine (sreq_getstdout) */

int sreq_closefds(sreq *op) noex {
	return sreq_fdfins(op) ;
}
/* end subroutine (sreq_closefds) */

int sreq_svcentbegin(sreq *op,LOCINFO *lip,SVCENT *sep) noex {
	int		rs ;
	if ((rs = svcentsub_start(&op->ss,lip,sep)) >= 0) {
	    op->fl.ss = true ;
	}
	return rs ;
}
/* end subroutine (sreq_evcentbegin) */

int sreq_svcentend(sreq *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fl.ss) {
	    op->fl.ss = false ;
	    rs1 = svcentsub_finish(&op->ss) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (sreq_svcentend) */

/* spawned thread calls this from its exit-handler */
int sreq_exiting(sreq *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    {
	        rs1 = sreq_fdfins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->f_exiting = true ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sreq_exiting) */

int sreq_thrdone(sreq *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fl.thread) {
	    pthread_t	tid = op->tid ;
	    int		trs ;
	    op->fl.thread = false ;
	    rs1 = uptjoin(tid,&trs) ;
	    if (rs >= 0) rs = rs1 ;
	    if (rs >= 0) rs = trs ;
	}
	return rs ;
}
/* end subroutine (sreq_thrdone) */

/* services-name - |help| service helper - create new instance */
int sreq_sncreate(sreq *op) noex {
	int		rs = SR_OK ;
	if (! op->open.namesvcs) {
	    setostr	*ssp = &op->namesvcs ;
	    cint	ne = 50 ;
	    if ((rs = setostr_start(ssp,ne)) >= 0) {
	        op->open.namesvcs = true ;
	    }
	}
	return rs ;
}
/* end subroutine (sreq_sncreate) */

/* services-name - |help| service helper - destroy existing instance */
int sreq_sndestroy(sreq *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->open.namesvcs) {
	    setostr	*ssp = &op->namesvcs ;
	    op->open.namesvcs = false ;
	    rs1 = setostr_finish(ssp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (sreq_sndestroy) */

/* services-name - |help| service helper - add a service name */
int sreq_snadd(sreq *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	if (op->open.namesvcs) {
	    setostr	*ssp = &op->namesvcs ;
	    rs = setostr_add(ssp,sp,sl) ;
	}
	return rs ;
}
/* end subroutine (sreq_snadd) */

/* services-name - |help| service helper - begin enumerate */
int sreq_snbegin(sreq *op,SREQ_SNCUR *scp) noex {
	int		rs = SR_OK ;
	if (op->open.namesvcs) {
	    setostr	*ssp = &op->namesvcs ;
	    setostr_cur	*curp = &scp->cur ;
	    rs = setostr_curbegin(ssp,curp) ;
	}
	return rs ;
}
/* end subroutine (sreq_snbegin) */

/* services-name - |help| service helper - end enumerate */
int sreq_snend(sreq *op,SREQ_SNCUR *scp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->open.namesvcs) {
	    setostr	*ssp = &op->namesvcs ;
	    setostr_cur	*curp = &scp->cur ;
	    rs1 = setostr_curend(ssp,curp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (sreq_snend) */

/* services-name - |help| service helper - execute enumerate */
int sreq_snenum(sreq *op,SREQ_SNCUR *scp,cchar **rpp) noex {
	int		rs = SR_OK ;
	if (op->open.namesvcs) {
	    setostr	*ssp = &op->namesvcs ;
	    setostr_cur	*curp = &scp->cur ;
	    rs = setostr_enum(ssp,curp,rpp) ;
	}
	return rs ;
}
/* end subroutine (sreq_snenum) */

/* biltin operation */
int sreq_builtload(sreq *op,MFSERVE_INFO *ip) noex {
	cint		osize = ip->objsize ;
	int		rs ;
	void		*p ;
	if (op == nullptr) return SR_FAULT ;
	if (op->magic != SREQ_MAGIC) return SR_NOTFOUND ;
	if ((rs = uc_malloc(osize,&p)) >= 0) {
	    op->binfo = *ip ;
	    op->objp = p ;
	}
	return rs ;
}
/* end subroutine (sreq_builtload) */

/* biltin operation */
int sreq_builtrelease(sreq *op) noex {
	int		rs = SR_OK ;
	if (op == nullptr) return SR_FAULT ;
	if (op->magic != SREQ_MAGIC) return SR_NOTFOUND ;
	if (op->objp != nullptr) {
	    if ((rs = uc_free(op->objp)) >= 0) {
	        op->objp = nullptr ;
	        op->binfo.objsize = 0 ;
	        op->fl.builtout = false ;
	        op->fl.builtdone = false ;
	        rs = 1 ;
	    }
	}
	return rs ;
}
/* end subroutine (sreq_builtrelease) */

/* biltin operation */
int sreq_objstart(sreq *op,cchar *pr,cchar **envv) noex {
	int		rs = SR_OK ;
	if (op == nullptr) return SR_FAULT ;
	if (op->magic != SREQ_MAGIC) return SR_NOTFOUND ;
	if (op->objp != nullptr) {
	    if (! op->fl.builtout) {
	        objstart_t	m = (objstart_t) op->binfo.start ;
	        cchar	**av = op->av ;
	        if ((rs = (*m)(op->objp,pr,op,av,envv)) >= 0) {
	            op->fl.builtout = true ;
	        }
	    } else {
	        rs = SR_NOANODE ;
	    }
	} else {
	    rs = SR_NOANODE ;
	}
	return rs ;
}
/* end subroutine (sreq_objstart) */

/* biltin operation, returns 0=not_done, (!0)=done */
int sreq_objcheck(sreq *op) noex {
	int		rs = SR_OK ;
	if (op == nullptr) return SR_FAULT ;
	if (op->magic != SREQ_MAGIC) return SR_NOTFOUND ;
	if (op->objp != nullptr) {
	    if (op->fl.builtout) {
	        if (! op->fl.builtdone) {
	            objcheck_t	m = (objcheck_t) op->binfo.check ;
	            if ((rs = (*m)(op->objp)) > 0) {
	                op->fl.builtdone = true ;
	            }
	        } else {
	            rs = true ;
	        }
	    }
	} else {
	    rs = SR_NOANODE ;
	}
	return rs ;
}
/* end subroutine (sreq_objcheck) */

int sreq_objabort(sreq *op) noex {
	int		rs = SR_OK ;
	if (op == nullptr) return SR_FAULT ;
	if (op->magic != SREQ_MAGIC) return SR_NOTFOUND ;
	if (op->objp != nullptr) {
	    if (op->fl.builtout) {
	        if (! op->fl.builtdone) {
	            objabort_t	m = (objabort_t) op->binfo.abort ;
	            rs = (*m)(op->objp) ;
	        } else {
	            rs = true ;
	        }
	    }
	} else {
	    rs = SR_NOANODE ;
	}
	return rs ;
}
/* end subroutine (sreq_objstart) */

int sreq_objfinish(sreq *op) noex {
	int		rs = SR_OK ;
	if (op == nullptr) return SR_FAULT ;
	if (op->magic != SREQ_MAGIC) return SR_NOTFOUND ;
	if (op->objp != nullptr) {
	    if (op->fl.builtout) {
	        objfinish_t	m = (objfinish_t) op->binfo.finish ;
	        if ((rs = (*m)(op->objp)) >= 0) {
	            op->fl.builtdone = false ;
	            op->fl.builtout = false ;
	        }
	    }
	} else {
	    rs = SR_NOANODE ;
	}
	return rs ;
}
/* end subroutine (sreq_objfinish) */

int sreq_stderrbegin(sreq *op,cchar *edname) noex {
	cint		elen = MAXPATHLEN ;
	int		rs ;
	int		fd = -1 ;
	cchar		*lead = "err" ;
	char		*ebuf ;
	if ((rs = uc_malloc((elen+1),&ebuf)) >= 0) {
	    cint	dlen = DIGBUFLEN ;
	    char	dbuf[DIGBUFLEN+1] ;
	    op->efname = ebuf ; /* load address */
	    if ((rs = ctdeci(dbuf,dlen,op->jsn)) >= 0) {
	        cint	clen = MAXNAMELEN ;
	        cchar		*x = "XXXXXX" ;
	        char		cbuf[MAXNAMELEN+1] ;
	        if ((rs = sncpy4(cbuf,clen,lead,dbuf,".",x)) >= 0) {
	            char	tbuf[MAXPATHLEN+1] ;
	            if ((rs = mkpath2(tbuf,edname,cbuf)) >= 0) {
	                const mode_t	om = 0664 ;
	                cint	of = (O_CREAT|O_WRONLY|O_TRUNC) ;
	                char		*ebuf = op->efname ;
	                if ((rs = opentmpfile(tbuf,of,om,ebuf)) >= 0) {
	                    op->efd = rs ;
	                    fd = rs ;
	                } /* end if (uc_open) */
	            } /* end if (mkpath) */
	        } /* end if (sncpy) */
	    } /* end if (cfdeci) */
	    if (rs < 0) {
	        uc_free(op->efname) ;
	        op->efname = nullptr ;
	    }
	} /* end if (m-a) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (sreq_stderrbegin) */

int sreq_stderrclose(sreq *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->efd >= 0) {
	    rs1 = u_close(op->efd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->efd = -1 ;
	}
	return rs ;
}
/* end subroutine (sreq_stderrclose) */

int sreq_stderrend(sreq *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->efd >= 0) {
	    rs1 = u_close(op->efd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->efd = -1 ;
	}
	if (op->efname != nullptr) {
	    if (op->efname[0] != '\0') {
	        cint	rsn = SR_NOTFOUND ;
	        rs1 = uc_unlink(op->efname) ;
	        if (rs1 == rsn) rs1 = SR_OK ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    rs1 = uc_free(op->efname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->efname = nullptr ;
	}
	return rs ;
}
/* end subroutine (sreq_stderrend) */


/* private subroutines */

static int sreq_builtdone(sreq *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->objp != nullptr) {
	    if ((rs = sreq_objabort(op)) >= 0) {
	        if ((rs = sreq_objfinish(op)) >= 0) {
	            rs1 = uc_free(op->objp) ;
	            if (rs >= 0) rs = rs1 ;
	            op->objp = nullptr ;
	        }
	    }
	}
	return rs ;
}
/* end subroutine (sreq_builtdone) */

static int sreq_fdfins(sreq *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = sreq_stderrclose(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (op->ofd >= 0) {
	    if (op->ofd != op->ifd) {
	        rs1 = u_close(op->ofd) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->ofd = -1 ;
	}
	if (op->ifd >= 0) {
	    rs1 = u_close(op->ifd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->ifd = -1 ;
	}

	return rs ;
}
/* end subroutine (sreq_fdfins) */

static int mkfile(cchar *tpat,cchar **rpp) noex {
	int		rs ;
	int		rs1 ;
	int		tl = 0 ;
	char		*tbuf{} ;
	if ((rs = malloc_mp(&tbuf)) >= 0) {
	    cmode	om = 0666 ;
	    tbuf[0] = '\0' ;
	    if ((rs = mktmpfile(tbuf,tpat,om)) >= 0) {
	        tl = rs ;
	        rs = uc_mallocstrw(tbuf,tl,rpp) ;
	        if (rs < 0) {
	            u_unlink(tbuf) ;
	            *rpp = nullptr ;
	        } /* end if (error-recovery) */
	    } /* end if (mktmpfile) */
	    rs1 = uc_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? tl : rs ;
}
/* end subroutines (mkfile) */


