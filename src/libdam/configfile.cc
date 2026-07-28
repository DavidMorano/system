/* configfile SUPPORT (Configuration File) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* parse a configuration file */
/* version %I% last-modified %G% */

#define	CF_EXPORTEQUAL	0		/* add equal for empty-value exports */

/* revision history:

	= 2000-01-21, David A­D­ Morano
	This subroutine was enhanced for use by LevoSim.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	configfile

	Description:
	This is the old configuration file reader object.  It is
	cheap, it is ill-conceived, it is a mess, it works well
	enough to be used for cheap code.  I did not want to use
	this junk for the Levo machine simulator but time pressure
	decided for us!  Although this whole configuration scheme
	is messy, it gives us enough of what we need to get some
	configuration information into the Levo machine simulator
	and to get a parameter file name.  This is good enough for
	now.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<sys/stat.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<cassert>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<new>			/* C++STD placement-new */
#include	<string>		/* C++STD */
#include	<string_view>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<buffer.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<fieldterms.h>		/* LIBUC */
#include	<field.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<matxstr.h>		/* LIBUC */
#include	<cfdecmf.h>		/* LIBUC */
#include	<baops.h>		/* LIBU |baset(3u)| */
#include	<localmisc.h>		/* LIBU |UC(3u)| */
#include	<bfile.h>		/* LIBB */

#include	"configfile.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/kvparse.ccm"

import libutil ;			/* |memclear(3u)| */
import kvparse ;

/* local defines */

#define	CF		configfile


/* imported namespaces */

using std::string ;			/* type */
using std::string_view ;		/* type */
using libuc::mem ;			/* variable */


/* local typedefs */

typedef string_view	strview ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int	maxlinelen ;
    	operator int () noex ;
    } ; /* end struct (vars) */
    struct starter {
	configfile	*op ;
	vecstr		*dlp ;
	vecstr		*elp ;
	vecstr		*ulp ;
	cchar		*cfn ;
	string		key ;
	int		nopts ;
	int		lineno ;
	starter(configfile *o,cchar *c) noex : op(o), cfn(c) { 
	    nopts = 0 ;
	    lineno = 1 ;
	} ; /* end ctor */
	operator int () noex ;
	int buf		() noex ;
	int file	(buffer *) noex ;
	int procln	(buffer *,cchar *,int,char *,int) noex ;
	int matkey	(buffer *,field *,cchar *,int) noex ;
	int procstr	(buffer *,field *,int,cchar **) noex ;
	int procopt	(buffer *,field *,int,cchar **) noex ;
	int procunset	(buffer *,field *,int,cchar **) noex ;
	int procvar	(buffer *,field *,int,cchar **) noex ;
	int procval	(buffer *,field *,int,cchar **) noex ;
	int optload	(buffer *) noex ;
    } ; /* end struct */
} /* end namespace */


/* forward references */

