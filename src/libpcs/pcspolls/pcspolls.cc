/* pcspolls SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* management interface to the PCSPOLL loadable-object poll facility */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */
#define	CF_DEBUGN	0		/* special debugging */
#define	CF_EARLY	1		/* early exit */

/* revision history:

	- 2008-10-07, David A­D­ Morano
	This module was originally written to allow for the main
	part of the PCS-poll facility to be a loadable module.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	pcspolls

	Description:
	This object module serves as the manager for the PCS
	loadable-object poll facility.

	Synopsis:
	int pcspolls_start(pcspolls *op,pcsconf *pcp,cchar *searchname) noex

	Arguments:
	- op		object pointer
	- pcp		PCS configuration pointer
	- searchname	search-name to use

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<dlfcn.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<vechand.h>
#include	<pcsconf.h>
#include	<fsdir.h>
#include	<storebuf.h>
#include	<upt.h>
#include	<six.h>
#include	<strwcpy.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"pcspolls.h"


/* local defines */

#define	LIBCNAME	"lib"
#define	POLLCNAME	PCSPOLLS_POLLCNAME

#define	THREAD		struct pcspolls_thread
#define	THREAD_ARGS	struct thread_args

#define	WORK		struct work_head

#define	POLLINFO	struct pollinfo

#define	POLLOBJ		struct pollobj
#define	POLLOBJ_FL	struct pollobj_flags

#ifndef	SYMNAMELEN
#define	SYMNAMELEN	60
#endif

#define	TO_CHECK	4

#define	NDF		"pcspolls.deb"


/* external subroutines */

#if	CF_DEBUGS
extern int	debugprintf(cchar *,...) noex ;
#endif

#if	CF_DEUGN
extern int	nprintf(cchar *,cchar *,...) noex ;
#endif


/* external variables */


/* local structures */

extern "C" {
    struct pollinfo {
	void		*sop ;
	cvoid		*objname ;
	int		(*start)(void *) ;
	int		(*check)(void *) ;
	int		(*info)(void *) ;
	int		(*cmd)(void *) ;
	int		(*finish)(void *) ;
    } ;
}

struct pollobj_flags {
	uint		running:1 ;
	uint		active:1 ;
} ;

extern "C" {
    struct pollobj {
	cchar		*name ;
	void		*sop ;
	void		*obj ;
	int		(*start)(void *) ;
	int		(*check)(void *) ;
	int		(*info)(void *) ;
	int		(*cmd)(void *) ;
	int		(*finish)(void *) ;
	POLLOBJ_FL	f ;
	uint		objsize ;
	uint		infosize ;
    } ;
}

struct thread_args {
	PCSPOLLS	*op ;
	PCSCONF		*pcp ;
} ;

struct work_head {
	THREAD		*tip ;
	VECHAND		polls ;
	volatile int	f_term ;
} ;

enum cmds {
	cmd_noop,
	cmd_exit,
	cmd_overlast
} ;


/* forward references */

static int pcspolls_valsbegin(pcspolls *,PCSCONF *,cchar *) noex ;
static int pcspolls_valsend(pcspolls *) noex ;

static int thread_start(THREAD *,pcspolls *) noex ;
static int thread_finish(THREAD *) noex ;
static int thread_cmdrecv(THREAD *,int) noex ;
static int thread_exiting(THREAD *) noex ;
static int thread_cmdexit(THREAD *) noex ;
static int thread_waitexit(THREAD *) noex ;
static int thread_worker(THREAD *) noex ;

#ifdef	COMMENT
static int thread_setdone(THREAD *) noex ;
#endif

static int work_start(WORK *,THREAD *) noex ;
static int work_term(WORK *) noex ;
static int work_finish(WORK *) noex ;
static int work_objloads(WORK *,THREAD *,char *,int) noex ;
static int work_objloadcheck(WORK *,cchar *,cchar *,int) noex ;
static int work_objload(WORK *,POLLINFO *) noex ;
static int work_objstarts(WORK *,THREAD *) noex ;
static int work_objchecks(WORK *) noex ;
static int work_objfins(WORK *) noex ;

static int pollinfo_syms(POLLINFO *,void *,cchar *,int) noex ;

