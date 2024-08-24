/* sreq SUPPORT */
/* version %I% last-modified %G% */

/* perform various functions on a job */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

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


/* impored namespaces */


/* local type-defs */

typedef int	(*objstart_t)(void *,cchar *,SREQ *,cchar **,cchar **) ;
typedef int	(*objcheck_t)(void *) ;
typedef int	(*objabort_t)(void *) ;
typedef int	(*objfinish_t)(void *) ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	sreq_fdfins(SREQ *) noex ;
static int	sreq_builtdone(SREQ *) noex ;

static int	mkfile(cchar *,cchar **) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int sreq_start(SREQ *jep,cchar *tpat,cchar *jobid,int ifd,int ofd) noex {
	const time_t	dt = time(nullptr) ;
	int		rs ;
	cchar		*cp ;

	if (jep == nullptr) return SR_FAULT ;
	if (tpat == nullptr) return SR_FAULT ;
	if (jobid == nullptr) return SR_FAULT ;

	memclear(jep) ;
	jep->ifd = ifd ;
	jep->ofd = ofd ;
	jep->efd = -1 ;
	jep->pid = -1 ;
	jep->stime = dt ;

	strwcpy(jep->logid,jobid,SREQ_JOBIDLEN) ;

	if ((rs = mkfile(tpat,&cp)) >= 0) {
	    jep->efname = (char *) cp ;
	    jep->magic = SREQ_MAGIC ;
	} /* end if */

	return rs ;
}
/* end subroutine (sreq_start) */

