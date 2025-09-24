/* pcsconfs SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface to query the PCS configuration-variable database */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-07, David A­D­ Morano
	This was created to allow for the main part of the PCSCONF
	facility to be a loadable module.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	pcsconfs

	Description:
	This little object provides access to the PCSCONF database
	and index (if any).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mktmp.h>
#include	<vecstr.h>
#include	<spawnproc.h>
#include	<ids.h>
#include	<fsdir.h>
#include	<varmk.h>
#include	<paramfile.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"pcsconfs.h"
#include	"var.h"
#include	"varmk.h"


/* local defines */

#ifndef	VARSYSNAME
#define	VARSYSNAME	"SYSNAME"
#endif

#ifndef	VARRELEASE
#define	VARRELEASE	"RELEASE"
#endif

#ifndef	VARVERSION
#define	VARVERSION	"VERSION"
#endif

#ifndef	VARMACHINE
#define	VARMACHINE	"MACHINE"
#endif

#ifndef	VARARCHITECTURE
#define	VARARCHITECTURE	"ARCHITECTURE"
#endif

#ifndef	VARHZ
#define	VARHZ		"HZ"
#endif

#ifndef	VARDOMAIN
#define	VARDOMAIN	"DOMAIN"
#endif

#ifndef	VARNODE
#define	VARNODE		"NODE"
#endif

#ifndef	VARHOMEDNAME
#define	VARHOMEDNAME	"HOME"
#endif

#ifndef	VARUSERNAME
#define	VARUSERNAME	"USERNAME"
#endif

#ifndef	VARLOGNAME
#define	VARLOGNAME	"LOGNAME"
#endif

#ifndef	VARTZ
#define	VARTZ		"TZ"
#endif

#ifndef	VARWSTATION
#define	VARWSTATION	"ESTATION"
#endif

#ifndef	VARPWD
#define	VARPWD		"PWD"
#endif

#ifndef	VARTMPDNAME
#define	VARTMPDNAME	"TMPDIR"
#endif

#ifndef	VARPRLOCAL
#define	VARPRLOCAL	"LOCAL"
#endif

#ifndef	TMPDNAME
#define	TMPDNAME	"/tmp"
#endif

#undef	VARSVDBNAME
#define	VARSVDBNAME	"PCSCONFS_DBNAME"

#undef	VARSVPR
#define	VARSVPR		"PCSCONFS_PROGRAMROOT"

#ifndef	TMPDNAME
#define	TMPDNAME	"/tmp"
#endif

#ifndef	TMPVARDNAME
#define	TMPVARDNAME	"/var/tmp"
#endif

#define	CONFVARS	struct confvars

#define	PRCONF		"conf"

#define	PCSCONFNAME	"conf"

#ifndef	VCNAME
#define	VCNAME		"var"
#endif

#define	INDDNAME	"pcsconfs"

#define	INDNAME		"pcsconfs"
#define	INDSUF		"vi"

#ifndef	PARAMBUFLEN
#define	PARAMBUFLEN	256
#endif

#ifndef	KBUFLEN
#define	KBUFLEN		120
#endif

#define	TO_FILEMOD	(60 * 24 * 3600)
#define	TO_MKWAIT	(5 * 50)

#define	PROG_MKPCSCONFSI	"mkpcsconf"

#ifndef	DEFINITFNAME
#define	DEFINITFNAME	"/etc/default/init"
#endif

#ifndef	DEFLOGFNAME
#define	DEFLOGFNAME	"/etc/default/login"
#endif

#ifndef	DEFNVARS
#define	DEFNVARS	20
#endif

#define	CONFVARS	confvars
#define	CONFVARS_FL	confvars_flags


/* external subroutines */