static int pollobj_callstart(POLLOBJ *,THREAD *) noex ;
static int pollobj_check(POLLOBJ *) noex ;
static int pollobj_finish(POLLOBJ *) noex ;

static int mksymname(char *,cchar *,int,cchar *) noex ;

static bool isrequired(int) noex ;


/* local variables */

static cchar	*exts[] = {
	"so",
	"o",
	"",
	nullptr
} ;

static cchar	*subs[] = {
	"start",
	"check",
	"info",
	"cmd",
	"finish",
	nullptr
} ;

enum subs {
	sub_start,
	sub_check,
	sub_info,
	sub_cmd,
	sub_finish,
	sub_overlast
} ;


/* exported variables */

PCSPOLLS_OBJ	pcspolls = {
	"pcspolls",
	sizeof(PCSPOLLS),
	sizeof(PCSPOLLS_INFO)
} ;


/* exported subroutines */


int pcspolls_start(pcspolls *op,PCSCONF *pcp,cchar *sn)
{
	int		rs ;

	if (op == nullptr) return SR_FAULT ;
	if (pcp == nullptr) return SR_FAULT ;
	if (sn == nullptr) return SR_FAULT ;

	if (sn[0] == '\0') return SR_INVALID ;

#if	CF_DEBUGS
	debugprintf("pcspolls_start: sn=%s\n",sn) ;
#endif

	memset(op,0,sizeof(PCSPOLLS)) ;

	if ((rs = pcspolls_valsbegin(op,pcp,sn)) >= 0) {
	    if ((rs = thread_start(&op->t,op)) >= 0) {
		op->fl.working = true ;
		op->magic = PCSPOLLS_MAGIC ;
	    }
	    if (rs < 0)
	        pcspolls_valsend(op) ;
	} /* end if (vals) */

#if	CF_DEBUGS
	debugprintf("pcspolls_start: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (pcspolls_start) */


int pcspolls_finish(pcspolls *op)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;
	if (op->magic != PCSPOLLS_MAGIC) return SR_NOTOPEN ;

#if	CF_DEBUGN
	nprintf(NDF,"pcspolls_finish: ent\n") ;
#endif

	if (op->fl.working) {
	    op->fl.working = false ;
	    rs1 = thread_finish(&op->t) ;
	    if (rs >= 0) rs = rs1 ;
	}

	rs1 = pcspolls_valsend(op) ;
	if (rs >= 0) rs = rs1 ;

#if	CF_DEBUGN
	nprintf(NDF,"pcspolls_finish: ret rs=%d\n",rs) ;
#endif

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (pcspolls_finish) */


int pcspolls_info(pcspolls *op,PCSPOLLS_INFO *ip)
{
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != PCSPOLLS_MAGIC) return SR_NOTOPEN ;

	if (ip != nullptr) {
	    memset(ip,0,sizeof(PCSPOLLS_INFO)) ;
	    ip->dummy = 1 ;
	}

	return rs ;
}
/* end subroutine (pcspolls_info) */


int pcspolls_cmd(pcspolls *op,int cmd)
{
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != PCSPOLLS_MAGIC) return SR_NOTOPEN ;

	return (rs >= 0) ? cmd : rs ;
}
/* end subroutine (pcspolls_cmd) */


/* private subroutines */


static int pcspolls_valsbegin(pcspolls *op,PCSCONF *pcp,cchar *sn)
{
	int		rs ;
	int		size = 0 ;
	cchar	*pr = pcp->pr ;
	char		*bp ;

	op->pcp = pcp ;			/* supposedly stable throughout */
	op->envv = pcp->envv ;		/* supposedly stable throughout */

	size += (strlen(pr)+1) ;
	size += (strlen(sn)+1) ;
	if ((rs = uc_malloc(size,&bp)) >= 0) {
	    op->a = bp ;
	    op->pr = bp ;
	    bp = (strwcpy(bp,pr,-1)+1) ;
	    op->sn = bp ;
	    bp = (strwcpy(bp,sn,-1)+1) ;
	} /* end if (memory-allocation) */

	return rs ;
}
/* end subroutine (pcspolls_valsbegin) */


static int pcspolls_valsend(pcspolls *op)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (op->a != nullptr) {
	    rs1 = uc_free(op->a) ;
	    if (rs >= 0) rs = rs1 ;
	    op->a = nullptr ;
	}

	return rs ;
}
/* end subroutine (pcspolls_valsend) */


