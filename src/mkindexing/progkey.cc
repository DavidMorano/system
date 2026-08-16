/* progkey SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* process the input files */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */
#define	CF_DEBUG 	0		/* run-time debugging */

/* revision history:

	= 1999-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	progkey

	Description:
	This subroutine processes all of the files given us through
	the program invoation arguments.

	Synopsis:
	int progkey(pip,aip,terms,delimiter,ignchrs,ofname)
	PROGINFO	*pip ;
	ARGINFO		*aip ;
	cuchar		terms[] ;
	cchar		delimiter[], ignchrs[] ;
	cchar		ofname[] ;

	Arguments:
	- pip		program information pointer

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<uclibmem.h>
#include	<ids.h>
#include	<bfile.h>
#include	<field.h>
#include	<hdb.h>
#include	<ptm.h>
#include	<psem.h>
#include	<eigendb.h>
#include	<localmisc.h>

#include	"fsi.h"
#include	"upt.h"
#include	"config.h"
#include	"defs.h"
#include	"progeigen.h"
#include	"mkcmds.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	SUBINFO		struct subinfo

#define	DISP		struct disp_head
#define	DISP_THR	struct disp_thr
#define	DISP_ARGS	struct disp_args


/* typedefs */


/* external subroutines */

#if	CF_DEBUGS || CF_DEBUG
extern int	debugprintf(cchar *,...) ;
extern int	debugprinthexblock(cchar *,int,const void *,int) ;
extern int	strlinelen(cchar *,int,int) ;
#endif

extern int	progkeyer(PROGINFO *,bfile *,PTM *,
			cuchar *,cchar *,cchar *,char *) ;

extern char	*strwcpy(char *,cchar *,int) ;


/* external variables */


/* local structures */

struct subinfo {
	PROGINFO	*pip ;
	ARGINFO		*aip ;
	const uchar	*terms ;
	cchar	*delimiter ;
	cchar	*ignchrs ;
	IDS		id ;
	int		pan ;
} ;

struct disp_args {
	PROGINFO	*pip ;
	DISP		*dop ;
	bfile		*ofp ;
	const uchar	*terms ;
	cchar	*delimiter ;
	cchar	*ignchrs ;
	int		npar ;
} ;

struct disp_thr {
	pthread_t	tid ;
	int		f_active ;
} ;

struct disp_head {
	PROGINFO	*pip ;
	DISP_THR	*threads ;
	DISP_ARGS	a ;		/* arguments */
	FSI		wq ;
	PSEM		wq_sem ;
	PTM		om ;		/* output mutex */
	volatile int	f_exit ;
	volatile int	f_done ;
	int		nthr ;
} ;


/* forward references */

local int	subinfo_start(SUBINFO *,PROGINFO *, ARGINFO *,
			cuchar *,cchar *,cchar *) ;
local int	subinfo_finish(SUBINFO *) ;
local int	subinfo_sendparams(SUBINFO *,bfile *) ;
local int	subinfo_sendparamseigens(SUBINFO *,bfile *) ;
local int	subinfo_sendparamsval(SUBINFO *,bfile *,cchar *,int) ;
local int	subinfo_sendparamsstr(SUBINFO *,bfile *,cchar *,cchar *) ;

local int	subinfo_args(SUBINFO *,DISP *) ;
local int	subinfo_argfile(SUBINFO *,DISP *) ;
local int	subinfo_stdin(SUBINFO *,DISP *) ;
local int	subinfo_procfile(SUBINFO *,DISP *,cchar *) ;

local int	ereport(PROGINFO *,cchar *,int) ;

local int	disp_start(DISP *,DISP_ARGS *) ;
local int	disp_starter(DISP *) ;
local int	disp_addwork(DISP *,cchar *,int) ;
local int	disp_finish(DISP *,int) ;
local int	disp_worker(DISP *) ;


/* local variables */


/* exported subroutines */


