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
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBU */
#include	<mktmp.h>		/* LIBU */
#include	<vecstr.h>		/* LIBU */
#include	<spawnproc.h>		/* LIBU */
#include	<ids.h>			/* LIBU */
#include	<fsdir.h>		/* LIBU */
#include	<varmk.h>		/* LIBU */
#include	<paramfile.h>		/* LIBU */
#include	<pathadd.h>		/* LIBU */
#include	<sfx.h>			/* LIBU */
#include	<strwcpy.h>		/* LIBU */
#include	<permx.h>		/* LIBU */
#include	<var.h>			/* LIBU */
#include	<varmk.h>		/* LIBU */
#include	<hasx.h>		/* LIBU */
#include	<isoneof.h>		/* LIBU */
#include	<isnot.h>		/* LIBU */
#include	<vardefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libpr.h>		/* LIBPR */

#include	"pcsconfs.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	TMPDNAME
#define	TMPDNAME	"/tmp"
#endif

#undef	VARSVDBNAME
#define	VARrSVDBNAME	"PCSCONFS_DBNAME"

#undef	VARSVPR
#define	VARSVPR		"PCSCONFS_PROGRAMROOT"

#ifndef	TMPDNAME
#define	TMPDNAME	"/tmp"
#endif

#ifndef	TMPVARDNAME
#define	TMPVARDNAME	"/var/tmp"
#endif

#define	PC		pcsconfs
#define	PC_CUR		pcsconfs_cur
#define	PC_MAGIC	PCSCONFS_MAGIC
#define	PC_CURMAGIC	PCSCONFS_CURMAGIC

#define	CV		confvars
#define	CV_FL		confvars_flags

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


/* external subroutines */


/* external variables */


/* local structures */

struct confvars_flags {
	uint		conf:1 ;
	uint		id:1 ;
	uint		cfname:1 ;	/* memory-allocated */
	uint		rstchown:1 ;
} ; /* end struct */

struct confvars {
	ids		id ;
	paramfile	pf ;
	varmk		v ;
	CV_FL		fl ;
	pcsconfs	*op ;
	mainv		envv ;
	cchar		*prconf ;
	cchar		*pr ;
	cchar		*cfname ;
	uid_t		uid_pcs ;
	gid_t		gid_pcs ;
	time_t		daytime ;
	time_t		cmtime ;	/* conf-file mtime */
} ; /* end struct */


/* forward references */

local int	pcsconfs_valsbegin(PC *,cchar *,cchar *) noex ;
local int	pcsconfs_valsend(PC *) noex ;
local int	pcsconfs_dbcheck(PC *) noex ;
local int	pcsconfs_dbclose(PC *) noex ;
local int	pcsconfs_finout(PC *) noex ;
local int	pcsconfs_envv(PC *,mainv) noex ;

local int	confvars_start(CV *,PC *) noex ;
local int	confvars_finish(CV *) noex ;
local int	confvars_dbstart(CV *) noex ;
local int	confvars_loadsubs(CV *,vecstr *) noex ;
local int	confvars_confglobal(CV *,char *) noex ;
local int	confvars_conflocal(CV *,char *) noex ;
local int	confvars_dbopen(CV *,cchar *) noex ;
local int	confvars_dbclose(CV *) noex ;
local int	confvars_dbmake(CV *,cchar *) noex ;
local int	confvars_proc(CV *) noex ;
local int	confvars_chown(CV *,char *,int) noex ;
local int	confvars_ids(CV *) noex ;


/* local variables */

constexpr cpcchar	schedconf[] = {
	"%p/etc/%n.%f",
	"%p/etc/%f",
	"%p/%n.%f",
	nullptr
} ; /* end array */

constexpr int		stales[] = {
	SR_STALE,
	SR_NOENT,
	SR_ACCESS,
	0
} ; /* end array */


/* exported variables */

const pcsconfs_obj	pcsconfs_mod = {
	"pcsconfs",
	szof(pcsconfs),
	szof(pcsconfs_cur)
} ; /* end object */


/* exported subroutines */