int sreq_finish(SREQ *jep) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (jep == nullptr) return SR_FAULT ;
	if (jep->magic != SREQ_MAGIC) return SR_NOTFOUND ;

	rs1 = sreq_builtdone(jep) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = sreq_thrdone(jep) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = sreq_fdfins(jep) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = sreq_stderrend(jep) ;
	if (rs >= 0) rs = rs1 ;

	if (jep->av != nullptr) {
	    rs1 = uc_free(jep->av) ;
	    if (rs >= 0) rs = rs1 ;
	    jep->av = nullptr ;
	}

	if (jep->argstab != nullptr) {
	    rs1 = uc_free(jep->argstab) ;
	    if (rs >= 0) rs = rs1 ;
	    jep->argstab = nullptr ;
	}

	if (jep->open.namesvcs) {
	    rs1 = sreq_sndestroy(jep) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (jep->f.ss) {
	    rs1 = sreq_svcentend(jep) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (jep->svcbuf != nullptr) {
	    rs1 = uc_free(jep->svcbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    jep->svcbuf = nullptr ;
	}

	if (jep->efname != nullptr) {
	    if (jep->efname[0] != '\0') {
	        u_unlink(jep->efname) ;
	        jep->efname[0] = '\0' ;
	    }
	    rs1 = uc_free(jep->efname) ;
	    if (rs >= 0) rs = rs1 ;
	    jep->efname = nullptr ;
	}

	if (jep->svc != nullptr) {
	    rs1 = uc_free(jep->svc) ;
	    if (rs >= 0) rs = rs1 ;
	    jep->svc = nullptr ;
	}

	jep->pid = -1 ;
	jep->logid[0] = '\0' ;

	jep->magic = 0 ;
	return rs ;
}
/* end subroutine (sreq_finish) */

int sreq_typeset(SREQ *op,int jt,int st) noex {
	op->jtype = jt ;
	op->stype = st ;
	return SR_OK ;
}
/* end subroutine (sreq_typeset) */

int sreq_getfd(SREQ *op) noex {
	return op->ifd ;
}
/* end subroutine (sreq_getfd) */

int sreq_havefd(SREQ *op,int fd) noex {
	int		f = false ;
	f = f || ((op->ifd >= 0) && (op->ifd == fd)) ;
	f = f || ((op->ofd >= 0) && (op->ofd == fd)) ;
	return f ;
}
/* end subroutine (sreq_havefd) */

int sreq_svcaccum(SREQ *op,cchar *sp,int sl) noex {
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

int sreq_svcmunge(SREQ *jep) noex {
	vecstr		sa ; /* server arguments */
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = vecstr_start(&sa,1,0)) >= 0) {
	    int	f_long = false ;
	    if ((rs = vecstr_svcargs(&sa,&f_long,jep->svcbuf)) >= 0) {
	        c = rs ;
	        if ((rs = sreq_svcparse(jep,f_long)) >= 0) {
	            cint	ts = vecstr_strsize(&sa) ;
	            int		as ;
	            cchar	**av = nullptr ;

	            as = ((c + 2) * sizeof(cchar *)) ;
	            if ((rs = uc_malloc(as,&av)) >= 0) {
	                char	*at = nullptr ;
	                if ((rs = uc_malloc(ts,&at)) >= 0) {

	                    if ((rs = vecstr_avmkstr(&sa,av,as,at,ts)) >= 0) {
	                        jep->nav = (c+1) ;
	                        jep->av = av ;
	                        jep->argstab = at ;
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
int sreq_svcparse(SREQ *op,int f_long) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if (op->svc == nullptr) {
	    int		sl ;
	    cchar	*sp ;
	    op->f.longopt = f_long ;
	    if ((sl = sfnext(op->svcbuf,op->svclen,&sp)) >= 0) {
	        cchar	*tp ;
	        char	*bp ;
	        if ((tp = strnchr(sp,sl,'/')) != nullptr) {
	            if (! f_long) {
	                if (((sp+sl)-tp) > 1) {
	                    op->f.longopt = (tolc(tp[1]) == 'w') ;
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

int sreq_setlong(SREQ *op,int f) noex {
	op->f.longopt = MKBOOL(f) ;
	return SR_OK ;
}
/* end subroutine (sreq_setlong) */

int sreq_setstate(SREQ *op,int state) noex {
	op->state = state ;
	if (state == sreqstate_done) {
	    op->etime = time(nullptr) ;
	    op->f_exiting = true ;
	}
	return SR_OK ;
}
/* end subroutine (sreq_setstate) */

int sreq_getjsn(SREQ *jep) noex {
	return jep->jsn ;
}
/* end subroutine (sreq_getjsn) */

int sreq_getsvc(SREQ *op,cchar **rpp) noex {
	int		sl = strlen(op->svc) ;
	if (rpp != nullptr) *rpp = op->svc ;
	return sl ;
}
/* end subroutine (sreq_getsvc) */

int sreq_getsubsvc(SREQ *op,cchar **rpp) noex {
	int		sl ;
	if (rpp != nullptr) *rpp = op->subsvc ;
	sl = strlen(op->subsvc) ;
	return sl ;
}
/* end subroutine (sreq_getsubsvc) */

int sreq_getstate(SREQ *op) noex {
	return op->state ;
}
/* end subroutine (sreq_getstate) */

int sreq_getav(SREQ *jep,cchar ***avp) noex {
	cint	nav = jep->nav ;
	if (avp != nullptr) *avp = jep->av ;
	return  nav ;
}
/* end subroutine (sreq_getav) */

int sreq_ofd(SREQ *op) noex {
	if (op->ofd < 0) op->ofd = op->ifd ;
	return op->ofd ;
}
/* end subroutine (sreq_ofd) */

int sreq_getstdin(SREQ *jep) noex {
	return jep->ifd ;
}
/* end subroutine (sreq_getstdin) */

int sreq_getstdout(SREQ *op) noex {
	if (op->ofd < 0) op->ofd = op->ifd ;
	return op->ofd ;
}
/* end subroutine (sreq_getstdout) */

int sreq_closefds(SREQ *jep) noex {
	return sreq_fdfins(jep) ;
}
/* end subroutine (sreq_closefds) */

int sreq_svcentbegin(SREQ *jep,LOCINFO *lip,SVCENT *sep) noex {
	int		rs ;
	if ((rs = svcentsub_start(&jep->ss,lip,sep)) >= 0) {
	    jep->f.ss = true ;
	}
	return rs ;
}
/* end subroutine (sreq_evcentbegin) */

int sreq_svcentend(SREQ *jep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (jep->f.ss) {
	    jep->f.ss = false ;
	    rs1 = svcentsub_finish(&jep->ss) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (sreq_svcentend) */

/* spawned thread calls this from its exit-handler */
int sreq_exiting(SREQ *jep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (jep) {
	    rs = SR_OK ;
	    {
	        rs1 = sreq_fdfins(jep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    jep->f_exiting = true ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sreq_exiting) */

int sreq_thrdone(SREQ *jep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (jep->f.thread) {
	    pthread_t	tid = jep->tid ;
	    int		trs ;
	    jep->f.thread = false ;
	    rs1 = uptjoin(tid,&trs) ;
	    if (rs >= 0) rs = rs1 ;
	    if (rs >= 0) rs = trs ;
	}
	return rs ;
}
/* end subroutine (sreq_thrdone) */

/* services-name - |help| service helper - create new instance */
int sreq_sncreate(SREQ *jep) noex {
	int		rs = SR_OK ;
	if (! jep->open.namesvcs) {
	    OSETSTR	*ssp = &jep->namesvcs ;
	    cint	ne = 50 ;
	    if ((rs = osetstr_start(ssp,ne)) >= 0) {
	        jep->open.namesvcs = true ;
	    }
	}
	return rs ;
}
/* end subroutine (sreq_sncreate) */

/* services-name - |help| service helper - destroy existing instance */
int sreq_sndestroy(SREQ *jep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (jep->open.namesvcs) {
	    OSETSTR	*ssp = &jep->namesvcs ;
	    jep->open.namesvcs = false ;
	    rs1 = osetstr_finish(ssp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (sreq_sndestroy) */

/* services-name - |help| service helper - add a service name */
int sreq_snadd(SREQ *jep,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	if (jep->open.namesvcs) {
	    OSETSTR	*ssp = &jep->namesvcs ;
	    rs = osetstr_add(ssp,sp,sl) ;
	}
	return rs ;
}
/* end subroutine (sreq_snadd) */

/* services-name - |help| service helper - begin enumerate */
int sreq_snbegin(SREQ *jep,SREQ_SNCUR *scp) noex {
	int		rs = SR_OK ;
	if (jep->open.namesvcs) {
	    OSETSTR	*ssp = &jep->namesvcs ;
	    OSETSTR_CUR	*curp = &scp->cur ;
	    rs = osetstr_curbegin(ssp,curp) ;
	}
	return rs ;
}
/* end subroutine (sreq_snbegin) */

/* services-name - |help| service helper - end enumerate */
int sreq_snend(SREQ *jep,SREQ_SNCUR *scp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (jep->open.namesvcs) {
	    OSETSTR	*ssp = &jep->namesvcs ;
	    OSETSTR_CUR	*curp = &scp->cur ;
	    rs1 = osetstr_curend(ssp,curp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (sreq_snend) */

/* services-name - |help| service helper - execute enumerate */
int sreq_snenum(SREQ *jep,SREQ_SNCUR *scp,cchar **rpp) noex {
	int		rs = SR_OK ;
	if (jep->open.namesvcs) {
	    OSETSTR	*ssp = &jep->namesvcs ;
	    OSETSTR_CUR	*curp = &scp->cur ;
	    rs = osetstr_enum(ssp,curp,rpp) ;
	}
	return rs ;
}
/* end subroutine (sreq_snenum) */

/* biltin operation */
int sreq_builtload(SREQ *jep,MFSERVE_INFO *ip) noex {
	cint		osize = ip->objsize ;
	int		rs ;
	void		*p ;
	if (jep == nullptr) return SR_FAULT ;
	if (jep->magic != SREQ_MAGIC) return SR_NOTFOUND ;
	if ((rs = uc_malloc(osize,&p)) >= 0) {
	    jep->binfo = *ip ;
	    jep->objp = p ;
	}
	return rs ;
}
/* end subroutine (sreq_builtload) */

/* biltin operation */
int sreq_builtrelease(SREQ *jep) noex {
	int		rs = SR_OK ;
	if (jep == nullptr) return SR_FAULT ;
	if (jep->magic != SREQ_MAGIC) return SR_NOTFOUND ;
	if (jep->objp != nullptr) {
	    if ((rs = uc_free(jep->objp)) >= 0) {
	        jep->objp = nullptr ;
	        jep->binfo.objsize = 0 ;
	        jep->f.builtout = false ;
	        jep->f.builtdone = false ;
	        rs = 1 ;
	    }
	}
	return rs ;
}
/* end subroutine (sreq_builtrelease) */

/* biltin operation */
int sreq_objstart(SREQ *jep,cchar *pr,cchar **envv) noex {
	int		rs = SR_OK ;
	if (jep == nullptr) return SR_FAULT ;
	if (jep->magic != SREQ_MAGIC) return SR_NOTFOUND ;
	if (jep->objp != nullptr) {
	    if (! jep->f.builtout) {
	        objstart_t	m = (objstart_t) jep->binfo.start ;
	        cchar	**av = jep->av ;
	        if ((rs = (*m)(jep->objp,pr,jep,av,envv)) >= 0) {
	            jep->f.builtout = true ;
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
int sreq_objcheck(SREQ *jep) noex {
	int		rs = SR_OK ;
	if (jep == nullptr) return SR_FAULT ;
	if (jep->magic != SREQ_MAGIC) return SR_NOTFOUND ;
	if (jep->objp != nullptr) {
	    if (jep->f.builtout) {
	        if (! jep->f.builtdone) {
	            objcheck_t	m = (objcheck_t) jep->binfo.check ;
	            if ((rs = (*m)(jep->objp)) > 0) {
	                jep->f.builtdone = true ;
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

int sreq_objabort(SREQ *jep) noex {
	int		rs = SR_OK ;
	if (jep == nullptr) return SR_FAULT ;
	if (jep->magic != SREQ_MAGIC) return SR_NOTFOUND ;
	if (jep->objp != nullptr) {
	    if (jep->f.builtout) {
	        if (! jep->f.builtdone) {
	            objabort_t	m = (objabort_t) jep->binfo.abort ;
	            rs = (*m)(jep->objp) ;
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

int sreq_objfinish(SREQ *jep) noex {
	int		rs = SR_OK ;
	if (jep == nullptr) return SR_FAULT ;
	if (jep->magic != SREQ_MAGIC) return SR_NOTFOUND ;
	if (jep->objp != nullptr) {
	    if (jep->f.builtout) {
	        objfinish_t	m = (objfinish_t) jep->binfo.finish ;
	        if ((rs = (*m)(jep->objp)) >= 0) {
	            jep->f.builtdone = false ;
	            jep->f.builtout = false ;
	        }
	    }
	} else {
	    rs = SR_NOANODE ;
	}
	return rs ;
}
/* end subroutine (sreq_objfinish) */

int sreq_stderrbegin(SREQ *jep,cchar *edname) noex {
	cint		elen = MAXPATHLEN ;
	int		rs ;
	int		fd = -1 ;
	cchar		*lead = "err" ;
	char		*ebuf ;
	if ((rs = uc_malloc((elen+1),&ebuf)) >= 0) {
	    cint	dlen = DIGBUFLEN ;
	    char	dbuf[DIGBUFLEN+1] ;
	    jep->efname = ebuf ; /* load address */
	    if ((rs = ctdeci(dbuf,dlen,jep->jsn)) >= 0) {
	        cint	clen = MAXNAMELEN ;
	        cchar		*x = "XXXXXX" ;
	        char		cbuf[MAXNAMELEN+1] ;
	        if ((rs = sncpy4(cbuf,clen,lead,dbuf,".",x)) >= 0) {
	            char	tbuf[MAXPATHLEN+1] ;
	            if ((rs = mkpath2(tbuf,edname,cbuf)) >= 0) {
	                const mode_t	om = 0664 ;
	                cint	of = (O_CREAT|O_WRONLY|O_TRUNC) ;
	                char		*ebuf = jep->efname ;
	                if ((rs = opentmpfile(tbuf,of,om,ebuf)) >= 0) {
	                    jep->efd = rs ;
	                    fd = rs ;
	                } /* end if (uc_open) */
	            } /* end if (mkpath) */
	        } /* end if (sncpy) */
	    } /* end if (cfdeci) */
	    if (rs < 0) {
	        uc_free(jep->efname) ;
	        jep->efname = nullptr ;
	    }
	} /* end if (m-a) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (sreq_stderrbegin) */

int sreq_stderrclose(SREQ *jep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (jep->efd >= 0) {
	    rs1 = u_close(jep->efd) ;
	    if (rs >= 0) rs = rs1 ;
	    jep->efd = -1 ;
	}
	return rs ;
}
/* end subroutine (sreq_stderrclose) */

int sreq_stderrend(SREQ *jep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (jep->efd >= 0) {
	    rs1 = u_close(jep->efd) ;
	    if (rs >= 0) rs = rs1 ;
	    jep->efd = -1 ;
	}
	if (jep->efname != nullptr) {
	    if (jep->efname[0] != '\0') {
	        cint	rsn = SR_NOTFOUND ;
	        rs1 = uc_unlink(jep->efname) ;
	        if (rs1 == rsn) rs1 = SR_OK ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    rs1 = uc_free(jep->efname) ;
	    if (rs >= 0) rs = rs1 ;
	    jep->efname = nullptr ;
	}
	return rs ;
}
/* end subroutine (sreq_stderrend) */


/* private subroutines */

static int sreq_builtdone(SREQ *jep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (jep->objp != nullptr) {
	    if ((rs = sreq_objabort(jep)) >= 0) {
	        if ((rs = sreq_objfinish(jep)) >= 0) {
	            rs1 = uc_free(jep->objp) ;
	            if (rs >= 0) rs = rs1 ;
	            jep->objp = nullptr ;
	        }
	    }
	}
	return rs ;
}
/* end subroutine (sreq_builtdone) */

static int sreq_fdfins(SREQ *jep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = sreq_stderrclose(jep) ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (jep->ofd >= 0) {
	    if (jep->ofd != jep->ifd) {
	        rs1 = u_close(jep->ofd) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    jep->ofd = -1 ;
	}
	if (jep->ifd >= 0) {
	    rs1 = u_close(jep->ifd) ;
	    if (rs >= 0) rs = rs1 ;
	    jep->ifd = -1 ;
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