extern int	sncpy1(char *,int,cchar *) ;
extern int	sncpy2(char *,int,cchar *,cchar *) ;
extern int	sncpy3(char *,int,cchar *,cchar *,cchar *) ;
extern int	sncpy4(char *,int, cchar *,cchar *,cchar *,cchar *) ;
extern int	mkpath1(char *,cchar *) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	mkpath3(char *,cchar *,cchar *,cchar *) ;
extern int	mkpath4(char *,cchar *,cchar *,cchar *,cchar *) ;
extern int	mkpath1w(char *,cchar *,int) ;
extern int	mkfnamesuf1(char *,cchar *,cchar *) ;
extern int	mkfnamesuf2(char *,cchar *,cchar *,cchar *) ;
extern int	pathadd(char *,int,cchar *) ;
extern int	sfbasename(cchar *,int,cchar **) ;
extern int	nextfield(cchar *,int,cchar **) ;
extern int	nleadstr(cchar *,cchar *,int) ;
extern int	pathclean(char *,cchar *,int) ;
extern int	permid(ids *,USTAT *,int) ;
extern int	perm(cchar *,uid_t,gid_t,gid_t *,int) ;
extern int	permf(int,uid_t,gid_t,gid_t *,int) ;
extern int	permsched(cchar **,vecstr *,char *,int,cchar *,int) ;
extern int	hasuc(cchar *,int) ;
extern int	isalnumlatin(int) ;
extern int	strpcmp(cchar *,cchar *) ;
extern int	vecstr_adduniq(vecstr *,cchar *,int) ;
extern int	vecstr_envadd(vecstr *,cchar *,cchar *,int) ;
extern int	vecstr_envset(vecstr *,cchar *,cchar *,int) ;
extern int	prmktmpdir(cchar *,char *,cchar *,cchar *,mode_t) ;
extern int	hasdots(cchar *,int) ;
extern int	hasNotDots(cchar *,int) ;
extern int	isNotPresent(int) ;
extern int	isOneOf(cint *,int) ;

extern char	*strwcpylc(char *,cchar *,int) ;
extern char	*timestr_logz(time_t,char *) ;


/* external variables */

extern char	**environ ;


/* local structures */

struct confvars_flags {
	uint		conf:1 ;
	uint		id:1 ;
	uint		cfname:1 ;	/* memory-allocated */
	uint		rstchown:1 ;
} ;

struct confvars {
	ids		id ;
	paramfile	pf ;
	varmk		v ;
	CONFVARS_FL	fl ;
	PCSCONFS	*op ;
	cchar		**envv ;
	cchar		*prconf ;
	cchar		*pr ;
	cchar		*cfname ;
	uid_t		uid_pcs ;
	gid_t		gid_pcs ;
	time_t		daytime ;
	time_t		cmtime ;	/* conf-file mtime */
} ;


/* forward references */

static int	pcsconfs_valsbegin(PCSCONFS *,cchar *,cchar *) noex ;
static int	pcsconfs_valsend(PCSCONFS *) noex ;
static int	pcsconfs_dbcheck(PCSCONFS *) noex ;
static int	pcsconfs_dbclose(PCSCONFS *) noex ;
static int	pcsconfs_finout(PCSCONFS *) noex ;

static int	confvars_start(CONFVARS *,PCSCONFS *) noex ;
static int	confvars_finish(CONFVARS *) noex ;
static int	confvars_dbstart(CONFVARS *) noex ;
static int	confvars_loadsubs(CONFVARS *,vecstr *) noex ;
static int	confvars_confglobal(CONFVARS *,char *) noex ;
static int	confvars_conflocal(CONFVARS *,char *) noex ;
static int	confvars_dbopen(CONFVARS *,cchar *) noex ;
static int	confvars_dbclose(CONFVARS *) noex ;
static int	confvars_dbmake(CONFVARS *,cchar *) noex ;
static int	confvars_proc(CONFVARS *) noex ;
static int	confvars_chown(CONFVARS *,char *,int) noex ;
static int	confvars_ids(CONFVARS *) noex ;


/* local variables */

static constexpr cpcchar	schedconf[] = {
	"%p/etc/%n.%f",
	"%p/etc/%f",
	"%p/%n.%f",
	nullptr
} ;

static constexpr int		stales[] = {
	SR_STALE,
	SR_NOENT,
	SR_ACCESS,
	0
} ;


/* exported variables */

PCSCONFS_OBJ	pcsconfs = {
	"pcsconfs",
	sizeof(PCSCONFS),
	sizeof(PCSCONFS_CUR)
} ;


/* exported subroutines */

int pcsconfs_start(PCSCONFS *op,cchar *pr,cchar **envv,cchar *cfname) noex {
	int		rs ;

	if (op == nullptr) return SR_FAULT ;
	if (pr == nullptr) return SR_FAULT ;

	if (pr[0] == '\0') return SR_INVALID ;

	if ((cfname != nullptr) && (cfname[0] == '\0')) return SR_INVALID ;

	memset(op,0,sizeof(PCSCONFS)) ;
	op->envv = (envv != nullptr) ? envv : (cchar **) environ ;
	op->fl.prdb = (cfname == nullptr) ;

	if ((rs = pcsconfs_valsbegin(op,pr,cfname)) >= 0) {
	    if ((rs = pcsconfs_dbcheck(op)) >= 0) {
		op->magic = PCSCONFS_MAGIC ;
	    }
	    if (rs < 0)
		pcsconfs_valsend(op) ;
	} /* end if */

	return rs ;
}
/* end subroutine (pcsconfs_start) */