#ifdef	COMMENT
struct pcspolls_thread {
	PCSPOLLS	*op ;
	PCSCONF		*pcp ;
	cchar	*sn ;
	cchar 	*pr ;
	cchar	*envv ;
	PSEM		s ;
	pthread_t	tid ;
	volatile int	cmd ;
	volatile int	trs ;
	volatile int	f_response ;
	volatile int	f_done ;
	volatile int	f_exiting ;
} ;
#endif /* COMMENT */


static int thread_start(THREAD *tip,pcspolls *op)
{
	int		rs ;

	memset(tip,0,sizeof(THREAD)) ;
	tip->op = op ;
	tip->pr = op->pr ;
	tip->sn = op->sn ;
	tip->envv = op->envv ;
	tip->pcp = op->pcp ;
	tip->trs = SR_INPROGRESS ;
	tip->pid = getpid() ;

	if ((rs = thrcomm_start(&tip->tc,0)) >= 0) {
	    sigset_t	osm, nsm ;
	    if ((rs = uc_sigsetfill(&nsm)) >= 0) {
		if ((rs = u_sigmask(SIG_BLOCK,&nsm,&osm)) >= 0) {
	    	    pthread_t	tid ;
	    	    uptsub_t	fn = (uptsub_t) thread_worker ;
	    	    if ((rs = uptcreate(&tid,nullptr,fn,tip)) >= 0) {
	    	        tip->tid = tid ;
		    }
		    u_sigmask(SIG_SETMASK,&osm,nullptr) ;
		} /* end if (sigmask) */
	    } /* end if (signal handling) */
	} /* end if (thrcomm-start) */

	return rs ;
}
/* end subroutine (thread_start) */