template<typename ... Args>
local inline int configfile_ctor(CF *op,Args ... args) noex {
    	CONFIGFILE	*hop = op ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if (vecstr *dlp = new(nt) vecstr ; dlp) {
		op->dlp = dlp ;
	        if (vecstr *elp = new(nt) vecstr ; elp) {
		    op->elp = elp ;
	            if (vecstr *ulp = new(nt) vecstr ; ulp) {
			op->ulp = ulp ;
			rs = SR_OK ;
	            } /* end if (new-vecstr) */
		    if (rs < 0) {
		        delete elp ;
		        op->elp = nullptr ;
		    } /* end if (error) */
	        } /* end if (new-vecstr) */
		if (rs < 0) {
		    delete dlp ;
		    op->dlp = nullptr ;
		} /* end if (error) */
	    } /* end if (new-vecstr) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (configfile_ctor) */

local int configfile_dtor(CF *op) noex {
	int		rs = SR_OK ;
	if (op->ulp) ylikely {
	    vecstr *ulp = resumelife<vecstr>(op->ulp) ;
	    delete ulp ;
	    op->ulp = nullptr ;
	} /* end if (memory-release) */
	if (op->elp) ylikely {
	    vecstr *elp = resumelife<vecstr>(op->elp) ;
	    delete elp ;
	    op->elp = nullptr ;
	} /* end if (memory-release) */
	if (op->dlp) ylikely {
	    vecstr *dlp = resumelife<vecstr>(op->dlp) ;
	    delete dlp ;
	    op->dlp = nullptr ;
	} /* end if (memory-release) */
	return rs ;
} /* end subroutine (configfile_dtor) */

template<typename ... Args>
local inline int configfile_magic(CF *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == CONFIGFILE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (configfile_magic) */

local int	configfile_initvars(configfile *) noex ;


/* local variables */

/* these are the terminators for most everything */
static char		fterms[fieldterms_termsize] ;
/* these are the terminators for options */
static char		oterms[fieldterms_termsize] ;

enum configkeys {
	configkey_define,
	configkey_export,
	configkey_tmpdir,
	configkey_root,
	configkey_pidfile,
	configkey_lockfile,
	configkey_log,
	configkey_loglen,
	configkey_workdir,
	configkey_port,
	configkey_user,
	configkey_group,
	configkey_userpass,
	configkey_machpass,
	configkey_srvtab,
	configkey_sendmail,
	configkey_envfile,
	configkey_pathfile,
	configkey_devicefile,
	configkey_seedfile,
	configkey_logsize,
	configkey_organization,
	configkey_unset,
	configkey_timeout,
	configkey_removemul,
	configkey_acctab,
	configkey_paramfile,
	configkey_nrecips,
	configkey_helpfile,
	configkey_paramtab,
	configkey_pingtab,
	configkey_pingstat,
	configkey_option,
	configkey_mintexec,
	configkey_interval,
	configkey_stampdir,
	configkey_maxjobs,
	configkey_directory,
	configkey_interrupt,
	configkey_polltime,
	configkey_filetime,
	configkey_passfile,
	configkey_eigenfile,
	configkey_minwordlen,
	configkey_maxwordlen,
	configkey_keys,
	configkey_overlast
} ; /* end enum (configkeys) */

namespace {
    struct configer {
	cchar		*n[configkey_overlast + 1] = {} ;
	consteval void mktab() noex {
	    n[configkey_define]		= "define" ;
	    n[configkey_export]		= "export" ;
	    n[configkey_tmpdir]		= "tmpdir" ;
	    n[configkey_root]		= "root" ;
	    n[configkey_pidfile]	= "pidfile" ;
	    n[configkey_lockfile]	= "lockfile" ;
	    n[configkey_log]		= "log" ;
	    n[configkey_loglen]		= "loglen" ;
	    n[configkey_workdir]	= "workdir" ;
	    n[configkey_port]		= "port" ;
	    n[configkey_user]		= "username" ;
	    n[configkey_group]		= "groupname" ;
	    n[configkey_userpass]	= "userpassword" ;
	    n[configkey_machpass]	= "machpassword" ;
	    n[configkey_srvtab]		= "srvtab" ;
	    n[configkey_sendmail]	= "sendmail" ;
	    n[configkey_envfile]	= "envfile" ;
	    n[configkey_pathfile]	= "pathfile" ;
	    n[configkey_devicefile]	= "devicefile" ;
	    n[configkey_seedfile]	= "seedfile" ;
	    n[configkey_logsize]	= "logsize" ;
	    n[configkey_organization]	= "organization" ;
	    n[configkey_unset]		= "unset" ;
	    n[configkey_timeout]	= "timeout" ;
	    n[configkey_removemul]	= "removemul" ;
	    n[configkey_acctab]		= "acctab" ;
	    n[configkey_paramfile]	= "paramfile" ;
	    n[configkey_nrecips]	= "nrecips" ;
	    n[configkey_helpfile]	= "helpfile" ;
	    n[configkey_paramtab]	= "paramtab" ;
	    n[configkey_pingtab]	= "pingtab" ;
	    n[configkey_pingstat]	= "pingstat" ;
	    n[configkey_option]		= "option" ;
	    n[configkey_mintexec]	= "mintexec" ;
	    n[configkey_interval]	= "interval" ;
	    n[configkey_stampdir]	= "stampdir" ;
	    n[configkey_maxjobs]	= "maxjobs" ;
	    n[configkey_directory]	= "directory" ;
	    n[configkey_interrupt]	= "interrupt" ;
	    n[configkey_polltime]	= "polltime" ;
	    n[configkey_filetime]	= "filetime" ;
	    n[configkey_passfile]	= "passfile" ;
	    n[configkey_eigenfile]	= "eigenfname" ;
	    n[configkey_minwordlen]	= "minwordlen" ;
	    n[configkey_maxwordlen]	= "maxwordlen" ;
	    n[configkey_keys]		= "keys" ;
	    n[configkey_overlast]	= nullptr ;
	} ; /* end method (mktab) */
	consteval configer() noex {
	    mktab() ;
	} /* end ctor */
    } ; /* end struct (configer) */
} /* end namespace */

constexpr configer	configname ;
static vars		var ;


/* exported variables */


/* exported subroutines */

int configfile_start(CF *op,cchar *cfn) noex {
	int		rs ;
	int		c = 0 ; /* return-value */
	assert(op && cfn) ;
	if ((rs = configfile_ctor(op,cfn)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (cfn[0]) ylikely {
		if (static cint rsv = var ; (rs = rsv) >= 0) ylikely {
		    if ((rs = configfile_initvars(op)) >= 0) {
		        starter so(op,cfn) ;
		        rs = so ;
		        c = rs ;
		    } /* end if (initvars) */
		} /* end if (vars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		configfile_dtor(op) ;
	    } /* end if (errro) */
        } /* end if (configfile_ctor) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (conigfile_start) */

starter::operator int () noex {
	cint		vn = 10 ;
	cint		vo = 0 ;
    	int		rs ;
	int		rs1 ;
	if ((rs = dlp->start(vn,vo)) >= 0) {
	    if ((rs = elp->start(vn,vo)) >= 0) {
	        if ((rs = ulp->start(vn,vo)) >= 0) {
		    {
			rs = buf() ;
		    }
	            rs1 = elp->finish() ;
	            if (rs >= 0) rs = rs1 ;
		} /* end if (vecstr_start) */
	        rs1 = elp->finish() ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (vecstr_start) */
	    rs1 = dlp->finish() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecstr_start) */
	return rs ;
} /* end method (starter::operator) */

int starter::buf() noex {
    	cnothrow	nt{} ;
    	int		rs = SR_NOMEM ;
	int		rs1 ;
	if (buffer *bop = new(nt) buffer ; bop) {
	    if ((rs = bop->start) >= 0) {
		if ((rs = file(bop)) >= 0) {
		    if ((rs = optload(bop)) >= 0) {
	    	        op->magval = CONFIGFILE_MAGIC ;
		    }
		} /* end if (file) */
		rs1 = bop->finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (buffer) */
	    delete bop ;
	    bop = nullptr ;
	} /* end if (m-a-f) */
	return rs ;
} /* end method (starter::buf) */

int starter::file(buffer *bop) noex {
    	cnothrow	nt{} ;
    	int		rs ;
	int		rs1 ;
	if (char *lbuf ; (rs = mem.ml(&lbuf)) >= 0) ylikely {
	    cint llen = rs ;
	    if (char *kbuf ; (rs = mem.sn(&kbuf)) >= 0) {
	        cint klen = rs ;
		rs = SR_NOMEM ;
	        if (bfile *bfp = new(nt) bfile ; bfp) ylikely {
		    if ((rs = bfp->open(cfn)) >= 0) {
		        while ((rs = bfp->readln(lbuf,llen)) > 0) {
			    cchar *lp ;
			    if (int ll ; (ll = sfcontent(lbuf,rs,&lp)) > 0) {
			        rs = procln(bop,lp,ll,kbuf,klen) ;
			    }
			    lineno += 1 ;
			    if (rs < 0) break ;
		        } /* end while */
		        rs1 = bfp->close ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (bfile) */
	            delete bfp ;
	        } /* end if (new-bfile) */
	        rs1 = mem.free(kbuf) ;
	        if (rs >= 0) rs = rs1 ;
            } /* end if (m-a-f) */
	    rs1 = mem.free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end method (starter::file) */

int starter::procln(buffer *bop,cchar *lp,int ll,char *kbuf,int klen) noex {
    	int		rs ;
	int		rs1 ;
	if (field fsb ; (rs = fsb.start(lp,ll)) >= 0) {
	    cchar *fp ;
	    if (int fl ; (fl = fsb.get(fterms,&fp)) > 0) {
	        cint kl = MIN(fl,klen) ;
	        strncpylc(kbuf,fp,kl) ;
		rs = matkey(bop,&fsb,kbuf,kl) ;
	    } /* end if (get key) */
	    rs1 = fsb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (field) */
	return rs ;
} /* end method (starter::procln) */

int starter::matkey(buffer *bop,field *fsbp,cchar *kbuf,int kl) noex {
    	int		rs = SR_OK ;
	if (int ci ; (ci = matpstr(configname.n,1,kbuf,kl)) >= 0) {
	    cchar	**pp = nullptr ;
	    (void) bop ;
	    (void) fsbp ;
	    switch (ci) {
	    case configkey_root:
		if (!pp) pp = &op->root ;
		falldown ;
	    case configkey_tmpdir:
		if (!pp) pp = &op->tmpdir ;
		falldown ;
	    case configkey_log:
		if (!pp) pp = &op->logfname ;
		falldown ;
	    case configkey_workdir:
		if (!pp) pp = &op->tmpdir ;
		falldown ;
	    case configkey_pidfile:
		if (!pp) pp = &op->pidfname ;
		falldown ;
	    case configkey_lockfile:
		if (!pp) pp = &op->lockfname ;
		falldown ;
	    case configkey_user:
		if (!pp) pp = &op->user ;
		falldown ;
	    case configkey_group:
		if (!pp) pp = &op->group ;
		falldown ;
	    case configkey_port:
		if (!pp) pp = &op->port ;
		falldown ;
	    case configkey_userpass:
		if (!pp) pp = &op->userpass ;
		falldown ;
	    case configkey_machpass:
		if (!pp) pp = &op->machpass ;
		falldown ;
	    case configkey_srvtab:
		if (!pp) pp = &op->srvtab ;
		falldown ;
	    case configkey_sendmail:
		if (!pp) pp = &op->sendmail ;
		falldown ;
	    case configkey_mintexec:
		if (!pp) pp = &op->sendmail ;
		falldown ;
	    case configkey_envfile:
		if (!pp) pp = &op->envfname ;
		falldown ;
	    case configkey_pathfile:
		if (!pp) pp = &op->pathfname ;
		falldown ;
	    case configkey_logsize:
		if (!pp) pp = &op->logsize ;
		falldown ;
	    case configkey_organization:
		if (!pp) pp = &op->organization ;
		falldown ;
	    case configkey_timeout:
		if (!pp) pp = &op->timeout ;
		falldown ;
	    case configkey_removemul:
		if (!pp) pp = &op->removemul ;
		falldown ;
	    case configkey_acctab:
		if (!pp) pp = &op->acctab ;
		falldown ;
	    case configkey_paramfile:
	    case configkey_paramtab:
	    case configkey_pingtab:
		if (!pp) pp = &op->paramfname ;
		falldown ;
	    case configkey_nrecips:
		if (!pp) pp = &op->nrecips ;
		falldown ;
	    case configkey_helpfile:
		if (!pp) pp = &op->helpfname ;
		falldown ;
	    case configkey_pingstat:
		if (!pp) pp = &op->statfname ;
		falldown ;
	    case configkey_interval:
		if (!pp) pp = &op->interval ;
		falldown ;
	    case configkey_stampdir:
		if (!pp) pp = &op->stampdir ;
		falldown ;
	    case configkey_maxjobs:
		if (!pp) pp = &op->maxjobs ;
		falldown ;
	    case configkey_directory:
		if (!pp) pp = &op->directory ;
		falldown ;
	    case configkey_interrupt:
		if (!pp) pp = &op->interrupt ;
		falldown ;
	    case configkey_polltime:
		if (!pp) pp = &op->polltime ;
		falldown ;
	    case configkey_filetime:
		if (!pp) pp = &op->filetime ;
		falldown ;
	    case configkey_passfile:
		if (!pp) pp = &op->passfname ;
		falldown ;
	    case configkey_eigenfile:
		if (!pp) pp = &op->eigenfname ;
	        if (pp) {
		    rs = procstr(bop,fsbp,ci,pp) ;
	        }
		break ;
	    case configkey_option:
		rs = procopt(bop,fsbp,ci,pp) ;
	        break ;
	    case configkey_unset:
		rs = procunset(bop,fsbp,ci,pp) ;
	        break ;
	    case configkey_define:
	    case configkey_export:
		rs = procvar(bop,fsbp,ci,pp) ;
		break ;
	    case configkey_loglen:
	    case configkey_minwordlen:
	    case configkey_maxwordlen:
	    case configkey_keys:
		rs = procval(bop,fsbp,ci,pp) ;
		break ;
	    } /* end switch */
	} /* end if (matpstr) */
	return rs ;
} /* end method (starter::matkey) */

int starter::procstr(buffer *bop,field *fsbp,int ci,cchar **pp) noex {
    	int		rs = SR_OK ;
	int		rs1 ;
    	(void) bop ;
	(void) ci ;
	if (*pp) {
	    voidp vp = voidp(*pp) ;
	    rs1 = mem.free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */
	if (rs >= 0) {
	    cchar *fp ;
	    if (int fl ; (fl = field_get(fsbp,fterms,&fp)) >= 0) {
		if (cchar *cp ; (rs = mem.strw(fp,fl,&cp)) >= 0) {
		    *pp = cp ;
		} /* end if (memory-acquire) */
	    }
	} /* end if (ok) */
	return rs ;
} /* end method (starter::procstr) */

int starter::procopt(buffer *bop,field *fsbp,int ci,cchar **pp) noex {
    	int		rs = SR_OK ;
	int		fl ;
	cchar		*fp ;
	(void) ci ;
	(void) pp ;
	while ((fsbp->term != '#') && ((fl = fsbp->get(oterms,&fp)) >= 0)) {
	    if (fl > 0) {
	        if (nopts > 0) {
		    rs = bop->chr(',') ;
		}
		if (rs >= 0) {
		   rs = bop->strw(fp,fl) ;
	           nopts += 1 ;
		} /* end if (ok) */
	    } /* end if (non-zero positive) */
	    if (rs < 0) break ;
	} /* end while */
	return rs ;
} /* end method (starter:procopt) */

int starter::procunset(buffer *bop,field *fsbp,int ci,cchar **pp) noex {
    	int		rs = SR_OK ;
	(void) bop ;
	(void) ci ;
	(void) *pp ;
	cchar *fp ;
	if (int fl ; (fl = fsbp->get(fterms,&fp)) > 0) {
	    rs = ulp->add(fp,fl) ;
	}
	return rs ;
} /* end method (starter::procunset) */

int starter::procvar(buffer *,field *fsbp,int ci,cchar **) noex {
    	int		rs ;
	if (kvparse per ; (rs = per.extract(fsbp->fp,fsbp->fl)) > 0) {
	    vecstr *vlp = nullptr ;
	    switch (ci) {
	    case configkey_define:
		vlp = dlp ;
		break ;
	    case configkey_export:
		vlp = elp ;
		break ;
	    } /* end switch */
	    if (vlp) {
		rs = vlp->addkeyval(per.keyp,per.keyl,per.valp,per.vall) ;
	    }
	} /* end if (kvparse) */
	return rs ;
} /* end method (starter::procvar) */

int starter::procval(buffer *bop,field *fsbp,int ci,cchar **pp) noex {
    	int		rs = SR_OK ;
	(void) bop ;
	(void) pp ;
	cchar *fp ;
	if (int fl ; (fl = fsbp->get(fterms,&fp)) > 0) {
	    if (int v ; (rs = cfdecmf(fp,fl,&v)) >= 0) {
		switch (ci) {
	        case configkey_loglen:
	            op->loglen		= v ;
		    break ;
	        case configkey_minwordlen:
	            op->minwordlen	= v ;
		    break ;
	        case configkey_maxwordlen:
	            op->maxwordlen	= v ;
		    break ;
	        case configkey_keys:
	            op->keys		= v ;
		    break ;
		} /* end switch */
	    } /* end if (value-conversion) */
	} /* end if (value-acquire) */
	return rs ;
} /* end method (starter::procval) */

int starter::optload(buffer *bop) noex {
    	int		rs = SR_OK ;
	if (nopts > 0) {
	    if (cchar *optp ; (rs = bop->get(&optp)) > 0) {
		if (cchar *cp ; (rs = mem.strw(optp,rs,&cp)) >= 0) {
	            op->options = cp ;
		}
	    }
	} /* end if (options) */
	return rs ;
} /* end method (starter::optload) */

namespace {
    struct memfreer {
	int	rs = SR_OK ;
	int	rs1 ;
	void operator () (ccharp &p) noex {
	    if (p) ylikely {
		voidp vp = voidp(p) ;
	        rs1 = mem.free(vp) ;
		if (rs >= 0) rs = rs1 ;
		p = nullptr ;
	    } /* end if (memory-release) */
	} ; /* end method */
	operator int () const noex {
	    return rs ;
	} ; /* end method */
    } ; /* end struct */
} /* end namespace */

int configfile_finish(CF *op) noex {
	int		rs ;
	int		rs1 ;
	assert(op) ;
	if ((rs = configfile_magic(op)) >= 0) {
	    /* free up the simple ones */
	    {
	        memfreer fo ;
	        fo(op->root) ;
	        fo(op->tmpdir) ;
	        fo(op->pidfname) ;
	        fo(op->lockfname) ;
	        fo(op->logfname) ;
	        fo(op->workdir) ;
	        fo(op->port) ;
	        fo(op->user) ;
	        fo(op->group) ;
	        fo(op->userpass) ;
	        fo(op->machpass) ;
	        fo(op->srvtab) ;
	        fo(op->sendmail) ;
	        fo(op->envfname) ;
	        fo(op->pathfname) ;
	        fo(op->devicefname) ;
	        fo(op->seedfname) ;
	        fo(op->logsize) ;
	        fo(op->organization) ;
	        fo(op->timeout) ;
	        fo(op->removemul) ;
	        fo(op->acctab) ;
	        fo(op->paramfname) ;
	        fo(op->nrecips) ;
	        fo(op->helpfname) ;
	        fo(op->statfname) ;
	        fo(op->options) ;
	        fo(op->interval) ;
	        fo(op->stampdir) ;
	        fo(op->maxjobs) ;
	        fo(op->directory) ;
	        fo(op->interrupt) ;
	        fo(op->polltime) ;
	        fo(op->filetime) ;
	        fo(op->passfname) ;
	        fo(op->eigenfname) ;
	        rs1 = fo ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (memory-release) */
	    {
		rs1 = configfile_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
        } /* end if (configfile_magic) */
	return rs ;
} /* end subroutine (configfile_finish) */


/* local subroutines */

local int configfile_initvars(configfile *op) noex {
	op->srs = 0 ;
	op->badline = -1 ;
	op->loglen = -1 ;
	op->minwordlen = -1 ;
	op->maxwordlen = -1 ;
	op->keys = -1 ;
	return SR_OK ;
} /* end subroutine (configfile_initvars) */

constexpr uchar		f_terms[] = {
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
	0x0E, 0x0F, 0x16, 0x17,
	0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F 
} ; /* end array */

vars::operator int () noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_ml)) >= 0) {
	    cbool	f = false ;
	    maxlinelen = rs ;
	    if ((rs = fieldterms(oterms,f,"\b\t\v #,")) >= 0) {
	        if ((rs = fieldterms(fterms,f,"\v\f\r\n !#':=")) >= 0) {
	    	    baset(fterms,CH_NUL) ;
	    	    baset(fterms,CH_DEL) ;
		    for (cauto &ch : f_terms) {
	    	        baset(fterms,ch) ;
		    } /* end for */
	        } /* end if (fieldterms) */
	    } /* end if (fieldterms) */
	} /* end if (bufsizeget) */
    	return rs ;
} /* end method (vars::operator) */