int pcsconfs_finish(PCSCONFS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != PCSCONFS_MAGIC) return SR_NOTOPEN ;

	rs1 = pcsconfs_finout(op) ;
	if (rs >= 0) rs = rs1 ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (pcsconfs_finish) */

int pcsconfs_audit(PCSCONFS *op) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != PCSCONFS_MAGIC) return SR_NOTOPEN ;

	if (op->fl.db) {
	    rs = var_audit(&op->db) ;
	}

	return rs ;
}
/* end subroutine (pcsconfs_audit) */

int pcsconfs_curbegin(PCSCONFS *op,PCSCONFS_CUR *curp) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != PCSCONFS_MAGIC) return SR_NOTOPEN ;

	memset(curp,0,sizeof(PCSCONFS_CUR)) ;

	if (op->fl.db) {
	    rs = var_curbegin(&op->db,&curp->vcur) ;
	    if (rs >= 0) op->ncursors += 1 ;
	}

	if (rs >= 0) curp->magic = PCSCONFS_CURMAGIC ;

	return rs ;
}
/* end subroutine (pcsconfs_curbegin) */

int pcsconfs_curend(PCSCONFS *op,PCSCONFS_CUR *curp) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != PCSCONFS_MAGIC) return SR_NOTOPEN ;
	if (curp->magic != PCSCONFS_CURMAGIC) return SR_NOTOPEN ;

	if (op->fl.db) {
	    rs1 = var_curend(&op->db,&curp->vcur) ;
	    if (rs >= 0) rs = rs1 ;
	    if (op->ncursors > 0) op->ncursors -= 1 ;
	}

	curp->magic = 0 ;
	return rs ;
}
/* end subroutine (pcsconfs_curend) */

int pcsconfs_fetch(PCSCONFS *op,cchar *kp,int kl,PCSCONFS_CUR *curp,
		char *vbuf,int vlen) noex {
	int		rs = SR_NOTFOUND ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (kp == nullptr) return SR_FAULT ;

	if (op->magic != PCSCONFS_MAGIC) return SR_NOTOPEN ;
	if (curp->magic != PCSCONFS_CURMAGIC) return SR_NOTOPEN ;

	if (op->fl.db) {
	    rs = var_fetch(&op->db,kp,kl,&curp->vcur,vbuf,vlen) ;
	}

	if ((rs < 0) && (vbuf != nullptr)) {
	    vbuf[0] = '\0' ;
	}

	return rs ;
}
/* end subroutine (pcsconfs_fetch) */


int pcsconfs_enum(PCSCONFS *op,PCSCONFS_CUR *curp,char *kbuf,int klen,
		char *vbuf,int vlen) noex {
	int		rs = SR_NOTFOUND ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (kbuf == nullptr) return SR_FAULT ;

	if (op->magic != PCSCONFS_MAGIC) return SR_NOTOPEN ;
	if (curp->magic != PCSCONFS_CURMAGIC) return SR_NOTOPEN ;

	if (op->fl.db) {
	    rs = var_enum(&op->db,&curp->vcur,kbuf,klen,vbuf,vlen) ;
	}

	if ((rs < 0) && (vbuf != nullptr)) {
	    vbuf[0] = '\0' ;
	}

	return rs ;
}
/* end subroutine (pcsconfs_enum) */

int pcsconfs_count(PCSCONFS *op) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != PCSCONFS_MAGIC) return SR_NOTOPEN ;

	if (op->fl.db) {
	    rs = var_count(&op->db) ;
	}

	return rs ;
}
/* end subroutine (pcsconfs_count) */


/* private subroutines */

static int pcsconfs_valsbegin(PCSCONFS *op,cchar *pr,cchar *cfname) noex {
	int		rs ;
	int		size = 0 ;
	char		*bp{} ;
	size += (strlen(pr)+1) ;
	if (cfname != nullptr) size += (strlen(cfname)+1) ;
	if ((rs = uc_malloc(size,&bp)) >= 0) {
	    op->a = bp ;
	    op->pr = bp ;
	    bp = (strwcpy(bp,pr,-1)+1) ;
	    if (cfname != nullptr) {
		op->cfname = bp ;
	        bp = (strwcpy(bp,cfname,-1)+1) ;
	    }
	} /* end if (memory-allocation) */
	return rs ;
}
/* end subroutine (pcsconfs_valsbegin) */

