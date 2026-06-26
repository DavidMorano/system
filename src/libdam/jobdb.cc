/* jobdb SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* perform various functions on a job */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debugging */

/* revision history:

	= 1998-09-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	jobdb

	Description:
	This module is responsible for providing means to store a
	job and the retrieve it later by its PID.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<sys/stat.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<estrings.h>		/* LIBUC */
#include	<mkdirs.h>		/* LIBUC */
#include	<rmdirfiles.h>		/* LIBUC */
#include	<mktmp.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG */

#include	"jobdb.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |memclear(3u)| */
import uconstants ;			/* |varname(3u)| */

/* local defines */

#ifndef	TMPDNAME
#define	TMPDNAME	"/tmp"
#endif

#ifndef	VARTMPDNAME
#define	VARTMPDNAME	"TMPDIR"
#endif

#define	INITSZ		10

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* compile-time debugging */
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		maxpathlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

template<typename ... Args>
local inline int jobdb_ctor(jobdb *op,Args ... args) noex {
    	JOBDB		*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    op->tmpdname = nullptr ;
	    if ((op->dbp = new(nt) vecitem) != np) ylikely {
		rs = SR_OK ;
	    } /* end if (new-vecitem) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (jobdb_ctor) */

local int jobdb_dtor(jobdb *op) noex {
	int		rs = SR_OK ;
	if (op->dbp) ylikely {
	    delete op->dbp ;
	    op->dbp = nullptr ;
	} /* end if (memory-release) */
	return rs ;
} /* end subroutine (jobdb_dtor) */

local int	jobdb_delit(jobdb *,int,jobdb_ent *) noex ;
local int	jobdb_checkdir(jobdb *) noex ;

local int	entry_start(jobdb_ent *,cchar *,cchar *,int) noex ;
local int	entry_finish(jobdb_ent *) noex ;

local int	mkfile(cchar *,cchar **) noex ;


/* local variables */

static vars		var ;
cbool			f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int jobdb_start(jobdb *op,int initsz,cchar *tmpdname) noex {
	int		rs ;
	if (initsz < 10) initsz = 10 ;
	if (tmpdname == nullptr) tmpdname = getenver(varname.tmpdir) ;
	if (tmpdname == nullptr) tmpdname = sysword.w_tmpdir ;
	if ((rs = jobdb_ctor(op)) >= 0) {
	    if (static cint rsv = var ; (rs = rsv) >= 0) {
	        if (cchar *cp ; (rs = mem.strw(tmpdname,-1,&cp)) >= 0) {
	            cint	vo = (vecitemm.reuse | vecitemm.conserve) ;
	            op->tmpdname = cp ;
	            rs = vecitem_start(op->dbp,initsz,vo) ;
	            if (rs < 0) {
	                voidp vp = voidp(op->tmpdname) ;
	                mem.free(vp) ;
	                op->tmpdname = nullptr ;
	            } /* end if (error) */
	        } /* end if (memory-acquire) */
	    } /* end if (vars) */
	    if (rs < 0) {
		jobdb_dtor(op) ;
	    } /* end if (error) */
	} /* end if (jobdb_ctor) */
	return rs ;
} /* end subroutine (jobdb_start) */

int jobdb_finish(jobdb *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	DEBUGPRINTF("ent\n") ;
	if (op) {
	    rs = SR_OK ;
	    if (vecitem *dbp = op->dbp ; dbp) {
	        void *vp ; 
	        for (int i = 0 ; dbp->get(i,&vp) >= 0 ; i += 1) {
	            jobdb_ent	*jep = resumelife<jobdb_ent>(vp) ;
	            if (vp) {
	                rs1 = entry_finish(jep) ;
	                if (rs >= 0) rs = rs1 ;
		        c += 1 ;
	            }
	        } /* end for */
	        {
	            rs1 = dbp->finish ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    } /* end if (non-null) */
	    if (op->tmpdname) {
	        voidp vp = voidp(op->tmpdname) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->tmpdname = nullptr ;
	    } /* end if (memory-release) */
	    {
	        rs1 = jobdb_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	DEBUGPRINTF("ret rs=%d c=%u\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (jobdb_finish) */

local int jobdb_newjobs(jobdb *,cchar *,int) noex ;

int jobdb_newjob(jobdb *op,cchar *jobid,int f_so) noex {
	int		rs = SR_FAULT ;
	DEBUGPRINTF("ent f_so=%u\n",f_so) ;
	if (op) {
	    if ((rs = jobdb_checkdir(op)) >= 0) {
		rs = jobdb_newjobs(op,jobid,f_so) ;
	    } /* end if (jobdb_checkdir) */
	} /* end if (non-null) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (jobdb_newjob) */

local int jobdb_newjobs(jobdb *op,cchar *jobid,int f_so) noex {
    	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	if (vecitem *dbp = op->dbp ; dbp) {
	    if (char *cbuf ; (rs = mem.mn(&cbuf)) >= 0) {
	        cint	clen = rs ;
	        cchar	pre[] = JOBDB_JOBPREFIX ;
	        cchar	x[] = "XXXXXXXXX" ;
	        if ((rs = sncpy(cbuf,clen,pre,x)) >= 0) {
		    if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) {
	                if ((rs = mkpath(tbuf,op->tmpdname,cbuf)) >= 0) {
	                    jobdb_ent	je ;
	                    if ((rs = entry_start(&je,tbuf,jobid,f_so)) >= 0) {
	                        cint esz = szof(jobdb_ent) ;
	                        rs = dbp->add(&je,esz) ;
	                        if (rs < 0) {
		                    entry_finish(&je) ;
			        } /* end if (error) */
		            } /* end if (entry_start) */
	                } /* end if (mkpath) */
		        rs1 = mem.free(tbuf) ;
		        if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
	        } /* end if (sncpy2) */
	        rs1 = mem.free(cbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (bug-check) */
	return rs ;
} /* end subroutine (jobdb_newjobs) */

/* search the job table for a PID match */
int jobdb_findpid(jobdb *op,pid_t pid,jobdb_ent **jepp) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ; /* return-value */
	if (op && jepp) {
	    rs = SR_BUGCHECK ;
	    if (vecitem *dbp = op->dbp ; dbp) {
		void *vp ;
		for (i = 0 ; (rs = dbp->get(i,&vp)) >= 0 ; i += 1) {
		    jobdb_ent *jap = resumelife<jobdb_ent>(vp) ;
		    if (vp) {
	      	        if (jap->pid == pid) {
			    *jepp = jap ;
			    break ;
			}
	    	    }
		} /* end for */
		if (rs < 0) {
	    	    *jepp = nullptr ;
		} /* end if (error) */
	    } /* end if (bug-check) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
} /* end subroutine (jobdb_findpid) */

/* search for a job in the job table via its filename */
int jobdb_search(jobdb *op,cchar *fname,jobdb_ent **jepp) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ; /* return-value */
	if (op && fname && jepp) {
	    rs = SR_BUGCHECK ;
	    if (vecitem *dbp = op->dbp ; dbp) {
		void *vp ;
	        for (i = 0 ; (rs = dbp->get(i,&vp)) >= 0 ; i += 1) {
		    jobdb_ent *jap = resumelife<jobdb_ent>(vp) ;
	            if (vp) {
	                if (strcmp(jap->efname,fname) == 0) {
			    *jepp = jap ;
			    break ;
			}
	            }
	        } /* end for */
	        if (rs < 0) {
	            *jepp = nullptr ;
	        } /* end if (error) */
	    } /* end if (bug-check) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
} /* end subroutine (jobdb_search) */

/* enumerate all of the jobs */
int jobdb_get(jobdb *op,int i,jobdb_ent **jepp) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_BUGCHECK ;
	    if (vecitem *dbp = op->dbp ; dbp) {
	        rs = dbp->get(i,jepp) ;
	    } /* end if (bug-check) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (jobdb_get) */

/* get a job by its structure pointer */
int jobdb_getent(jobdb *op,jobdb_ent *jep) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ; /* return-value */
	if (op) {
	    rs = SR_BUGCHECK ;
	    if (vecitem *dbp = op->dbp ; dbp) {
		void *vp ;
	        for (i = 0 ; (rs = dbp->get(i,&vp)) >= 0 ; i += 1) {
	            jobdb_ent *jap = resumelife<jobdb_ent>(vp) ;
	            if (vp) {
	                if (jap == jep) break ;
	            }
	        } /* end for */
	    } /* end if (bug-check) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
} /* end subroutine (jobdb_getent) */

/* delete a job by index */
int jobdb_del(jobdb *op,int i) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	DEBUGPRINTF("ent i=%i\n",i) ;
	if (op) {
	    rs = SR_BUGCHECK ;
	    if (vecitem *dbp = op->dbp ; dbp) {
		void *vp ;
		if ((rs = dbp->get(i,&vp)) >= 0) {
		    jobdb_ent *jap = resumelife<jobdb_ent>(vp) ;
	    	    if (vp) {
	        	rs1 = jobdb_delit(op,i,jap) ;
			if (rs >= 0) rs = rs1 ;
		    }
	        } /* end if (found entry) */
	    } /* end if (bug-check) */
	} /* end if (non-null) */
	if_constexpr (f_debug) {
	    cint c = vecitem_count(op->dbp) ;
	    DEBUGPRINTF("ret rs=%d c=%u\n",rs,c) ;
	} /* end if_constexpr (f_debug) */
	return rs ;
} /* end subroutine (jobdb_del) */

/* delete a job by entry (pointer) */
int jobdb_delent(jobdb *op,jobdb_ent *jep) noex {
	int		rs = SR_FAULT ;
	int		i ; /* return-value */
	DEBUGPRINTF("ent\n") ;
	if (op && jep) {
	    rs = SR_BUGCHECK ;
	    if (vecitem *dbp = op->dbp ; dbp) {
	        void *vp ;
	        for (i = 0 ; (rs = dbp->get(i,&vp)) >= 0 ; i += 1) {
	            jobdb_ent *jap = resumelife<jobdb_ent>(vp) ;
	            if (vp) {
	                if (jap == jep) {
	                    rs = jobdb_delit(op,i,jep) ;
		            break ;
	                }
	            }
	        } /* end for */
	    } /* end if (bug-check) */
	} /* end if (non-null) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return (rs >= 0) ? i : rs ;
} /* end subroutine (jobdb_delent) */

int jobdb_count(jobdb *op) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    rs = vecitem_count(op->dbp) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (jobdb_count) */

int jobdb_check(jobdb *op,time_t dt,int to) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if ((dt - op->ti_jobdir) >= to) {
	        cint	jto = JOBDB_JOBFILETO ;
	        cchar	*pref = JOBDB_JOBPREFIX ;
	        op->ti_jobdir = dt ;
	        rs = rmdirfiles(op->tmpdname,pref,jto) ;
	    } /* end if (time-out) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (jobdb_check) */


/* private subroutines */

/* delete stuff associated with this job */
local int jobdb_delit(jobdb *op,int i,jobdb_ent *jep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	DEBUGPRINTF("ent i=%u\n",i) ;
	if (op) {
	    rs = SR_OK ;
	    {
		rs1 = entry_finish(jep) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = vecitem_del(op->dbp,i) ;
		if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (jobdb_delit) */

/* check if the spool directory is present */
local int jobdb_checkdir(jobdb *op) noex {
	cint		rsn = SR_NOENT ;
	int		rs ;
	if (ustat sb ; (rs = u_stat(op->tmpdname,&sb)) == rsn) {
	    cmode	dm = (0777 | S_ISVTX) ;
	    if ((rs = mkdirs(op->tmpdname,dm)) >= 0) {
		rs = chmod(op->tmpdname,dm) ;
	    }
	} /* end if (needed to create) */
	return rs ;
} /* end subroutine (jobdb_checkdir) */

local int entry_start(jobdb_ent *jep,cchar *tpat,cchar *jobid,int f_so) noex {
	custime		dt = getustime ;
	int		rs ;
	memclear(jep) ;
	jep->pid = -1 ;
	jep->atime = dt ;
	jep->stime = dt ;
	strwcpy(jep->jobid,jobid,JOBDB_JOBIDLEN) ;
	if (cchar *cp ; (rs = mkfile(tpat,&cp)) >= 0) {
	    jep->efname = charp(cp) ;
	    if (f_so) {
	        rs = mkfile(tpat,&cp) ;
	        jep->ofname = (char *) cp ;
	    }
	    if (rs < 0) {
	        if (jep->efname) {
	            if (jep->efname[0] != '\0') {
		        u_unlink(jep->efname) ;
		        jep->efname[0] = '\0' ;
		    }
	            mem.free(jep->efname) ;
	            jep->efname = nullptr ;
	        } /* end if (memory-release) */
	    } /* end if (error) */
	} /* end if (mkfile) */
	return rs ;
} /* end subroutine (entry_start) */

local int entry_finish(jobdb_ent *jep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	DEBUGPRINTF("ent jodb=%s\n",jep->jobid) ;
	if (jep->ofname) {
	    if (jep->ofname[0] != '\0') {
		u_unlink(jep->ofname) ;
		jep->ofname[0] = '\0' ;
	    }
	    rs1 = mem.free(jep->ofname) ;
	    if (rs >= 0) rs = rs1 ;
	    jep->ofname = nullptr ;
	} /* end if (memory-release) */
	if (jep->efname) {
	    if (jep->efname[0] != '\0') {
		u_unlink(jep->efname) ;
		jep->efname[0] = '\0' ;
	    }
	    rs1 = mem.free(jep->efname) ;
	    if (rs >= 0) rs = rs1 ;
	    jep->efname = nullptr ;
	} /* end if (memory-release) */
	jep->pid = -1 ;
	jep->jobid[0] = '\0' ;
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (entry_finish) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_mp)) >= 0) {
	    maxpathlen = rs ;
	} /* end if (bufsizeget) */
    	return rs ;
} /* end method (vars::operator) */

local int mkfile(cchar *tpat,cchar **rpp) noex {
	int		rs ;
	int		rs1 ;
	int		tl = 0 ; /* return-value */
	if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) {
	    cmode fm = 0666 ;
	    if ((rs = mktmpfile(tbuf,tpat,fm)) >= 0) {
	        tl = rs ;
	        rs = mem.strw(tbuf,tl,rpp) ;
	        if (rs < 0) {
	            u_unlink(tbuf) ;
		    *rpp = nullptr ;
	        } /* end if (error) */
	    } /* end if (mktmpfile) */
	    rs1 = mem.free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? tl : rs ;
} /* end subroutine (mkfile) */


