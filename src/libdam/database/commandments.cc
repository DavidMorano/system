/* commandments SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* COMMANDMENTS object implementation */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#define	CF_DEBUGLINE	0		/* special debug-line subroutine */
#define	CF_EMPTYTERM	0		/* terminate entry on empty line */

/* revision history:

	= 2008-03-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	commandments

	Description:
	This module implements an interface (a trivial one) that
	allows access to the COMMANDMENTS datbase.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>		/* POSIX */
#include	<sys/stat.h>		/* POSIX */
#include	<sys/mman.h>		/* POSIX */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<new>			/* C++STD placement-new */
#include	<memory>		/* C++STD |destroy_at(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<nulstr.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<opentmp.h>		/* LIBUC */
#include	<getx.h>		/* LIBUC */
#include	<getxname.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<removes.h>		/* LIBUC */
#include	<cfdec.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<mkx.h>			/* LIBUC */
#include	<mkdirs.h>		/* LIBUC */
#include	<path.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<nulstr.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<isoneof.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<ischarx.h>		/* LIBUC */
#include	<hasx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libpr.h>		/* LIBPR |prmktmpdir(3pr)| */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */
#include	<cmimk.h>		/* LIBDAM */
#include	<cmi.h>			/* LIBDAM */

#include	"commandments.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |memclear(3u)| */
import uconstants ;			/* |sysword(3u)| */

/* local defines */

#define	CMS_SN		"commandments"
#define	CMS_DBDNAME	"share/commandments"
#define	CMS_TMPDNAME	sysword.w_vartmpdir
#define	CMS_MAGIC	COMMANDMENTS_MAGIC
#define	CMS_DBNAME	COMMANDMENTS_DBNAME
#define	CMS_NLINES	40

#define	CMS		commandments
#define	CMS_CUR		commandments_cur
#define	CMS_ENT		commandments_ent

#define	TO_CHECK	4

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#endif
#ifndef	CF_EMPTYTERM
#define	CF_EMPTYTERM	0		/* terminate entry on empty line */
#endif


/* imported namespaces */

using std::destroy_at ;			/* subroutine */
using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct mkent_line {
	uint		loff ;
	uint		llen ;
} ; /* end struct */

struct mkent {
	mkent_line	*lines ;
	uint		eoff ;
	uint		elen ;
	int		i ;
	int		e ;
	ushort		cn ;
} ; /* end struct */