int pcsconfs_start(PC *op,cchar *pr,mainv ev,cchar *cfname) noex {
	int		rs = SR_FAULT ;
	if (op && pr) {
	    rs = SR_INVALID ;
	    memclear(op) ; /* dangerous */
	    op->fl.prdb = (cfname == nullptr) ;
	    if (pr[0]) {
		if ((rs = pcsconfs_envv(op,ev)) >= 0) {
	            if ((rs = pcsconfs_valsbegin(op,pr,cfname)) >= 0) {
	                if ((rs = pcsconfs_dbcheck(op)) >= 0) {
		            op->magval = PC_MAGIC ;
	                }
	                if (rs < 0) {
		            pcsconfs_valsend(op) ;
	                }
	            } /* end if */
		} /* end if (pcsconfs_envv) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (pcsconfs_start) */

int pcsconfs_finish(PC *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->magval == PC_MAGIC) {
	        rs1 = pcsconfs_finout(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (pcsconfs_finish) */

int pcsconfs_audit(PC *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->magval == PC_MAGIC) {
	        if (op->fl.db) {
	            rs = var_audit(&op->db) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (pcsconfs_audit) */

int pcsconfs_curbegin(PC *op,PC_CUR *curp) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magval != PC_MAGIC) return SR_NOTOPEN ;

	memclear(curp) ;

	if (op->fl.db) {
	    rs = var_curbegin(&op->db,&curp->vcur) ;
	    if (rs >= 0) op->ncursors += 1 ;
	}

	if (rs >= 0) curp->magval = PC_CURMAGIC ;

	return rs ;
} /* end subroutine (pcsconfs_curbegin) */

int pcsconfs_curend(PC *op,PC_CUR *curp) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magval != PC_MAGIC) return SR_NOTOPEN ;
	if (curp->magval != PC_CURMAGIC) return SR_NOTOPEN ;

	if (op->fl.db) {
	    rs1 = var_curend(&op->db,&curp->vcur) ;
	    if (rs >= 0) rs = rs1 ;
	    if (op->ncursors > 0) op->ncursors -= 1 ;
	}

	curp->magval = 0 ;
	return rs ;
} /* end subroutine (pcsconfs_curend) */

int pcsconfs_fetch(PC *op,cchar *kp,int kl,PC_CUR *curp,
		char *vbuf,int vlen) noex {
	int		rs = SR_NOTFOUND ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (kp == nullptr) return SR_FAULT ;

	if (op->magval != PC_MAGIC) return SR_NOTOPEN ;
	if (curp->magval != PC_CURMAGIC) return SR_NOTOPEN ;

	if (op->fl.db) {
	    rs = var_fetch(&op->db,kp,kl,&curp->vcur,vbuf,vlen) ;
	}

	if ((rs < 0) && (vbuf != nullptr)) {
	    vbuf[0] = '\0' ;
	}

	return rs ;
} /* end subroutine (pcsconfs_fetch) */

int pcsconfs_curenum(PC *op,PC_CUR *curp,char *kbuf,int klen,
		char *vbuf,int vlen) noex {
	int		rs = SR_NOTFOUND ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (kbuf == nullptr) return SR_FAULT ;

	if (op->magval != PC_MAGIC) return SR_NOTOPEN ;
	if (curp->magval != PC_CURMAGIC) return SR_NOTOPEN ;

	if (op->fl.db) {
	    rs = var_curenum(&op->db,&curp->vcur,kbuf,klen,vbuf,vlen) ;
	}

	if ((rs < 0) && (vbuf != nullptr)) {
	    vbuf[0] = '\0' ;
	}

	return rs ;
} /* end subroutine (pcsconfs_curenum) */

int pcsconfs_count(PC *op) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != PC_MAGIC) return SR_NOTOPEN ;

	if (op->fl.db) {
	    rs = var_count(&op->db) ;
	}

	return rs ;
} /* end subroutine (pcsconfs_count) */


/* private subroutines */

local int pcsconfs_valsbegin(PC *op,cchar *pr,cchar *cfname) noex {
	int		rs ;
	int		sz = 0 ;
	sz += (lenstr(pr)+1) ;
	if (cfname) {
	    sz += (lenstr(cfname)+1) ;
	}
	if (char *bp ; (rs = lm_mall(sz,&bp)) >= 0) {
	    op->a = bp ;
	    op->pr = bp ;
	    bp = (strwcpy(bp,pr,-1)+1) ;
	    if (cfname != nullptr) {
		op->cfname = bp ;
	        bp = (strwcpy(bp,cfname,-1)+1) ;
	    }
	} /* end if (memory-allocation) */
	return rs ;
} /* end subroutine (pcsconfs_valsbegin) */

local int pcsconfs_valsend(PC *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->a != nullptr) {
	    rs1 = lm_free(op->a) ;
	    if (rs >= 0) rs = rs1 ;
	    op->a = nullptr ;
	}
	return rs ;
} /* end subroutine (pcsconfs_valsend) */