int progkey(pip,aip,terms,delimiter,ignchrs,ofname)
PROGINFO	*pip ;
ARGINFO		*aip ;
const uchar	terms[] ;
cchar	delimiter[] ;
cchar	ignchrs[] ;
cchar	ofname[] ;
{
	SUBINFO		si, *sip = &si ;
	int		rs ;
	int		rs1 ;
	int		pan = 0 ;

	if ((rs = subinfo_start(sip,pip,aip,terms,delimiter,ignchrs)) >= 0) {
	    bfile	ofile, *ofp = &ofile ;
	    char	openstr[10 + 1] ;

/* open the output key file */

	    if ((ofname == nullptr) || (ofname[0] == '\0'))
	        ofname = BFILE_STDOUT ;

	    strcpy(openstr,"wc") ;
	    if (pip->fl.append) {
	        strcat(openstr,"a") ;
	    } else {
	        strcat(openstr,"t") ;
	    }

	    if ((rs = bopen(ofp,ofname,openstr,0666)) >= 0) {

#if	CF_DEBUG
	        if (DEBUGLEVEL(2)) {
	            debugprintf( "progkey: ofname=%s\n",ofname) ;
	            debugprintf( "progkey: bopen() rs=%d\n",rs) ;
	        }
#endif

/* output parameters */

	        if ((rs = subinfo_sendparams(sip,ofp)) >= 0) {
	            DISP_ARGS	wa{} ;
	            DISP	disp ;

/* process the arguments */

	            wa.pip = pip ;
	            wa.terms = terms ;
	            wa.delimiter = delimiter ;
	            wa.ignchrs = ignchrs ;
	            wa.ofp = ofp ;
		    wa.npar = pip->npar ;

	            if ((rs = disp_start(&disp,&wa)) >= 0) {
	                if ((rs = subinfo_args(sip,&disp)) >= 0) {
	                    if ((rs = subinfo_argfile(sip,&disp)) >= 0) {
	                        rs = subinfo_stdin(sip,&disp) ;
	                    }
	                }
	                rs1 = disp_finish(&disp,(rs < 0)) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (disp) */

	        } /* end if (subinfo-sendparams) */

	        rs1 = bclose(ofp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (output-opened) */

	    pan = subinfo_finish(sip) ;
	    if (rs >= 0) rs = pan ;
	} /* end if (subinfo) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progkey: ret rs=%d pan=%u\n",rs,pan) ;
#endif

	return (rs >= 0) ? pan : rs ;
}
/* end subroutine (progkey) */


/* local subroutines */

local int subinfo_start(sip,pip,aip,terms,delimiter,ignchrs)
SUBINFO		*sip ;
PROGINFO	*pip ;
ARGINFO		*aip ;
const uchar	terms[] ;
cchar	delimiter[] ;
cchar	ignchrs[] ;
{
	int		rs = SR_OK ;

	memclear(sip) ; /* dangerous */
	sip->pip = pip ;
	sip->aip = aip ;
	sip->terms = terms ;
	sip->delimiter = delimiter ;
	sip->ignchrs = ignchrs ;
	{
	    rs = ids_load(&sip->id) ;
	}
	return rs ;
}
/* end subroutine (subinfo_start) */

local int subinfo_finish(SUBINFO *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		pan = sip->pan ;
	{
	    rs1 = ids_release(&sip->id) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return (rs >= 0) ? pan : rs ;
}
/* end subroutine (subinfo_finish) */


local int subinfo_sendparams(SUBINFO *sip,bfile *ofp)
{
	PROGINFO	*pip = sip->pip ;
	int		rs = SR_OK ;
	int		wlen = 0 ;

	if (pip->fl.optsendparams) {
	    int		i ;
	    int		v ;
	    cchar	*cp ;
	    for (i = 0 ; i < mkcmd_overlast ; i += 1) {
	        cp = mkcmds[i] ;
	        switch (i) {
	        case mkcmd_minwordlen:
	            v = pip->minwordlen ;
	            rs = subinfo_sendparamsval(sip,ofp,cp,v) ;
	            wlen += rs ;
	            break ;
	        case mkcmd_maxwordlen:
	            v = pip->maxwordlen ;
	            rs = subinfo_sendparamsval(sip,ofp,cp,v) ;
	            wlen += rs ;
	            break ;
	        case mkcmd_eigenwords:
	            if (pip->open.eigendb) {
	                rs = subinfo_sendparamseigens(sip,ofp) ;
	                wlen += rs ;
	            }
	            break ;
	        case mkcmd_nkeys:
	            v = pip->maxkeys ;
	            rs = subinfo_sendparamsval(sip,ofp,cp,v) ;
	            wlen += rs ;
	            break ;
	        case mkcmd_tablen:
	            rs = subinfo_sendparamsval(sip,ofp,cp,v) ;
	            wlen += rs ;
	            break ;
	        case mkcmd_sdn:
	            rs = subinfo_sendparamsstr(sip,ofp,cp,pip->sdn) ;
	            wlen += rs ;
	            break ;
	        case mkcmd_sfn:
	            rs = subinfo_sendparamsstr(sip,ofp,cp,pip->sfn) ;
	            wlen += rs ;
	            break ;
	        case mkcmd_lang:
	            if (pip->eigenlang != nullptr) {
			cchar	*elang = pip->eigenlang ;
	                rs = subinfo_sendparamsstr(sip,ofp,cp,elang) ;
	                wlen += rs ;
	            }
	            break ;
	        } /* end switch */
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (subinfo_sendparams) */


local int subinfo_sendparamseigens(SUBINFO *sip,bfile *ofp)
{
	PROGINFO	*pip = sip->pip ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		wlen = 0 ;

	if (pip->open.eigendb) {
	    if ((rs = progeigen_count(pip)) > 0) {
	        cint	ci = mkcmd_eigenwords ;
	        cint	linelen = COLUMNS ;
	        int		llen = 0 ;
	        if ((rs = bprintf(ofp,"-%s",mkcmds[ci])) >= 0) {
	            PROGEIGEN_CUR	ecur ;
	            int			wl ;
	            cchar		*wp ;
	            wlen += rs ;
	            llen += rs ;
	            if ((rs = progeigen_curbegin(pip,&ecur)) >= 0) {

	                while (rs >= 0) {
	                    wl = progeigen_curenum(pip,&ecur,&wp) ;
	                    if (wl == SR_NOTFOUND) break ;
	                    rs = wl ;

	                    if ((rs >= 0) && (wl > 0)) {

	                        if ((wl + 1) > (linelen - llen)) {
	                            llen = 0 ;
	                            rs = bprintf(ofp,"\n") ;
	                            wlen += rs ;
	                            if (rs >= 0) {
	                                rs = bprintf(ofp,"-%s",mkcmds[ci]) ;
	                                wlen += rs ;
	                                llen += rs ;
	                            }
	                        }

	                        if (rs >= 0) {
	                            rs = bprintf(ofp," %r",wp,wl) ;
	                            wlen += rs ;
	                            llen += rs ;
	                        }

	                    } /* end if (have eigen-word) */

	                } /* end while */

	                if ((rs >= 0) && (llen > 0)) {
	                    llen = 0 ;
	                    rs = bprintf(ofp,"\n") ;
	                    wlen += rs ;
	                }

	                rs1 = progeigen_curend(pip,&ecur) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (progeigen-cur) */
	        } /* end if (ok) */
	    } /* end if (progeigen_count) */
	} /* end if (eigen-open) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (subinfo_sendparamseigens) */


local int subinfo_sendparamsval(SUBINFO *sip,bfile *ofp,cchar *cmd,int v)
{
	int		rs = SR_OK ;
	int		wlen = 0 ;

	if (sip == nullptr) return SR_FAULT ;

	if (v >= 0) {
	    rs = bprintf(ofp,"-%s %u\n",cmd,v) ;
	    wlen += rs ;
	}

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (subinfo_sendparamsval) */


local int subinfo_sendparamsstr(SUBINFO *sip,bfile *ofp,cchar *cmd,cchar *s)
{
	int		rs = SR_OK ;
	int		wlen = 0 ;

	if (sip == nullptr) return SR_FAULT ;

	if ((s != nullptr) && (s[0] != '\0')) {
	    rs = bprintf(ofp,"-%s %s\n",cmd,s) ;
	    wlen += rs ;
	}

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (subinfo_sendparamsstr) */


local int subinfo_args(SUBINFO *sip,DISP *dop)
{
	PROGINFO	*pip = sip->pip ;
	ARGINFO		*aip = sip->aip ;
	int		rs = SR_OK ;
	int		ai ;
	int		f ;
	cchar	*cp ;

	if (pip == nullptr) return SR_FAULT ;

	for (ai = 1 ; ai < aip->argc ; ai += 1) {
	    f = (ai <= aip->ai_max) && (bits_test(&aip->pargs,ai) > 0) ;
	    f = f || ((ai > aip->ai_pos) && (aip->argv[ai] != nullptr)) ;
	    if (f) {
	        cp = aip->argv[ai] ;
	        sip->pan += 1 ;
	        rs = subinfo_procfile(sip,dop,cp) ;
	    }
	    if (rs < 0) break ;
	} /* end for */

	return rs ;
}
/* end subroutine (subinfo_args) */


local int subinfo_argfile(SUBINFO *sip,DISP *dop)
{
	PROGINFO	*pip = sip->pip ;
	ARGINFO		*aip = sip->aip ;
	int		rs = SR_OK ;
	int		rs1 ;

	if ((aip->afname != nullptr) && (aip->afname[0] != '\0')) {
	    bfile	afile ;
	    cchar	*afname = aip->afname ;

	    if (afname[0] == '-') afname = BFILE_STDIN ;

	    if ((rs = bopen(&afile,afname,"r",0666)) >= 0) {
	        cint	llen = LINEBUFLEN ;
	        int		cl ;
	        cchar		*cp ;
	        char		lbuf[LINEBUFLEN + 1] ;

	        while ((rs = breadln(&afile,lbuf,llen)) > 0) {
	            int	len = rs ;

	            if (lbuf[len - 1] == '\n') len -= 1 ;
	            lbuf[len] = '\0' ;

	            if ((cl = sfshrink(lbuf,len,&cp)) > 0) {
	                if (cp[0] != '#') {
	                    lbuf[(cp+cl)-lbuf] = '\0' ;
	                    sip->pan += 1 ;
	                    rs = subinfo_procfile(sip,dop,cp) ;
	                }
	            }

	            if (rs < 0) break ;
	        } /* end while (reading lines) */

	        rs1 = bclose(&afile) ;
	        if (rs >= 0) rs = rs1 ;
	    } else if (! pip->fl.quiet) {
	        cchar	*pn = pip->progname ;
	        cchar	*fmt = "%s: inaccessible (%d) afile=%s\n" ;
	        bprintf(pip->efp,fmt,pn,rs,aip->afname) ;
	    } /* end if */

	} /* end if (have) */

	return rs ;
}
/* end subroutine (subinfo_argfile) */


local int subinfo_stdin(SUBINFO *sip,DISP *dop)
{
	int		rs = SR_OK ;

	if (sip->pan == 0) {
	    cchar	*cp = "-" ;
	    sip->pan += 1 ;
	    rs = subinfo_procfile(sip,dop,cp) ;
	}

	return rs ;
}
/* end subroutine (subinfo_stdin) */


local int subinfo_procfile(SUBINFO *sip,DISP *dop,cchar *fname)
{
	PROGINFO	*pip = sip->pip ;
	int		rs = SR_OK ;

/* ignore all files that start w/ a '-' character */

	if (pip->debuglevel > 0) {
	    bprintf(pip->efp,"%s: processing file=%s\n",
	        pip->progname,fname) ;
	}

	if (fname[0] != '-') {
	    USTAT	sb ;
	    if ((rs = u_stat(fname,&sb)) >= 0) {
	        if ((rs = permids(&sip->id,&sb,R_OK)) >= 0) {
	            rs = disp_addwork(dop,fname,-1) ;
	        } else if (isNotAccess(rs)) {
		    if (pip->fl.iacc) {
			if (pip->debuglevel > 0) {
	                    ereport(pip,fname,rs) ;
			}
			rs = SR_OK ;
		    } else {
	                ereport(pip,fname,rs) ;
		    }
	        }
	    } else if (isNotPresent(rs)) {
	        rs = SR_OK ;
	    }
	} /* end if (flie check) */

	return rs ;
}
/* end subroutine (subinfo_procfile) */

local int disp_start(DISP *dop,DISP_ARGS *wap) noex {
	PROGINFO	*pip ;
	int		rs = SR_OK ;

	if (dop == nullptr) return SR_FAULT ;
	if (wap == nullptr) return SR_FAULT ;

	pip = wap->pip ;

	memclear(dop) ; /* dangerous */
	dop->pip = pip ;
	dop->a = *wap ;
	dop->nthr = wap->npar ;

	if ((rs = fsi_start(&dop->wq)) >= 0) {
	    if ((rs = psem_create(&dop->wq_sem,false,0)) >= 0) {
		ptm *omp = &dop->om ;
		if ((rs = omp->create) >= 0) {
		    cint	dsz = (dop->nthr * szof(DISP_THR)) ;
		    if (void *p ; (rs = lm_mall(dsz,&p)) >= 0) {
		        dop->threads = p ;
		        memclear(p,dsz) ;
			rs = disp_starter(dop) ;
		        if (rs < 0) {
			    lm_free(dop->threads) ;
			    dop->threads = nullptr ;
			}
		    } /* end if (m-a) */
		    if (rs < 0) {
			omp->destroy() ;
		    }
		} /* end if (ptm_create) */
		if (rs < 0) {
		    psem_destroy(&dop->wq_sem) ;
		}
	    } /* end if (psem_create) */
	    if (rs < 0) {
		fsi_finish(&dop->wq) ;
	    }
	} /* end if (fsi_start) */

	return rs ;
}
/* end subroutine (disp_start) */

local int disp_starter(DISP *dop) noex {
	uptsub_t	fn = (uptsub_t) disp_worker ;
	pthread_t	tid ;
	int		rs = SR_OK ;
	int		i ;

	for (i = 0 ; (rs >= 0) && (i < dop->nthr) ; i += 1) {
	    if ((rs = uptcreate(&tid,nullptr,fn,dop)) >= 0) {
	        dop->threads[i].tid = tid ;
	        dop->threads[i].f_active = true ;
	    }
	} /* end for */

	if (rs < 0) {
	    cint	n = i ;
	    dop->f_exit = true ;
	    for (i = 0 ; i < n ; i += 1) {
	        psem_post(&dop->wq_sem) ;
	    }
	    for (i = 0 ; i < n ; i += 1) {
	        tid = dop->threads[i].tid ;
	        uptjoin(tid,nullptr) ;
		dop->threads[i].f_active = false ;
	    }
	} /* end if (failure) */

	return rs ;
}
/* end subroutine (disp_starter) */

local int disp_finish(DISP *dop,int f_abort) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;

	if (dop == nullptr) return SR_FAULT ;

	dop->f_done = true ;
	if (f_abort) {
	    dop->f_exit = true ;
	}
	for (i = 0 ; i < dop->nthr ; i += 1) {
	    psem_post(&dop->wq_sem) ;
	}
	if (dop->threads != nullptr) {
	    DISP_THR	*dtp ;
	    pthread_t	tid ;
	    int		trs ;
	    for (i = 0 ; i < dop->nthr ; i += 1) {
		dtp = (dop->threads+i) ;
	        if (dtp->f_active) {
	            dtp->f_active = false ;
	            tid = dtp->tid ;
	            rs1 = uptjoin(tid,&trs) ;
		    if (rs >= 0) rs = rs1 ;
		    if (rs >= 0) rs = trs ;
	        } /* end if (active) */
	    } /* end for */
	    rs1 = lm_free(dop->threads) ;
	    if (rs >= 0) rs = rs1 ;
	    dop->threads = nullptr ;
	} /* end if (threads) */
	{
	    ptm *omp = &dop->om ;
	    rs1 = omp->destroy ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = psem_destroy(&dop->wq_sem) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = fsi_finish(&dop->wq) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (disp_finish) */

local int disp_addwork(DISP *dop,cchar *tagbuf,int taglen) noex {
	PROGINFO	*pip = dop->pip ;
	int		rs ;

	if (pip == nullptr) return SR_FAULT ;

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("disp_addwork: tag=>%r<\n",tagbuf,taglen) ;
#endif

	if ((rs = fsi_add(&dop->wq,tagbuf,taglen)) >= 0) {
	    rs = psem_post(&dop->wq_sem) ;
	}

	return rs ;
}
/* end subroutine (disp_addwork) */


local int disp_worker(DISP *dop) noex {
	PROGINFO	*pip = dop->pip ;
	DISP_ARGS	*wap = &dop->a ;
	PTM		*omp = &dop->om ;
	cint	rlen = MAXPATHLEN ;
	int		rs ;
	int		c = 0 ;
	char		rbuf[MAXPATHLEN + 1] ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4)) {
	    pthread_t	tid = pthread_self() ;
	    debugprintf("progkey/worker: ent tid=%u\n",tid) ;
	}
#endif

	while ((rs = psem_wait(&dop->wq_sem)) >= 0) {
	    if (dop->f_exit) break ;

	    if ((rs = fsi_remove(&dop->wq,rbuf,rlen)) >= 0) {

	        rs = progkeyer(pip,wap->ofp,omp,
	            wap->terms,wap->delimiter,wap->ignchrs,rbuf) ;
	        if (rs > 0) c += 1 ;

	    } else if (rs == SR_NOTFOUND) {
		rs = SR_OK ;
	        if (dop->f_done) break ;
	    } /* end if (work to do) */

	    if (rs < 0) break ;
	} /* end while (server loop) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4)) {
	    pthread_t	tid = pthread_self() ;
	    debugprintf("progkey/worker: ret tid=%u rs=%d c=%u\n",tid,rs,c) ;
	}
#endif

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (disp_worker) */

local int ereport(PROGINFO *pip,cchar *fname,int frs) noex {
	int		rs = SR_OK ;
	if (! pip->fl.quiet) {
	    cchar	*pn = pip->progname ;
	    bprintf(pip->efp,"%s: file-processing error (%d)\n",pn,frs) ;
	    bprintf(pip->efp,"%s: file=%s\n",pn,fname) ;
	}
	return rs ;
}
/* end subroutine (ereport) */