namespace {
    struct vars {
	int	maxpathlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

template<typename ... Args>
local inline int commandments_ctor(commandments *op,Args ... args) noex {
    	COMMANDMENTS	*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->idxp = new(nt) cmi) != np) ylikely {
	        rs = SR_OK ;
	    } /* end if (new-cmi) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (commandments_ctor) */

local int commandments_dtor(commandments *op) noex {
	int		rs = SR_OK ;
	if (op->idxp) ylikely {
	    delete op->idxp ;
	    op->idxp = nullptr ;
	} /* end if (delete-cmi) */
	return rs ;
} /* end subroutine (commandments_dtor) */

template<typename ... Args>
local inline int commandments_magic(commandments *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == COMMANDMENTS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (commandments_magic) */

local int	commandments_argsbegin(CMS *,cchar *,cchar *) noex ;
local int	commandments_argsend(CMS *) noex ;

local int	commandments_findbegin(CMS *,cchar *,cchar *) noex ;
local int	commandments_findend(CMS *) noex ;
local int	commandments_tmpcheck(CMS *,char *,ustat *,cchar *) noex ;
local int	commandments_tmpcopy(CMS *,char *,cchar *,char *) noex ;

local int	commandments_fileloadbegin(CMS *,cchar *) noex ;
local int	commandments_fileloadend(CMS *) noex ;
local int	commandments_dbmapbegin(CMS *,time_t) noex ;
local int	commandments_dbmapend(CMS *) noex ;
local int	commandments_dbproc(CMS *,cmimk *) noex ;
local int	commandments_checkupdate(CMS *,time_t) noex ;
local int	commandments_loadbuf(CMS *,cmi_ent *,char *,int rlen) noex ;

local int	commandments_userhome(CMS *) noex ;
local int	commandments_usridname(CMS *,char *) noex ;
local int	commandments_sysidname(CMS *,char *) noex ;

local int	commandments_idxbegin(CMS *,cchar *) noex ;
local int	commandments_idxend(CMS *) noex ;
local int	commandments_idxmkname(CMS *,char *,cchar *) noex ;
local int	commandments_idxopencheck(CMS *,cchar *) noex ;
local int	commandments_idxmk(CMS *,cchar *) noex ;
local int	commandments_idxmapbegin(CMS *,cchar *) noex ;
local int	commandments_idxmapend(CMS *) noex ;
local int	commandments_chownpr(CMS *,cchar *) noex ;
local int	commandments_ids(CMS *) noex ;

local int	mkent_start(mkent *,int,uint,uint) noex ;
local int	mkent_add(mkent *,uint,uint) noex ;
local int	mkent_finish(mkent *) noex ;

local int	cmimkent_start	(cmimk_ent *,mkent *) noex ;
local int	cmimkent_finish	(cmimk_ent *) noex ;

local bool	hasourdig(cchar *,int) noex ;

local bool	isempty(cchar *,int) noex ;
local bool	isstart(cchar *,int,int *,int *) noex ;

local bool	isNotOurAccess(int) noex ;
local bool	isStale(int) noex ;


/* local variables */

constexpr int		rsold[] = {
	SR_STALE,
	0
} ; /* end array */

static vars		var ;
constexpr uid_t		uidend		= uid_t(-1) ;
cbool			f_debug		= CF_DEBUG ;
cbool			f_emptyterm	= CF_EMPTYTERM ;


/* exported variables */

const commandments_obj	commandments_modinfo = {
	"commandments",
	szof(commandments),
	szof(commandments_cur)
} ; /* end initialization */


/* exported variables */


/* exported subroutines */

local int commandments_opens(CMS *op,cchar *pr,cchar *dbn) noex ;

int commandments_open(CMS *op,cchar *pr,cchar *dbn) noex {
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((dbn == nullptr) || (dbn[0] == '\0')) {
	    dbn = CMS_DBNAME ;
	}
	if ((rs = commandments_ctor(op,pr)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (pr[0]) ylikely {
		if (static cint rsv = var ; (rs = rsv) >= 0) ylikely {
	            op->uid = -1 ;
	            op->uid_pr = -1 ;
	            op->gid_pr = -1 ;
		    rs = commandments_opens(op,pr,dbn) ;
		    c = rs ;
		} /* end if (vars) */
	    } /* end if (valid) */
	    if (rs < 0) {
	        commandments_dtor(op) ;
	    } /* end if (error) */
	} /* end if (ctor) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (commandments_open) */

local int commandments_opens(CMS *op,cchar *pr,cchar *dbn) noex {
    	int		rs ;
	int		c = 0 ; /* return-value */
        if ((rs = commandments_argsbegin(op,pr,dbn)) >= 0) ylikely {
            if ((rs = commandments_findbegin(op,pr,dbn)) >= 0) ylikely {
                if ((rs = commandments_fileloadbegin(op,dbn)) >= 0) ylikely {
                    c = op->nents ;
                    op->magval = CMS_MAGIC ;
                } /* end if (commandments_fileloadbegin) */
                if (rs < 0) {
                    commandments_findend(op) ;
                } /* end if (error) */
            } /* end if (commandments_findbegin) */
            if (rs < 0) {
                commandments_argsend(op) ;
            } /* end if (error) */
        } /* end if (commandments_argsbegin) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (commandments_opens) */

int commandments_close(CMS *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = commandments_magic(op)) >= 0) ylikely {
	    {
	        rs1 = commandments_fileloadend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->fname) {
		void *vp = voidp(op->fname) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fname = nullptr ;
	    } /* end if (memory-release) */
	    if (op->uhome) {
		void *vp = voidp(op->uhome) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->uhome = nullptr ;
	    } /* end if (memory-release) */
	    {
	        rs1 = commandments_argsend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = commandments_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (commandments_close) */

int commandments_audit(CMS *op) noex {
	int		rs ;
	if ((rs = commandments_magic(op)) >= 0) ylikely {
	    if (op->fl.idx) {
	        rs = cmi_audit(op->idxp) ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (commandments_audit) */

int commandments_count(CMS *op) noex {
	int		rs ;
	if ((rs = commandments_magic(op)) >= 0) ylikely {
	    if (op->fl.idx) {
	        cmi	*cip = op->idxp ;
	        rs = cmi_count(cip) ;
	    } else {
	        rs = SR_BUGCHECK ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (commandments_count) */

int commandments_max(CMS *op) noex {
    	int		rs ;
	if ((rs = commandments_magic(op)) >= 0) ylikely {
	    rs = op->maxent ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (commandments_max) */

int commandments_read(CMS *op,char *vbuf,int vlen,uint cn) noex {
	int		rs ;
	int		len = 0 ; /* return-value */
	if ((rs = commandments_magic(op,vbuf)) >= 0) ylikely {
	    cmi_line	lines[CMS_NLINES+1] ;
	    time_t	dt = 0 ;
	    cint	nlines = CMS_NLINES ;
	    /* check for update */
	    if (op->ncursors == 0) {
	        rs = commandments_checkupdate(op,dt) ;
	    }
	    DEBUGPRINTF("mid1 rs=%u\n",rs) ;
	    if (rs >= 0) ylikely {
	        cmi_ent	viv{} ;
	        cint	lsz = ((nlines + 1) * szof(cmi_line)) ;
	        char	*lb = charp(lines) ;
	        if ((rs = cmi_read(op->idxp,&viv,lb,lsz,cn)) >= 0) ylikely {
	            if (vbuf) {
	                rs = commandments_loadbuf(op,&viv,vbuf,vlen) ;
		        len = rs ;
		    }
	        } /* end if (cmi_read) */
	    } /* end if (ok) */
	} /* end if (magic) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return (rs >= 0) ? len : rs ;
} /* end subroutine (commandments_read) */

int commandments_curbegin(CMS *op,CMS_CUR *curp) noex {
	int		rs ;
	if ((rs = commandments_magic(op,curp)) >= 0) ylikely {
	    cint csz = szof(cmi_cur) ;
	    if (void *vp ; (rs = mem.mall(csz,&vp)) >= 0) ylikely {
		if (curp->vicurp = new(vp) cmi_cur ; curp->vicurp) ylikely {
	            if ((rs = cmi_curbegin(op->idxp,curp->vicurp)) >= 0) {
	                op->ncursors += 1 ;
	            } /* end if (cmi_curbegin) */
		    if (rs < 0) {
			destroy_at(curp->vicurp) ;
		    } /* end if (error) */
		} /* end if (placement-new) */
		if (rs < 0) {
		    mem.free(vp) ;
		} /* end if (error) */
	    } /* end if (memory-acquire) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (commandments_curbegin) */

int commandments_curend(CMS *op,CMS_CUR *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = commandments_magic(op,curp)) >= 0) ylikely {
	    rs = SR_FAULT ;
	    if (curp->vicurp) {
		cmi_cur	*vicurp = curp->vicurp ;
		rs = SR_OK ;
		{
		    rs1 = cmi_curend(op->idxp,vicurp) ;
		    if (rs >= 0) rs = rs1 ;
		}
		if (op->ncursors > 0) {
	    	    op->ncursors -= 1 ;
		} /* end if */
		{
		    destroy_at(vicurp) ;
		}
		{
		    rs1 = mem.free(vicurp) ;
		    if (rs >= 0) rs = rs1 ;
		    curp->vicurp = nullptr ;
		} /* end if (memory-release) */
	    } /* end if (non-null) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (commandments_curend) */

int commandments_curenum(CMS *op,CMS_CUR *curp,CMS_ENT *ep,
		char *vbuf,int vlen) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = commandments_magic(op,curp,ep,vbuf)) >= 0) ylikely {
	    time_t	dt = 0 ;
	    /* check for update */
	    if (op->ncursors == 0) {
	        rs = commandments_checkupdate(op,dt) ;
	    }
	    if (rs >= 0) ylikely {
	        cmi_cur		*bcurp = curp->vicurp ;
	        cmi_ent		viv ;
	        cmi_line	lines[CMS_NLINES + 1] ;
	        cint		ls = ((CMS_NLINES + 1) * szof(cmi_line)) ;
		char		*lbuf = charp(lines) ;
	        if ((rs = cmi_curenum(op->idxp,bcurp,&viv,lbuf,ls)) >= 0) {
	            if (vbuf) {
	                rs = commandments_loadbuf(op,&viv,vbuf,vlen) ;
		        len = rs ;
		    }
	            if ((rs >= 0) && ep) {
		        memclear(ep) ;
		        ep->cn = viv.cn ;
	            } /* end if */
	        } /* end if (cmi_xurenum) */
	    } /* end if (ok) */
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (commandments_curenum) */

int commandments_get(CMS *op,int cn,char *rbuf,int rlen) noex {
	return commandments_read(op,rbuf,rlen,cn) ;
} /* end subroutine (commandments_get) */


/* private subroutines */

local int commandments_argsbegin(CMS *op,cchar *pr,cchar *dbname) noex {
	int		rs ;
	int		sz = 0 ;
	sz += (lenstr(pr)+1) ;
	sz += (lenstr(dbname)+1) ;
	if (char *bp ; (rs = mem.mall(sz,&bp)) >= 0) {
	    op->a = bp ;
	    op->pr = bp ;
	    bp = (strwcpy(bp,pr,-1)+1) ;
	    op->dbname = bp ;
	    bp = (strwcpy(bp,dbname,-1)+1) ;
	} /* end if (memory-acquire) */
	return rs ;
} /* end subroutine (commandments_argsbegin) */

local int commandments_argsend(CMS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->a) {
	    rs1 = mem.free(op->a) ;
	    if (rs >= 0) rs = rs1 ;
	    op->a = nullptr ;
	} /* end if (memory-release) */
	return rs ;
} /* end subroutine (commandments_argsend) */

local int commandments_findbegin(CMS *op,cchar *pr,cchar *dbname) noex {
	int		rs ;
	int		rs1 ;
	DEBUGPRINTF("ent db=%s\n",dbname) ;
	if ((rs = commandments_userhome(op)) >= 0) ylikely {
	    cchar	*cname = CMS_DBDNAME ;
	    if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) ylikely {
	        if ((rs = mkpath(tbuf,op->uhome,cname,dbname)) >= 0) ylikely {
		    int	tl = rs ;
	            if (ustat sb ; (rs = uc_stat(tbuf,&sb)) >= 0) {
		        if (S_ISREG(sb.st_mode)) {
			    op->fl.user		= true ;
			    op->ti_db		= sb.st_mtime ;
			    op->db_size		= size_t(sb.st_size) ;
		        } else {
			    rs = SR_ISDIR ;
		        }
		    } else if (isNotOurAccess(rs)) {
	   	        if ((rs = mkpath(tbuf,pr,cname,dbname)) >= 0) {
			    tl = rs ;
			    if ((rs = uc_stat(tbuf,&sb)) >= 0) {
		                if (S_ISREG(sb.st_mode)) {
				    cauto c_tc = commandments_tmpcheck ;
				    op->ti_db = sb.st_mtime ;
				    op->db_size = sb.st_size ;
				    rs = c_tc(op,tbuf,&sb,dbname) ;
				    tl = rs ;
			        } else {
				    rs = SR_ISDIR ;
			        }
			    } /* end if (uc_stat) */
		        } /* end if (mkpath) */
		    } /* end if (user or system) */
		    if (rs >= 0) ylikely {
		        if (cchar *cp ; (rs = mem.strw(tbuf,tl,&cp)) >= 0) {
		            op->fname = cp ;
		        } /* end if (memory-acquire) */
		    } /* end if (ok) */
	        } /* end if (mkpath) */
	        rs1 = mem.free(tbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (commandments_userhome) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	DEBUGPRINTF("ret fname=%s\n",op->fname) ;
	return rs ;
} /* end subroutine (commandments_findbegin) */

local int commandments_findend(CMS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fname) {
	    void *vp = voidp(op->fname) ;
	    rs1 = mem.free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fname = nullptr ;
	} /* end if (memory-release) */
	return rs ;
} /* end subroutine (commandments_findend) */

local int commandments_tmpcheck(CMS *op,char *tbuf,ustat *sbp,cc *dbn) noex {
	cint		plen = var.maxpathlen ;
	int		rs ;
	int		rs1 ;
	int		sz = 0 ;
	int		ai = 2 ; /* double path-len allocation */
	cchar		*vtmp = CMS_TMPDNAME ;
	cchar		*sn = CMS_SN ;
	cmode		dm = 0777 ;
	sz += (plen + 1) ;
	sz += (plen + 1) ;
	if (char *a ; (rs = mem.mall(sz,&a)) >= 0) ylikely {
	    char	*abuf = (a + (--ai * (plen + 1))) ;
	    char	*dbuf = (a + (--ai * (plen + 1))) ;
	    if ((rs = prmktmpdir(op->pr,dbuf,vtmp,sn,dm)) >= 0) ylikely {
	        if ((rs = mkpath(abuf,dbuf,dbn)) >= 0) ylikely {
		    if (ustat tsb ; (rs = uc_stat(abuf,&tsb)) >= 0) ylikely {
			if (S_ISREG(tsb.st_mode)) {
			    int	f = false ;
			    f = f || (sbp->st_mtime > tsb.st_mtime) ;
			    f = f || (sbp->st_size != tsb.st_size) ;
			    if (f) {
				rs = commandments_tmpcopy(op,tbuf,abuf,dbuf) ;
			    }
			} else {
			    if ((rs = removes(abuf)) >= 0) {
				rs = commandments_tmpcopy(op,tbuf,abuf,dbuf) ;
			    }
			}
		    } else if (isNotPresent(rs)) {
			rs = commandments_tmpcopy(op,tbuf,abuf,dbuf) ;
		    } /* end if (uc_stat) */
	        } /* end if (mkpath) */
	    } /* end if (prmktmpdir) */
	    rs1 = mem.free(a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (commandments_tmpcheck) */

local int commandments_tmpcopy(CMS *op,char *tbuf,
		cchar *abuf, char *dbuf) noex {
	int		rs ;
	int		rs1 ;
	if (char *xbuf ; (rs = mem.mp(&xbuf)) >= 0) ylikely {
	    cint	dlen = lenstr(dbuf) ;
	    cchar	*ft = "cmdXXXXXXXXXXX" ;
	    if ((rs = pathadd(dbuf,dlen,ft)) >= 0) ylikely {
		cmode	om = 0664 ;
		cint	of = O_RDWR ;
	        if ((rs = opentmpfile(dbuf,of,om,xbuf)) >= 0) ylikely {
		    cint	fd = rs ;
		    rs = uc_writefile(fd,tbuf) ;
		    if ((rs < 0) && (xbuf[0] != '\0')) {
			u_unlink(xbuf) ;
		    } /* end if (error) */
		    u_close(fd) ;
		} /* end if (open-file) */
		if (rs >= 0) ylikely {
		    UTIMBUF ut{} ;
		    ut.actime = op->ti_db ;
		    ut.modtime = op->ti_db ;
		    if ((rs = uc_filetime(xbuf,&ut)) >= 0) {
			if ((rs = u_rename(xbuf,abuf)) >= 0) {
			    xbuf[0] = '\0' ;
			    rs = mkpath1(tbuf,abuf) ;
			}
		    } /* end if (uc_filetime) */
		} /* end if (ok) */
		dbuf[dlen] = '\0' ;
	    } /* end if (pathadd) */
	    rs1 = mem.free(xbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (commandments_tmpcopy) */

local int commandments_fileloadbegin(CMS *op,cchar *dbname) noex {
	custime		dt = getustime ;
	int		rs ;
	int		c = 0 ;
	DEBUGPRINTF("ent dbname=%s\n",dbname) ;
	if ((rs = commandments_dbmapbegin(op,dt)) >= 0) ylikely {
	    rs = commandments_idxbegin(op,dbname) ;
	    c = rs ;
	    if (rs < 0) {
	        commandments_dbmapend(op) ;
	    } /* end if (error) */
	} /* end if */
	DEBUGPRINTF("ret rs=%d c=%u\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (commandments_fileloadbegin) */

local int commandments_fileloadend(CMS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = commandments_idxend(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = commandments_dbmapend(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (commandments_fileloadend) */

local int commandments_idxbegin(CMS *op,cchar *dbname) noex {
	int		rs ;
	int		rs1 ;
	DEBUGPRINTF("ent dbname=%s\n",dbname) ;
	if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) {
	    if ((rs = commandments_idxmkname(op,tbuf,dbname)) >= 0) ylikely {
	        DEBUGPRINTF("tbuf=%s\n",tbuf) ;
	        rs = commandments_idxmapbegin(op,tbuf) ;
	    } /* end if (commandments_idxmkname) */
	    rs1 = mem.free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (commandments_idxbegin) */

local int commandments_idxend(CMS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = commandments_idxmapend(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (commandments_idxend) */

local int commandments_idxmkname(CMS *op,char *tbuf,cchar *dbname) noex {
	int		rs = SR_OK ;
	int		pl = 0 ;
	DEBUGPRINTF("ent dbname=%s\n",dbname) ;
	DEBUGPRINTF("f_user=%u\n",op->fl.user) ;
	tbuf[0] = '\0' ;
	    if (op->fl.user) {
	        if ((rs = commandments_userhome(op)) >= 0) {
		    if ((rs = commandments_usridname(op,tbuf)) >= 0) {
		        rs = pathadd(tbuf,rs,dbname) ;
			pl = rs ;
		    }
	        }
	    } else {
		if ((rs = commandments_sysidname(op,tbuf)) >= 0) {
		    rs = pathadd(tbuf,rs,dbname) ;
		    pl = rs ;
		}
	    } /* end if */
	DEBUGPRINTF("ret rs=%d pl=%u\n",rs,pl) ;
	DEBUGPRINTF("ret tbuf=%s\n",tbuf) ;
	return (rs >= 0) ? pl : rs ;
} /* end subroutine (commandments_idxmkname) */

local int commandments_idxopencheck(CMS *op,cchar *dbname) noex {
	cmi		*cip = op->idxp ;
	int		rs ;
	int		nents = 0 ;
	DEBUGPRINTF("ent dbname=%s\n",dbname) ;
	if ((rs = cmi_open(cip,dbname)) >= 0) ylikely {
	    DEBUGPRINTF("cmi_open() rs=%d\n",rs) ;
	    if (cmi_info cinfo ; (rs = cmi_getinfo(cip,&cinfo)) >= 0) ylikely {
		cuint	dbsz = conv<uint>(op->db_size) ;
		bool	f = true ;
	        f = f && (cinfo.idxctime >= op->ti_db) ;
	        f = f && (cinfo.dbsz == dbsz) ;
		if (f) {
	            op->fl.idx = true ;
		    op->nents = cinfo.nents ;
		    op->maxent = cinfo.maxent ;
		    op->ti_idx = cinfo.idxmtime ;
		    nents = cinfo.nents ;
		} else {
	            rs = SR_STALE ;
	        }
	        if (rs < 0) {
	            cmi_close(cip) ;
		} /* end if (error) */
	    } /* end if (cmi_getinfo) */
	} /* end if (cmi_open) */
	DEBUGPRINTF("ret rs=%d n=%u\n", rs,nents) ;
	return (rs >= 0) ? nents : rs ;
} /* end subroutine (commandments_idxopencheck) */

local int commandments_idxmk(CMS *op,cchar *tbuf) noex {
	cint		of = 0 ;
	int		rs ;
	int		rs1 ;
	cmode		om = 0664 ;
	DEBUGPRINTF("ent tbuf=%s\n",tbuf) ;
	if (cmimk mk ; (rs = cmimk_open(&mk,tbuf,of,om)) >= 0) ylikely {
	    if ((rs = cmimk_setdb(&mk,op->db_size,op->ti_db)) >= 0) ylikely {
		rs = commandments_dbproc(op,&mk) ;
	    }
	    rs1 = cmimk_close(&mk) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cmimk) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (commandments_idxmk) */

local int commandments_idxmapbegin(CMS *op,cchar *tbuf) noex {
	int		rs ;
	DEBUGPRINTF("ent tbuf=%s\n",tbuf) ;
	if ((rs = commandments_idxopencheck(op,tbuf)) >= 0) {
	    rs = SR_OK ; /* <- redundant statement */
	} else if (isStale(rs)) {
	    if ((rs = commandments_idxmk(op,tbuf)) >= 0) {
		rs = commandments_idxopencheck(op,tbuf) ;
	    }
	} /* end if (need or not) */
	DEBUGPRINTF("ret rs=%d n=%u\n",rs) ;
	return rs ;
} /* end subroutine (commandments_idxmapbegin) */

local int commandments_idxmapend(CMS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fl.idx) {
	    cmi		*cip = op->idxp ;
	    rs1 = cmi_close(cip) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fl.idx = false ;
	}
	return rs ;
} /* end subroutine (commandments_idxmapend) */

local int commandments_usridname(CMS *op,char *tbuf) noex {
    	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		rl = 0 ;
	if (tbuf[0] == '\0') {
	    cchar	*vd = "var" ;
	    cchar	*sn = CMS_SN ;
	    if ((rs = mkpath(tbuf,op->uhome,vd,sn)) >= 0) ylikely {
		cint	rsn = SR_NOTFOUND ;
	        rl = rs ;
		if (ustat sb ; (rs = uc_stat(tbuf,&sb)) == rsn) {
	            cmode	dm = 0775 ;
	            if ((rs = mkdirs(tbuf,dm)) >= 0) ylikely {
			if ((rs = uc_minmod(tbuf,dm)) >= 0) ylikely {
	    	            cint	n = _PC_CHOWN_RESTRICTED ;
	    	            if ((rs = u_pathconf(tbuf,n,np)) == 0) {
		                rs = commandments_chownpr(op,tbuf) ;
		            }
			} /* end if (uc_chmod) */
		    } /* end if (mkdirs) */
		} /* end if (uc_stat) */
	    } /* end if (mkpath) */
	} else {
	    rl = lenstr(tbuf) ;
	}
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (commandments_usridname) */

local int commandments_sysidname(CMS *op,char *tbuf) noex {
    	cnullptr	np{} ;
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		rl = 0 ; /* return-value */
	DEBUGPRINTF("ent pr=%s\n",op->pr) ;
	if (tbuf[0] == '\0') {
	    int		prnl ;
	    cchar	*prnp ;
	    if ((rs = sfrootname(op->pr,-1,&prnp)) > 0) {
		cchar	*prn ;
		prnl = rs ;
		nulstr	spr ;
		if ((rs = nulstr_start(&spr,prnp,prnl,&prn)) >= 0) ylikely {
	            cchar	*tmpdname = CMS_TMPDNAME ;
	            cchar	*sn = CMS_SN ;
	            if ((rs = mkpath(tbuf,tmpdname,prn,sn)) >= 0) ylikely {
	                rl = rs ;
		        if (ustat sb ; (rs = uc_stat(tbuf,&sb)) == rsn) {
	                    cmode	dm = 0777 ;
	                    if ((rs = mkdirs(tbuf,dm)) >= 0) ylikely {
				if ((rs = uc_chmod(tbuf,dm)) >= 0) {
	    	                    cint	n = _PC_CHOWN_RESTRICTED ;
	    	                    if ((rs = u_pathconf(tbuf,n,np)) == 0) {
		                        rs = commandments_chownpr(op,tbuf) ;
				    }
		                } /* end if (uc_chmod) */
		            } /* end if (mkdirs) */
		        } /* end if (stat) */
	            } /* end if (mkpath) */
		    rs1 = nulstr_finish(&spr) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (nulstr) */
	    } else {
		rs = SR_NOENT ;
	    }
	} else {
	    rl = lenstr(tbuf) ;
	}
	DEBUGPRINTF("ret rs=%d rl=%u\n",rs,rl) ;
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (commandments_sysidname) */

local int commandments_dbmapbegin(CMS *op,time_t dt) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	int		fsz = 0 ; /* return-value */
	DEBUGPRINTF("ent fn=%s\n",op->fname) ;
	if ((rs = u_open(op->fname,O_RDONLY,0666)) >= 0) ylikely {
	    cint	fd = rs ;
	    if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) ylikely {
		csize fsize = size_t(sb.st_size) ;
	        if (S_ISREG(sb.st_mode)) {
	            csize	ms = fsize ;
	            int		mp = PROT_READ ;
	            int		mf = MAP_SHARED ;
	            void	*md ;
	            if ((rs = u_mmapbegin(np,ms,mp,mf,fd,0z,&md)) >= 0) {
	                op->db_data = md ;
	                op->db_size = ms ;
	                op->ti_map = dt ;
			fsz = intsat(fsize) ;
	            } /* end if (u_mmap) */
	        } else {
	            rs = SR_NOTSUP ;
		}
	    } /* end if (stat) */
	    rs1 = u_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (file) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return (rs >= 0) ? fsz : rs ;
} /* end subroutine (commandments_dbmapbegin) */

local int commandments_dbmapend(CMS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->db_data) {
	    voidp md = op->db_data ;
	    csize ms = op->db_size ;
	    rs1 = u_mmapend(md,ms) ;
	    if (rs >= 0) rs = rs1 ;
	    op->db_data = nullptr ;
	} /* end if (u_mmapend) */
	return rs ;
} /* end subroutine (commandments_dbmapend) */

local int commandments_dbproc(CMS *op,cmimk *cmp) noex {
	mkent		e ;
	cnullptr	np{} ;
	uint		foff = 0 ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		ml = (op->db_size & INT_MAX) ;
	int		n = 0 ;
	int		c = 0 ; /* return-value */
	int		f_ent = false ;
	cchar		*mp = charp(op->db_data) ;
	DEBUGPRINTF("ent ml=%d\n",ml) ;
	for (cchar *tp ; (tp = strnchr(mp,ml,'\n')) != np ; ) {
	    cint	len = intconv((tp + 1) - mp) ;
	    cchar	*lp = mp ;
	    mut int	ll = (len - 1) ;
	    if ((ll > 0) && (! isempty(lp,ll))) {
		int	si ;
	        if ((tp = strnchr(lp,ll,'#')) != nullptr) {
	            ll = intconv(tp - lp) ;
		}
	        if (isstart(lp,ll,&n,&si)) {
	            if (f_ent) {
			cmimk_ent ce ;
	                c += 1 ;
		        if ((rs = cmimkent_start(&ce,&e)) >= 0) {
			    {
	                        rs = cmimk_add(cmp,&ce) ;
			    }
			    rs1 = cmimkent_finish(&ce) ;
			    if (rs >= 0) rs = rs1 ;
			} /* end if (cmimkent) */
			f_ent = false ;
	                rs1 = mkent_finish(&e) ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (entry) */
	            if (rs >= 0) {
			cuint	soff = (foff+si) ;
			cint	slen = (ll-si) ;
	                if ((rs = mkent_start(&e,n,soff,slen)) >= 0) {
	                    f_ent = true ;
			} /* end if (mkent_start) */
	            } /* end if (ok) */
	        } else {
	            if (f_ent) {
	                rs = mkent_add(&e,foff,ll) ;
		    }
	        } /* end if (entry start of add) */
	    } else {
		if_constexpr (f_emptyterm) {
	            if (f_ent) {
	    	        c += 1 ;
	    	        cmimk_ent ce ;
	    	        if ((rs = cmimkent_start(&ce,&e)) >= 0) {
			    {
			        rs = cmimk_add(cmp,&ce) ;
			    }
			    rs1 = cmimkent_finish(&ce) ;
	    	        } /* end if (cmimkent) */
	    	        f_ent = false ;
	    	        mkent_finish(&e) ;
	            } /* end if (f_ent) */
		} else {
	            rs = SR_OK ;
		} /* end if_constexpr (f_emptyterm) */
	    } /* end if (not empty) */
	    foff += len ;
	    ml -= len ;
	    mp += len ;
	    if (rs < 0) break ;
	} /* end for (readling lines) */
	if ((rs >= 0) && f_ent) {
	    c += 1 ;
	    if (cmimk_ent ce ; (rs = cmimkent_start(&ce,&e)) >= 0) {
		{
	            rs = cmimk_add(cmp,&ce) ;
		}
	        rs1 = cmimkent_finish(&ce) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (cmimkent_start) */
	    f_ent = false ;
	    mkent_finish(&e) ;
	} /* end if (entry) */
	if (f_ent) {
	    mkent_finish(&e) ;
	}
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (commandments_dbproc) */

local int commandments_checkupdate(CMS *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		f = false ; /* return-value */
	if (op->ncursors == 0) {
	    if (dt == 0) dt = getustime ;
	    if ((dt - op->ti_lastcheck) >= TO_CHECK) {
	        op->ti_lastcheck = dt ;
	        if (ustat sb ; (rs = u_stat(op->fname,&sb)) >= 0) {
	            f = f || (sb.st_mtime > op->ti_db) ;
		    f = f || (sb.st_mtime > op->ti_map) ;
	            if (f) {
			cchar	*dbname = op->dbname ;
	                commandments_fileloadend(op) ;
	                rs = commandments_fileloadbegin(op,dbname) ;
	            } /* end if (update) */
	        } else if (isNotPresent(rs)) {
	            rs = SR_OK ;
	        } /* end if (stat) */
	    } /* end if (timed-out) */
	} /* end if (no cursors out) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (commandments_checkupdate) */

local int commandments_loadbuf(CMS *op,cmi_ent *vivp,
		char *rbuf,int rlen) noex {
	int		rs ;
	int		len = 0 ; /* return-value */
	DEBUGPRINTF("ent eoff=%u elen=%u\n",vivp->eoff,vivp->elen) ;
	if (sbuf b ; (rs = sbuf_start(&b,rbuf,rlen)) >= 0) ylikely {
	    cmi_line	*lines = vivp->lines ;
	    cint	nlines = vivp->nlines ;
	    int		ll ;
	    cchar	*lp ;
	    for (int i = 0 ; i < nlines ; i += 1) {
	        if (i > 0) {
		    sbuf_chr(&b,' ') ;
		}
	        lp = (charp(op->db_data) + lines[i].loff) ;
	        ll = lines[i].llen ;
	        rs = sbuf_strw(&b,lp,ll) ;
	        if (rs < 0) break ;
	    } /* end for */
	    len = sbuf_finish(&b) ;
	    if (rs >= 0) rs = len ;
	} /* end if (sbuf) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (commandments_loadbuf) */

local int commandments_userhome(CMS *op) noex {
	int		rs ;
	int		rs1 ;
	if (op->uhome == nullptr) {
	    if (char *hbuf ; (rs = mem.mp(&hbuf)) >= 0) ylikely {
	        cint	hlen = rs ;
	        if ((rs = getuserhome(hbuf,hlen,"-")) >= 0) ylikely {
		    if (cchar *uh ; (rs = mem.strw(hbuf,rs,&uh)) >= 0) {
		        op->uhome = uh ;
		    } /* end if (memory-acquire) */
	        } /* end if (getuserhome) */
		rs1 = mem.free(hbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} else {
	    rs = lenstr(op->uhome) ;
	}
	return rs ;
} /* end subroutine (commandments_userhome) */

local int commandments_chownpr(CMS *op,cchar *tbuf) noex {
	int		rs ;
	if (op->uid == uidend) {
	    op->uid = getuid() ;
	}
	if ((rs = commandments_ids(op)) >= 0) ylikely {
	    const uid_t	uid_pr = op->uid_pr ;
	    const gid_t	gid_pr = op->gid_pr ;
	    rs = u_chown(tbuf,uid_pr,gid_pr) ;
	} /* end if (commandments_ids) */
	return rs ;
} /* end subroutine (commandments_chownpr) */

local int commandments_ids(CMS *op) noex {
	int		rs = SR_OK ;
	if (! op->fl.ids) {
	    op->fl.ids = true ;
	    if (ustat sb ; (rs = uc_stat(op->pr,&sb)) >= 0) {
		op->uid_pr = sb.st_uid ;
		op->gid_pr = sb.st_gid ;
	    }
	} /* end if (needed) */
	return rs ;
} /* end subroutine (commandments_ids) */

local int mkent_start(mkent *ep,int cn,uint eoff,uint elen) noex {
	int		rs = SR_FAULT ;
	DEBUGPRINTF("ent cn=%u\n",cn) ;
	if (ep) ylikely {
	    cint	ne = CMIMK_NE ; /* use their value for starters */
	    cint	lsz = szof(mkent_line) ;
	    rs = memclear(ep) ;
	    ep->cn = ushort(cn) ;
	    ep->eoff = eoff ;
	    ep->elen = elen ;
	    if (void *vp ; (rs = mem.call(ne,lsz,&vp)) >= 0) ylikely {
	        ep->lines = resumelife<mkent_line>(vp) ;
	        ep->e = ne ;
	        ep->i += 1 ; /* <- fill one entry here below */
	        {
		    mkent_line *lnarr = ep->lines ;
	            lnarr[0].loff = eoff ;
	            lnarr[0].llen = elen ;
	        } /* end block (first line) */
	    } /* end if (memory-acquire) */
	} /* end if (non-null) */
	DEBUGPRINTF("ret rs=%d i=%u\n",rs,ep->i) ;
	return rs ;
} /* end subroutine (mkent_start) */

local int mkent_finish(mkent *ep) noex {
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	if (ep) ylikely {
	    DEBUGPRINTF("ent e=%u i=%u\n",ep->e,ep->i) ;
	    DEBUGPRINTF("i=%u\n",ep->i) ;
	    if (ep->lines) {
	        rs1 = mem.free(ep->lines) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->lines = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (mkent_finish) */

local int mkent_add(mkent *ep,uint eoff,uint elen) noex {
	int		rs = SR_BUGCHECK ;
	if (ep) ylikely {
	    rs = SR_NOTOPEN ;
	    if (ep->e > 0) ylikely {
	        rs = SR_BADFMT ;
	        DEBUGPRINTF("ent e=%u i=%u\n",ep->e,ep->i) ;
	        if ((ep->i >= 0) && (ep->i <= ep->e)) ylikely {
	            mkent_line	*elp ;
	            int		ne ;
	            int		sz ;
		    rs = SR_OK ;
	            if (ep->i == ep->e) {
	                ne = (ep->e + CMIMK_NE) ;
	                sz = (ne * szof(mkent_line)) ;
	                if ((rs = mem.rall(ep->lines,sz,&elp)) >= 0) {
	                    ep->lines = elp ;
		            ep->e = ne ;
	                } /* end if (memory-acquire) */
	            } /* end if */
	            if (rs >= 0) {
	                elp = (ep->lines + ep->i) ;
	                elp->loff = eoff ;
	                elp->llen = elen ;
	                ep->i += 1 ;
	            } /* end if (ok) */
	        } /* end if (valid) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (mkent_add) */

local int cmimkent_start(cmimk_ent *bvep,mkent *ep) noex {
	int		rs = SR_FAULT ;
	int		nlines = 0 ; /* return-value */
	if (ep) ylikely {
	    rs = memclear(bvep) ;
	    bvep->cn = ep->cn ;
	    bvep->eoff = ep->eoff ;
	    bvep->elen = ep->elen ;
	    bvep->lines = nullptr ;
	    nlines = ep->i ;
	    DEBUGPRINTF("nlines=%u\n",nlines) ;
	    if (nlines <= USHORT_MAX) ylikely {
	        int	sz ;
	        bvep->nlines = ushort(nlines) ;
	        sz = ((nlines + 1) * szof(cmimk_line)) ;
	        if (cmimk_line *lines ; (rs = mem.mall(sz,&lines)) >= 0) {
	            bvep->lines = lines ;
		    int	i ; /* used-afterwards */
		    int	n = int(nlines) ;
	            for (i = 0 ; i < n ; i += 1) {
	                lines[i].loff = ep->lines[i].loff ;
	                lines[i].llen = ep->lines[i].llen ;
	            } /* end if */
	            lines[i].loff = 0 ;
	            lines[i].llen = 0 ;
	        } /* end if (memory-acquire) */
	    } else {
	        rs = SR_TOOBIG ;
	    }
	} /* end if (non-null) */
	DEBUGPRINTF("ret rs=%d nlines=%u\n",rs,nlines) ;
	return (rs >= 0) ? nlines : rs ;
} /* end subroutine (cmimkent_start) */

local int cmimkent_finish(cmimk_ent *bvep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (bvep) ylikely {
	    rs = SR_OK ;
	    if (bvep->lines) {
	        rs1 = mem.free(bvep->lines) ;
	        if (rs >= 0) rs = rs1 ;
	        bvep->lines = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (cmimkent_finish) */

local bool isempty(cchar *lp,int ll) noex {
	int		cl ;
	bool		f = false ;
	cchar	*cp ;
	f = f || (ll == 0) ;
	f = f || (lp[0] == '#') ;
	if ((! f) && CHAR_ISWHITE(*lp)) {
	    cl = sfskipwhite(lp,ll,&cp) ;
	    f = f || (cl == 0) ;
	    f = f || (cp[0] == '#') ;
	}
	return f ;
} /* end subroutine (isempty) */

local bool isstart(cchar *lp,int ll,int *namp,int *sip) noex {
	bool		f = false ;
	*namp = -1 ;
	*sip = 0 ;
	if (cchar *tp = strnchr(lp,ll,'.') ; tp) ylikely {
	    cc	*cp = lp ;
	    int	cl = intconv(tp - lp) ;
	    f = hasourdig(cp,cl) && (cfdeci(cp,cl,namp) >= 0) ;
	    if (f) {
	        *sip = intconv((tp + 1) - lp) ;
	    }
	} /* end if (strnchr) */
	return f ;
} /* end subroutine (isstart) */

local bool hasourdig(cchar *sp,int sl) noex {
	bool		f = false ; /* return-value */
	cchar	*cp ;
	if (int cl ; (cl = sfshrink(sp,sl,&cp)) > 0) ylikely {
	    f = true ;
	    for ( ; cl-- && *cp ; cp += 1) {
	        f = isdigitlatin(*cp) ;
		if (!f) break ;
	    } /* end while */
	} /* end if */
	return f ;
} /* end subroutine (hasourdig) */

#if	CF_DEBUG && CF_DEBUGLINE
local int linenlen(cchar *lp,int ll,int ml) noex {
	int		len = INT_MAX ; /* return-value */
	if (lp) {
	    if (ll > 0) len = MIN(len,ll) ;
	    if (ml > 0) len = MIN(len,ml) ;
	    if (cchar *tp = strnchr(lp,len,'\n') ; tp) {
	        len = intconv(tp - lp) ;
	    } /* end if (strnchr) */
	} /* end if (non-null) */
	return len ;
} /* end subroutine (linenlen) */
#endif /* CF_DEBUG */

local bool isNotOurAccess(int rs) noex {
	bool		f = false ;
	if (rs < 0) {
	    f = f || isNotPresent(rs) ;
	    f = f || (rs == SR_ISDIR) ;
	} /* end if */
	return f ;
} /* end subroutine (isNotOurAccess) */

local bool isStale(int rs) noex {
	bool		f = false ; /* return-value */
	if (rs < 0) {
	    f = f || isNotPresent(rs) ;
	    f = f || isOneOf(rsold,rs) ;
	} /* end if */
	return f ;
} /* end subroutine (isStale) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_mp)) >= 0) ylikely {
	    maxpathlen = rs ;
	}
    	return rs ;
} /* end method (vars::operator) */