static int thread_finish(THREAD *tip)
{
	const pid_t	pid = getpid() ;
	int		rs = SR_OK ;
	int		rs1 ;

#if	CF_DEBUGN
	nprintf(NDF,"pcspolls/thread__finish: ent\n") ;
#endif

	if (pid == tip->pid) {
	    if ((rs = thread_cmdexit(tip)) >= 0) {
	        rs = thread_waitexit(tip) ;
	        if (rs >= 0) rs = tip->trs ;
	    }
	} else {
	    tip->tid = 0 ;
	} /* end if (ours) */

#if	CF_DEBUGN
	nprintf(NDF,"pcspolls/thread__finish: mid rs=%d\n",rs) ;
#endif

	rs1 = thrcomm_finish(&tip->tc) ;
	if (rs >= 0) rs = rs1 ;

#if	CF_DEBUGN
	nprintf(NDF,"pcspolls/thread__finish: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (thread_finish) */


static int thread_cmdrecv(THREAD *tip,int to)
{
	int		rs ;
	int		cmd = 0 ;

	if ((rs = thrcomm_cmdrecv(&tip->tc,to)) >= 0) {
	    cmd = rs ;
	} else if (rs == SR_TIMEDOUT) {
	    rs = SR_OK ;
	}

	return (rs >= 0) ? cmd : rs ;
}
/* end subroutine (thread_cmdrecv) */


static int thread_exiting(THREAD *tip)
{
	tip->f_exiting = true ;
	return thrcomm_exiting(&tip->tc) ;
}
/* end subroutine (thread_exiting) */


static int thread_cmdexit(THREAD *tip)
{
	int		rs = SR_OK ;
	if (! tip->f_exiting) {
	    cint	cmd = cmd_exit ;
	    rs = thrcomm_cmdsend(&tip->tc,cmd,-1) ;
	}
	return rs ;
}
/* end subroutine (thread_cmdexit) */


static int thread_waitexit(THREAD *tip)
{
	int		rs ;
	int		trs ;
	if ((rs = uptjoin(tip->tid,&trs)) >= 0) {
	    tip->trs = trs ;
	}
	return rs ;
}
/* end subroutine (thread_waitexit) */


#ifdef	COMMENT
static int thread_setdone(THREAD *tip)
{
	cint	rrs = 1 ;
	return thrcomm_rspsend(&tip->tc,rrs,-1) ;
}
/* end subroutine (thread_setdone) */
#endif /* COMMENT */


static int thread_worker(THREAD *tip)
{
	WORK		w ;
	cint	to = 4 ;
	int		rs ;
	int		rs1 ;
	int		ctime = 0 ;

#if	CF_DEBUGS
	debugprintf("pcspolls/worker: ent\n") ;
#endif

#if	CF_DEBUGN
	nprintf(NDF,"pcspolls/worker: ent\n") ;
#endif

	if ((rs = work_start(&w,tip)) >= 0) {
	    int		f_exit = false ;
	    while ((rs = thread_cmdrecv(tip,to)) >= 0) {
	        cint	cmd = rs ;
		if ((rs = work_objchecks(&w)) >= 0) {
	            switch (cmd) {
	            case cmd_noop:
	                ctime += 1 ;
	                break ;
	            case cmd_exit:
	                f_exit = true ;
	                rs = work_term(&w) ;
	                break ;
	            } /* end switch */
		} /* end if (work_objchecks) */
	        if (f_exit) break ;
		if (rs < 0) break ;
	    } /* end while */
	    rs1 = work_finish(&w) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (work) */

	rs1 = thread_exiting(tip) ;
	if (rs >= 0) rs = rs1 ;

#if	CF_DEBUGS
	debugprintf("pcspolls/worker: ret rs=%d ctime=%u\n",rs,ctime) ;
#endif

#if	CF_DEBUGN
	nprintf(NDF,"pcspolls/worker: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (thread_worker) */

static int work_start(WORK *wp,THREAD *tip) noex {
	int		rs ;
	int		c = 0 ;
	cchar	*pr = tip->pr ;

	if (wp == nullptr) return SR_FAULT ;

	memset(wp,0,sizeof(WORK)) ;
	wp->tip = tip ;

#if	CF_DEBUGS
	debugprintf("pcspolls/work_start: ent\n") ;
#endif

	if ((rs = vechand_start(&wp->polls,2,0)) >= 0) {
	    cchar	*ld = LIBCNAME ;
	    cchar	*pd = POLLCNAME ;
	    char	pdname[MAXPATHLEN+1] ;

	    if ((rs = mkpath3(pdname,pr,ld,pd)) >= 0) {
	        int	dl = rs ;

#if	CF_DEBUGS
	        debugprintf("pcspolls/work_start: pdname=%s\n",pdname) ;
#endif

	        if (ustat sb ; (rs = u_stat(pdname,&sb)) >= 0) {
		    if (S_ISDIR(sb.st_mode)) {
	                if ((rs = work_objloads(wp,tip,pdname,dl)) >= 0) {
	                    if ((c = rs) > 0) {
	                        rs = work_objstarts(wp,tip) ;
	                    }
	                    if (rs < 0)
	                        work_objfins(wp) ;
		        } /* end if (work_objloads) */
		    } /* end if (is-dir) */
		} else if (isNotPresent(rs)) {
		    rs = SR_OK ;
	        } /* end if (stat) */
	    } /* end if (mkpath) */

	    if (rs < 0)
	        vechand_finish(&wp->polls) ;
	} /* end if (vechand-objs) */

#if	CF_DEBUGS
	debugprintf("pcspolls/work_start: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (work_start) */


static int work_finish(WORK *wp)
{
	VECHAND		*plp ;
	int		rs = SR_OK ;
	int		rs1 ;

	if (wp == nullptr) return SR_FAULT ;

#if	CF_DEBUGS
	debugprintf("pcspolls/work_finish: ent\n") ;
#endif

#if	CF_DEBUGN
	nprintf(NDF,"pcspolls/work_finish: ent\n") ;
#endif

	plp = &wp->polls ;

	rs1 = work_objfins(wp) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = vechand_finish(plp) ;
	if (rs >= 0) rs = rs1 ;

#if	CF_DEBUGS
	debugprintf("pcspolls/work_finish: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (work_finish) */


static int work_term(WORK *wp)
{
	if (wp == nullptr) return SR_FAULT ;
	wp->f_term = true ;

#if	CF_DEBUGS
	debugprintf("pcspolls/work_term: ent\n") ;
#endif
	return SR_OK ;
}
/* end subroutine (work_term) */

static int work_objloads(WORK *wp,THREAD *tip,char *dbuf,int dlen) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (tip == nullptr) return SR_FAULT ;
	if (fsdir d ; (rs = fsdir_open(&d,dbuf)) >= 0) {
	    int		nl ;
	    fsdir_ent	e ;
	    while ((rs = fsdir_read(&d,&e)) > 0) {
	        cchar	*sp = e.name ;
	        nl = rs ;
	        if (hasNotDots(sp,nl)) {
	            if (int cl ; (ol = sifext(sp,nl,exts)) > 0) {
	                if ((rs = pathadd(dbuf,dlen,sp)) >= 0) {
	                    rs = work_objloadcheck(wp,dbuf,sp,ol) ;
	                    c += rs ;
	                } /* end if (pathadd) */
	            } /* end if (has proper suffix) */
	        } /* end if (not-dots) */
		if (rs < 0) break ;
	    } /* end while (entries) */
	    dbuf[dlen] = '\0' ;
	    rs1 = fsdir_close(&d) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (fsdir) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (work_objloads) */

static int work_objloadcheck(WORK *wp,cchar *fname,cchar *sp,int sl) noex {
	struct pollinfo	oi ;
	int		rs ;
	int		c = 0 ;

#if	CF_DEBUGS
	debugprintf("pcspolls/work_objloadcheck: ent\n") ;
#endif

	if (ustat sb ; (rs = u_stat(fname,&sb)) >= 0) {
	    if (S_ISREG(sb.st_mode) && (sb.st_size > 0)) {
	        if ((rs = fileobject(fname)) > 0) {
	            cint	m = RTLD_LAZY ;
	            void	*sop ;
	            if ((sop = dlopen(fname,m)) != nullptr) {
	                memset(&oi,0,sizeof(struct pollinfo)) ;
	                oi.sop = sop ;
	                if ((rs = pollinfo_syms(&oi,sop,sp,sl)) > 0) {
#if	CF_DEBUGS
	debugprintf("pcspolls/work_objloadcheck: work_objload()\n") ;
#endif
	                    c = 1 ;
	                    rs = work_objload(wp,&oi) ;
#if	CF_DEBUGS
	                    debugprintf("pcspolls/work_objloadcheck: "
				"fn=%s rs=%d\n",fname,
	                        rs) ;
#endif
	                }
	                if ((rs < 0) || (c == 0))
	                    dlclose(sop) ;
	            } /* end if (dlopen) */
	        } /* end if (file-object-code) */
	    } /* end if (non-zero file-size) */
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	} /* end if (stat) */

#if	CF_DEBUGS
	debugprintf("pcspolls/work_objloadcheck: ret rs=%d c=%d\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (work_objloadcheck) */


static int work_objload(WORK *wp,POLLINFO *oip)
{
	cint	psize = sizeof(POLLOBJ) ;
	int		rs ;
	void		*p ;

	if (wp == nullptr) return SR_FAULT ;
	if (oip == nullptr) return SR_FAULT ;

	if ((rs = uc_malloc(psize,&p)) >= 0) {
	    POLLOBJ	*pop = p ;
	    memset(pop,0,psize) ;
	    pop->sop = oip->sop ;
	    pop->start = oip->start ;
	    pop->check = oip->check ;
	    pop->info = oip->info ;
	    pop->cmd = oip->cmd ;
	    pop->finish = oip->finish ;
	    {
	        PCSPOLLS_NAME	*pnp = (PCSPOLLS_NAME *) oip->objname ;
	        pop->name = pnp->name ;
	        pop->objsize = pnp->objsize ;
	        pop->infosize = pnp->infosize ;
	    }
	    {
	        cint	osize = pop->objsize ;
	        if ((rs = uc_malloc(osize,&p)) >= 0) {
	            pop->obj = p ;
	            rs = vechand_add(&wp->polls,pop) ;
#if	CF_DEBUGS
	            debugprintf("pcspolls/work_objload: vechand_add() rs=%d\n",
			rs) ;
#endif
	            if (rs < 0) {
	                uc_free(pop->obj) ;
	                pop->obj = nullptr ;
	            }
	        } /* end if (memory-allocation) */
	    }
	    if (rs < 0)
	        uc_free(pop) ;
	} /* end if (memory-allocation) */

	return rs ;
}
/* end subroutine (work_objload) */


static int work_objstarts(WORK *wp,THREAD *tip)
{
	VECHAND		*plp = &wp->polls ;
	POLLOBJ		*pop ;
	int		rs = SR_OK ;
	int		i ;
#if	CF_DEBUGS
	debugprintf("pcspolls/work_objstarts: ent\n") ;
#endif
	for (i = 0 ; vechand_get(plp,i,&pop) >= 0 ; i += 1) {
	    if (pop != nullptr) {
	        rs = pollobj_callstart(pop,tip) ;
	        if (rs < 0) break ;
	    }
	} /* end for */
#if	CF_DEBUGS
	debugprintf("pcspolls/work_objstarts: ret rs=%d u=%d\n",rs,i) ;
#endif
	return rs ;
}
/* end subroutine (work_objstarts) */


static int work_objchecks(WORK *wp)
{
	VECHAND		*plp = &wp->polls ;
	POLLOBJ		*pop ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	int		i ;
#if	CF_DEBUGN
	nprintf(NDF,"pcspolls/work_objchecks: ent\n") ;
#endif
	for (i = 0 ; vechand_get(plp,i,&pop) >= 0 ; i += 1) {
	    if (pop != nullptr) {
	        rs1 = pollobj_check(pop) ;
	        if (rs >= 0) rs = rs1 ;
		c += ((rs1 > 0)?1:0) ;
	    }
	} /* end for */
#if	CF_DEBUGN
	nprintf(NDF,"pcspolls/work_objchecks: ret rs=%d c=%u\n",rs,c) ;
#endif
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (work_objchecks) */


static int work_objfins(WORK *wp)
{
	VECHAND		*plp = &wp->polls ;
	POLLOBJ		*pop ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;
	int		c = 0 ;
#if	CF_DEBUGN
	nprintf(NDF,"pcspolls/work_objfins: ent\n") ;
#endif
	for (i = 0 ; vechand_get(plp,i,&pop) >= 0 ; i += 1) {
	    if (pop != nullptr) {
	        rs1 = pollobj_finish(pop) ;
	        if (rs >= 0) rs = rs1 ;
		c += ((rs1 > 0)?1:0) ;
	        rs1 = uc_free(pop) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
#if	CF_DEBUGN
	nprintf(NDF,"pcspolls/work_objfins: ret rs=%d c=%u\n",rs,c) ;
#endif
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (work_objfins) */


static int pollinfo_syms(POLLINFO *oip,void *sop,cchar *sp,int sl)
{
	int		rs ;
	char		symname[SYMNAMELEN+1] ;

	if ((rs = snwcpy(symname,SYMNAMELEN,sp,sl)) >= 0) {
	    void	*snp ;
	    if ((snp = dlsym(sop,symname)) != nullptr) {
	        int	i ;
	        oip->objname = snp ;
	        for (i = 0 ; subs[i] != nullptr ; i += 1) {
	            if ((rs = mksymname(symname,sp,sl,subs[i])) >= 0) {
	                if ((snp = dlsym(sop,symname)) != nullptr) {
	                    switch (i) {
	                    case sub_start:
	                        oip->start = (int (*)(void *)) snp ;
	                        break ;
	                    case sub_check:
	                        oip->check = (int (*)(void *)) snp ;
	                        break ;
	                    case sub_info:
	                        oip->info = (int (*)(void *)) snp ;
	                        break ;
	                    case sub_cmd:
	                        oip->cmd = (int (*)(void *)) snp ;
	                        break ;
	                    case sub_finish:
	                        oip->finish = (int (*)(void *)) snp ;
	                        break ;
	                    } /* end switch */
	                } /* end if (dlsym) */
	                if ((snp == nullptr) && isrequired(i)) {
	                    rs = 0 ; /* mark failure */
	                }
	            } /* end if (mksymname) */
	            if (rs <= 0) break ;
	        } /* end for */
	    } /* end if (dlsym) */
	} /* end if (snwcpy) */

	return rs ;
}
/* end subroutine (pollinfo_syms) */


static int pollobj_callstart(POLLOBJ *pop,THREAD *tip)
{
	int		rs = SR_OK ;

	if (tip == nullptr) return SR_FAULT ;

	if (pop->obj != nullptr) {
	    void	*pr = (void *) tip->pr ;
	    void	*sn = (void *) tip->sn ;
	    void	*envv = (void *) tip->envv ;
	    PCSCONF	*pcp = tip->pcp ;
	    void	*saddr = (void *) pop->start ;
	    int	(*start)(void *,void *,void *,void *,void *) ;
	    start = (int (*)(void *,void *,void *,void *,void *)) saddr ;
	    if ((rs = (*start)(pop->obj,pr,sn,envv,pcp)) >= 0) {
	        pop->fl.running = true ;
	        pop->fl.active = true ;
	    }
#if	CF_DEBUGS
	    debugprintf("pcspolls/pollobj_callstart: "
		"pollobj->start() rs=%d\n",rs) ;
#endif
	} else
	    rs = SR_BUGCHECK ;

	return rs ;
}
/* end subroutine (pollobj_callstart) */


static int pollobj_check(POLLOBJ *pop)
{
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = false ;

#if	CF_DEBUGN
	nprintf(NDF,"pcspolls/pollobj_check: ent n=%s\n",pop->name) ;
#endif

	if ((pop->check != nullptr) && pop->fl.running) {
	    int	(*check)(void *) = pop->check ;
	    rs1 = (*check)(pop->obj) ;
	    if (rs >= 0) rs = rs1 ;
	    f = ((rs1 > 0)?1:0) ;
#if	CF_EARLY
	    if (f) {
	    	int	(*finish)(void *) = pop->finish ;
		pop->fl.running = false ;
	    	rs1 = (*finish)(pop->obj) ;
	    	if (rs >= 0) rs = rs1 ;
		pop->fl.active = false ;
	    }
#endif /* CF_EARLY */
	} /* end if (checking) */

#if	CF_DEBUGN
	nprintf(NDF,"pcspolls/pollobj_check: ret rs=%d\n",rs,f) ;
#endif

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (pollobj_check) */


static int pollobj_finish(POLLOBJ *pop)
{
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = false ;

#if	CF_DEBUGS
	debugprintf("pcspolls/pollobj_finish: ent n=%s\n",pop->name) ;
#endif

#if	CF_DEBUGN
	nprintf(NDF,"pcspolls/pollobj_finish: ent n=%s\n",pop->name) ;
#endif

	if ((pop->finish != nullptr) && pop->fl.active) {
	    int	(*finish)(void *) = pop->finish ;
	    rs1 = (*finish)(pop->obj) ;
#if	CF_DEBUGS
	    debugprintf("pcspolls/pollobj_finish: obj->finish() rs=%d\n",rs1) ;
#endif
	    if (rs >= 0) rs = rs1 ;
	    pop->fl.running = false ;
	    pop->fl.active = false ;
	    f = true ;
	}

	if (pop->obj != nullptr) {
	    rs1 = uc_free(pop->obj) ;
	    if (rs >= 0) rs = rs1 ;
	    pop->obj = nullptr ;
	}

	if (pop->sop != nullptr) {
	   dlclose(pop->sop) ;
	   pop->sop = nullptr ;
	}

#if	CF_DEBUGN
	nprintf(NDF,"pcspolls/pollobj_finish: ret rs=%d\n",rs) ;
#endif

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (pollobj_finish) */

static int mksymname(char *rbuf,cchar *sp,int sl,cchar *sub) noex {
	cint		rlen = SYMNAMELEN ;
	int		rs = SR_OK ;
	int		nl = 0 ;
	if (rs >= 0) {
	    rs = storebuf_strw(rbuf,rlen,nl,sp,sl) ;
	    nl += rs ;
	}
	if (rs >= 0) {
	    rs = storebuf_chr(rbuf,rlen,nl,'_') ;
	    nl += rs ;
	}
	if (rs >= 0) {
	    rs = storebuf_strw(rbuf,rlen,nl,sub,-1) ;
	    nl += rs ;
	}
	return (rs >= 0) ? nl : rs ;
}
/* end subroutine (mksymname) */

static bool isrequired(int i) noex {
	int		f = false ;
	switch (i) {
	case sub_start:
	case sub_finish:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
}
/* end subroutine (isrequired) */