static int pcsconfs_valsend(PCSCONFS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op->a != nullptr) {
	    rs1 = uc_free(op->a) ;
	    if (rs >= 0) rs = rs1 ;
	    op->a = nullptr ;
	}

	return rs ;
}
/* end subroutine (pcsconfs_valsend) */

static int pcsconfs_dbcheck(PCSCONFS *op) noex {
	CONFVARS	si, *sip = &si ;
	int		rs ;
	int		rs1 ;
	int		f_conf = false ;

	if ((rs = confvars_start(sip,op)) >= 0) {

	    if (sip->fl.conf) {
	        f_conf = true ;
	        rs = confvars_dbstart(sip) ;
	    }

	    rs1 = confvars_finish(sip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (confvars) */

	return (rs >= 0) ? f_conf : rs ;
}
/* end subroutine (pcsconfs_dbcheck) */

static int pcsconfs_dbclose(PCSCONFS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fl.db) {
	    op->fl.db = false ;
	    rs1 = var_close(&op->db) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (pcsconfs_dbclose) */

static int pcsconfs_finout(PCSCONFS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = pcsconfs_dbclose(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = pcsconfs_valsend(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (pcsconfs_finout) */

static int confvars_start(CONFVARS *sip,PCSCONFS *op) noex {
	int		rs = SR_OK ;
	int		rs1 = SR_OK ;
	cchar		*cfname ;
	char		tmpfname[MAXPATHLEN+1] ;

	memset(sip,0,sizeof(CONFVARS)) ;
	sip->op = op ;
	sip->envv = op->envv ;
	sip->prconf = PRCONF ;
	sip->pr = op->pr ;
	sip->cfname = op->cfname ;
	sip->uid_pcs = -1 ;
	sip->gid_pcs = -1 ;

/* find the PCS-configuration file if necessary */

	cfname = op->cfname ;

	if (op->fl.prdb) {
	    vecstr	subs ;
	    if ((rs = vecstr_start(&subs,4,0)) >= 0) {

	        if ((rs = confvars_loadsubs(sip,&subs)) >= 0) {
	            const mode_t	m = R_OK ;
	            cint	plen = MAXPATHLEN ;
		    cchar	*cn = sip->prconf ;
	            char	*pbuf = tmpfname ;
	            cfname = tmpfname ;
	            rs1 = permsched(schedconf,&subs,pbuf,plen,cn,m) ;
		    if (rs1 >= 0) {
			cchar	*cp ;
			if ((rs = uc_mallocstrw(pbuf,rs1,&cp)) >= 0) {
			    sip->fl.cfname = true ;
			    sip->cfname = cp ;
			}
		    }
	        } /* end if (subs loaded) */

	        vecstr_finish(&subs) ;
	    } /* end if (subs) */
	} /* end if */

	if (rs >= 0) {
	    if (rs1 >= 0) {
	        USTAT	sb ;
	        rs1 = u_stat(cfname,&sb) ;
	        if ((rs1 >= 0) && S_ISREG(sb.st_mode)) {
	            sip->fl.conf = true ;
	            sip->cmtime = sb.st_mtime ;
	        }
	    }
	} /* end if */

	return rs ;
}
/* end subroutine (confvars_start) */

static int confvars_finish(CONFVARS *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (sip->fl.cfname && (sip->cfname != nullptr)) {
	    sip->fl.cfname = false ;
	    rs1 = uc_free(sip->cfname) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->cfname = nullptr ;
	}

	if (sip->fl.id) {
	    sip->fl.id = false ;
	    rs1 = ids_release(&sip->id) ;
	    if (rs >= 0) rs = rs1 ;
	}

	return rs ;
}
/* end subroutine (confvars_finish) */

static int confvars_loadsubs(CONFVARS *sip,vecstr *slp) noex {
	int		rs = SR_OK ;
	int		bl ;
	cchar		*bp ;

	bl = sfbasename(sip->pr,-1,&bp) ;
	if (bl <= 0) rs = SR_INVALID ;

	if (rs >= 0)
	    rs = vecstr_envadd(slp,"p",sip->pr,-1) ;

	if (rs >= 0)
	    rs = vecstr_envadd(slp,"n",bp,bl) ;

	return rs ;
}
/* end subroutine (confvars_loadsubs) */

static int confvars_dbstart(CONFVARS *sip) noex {
	PCSCONFS	*op = sip->op ;
	int		rs = SR_OK ;
	int		dl = -1 ;
	char		dbname[MAXPATHLEN+1] ;

	if (op->fl.prdb) {
	    rs = confvars_confglobal(sip,dbname) ;
	    dl = rs ;
	} else {
	    rs = confvars_conflocal(sip,dbname) ;
	    dl = rs ;
	}
	if (rs >= 0) {
	    rs = pathadd(dbname,dl,sip->prconf) ;
	}
	if (rs >= 0) {
	    rs = confvars_dbopen(sip,dbname) ;
	    if (isOneOf(stales,rs)) {
	        if (rs == SR_ACCESS) varunlink(dbname,-1) ;
	        rs = confvars_dbmake(sip,dbname) ;
		if (isNotPresent(rs) && op->fl.prdb) {
		    op->fl.prdb = false ;
	    	    if ((rs = confvars_conflocal(sip,dbname)) >= 0) {
			dl = rs ;
	    		if ((rs = pathadd(dbname,dl,sip->prconf)) >= 0)
	        	    rs = confvars_dbmake(sip,dbname) ;
		    }
		}
	        if (rs >= 0) {
	            rs = confvars_dbopen(sip,dbname) ;
		    if ((rs >= 0) && op->fl.prdb) {
		        dbname[dl] = '\0' ;
		        rs = confvars_chown(sip,dbname,dl) ;
		    }
		    if (rs < 0)
			confvars_dbclose(sip) ;
		}
	    } /* end if */
	} /* end if */

#if	CF_DEBUGS
	debugprintf("confvars_dbstart: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (confvars_dbstart) */

static int confvars_confglobal(CONFVARS *sip,char *dname) noex {
	int		rs ;
	cmode		dm = 0777 ;
	cchar		*tmpdname = getenv(VARTMPDNAME) ;
	cchar		*cdname = "pcsconf" ;

	if (sip == nullptr) return SR_FAULT ;

	if (tmpdname == nullptr) tmpdname = TMPDNAME ;

#if	CF_DEBUGS
	debugprintf("confvars_confglobal/prmktmpdir: pr=%s\n",sip->pr) ;
	debugprintf("confvars_confglobal/prmktmpdir: prconf=%s\n",sip->prconf) ;
#endif

	rs = prmktmpdir(sip->pr,dname,tmpdname,cdname,dm) ;

	return rs ;
}
/* end subroutine (confvars_confglobal) */

static int confvars_conflocal(CONFVARS *sip,char *dname) noex {
	int		rs ;
	cmode		dm = 0775 ;
	cchar		*cdname = "pcsconf" ;

	if (sip == nullptr) return SR_FAULT ;

	rs = mktmpuserdir(dname,"-",cdname,dm) ;

	return rs ;
}
/* end subroutine (confvars_conflocal) */

static int confvars_dbopen(CONFVARS *sip,cchar *dbname) noex {
	PCSCONFS	*op = sip->op ;
	VAR		*vdp ;
	int		rs ;

#if	CF_DEBUGS
	debugprintf("confvars_dbopen: dbname=%s\n",dbname) ;
#endif

	vdp = &op->db ;
	if ((rs = var_open(vdp,dbname)) >= 0) {
	    VAR_INFO	vi ;
	    op->fl.db = true ;
	    if ((rs = var_info(vdp,&vi)) >= 0) {
#if	CF_DEBUGS
	{
		char	timebuf[TIMEBUFLEN+1] ;
		time_t	t ;
		t = (time_t) sip->cmtime ;
		timestr_logz(t,timebuf) ;
		debugprintf("confvars_dbopen: ct=%s\n",timebuf) ;
		t = (time_t) vi.wtime ;
		timestr_logz(t,timebuf) ;
		debugprintf("confvars_dbopen: wt=%s\n",timebuf) ;
	}
#endif

	        if (sip->cmtime > vi.wtime) rs = SR_STALE ;
	    }
	    if (rs < 0) {
	        op->fl.db = false ;
	        var_close(vdp) ;
	    }
	} /* end if (attempted open) */

#if	CF_DEBUGS
	debugprintf("confvars_dbopen: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (confvars_dbopen) */

static int confvars_dbclose(CONFVARS *sip) noex {
	PCSCONFS	*op = sip->op ;
	VAR		*vdp ;
	int		rs = SR_OK ;
	int		rs1 ;

	vdp = &op->db ;
	if (op->fl.db) {
	    op->fl.db = false ;
	    rs1 = var_close(vdp) ;
	    if (rs >= 0) rs = rs1 ;
	}

	return rs ;
}
/* end subroutine (confvars_dbclose) */

static int confvars_dbmake(CONFVARS *sip,cchar *dbname) noex {
	varmk		*vmp = &sip->v ;
	cint		of = O_CREAT ;
	cint		n = 40 ;
	cmode		vm = 0664 ;
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if ((rs = varmk_open(&sip->v,dbname,of,vm,n)) >= 0) {
	    {
	        rs = confvars_proc(sip) ;
		rv = rs ;
	    }
	    rs1 = varmk_close(vmp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (varmk) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (confvars_dbmake) */

static int confvars_proc(CONFVARS *sip) noex {
	USTAT		sb ;
	int		rs ;
	int		rs1 ;

	if ((rs = u_stat(sip->cfname,&sb)) >= 0) {
	    paramfile		*pfp = &sip->pf ;
	    paramfile_cur	cur ;
	    paramfile_ent	pe ;
	    if ((rs = paramfile_open(pfp,sip->envv,sip->cfname)) >= 0) {

	        if ((rs = paramfile_curbegin(pfp,&cur)) >= 0) {
	            cint	plen = PARAMBUFLEN ;
	            int		kl ;
	            char	pbuf[PARAMBUFLEN+1] ;

	            while (rs >= 0) {
	                kl = paramfile_curenum(pfp,&cur,&pe,pbuf,plen) ;
	                if (kl == SR_NOTFOUND) break ;
	                rs = kl ;
	                if (rs < 0) break ;

	                rs = varmk_addvar(&sip->v,pe.key,pe.value,pe.vlen) ;

	            } /* end while (reading parameters) */

	            paramfile_curend(pfp,&cur) ;
	        } /* end if (cursor) */

	        rs1 = paramfile_close(pfp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (paramfile) */
	} /* end if (stat-file) */

	return rs ;
}
/* end subroutine (confvars_proc) */

static int confvars_chown(CONFVARS *sip,char *dname,int dl) noex {
	cint		req = _PC_CHOWN_RESTRICTED ;
	int		rs = SR_OK ;

#ifdef	_PC_CHOWN_RESTRICTED
	if ((rs = u_pathconf(dname,req,nullptr)) == 0) {
	    if ((rs = confvars_ids(sip)) >= 0) {
		ids		*idp = &sip->id ;
	        fsdir		d ;
	        fsdir_ent	e ;
		uid_t		u = sip->uid_pcs ;
		gid_t		g = sip->gid_pcs ;
	        if ((rs = fsdir_open(&d,dname)) >= 0) {
		    USTAT	sb ;
		    int		nl ;
		    cchar	*np ;
		    while ((rs = fsdir_read(&d,&e)) > 0) {
			np = e.name ;
			nl = rs ;
			if (hasNotDots(np,nl)) {
			    if ((rs = pathadd(dname,dl,np)) >= 0) {
		                if (u_stat(dname,&sb) >= 0) {
				    if (sb.st_uid == idp->euid) {
					int	f = false ;
    			                f = f || (sb.st_uid != u) ;
			                f = f || (sb.st_gid != g) ;
			                if (f) u_chown(dname,u,g) ;
				    }
		                } /* end if (stat) */
			    } /* end if (pathadd) */
			} /* end if (hasNotDots) */
		        if (rs < 0) break ;
		    } /* end while (reading entries) */
		    dname[dl] = '\0' ;
		    fsdir_close(&d) ;
	        } /* end if (fsdir) */
	    } /* end if (confvars_ids) */
	} else if (rs == SR_NOSYS) {
	    rs = SR_OK ;
	}
#endif /* _PC_CHOWN_RESTRICTED */

	return rs ;
}
/* end subroutine (confvars_chown) */

static int confvars_ids(CONFVARS *sip) noex {
	int		rs = SR_OK ;
	if (! sip->fl.id) {
	    sip->fl.id = true ;
	    if ((rs = ids_load(&sip->id)) >= 0) {
	        if (ustat sb ; (rs = u_stat(sip->pr,&sb)) >= 0) {
		    sip->uid_pcs = sb.st_uid ;
		    sip->gid_pcs = sb.st_gid ;
	        }
	        if (rs < 0) {
	            sip->fl.id = false ;
	            ids_release(&sip->id) ;
		}
	    } /* end if (loaded IDs) */
	} /* end if (needed IDs) */
	return rs ;
}
/* end subroutine (confvars_ids) */


