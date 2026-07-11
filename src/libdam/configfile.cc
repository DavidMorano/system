/* CF SUPPORT */
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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<new>			/* C++STD placement-new */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<buffer.h>		/* LIBUC */
#include	<fieldterms.h>		/* LIBUC */
#include	<field.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<baops.h>		/* LIBU |baset(3u)| */
#include	<localmisc.h>		/* LIBU |UC(3u)| */
#include	<bfile.h>		/* LIBB */

#include	"configfile.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN	2048
#endif
#endif

#undef	BUFLEN
#define	BUFLEN		(LINEBUFLEN * 2)

#define	CF		configfile


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


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
	buffer		*bop ;
	starter(configfile *o,cchar *c) noex : op(o), cfn(c) { } ;
	operator int () noex ;
	int buf		() noex ;
    } ; /* end struct */
} /* end namespace */


/* forward references */

template<typename ... Args>
local inline int configfile_ctor(CF *op,Args ... args) noex {
    	COFIGFILE	*hop = op ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if (vecstr *dlp = new(nt) vecstr ; dlp) {
		op->dlp = dlp ;
	        if (vecstr *elp = new(nt) vecstr ; ulp) {
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
	    delete op->ulp ;
	    op->ulp = nullptr ;
	} /* end if (memory-release) */
	if (op->elp) ylikely {
	    delete op->elp ;
	    op->elp = nullptr ;
	} /* end if (memory-release) */
	if (op->dlp) ylikely {
	    delete op->dlp ;
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

local void	checkfree(char **) noex ;


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

constexpr cpcchar	configkeys[] = {
	"define",
	"export",
	"tmpdir",
	"root",
	"pidfile",
	"lockfile",
	"log",
	"loglen",
	"workdir",
	"port",
	"username",
	"groupname",
	"userpassword",
	"machpassword",
	"srvtab",
	"sendmail",
	"envfile",
	"pathfile",
	"devicefile",
	"seedfile",
	"logsize",
	"organization",
	"unset",
	"timeout",
	"removemul",
	"acctab",
	"paramfile",
	"nrecips",
	"helpfile",
	"paramtab",
	"pingtab",
	"pingstat",
	"option",
	"mintexec",
	"interval",
	"stampdir",
	"maxjobs",
	"directory",
	"interrupt",
	"polltime",
	"filetime",
	"passfile",
	nullptr
} ; /* end array */

static vars		var ;


/* exported variables */


/* exported subroutines */

int configfile_start(CF *op,cchar *cfn) noex {
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = configfile_ctor(op,cfn)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (cfn[0]) ylikely {
		if (static cint rsv = var ; (rs = rsv) >= 0) ylikely {
		    starter so(op,cfn) ;
		    rs = so ;
		    c = rs ;
		} /* end if (vars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		cofigfile_dtor(op) ;
	    } /* end if (errro) */
        } /* end if (configfile_ctor) */
	return (rs >= 0) ? ? c : rs ;
} /* end subroutine (conigfile_start) */

starter::operator int () noex {
    	cnothrow	nt{} ;
    	int		rs = SR_NOMEM ;
	int		rs1 ;
	if (dlp = new(nt) vecstr ; dlp) {
	    if (elp = new(nt) vecstr ; elp) {
	        if (ulp = new(nt) vecstr ; ulp) {
    		    cint	vn = 10 ;
		    cint	vo = 0 ;
	            if ((rs = dlp->startop(vn,vo)) >= 0) {
	                if ((rs = elp->startop(vn,vo)) >= 0) {
	                    if ((rs = ulp->startop(vn,vo)) >= 0) {
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
	            delete ulp ;
	            ulp = nullptr ;
	        } /* end if (m-a-f) */
	        delete elp ;
	        elp = nullptr ;
	    } /* end if (m-a-f) */
	    delete dlp ;
	    dlp = nullptr ;
	} /* end if (m-a-f) */
	return rs ;
} /* end method (starter::operator) */

int starter::buf() noex {
    	cnothrow	nt{} ;
    	int		rs = SR_NOMEM ;
	int		rs1 ;
	if (bop = new(nt) buffer ; bop) {
	    {

	    }
	    delete bop ;
	    bop = nullptr ;
	} /* end if (m-a-f) */
	return rs ;
} /* end method (starter::bufoperator) */

int configfile_starts(CF *csp,cchar *cfn) noex {
	buffer		options ;
	field		fsb ;
	bfile		cfile, *cfp = &cfile ;
	vecstr		*vsp ;

	int	rs = SR_OK ;
	int	rs1 ;
	int	i ;
	int	c, len1, len ;
	int	bl, cl ;
	int	fl ;
	int	line = 0 ;
	int	noptions = 0 ;

	cchar	*fp ;

	char	linebuf[LINEBUFLEN + 1] ;
	char	buf[BUFLEN + 1] ;
	char	buf2[BUFLEN + 1] ;
	char	*bp, *cp ;

	if (csp == nullptr)
	    return SR_FAULT ;

	/* initialize */

	rs = vecstr_start(op->dlp,10,0) ;
	if (rs < 0)
	    goto bad0 ;

	rs = vecstr_start(op->ulp,10,0) ;
	if (rs < 0)
	    goto bad1 ;

	rs = vecstr_start(op->elp,10,0) ;
	if (rs < 0)
	    goto bad2 ;

/* buffer initialization */

	rs = buffer_start(&options,-1) ;
	if (rs < 0)
		goto bad3 ;

/* open configuration file */

	rs = bopen(cfp,configfname,"r",0664) ;
	if (rs < 0)
	    goto ret1 ;

/* start processing the configuration file */

	while ((rs = breadln(cfp,linebuf,LINEBUFLEN)) > 0) {

	    len = rs ;
	    line += 1 ;
	    if (len == 1) continue ;	/* blank line */

	    if (linebuf[--len] != '\n') {

#ifdef	COMMENT
	        f_trunc = true ;
#endif
	        while ((c = bgetc(cfp)) >= 0)
	            if (c == '\n') break ;

	        continue ;
	    }

	    if ((len == 0) || (linebuf[0] == '#'))
	        continue ;

	    if ((rs = field_start(&fsb,linebuf,len)) >= 0) {

	    	fl = field_get(&fsb,fterms,&fp) ;

/* convert key to lower case */

	    bl = MIN(fl,BUFLEN) ;
	    strncpylc(buf,fp,bl) ;

	    if ((i = matpstr(configkeys,1,buf,bl)) >= 0) {
	        switch (i) {
	        case configkey_root:
	        case configkey_tmpdir:
	        case configkey_log:
	        case configkey_workdir:
	        case configkey_pidfile:
	        case configkey_lockfile:
	        case configkey_user:
	        case configkey_group:
	        case configkey_port:
	        case configkey_userpass:
	        case configkey_machpass:
	        case configkey_srvtab:
	        case configkey_sendmail:
	        case configkey_mintexec:
	        case configkey_envfile:
	        case configkey_pathfile:
	        case configkey_logsize:
	        case configkey_organization:
	        case configkey_timeout:
	        case configkey_removemul:
	        case configkey_acctab:
	        case configkey_paramfile:
	        case configkey_paramtab:
	        case configkey_nrecips:
	        case configkey_helpfile:
	        case configkey_pingtab:
	        case configkey_pingstat:
	        case configkey_interval:
	        case configkey_stampdir:
	        case configkey_maxjobs:
	        case configkey_directory:
	        case configkey_interrupt:
	        case configkey_polltime:
	        case configkey_filetime:
	        case configkey_passfile:
	        case configkey_eigenfile:
	            fl = field_get(&fsb,fterms,&fp) ;

	            if (fl > 0) {
	                bp = mallocstrw(fp,fl) ;
		    } else {
	                bp = mallocstrw(buf,0) ;
		    }

	            switch (i) {
	            case configkey_root:
	                if (op->root != nullptr)
	                    uc_free(op->root) ;

	                op->root = bp ;
	                break ;

	            case configkey_log:
	                if (op->logfname != nullptr)
	                    uc_free(op->logfname) ;

	                op->logfname = bp ;
	                break ;

	            case configkey_tmpdir:
	                if (op->tmpdir != nullptr)
	                    uc_free(op->tmpdir) ;

	                op->tmpdir = bp ;
	                break ;

	            case configkey_workdir:
	                if (op->workdir != nullptr)
	                    uc_free(op->workdir) ;

	                op->workdir = bp ;
	                break ;

	            case configkey_user:
	                if (op->user != nullptr)
	                    uc_free(op->user) ;

	                op->user = bp ;
	                break ;

	            case configkey_group:
	                if (op->group != nullptr)
	                    uc_free(op->group) ;

	                op->group = bp ;
	                break ;

	            case configkey_pidfile:
	                if (op->pidfname != nullptr)
	                    uc_free(op->pidfname) ;

	                op->pidfname = bp ;
	                break ;

	            case configkey_lockfile:
	                if (op->lockfname != nullptr)
	                    uc_free(op->lockfname) ;

	                op->lockfname = bp ;
	                break ;

	            case configkey_port:
	                if (op->port != nullptr)
	                    uc_free(op->port) ;

	                op->port = bp ;
	                break ;

	            case configkey_userpass:
	                if (op->userpass != nullptr)
	                    uc_free(op->userpass) ;

	                op->userpass = bp ;
	                break ;

	            case configkey_machpass:
	                if (op->machpass != nullptr)
	                    uc_free(op->machpass) ;

	                op->machpass = bp ;
	                break ;

	            case configkey_srvtab:
	                if (op->srvtab != nullptr)
	                    uc_free(op->srvtab) ;

	                op->srvtab = bp ;
	                break ;

	            case configkey_sendmail:
	            case configkey_mintexec:
	                if (op->sendmail != nullptr)
	                    uc_free(op->sendmail) ;

	                op->sendmail = bp ;
	                break ;

	            case configkey_envfile:
	                if (op->envfname != nullptr)
	                    uc_free(op->envfname) ;

	                op->envfname = bp ;
	                break ;

	            case configkey_pathfile:
	                if (op->pathfname != nullptr)
	                    uc_free(op->pathfname) ;

	                op->pathfname = bp ;
	                break ;

	            case configkey_devicefile:
	                if (op->devicefname != nullptr)
	                    uc_free(op->devicefname) ;

	                op->devicefname = bp ;
	                break ;

	            case configkey_seedfile:
	                if (op->seedfname != nullptr)
	                    uc_free(op->seedfname) ;

	                op->seedfname = bp ;
	                break ;

	            case configkey_logsize:
	                if (op->logsize != nullptr)
	                    uc_free(op->logsize) ;

	                op->logsize = bp ;
	                break ;

	            case configkey_organization:
	                if (op->organization != nullptr)
	                    uc_free(op->organization) ;

	                op->organization = bp ;
	                break ;

	            case configkey_timeout:
	                if (op->timeout != nullptr)
	                    uc_free(op->timeout) ;

	                op->timeout = bp ;
	                break ;

	            case configkey_interval:
	                if (op->interval != nullptr)
	                    uc_free(op->interval) ;

	                op->interval = bp ;
	                break ;

	            case configkey_removemul:
	                if (op->removemul != nullptr)
	                    uc_free(op->removemul) ;

	                op->removemul = bp ;
	                break ;

	            case configkey_acctab:
	                if (op->acctab != nullptr)
	                    uc_free(op->acctab) ;

	                op->acctab = bp ;
	                break ;

	            case configkey_paramfile:
	            case configkey_paramtab:
	            case configkey_pingtab:
	                if (op->paramfname != nullptr)
	                    uc_free(op->paramfname) ;

	                op->paramfname = bp ;
	                break ;

	            case configkey_nrecips:
	                if (op->nrecips != nullptr)
	                    uc_free(op->nrecips) ;

	                op->nrecips = bp ;
	                break ;

	            case configkey_helpfile:
	                if (op->helpfname != nullptr)
	                    uc_free(op->helpfname) ;

	                op->helpfname = bp ;
	                break ;

	            case configkey_pingstat:
	                if (op->statfname != nullptr)
	                    uc_free(op->statfname) ;

	                op->statfname = bp ;
	                break ;

	            case configkey_stampdir:
	                if (op->stampdir != nullptr)
	                    uc_free(op->stampdir) ;

	                op->stampdir = bp ;
	                break ;

	            case configkey_maxjobs:
	                if (op->maxjobs != nullptr)
	                    uc_free(op->maxjobs) ;

	                op->maxjobs = bp ;
	                break ;

	            case configkey_directory:
	                if (op->directory != nullptr)
	                    uc_free(op->directory) ;

	                op->directory = bp ;
	                break ;

	            case configkey_interrupt:
	                if (op->interrupt != nullptr)
	                    uc_free(op->interrupt) ;

	                op->interrupt = bp ;
	                break ;

	            case configkey_polltime:
	                if (op->polltime != nullptr)
	                    uc_free(op->polltime) ;

	                op->polltime = bp ;
	                break ;

	            case configkey_filetime:
	                if (op->filetime != nullptr)
	                    uc_free(op->filetime) ;

	                op->filetime = bp ;
	                break ;

	            case configkey_passfile:
	                if (op->passfname != nullptr)
	                    uc_free(op->passfname) ;

	                op->passfname = bp ;
	                break ;

	            case configkey_eigenfile:
	                if (op->eigenfname != nullptr)
	                    uc_free(op->eigenfname) ;

	                op->eigenfname = bp ;
	                break ;

	            } /* end switch (inner) */

	            break ;

/* options */
	        case configkey_option:
	            while ((fsb.term != '#') &&
	                ((fl = field_get(&fsb,oterms,&fp)) >= 0)) {
	                if (fl > 0) {
	                    if (noptions > 0) {
	                        rs = buffer_chr(&options,',') ;
			    }
			    if (rs >= 0) {
	                        buffer_strw(&options,fp,fl) ;
			    }
	                    noptions += 1 ;
	                }
	            } /* end while */

	            break ;

/* unsets */
	        case configkey_unset:
	            fl = field_get(&fsb,fterms,&fp) ;

	            if (fl > 0)
	                rs = vecstr_add(op->ulp,fp,fl) ;

	            break ;

/* export environment */
	        case configkey_define:
	        case configkey_export:
	            {
	                int	index, f1l, f2l ;
			int	f_equal, f ;
	                char	*f1p, *f2p ;

/* get first part */

	                fl = field_get(&fsb,fterms,&fp) ;

	                if (fl <= 0) {
	                    rs = SR_INVALID ;
	                    op->badline = line ;
	                    break ;
	                }

	                if (fsb.term == '#')
	                    break ;

			f_equal = (fsb.term == '=') ;
	                len1 = fl ;
	                f1p = (char *) fp ;
	                f1l = fl ;

/* get second part */

	                fl = field_get(&fsb,fterms,&fp) ;

	                f2l = 0 ;
	                if (fl >= 0) {
	                    f2p = (char *) fp ;
	                    f2l = fl ;
	                } /* end if */

#if	CF_EXPORTEQUAL
	                f1p[f1l] = '\0' ;
	                if (f2l > 0) {
	                    f2p[f2l] = '\0' ;
	                    rs1 = sncpy3(buf2,BUFLEN,f1p,"=",f2p) ;
	                } else {
	                    rs1 = sncpy2(buf2,BUFLEN,f1p,"=") ;
			}

#else /* CF_EXPORTEQUAL */

	                f1p[f1l] = '\0' ;
	                if (f2l > 0) {
	                    f2p[f2l] = '\0' ;
	                    rs1 = sncpy3(buf2,BUFLEN,f1p,"=",f2p) ;
			} else if (f_equal) {
	                    rs1 = sncpy2(buf2,BUFLEN,f1p,"=") ;
	                } else {
	                    rs1 = sncpy1(buf2,BUFLEN,f1p) ;
			}

#endif /* CF_EXPORTEQUAL */

/* store it away */

	                if (i == configkey_export) {
	                    vsp = op->elp ;
			} else {
	                    vsp = op->dlp ;
			}

			f = (rs1 > 0) ;

#if	CF_EXPORTEQUAL
			f = f && (strchr(buf2,'=') != nullptr) ;
#endif

			if (f) {

	                    rs = vecstr_add(vsp,buf2,rs1) ;

	                    index = rs ;
	                    if (rs < 0)
	                        break ;

/* if this is an export variable, we do extra stuff */

	                    if (f_equal && (i == configkey_export)) {

/* check for our favorite environment variables */

	                        if (strncmp(buf2,"TMPDIR",len1) == 0) {

	                            if (op->tmpdir != nullptr)
	                                uc_free(op->tmpdir) ;

	                            op->tmpdir = 
	                                mallocstr((op->exports).va[index]) ;

	                        } /* end if (handling TMPDIR specially) */

	                    } /* end if (got an export) */

	                } /* end if */

	            } /* end block */

	            break ;

	        case configkey_loglen:
	            fl = field_get(&fsb,fterms,&fp) ;

	            if ((fl <= 0) ||
	                (cfdecmfi(fp,fl,&op->loglen) < 0)) {

	                op->badline = line ;
	                rs = SR_INVALID ;
	                break ;
	            }

	            break ;

	        case configkey_minwordlen:
	            fl = field_get(&fsb,fterms,&fp) ;

	            if ((fl <= 0) ||
	                (cfdecmfi(fp,fl,&op->minwordlen) < 0)) {

	                op->badline = line ;
	                rs = SR_INVALID ;
	                break ;
	            }

	            break ;

	        case configkey_maxwordlen:
	            fl = field_get(&fsb,fterms,&fp) ;

	            if ((fl <= 0) ||
	                (cfdecmfi(fp,fl,&op->maxwordlen) < 0)) {

	                op->badline = line ;
	                rs = SR_INVALID ;
	                break ;
	            }

	            break ;

	        case configkey_keys:
	            fl = field_get(&fsb,fterms,&fp) ;

	            if ((fl <= 0) ||
	                (cfdecmfi(fp,fl,&op->keys) < 0)) {

	                op->badline = line ;
	                rs = SR_INVALID ;
	                break ;
	            }

	            break ;

	        default:
	            rs = SR_NOTSUP ;
	            break ;

	        } /* end switch */

	    } /* end if (valid key) */
	    field_finish(&fsb) ;
	    } /* end if */
	    if (rs < 0) break ;
	} /* end while (reading lines) */
	bclose(cfp) ;

/* load up the options if we got any */

	if ((rs >= 0) && (noptions > 0)) {

	    cl = buffer_get(&options,&cp) ;

	    if (cl > 0)
	        op->options = mallocstrw(cp,cl) ;

	} /* end if (options) */

/* done with configuration file processing */

	if (rs >= 0)
	    op->magval = CONFIGFILE_MAGIC ;

ret1:
	buffer_finish(&options) ;

	if (rs < 0)
		goto bad3 ;

ret0:
	return rs ;

/* handle bad things */
bad3:
	vecstr_finish(op->elp) ;

bad2:
	vecstr_finish(op->ulp) ;

bad1:
	vecstr_finish(op->dlp) ;

bad0:
	goto ret0 ;
} /* end subroutine (configfile_start) */

int configfile_finish(CF *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = configfile_magic(op)) >= 0) {
	        /* free up the complex data types */
		vecstr_finish(op->dlp) ;
		vecstr_finish(op->ulp) ;
		vecstr_finish(op->elp) ;
		/* free up the simple ones */
		checkfree(&op->root) ;
		checkfree(&op->tmpdir) ;
		checkfree(&op->pidfname) ;
		checkfree(&op->lockfname) ;
		checkfree(&op->logfname) ;
		checkfree(&op->workdir) ;
		checkfree(&op->port) ;
		checkfree(&op->user) ;
		checkfree(&op->group) ;
		checkfree(&op->userpass) ;
		checkfree(&op->machpass) ;
		checkfree(&op->srvtab) ;
		checkfree(&op->sendmail) ;
		checkfree(&op->envfname) ;
		checkfree(&op->pathfname) ;
		checkfree(&op->devicefname) ;
		checkfree(&op->seedfname) ;
		checkfree(&op->logsize) ;
		checkfree(&op->organization) ;
		checkfree(&op->timeout) ;
		checkfree(&op->removemul) ;
		checkfree(&op->acctab) ;
		checkfree(&op->paramfname) ;
		checkfree(&op->nrecips) ;
		checkfree(&op->helpfname) ;
		checkfree(&op->statfname) ;
		checkfree(&op->options) ;
		checkfree(&op->interval) ;
		checkfree(&op->stampdir) ;
		checkfree(&op->maxjobs) ;
		checkfree(&op->directory) ;
		checkfree(&op->interrupt) ;
		checkfree(&op->polltime) ;
		checkfree(&op->filetime) ;
		checkfree(&op->passfname) ;
		checkfree(&op->eigenfname) ;
		op->magval = 0 ;
		{
		    rs1 = configfile_dtor(op) ;
		    if (rs >= 0) rs = rs1 ;
		}
        } /* end if (configfile_magic) */
	return rs ;
} /* end subroutine (configfile_finish) */


/* local subroutines */

local int configfile_initvars() noex {
	op->srs = 0 ;
	op->badline = -1 ;
	op->loglen = -1 ;
	op->minwordlen = -1 ;
	op->maxwordlen = -1 ;
	op->keys = -1 ;
	return SR_OK ;
} /* end subroutine */

local void checkfree(char **vp) noex {
	if (*vp) {
	    mem.free(*vp) ;
	    *vp = nullptr ;
	}
} /* end subroutine (checkfree) */

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
	        if ((rs = fieldterms(fterms,f,"\v\f\r\n !#':=") >= 0) {
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