local int pcsconfs_dbcheck(PC *op) noex {
	CV	si, *sip = &si ;
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
} /* end subroutine (pcsconfs_dbcheck) */

local int pcsconfs_dbclose(PC *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fl.db) {
	    op->fl.db = false ;
	    rs1 = var_close(&op->db) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (pcsconfs_dbclose) */

local int pcsconfs_finout(PC *op) noex {
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
} /* end subroutine (pcsconfs_finout) */

local int pcsconfs_envv(PC *op,mainv ev) noex {
    	int	rs = SR_OK ;
	if ((op->envv = ev) == nullptr) {
	    if ((rs = u_getenviron(&ev)) >= 0) {
		op->envv = ev ;
	    }
	}
	return rs ;
} /* end subroutine (pcsconfs_envv) */

local int confvars_start(CV *sip,PC *op) noex {
	int		rs = SR_OK ;
	int		rs1 = SR_OK ;
	cchar		*cfname ;
	char		tmpfname[MAXPATHLEN+1] ;

	memclear(sip) ;
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
			if ((rs = lm_strw(pbuf,rs1,&cp)) >= 0) {
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
	        ustat	sb ;
	        rs1 = u_stat(cfname,&sb) ;
	        if ((rs1 >= 0) && S_ISREG(sb.st_mode)) {
	            sip->fl.conf = true ;
	            sip->cmtime = sb.st_mtime ;
	        }
	    }
	} /* end if */
	return rs ;
} /* end subroutine (confvars_start) */

local int confvars_finish(CV *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (sip->fl.cfname && (sip->cfname != nullptr)) {
	    sip->fl.cfname = false ;
	    void *vp = voidp(sip->cfname) ;
	    rs1 = lm_free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->cfname = nullptr ;
	}
	if (sip->fl.id) {
	    sip->fl.id = false ;
	    rs1 = ids_release(&sip->id) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (confvars_finish) */

local int confvars_loadsubs(CV *sip,vecstr *slp) noex {
	int		rs = SR_OK ;
	int		bl ;
	cchar		*bp ;

	bl = sfbasename(sip->pr,-1,&bp) ;
	if (bl <= 0) rs = SR_INVALID ;

	if (rs >= 0) {
	    rs = vecstr_envadd(slp,"p",sip->pr,-1) ;
	}
	if (rs >= 0) {
	    rs = vecstr_envadd(slp,"n",bp,bl) ;
	}
	return rs ;
} /* end subroutine (confvars_loadsubs) */

local int confvars_dbstart(CV *sip) noex {
	pcsconfs	*op = sip->op ;
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
} /* end subroutine (confvars_dbstart) */

local int confvars_confglobal(CV *sip,char *dname) noex {
	int		rs ;
	cmode		dm = 0777 ;
	cchar		*tmpdname = getenv(varTMPDNAME) ;
	cchar		*cdname = "pcsconf" ;

	if (sip == nullptr) return SR_FAULT ;

	if (tmpdname == nullptr) tmpdname = TMPDNAME ;

#if	CF_DEBUGS
	debugprintf("confvars_confglobal/prmktmpdir: pr=%s\n",sip->pr) ;
	debugprintf("confvars_confglobal/prmktmpdir: prconf=%s\n",sip->prconf) ;
#endif

	rs = prmktmpdir(sip->pr,dname,tmpdname,cdname,dm) ;

	return rs ;
} /* end subroutine (confvars_confglobal) */

local int confvars_conflocal(CV *sip,char *dname) noex {
	int		rs ;
	cmode		dm = 0775 ;
	cchar		*cdname = "pcsconf" ;

	if (sip == nullptr) return SR_FAULT ;

	rs = mktmpuserdir(dname,"-",cdname,dm) ;

	return rs ;
} /* end subroutine (confvars_conflocal) */

local int confvars_dbopen(CV *sip,cchar *dbname) noex {
	pcsconfs	*op = sip->op ;
	var		*vdp = &op->db ;
	int		rs ;

#if	CF_DEBUGS
	debugprintf("confvars_dbopen: dbname=%s\n",dbname) ;
#endif

	if ((rs = var_open(vdp,dbname)) >= 0) {
	    var_info	vi ;
	    op->fl.db = true ;
	    if ((rs = var_getinfo(vdp,&vi)) >= 0) {
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
} /* end subroutine (confvars_dbopen) */

local int confvars_dbclose(CV *sip) noex {
	pcsconfs	*op = sip->op ;
	var		*vdp = &op->db ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fl.db) {
	    op->fl.db = false ;
	    rs1 = var_close(vdp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (confvars_dbclose) */

local int confvars_dbmake(CV *sip,cchar *dbname) noex {
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
} /* end subroutine (confvars_dbmake) */

local int confvars_proc(CV *sip) noex {
	int		rs ;
	int		rs1 ;
	if (ustat sb ; (rs = u_stat(sip->cfname,&sb)) >= 0) {
	    paramfile	*pfp = &sip->pf ;
	    if ((rs = paramfile_open(pfp,sip->envv,sip->cfname)) >= 0) {
	        paramfile_cur	cur ;
	        paramfile_ent	pe ;
	        if ((rs = paramfile_curbegin(pfp,&cur)) >= 0) {
	            cint	plen = PARAMBUFLEN ;
	            int		kl ;
	            char	pbuf[PARAMBUFLEN+1] ;
	            while (rs >= 0) {
	                kl = paramfile_curenum(pfp,&cur,&pe,pbuf,plen) ;
	                if (kl == SR_NOTFOUND) break ;
	                rs = kl ;
	                if (rs < 0) break ;
	                rs = varmk_addvar(&sip->v,pe.key,pe.val,pe.vlen) ;
	            } /* end while (reading parameters) */
	            paramfile_curend(pfp,&cur) ;
	        } /* end if (cursor) */
	        rs1 = paramfile_close(pfp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (paramfile) */
	} /* end if (stat-file) */

	return rs ;
} /* end subroutine (confvars_proc) */

local int confvars_chown(CV *sip,char *dname,int dl) noex {
	cint		req = _PC_CHOWN_RESTRICTED ;
	int		rs = SR_OK ;
	int		rs1 ;

#ifdef	_PC_CHOWN_RESTRICTED
	if (char *dbuf ; (rs = lm_mn(&dbuf)) >= 0) {
	    cint dlen = rs ;
	if ((rs = u_pathconf(dname,req,nullptr)) == 0) {
	    if ((rs = confvars_ids(sip)) >= 0) {
		ids		*idp = &sip->id ;
		uid_t		u = sip->uid_pcs ;
		gid_t		g = sip->gid_pcs ;
	        if (fsdir d ; (rs = fsdir_open(&d,dname)) >= 0) {
	            fsdir_ent	e ;
		    int		naml ;
		    cchar	*namp ;
		    while ((rs = fsdir_read(&d,&e,dbuf,dlen)) > 0) {
			namp = e.name ;
			naml = rs ;
			if (hasNotDots(namp,naml)) {
			    if ((rs = pathadd(dname,dl,namp)) >= 0) {
		    	        ustat sb ;
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
	rs1 = lm_free(dbuf) ;
	if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
#endif /* _PC_CHOWN_RESTRICTED */

	return rs ;
} /* end subroutine (confvars_chown) */

local int confvars_ids(CV *sip) noex {
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
		} /* end if (error) */
	    } /* end if (loaded IDs) */
	} /* end if (needed IDs) */
	return rs ;
} /* end subroutine (confvars_ids) */


