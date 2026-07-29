/* progoutput SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* program-output */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */
#define	CF_DEBUG	0		/* run-time debugging */

/* revision history:

	= 2001-09-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	This subroutine forms the front-end part of a generic PCS
	type of program. This front-end is used in a variety of PCS
	programs.  This code was originally part of the Personal
	Communications Services (PCS) package but can also be used
	independently from it.  Historically, this was developed
	as part of an effort to maintain high function (and reliable)
	email communications in the face of increasingly draconian
	security restrictions imposed on the computers in the
	DEFINITY development organization.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<bits.h>		/* LIBUC */
#include	<logfile.h>		/* LIBUC */
#include	<vechand.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<bfile.h>		/* LIBB */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */
#include	<pinghost.h>		/* LIBDAM */
#include	<pingstatdb.h>		/* LIBDAM */
#include	<pingtab.h>		/* LIBDAM */

#include	"config.h"
#include	"defs.h"
#include	"proglog.h"


/* local defines */

#ifndef	PI
#define	PI	proginfo
#endif
#define	VN	vechand
#define	VS	vecstr
#define	PT	pingtab
#define	PT_ENT	pingtab_ent


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */



/* external subroutines */

extern int	progpingtabadd(PI *,cchar *,int) noex ;
extern int	proghost(PI *,cchar *,int,int) noex ;


/* externals variables */


/* local global variabes */


/* local structures */


/* forward references */

local int	procargs(PI *,ARGINFO *,bits *,vechand *) noex ;
local int	procdefpingtab(PI *) noex ;
local int	prochostsfins(PI *,VH *) noex ;
local int	prochosts(PI *,VH *) noex ;
local int	mungepingtab(PI *,char *,cchar *) noex ;
local int	loadpingtabs(PI *,VH *) noex ;
local int	loadpingtab(PI *,VH *,cchar *) noex ;
local int	loadhost(PI *,VH *,cchar *,int,int,int) noex ;

local int	procout_begin(PI *,bfile *,cchar *) noex ;
local int	procout_end(PI *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int progoutput(PI *pip,ARGINFO *aip,bits *bop) noex {
	bfile		ofile ;
	int		rs ;
	int		rs1 ;
	int		pan = 0 ;
	cchar	*ofn = pip->ofname ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3)) {
	    debugprintf("progoutput: ent defintmin=%d\n",
	        pip->defintminping) ;
	    debugprintf("progoutput: intmin=%d\n",pip->intminping) ;
	}
#endif

	if ((ofn == nullptr) || (ofn[0] == '\0') || (ofn[0] == '-'))
	    ofn = BFILE_STDOUT ;

	if ((rs = procout_begin(pip,&ofile,ofn)) >= 0) {
	    VH	phosts ;
	    if ((rs = vechand_start(&phosts,20,0)) >= 0) {

	        if ((rs = procargs(pip,aip,bop,&phosts)) >= 0) {
	            if ((rs == 0) && pip->fl.update) {
	                    rs = procdefpingtab(pip) ;
	                    pan += rs ;
	            }
	            if (rs >= 0) {
	                if ((rs = loadpingtabs(pip,&phosts)) >= 0) {
	                    rs = prochosts(pip,&phosts) ;
	                }
	            }
	        } /* end if (procargs) */

/* free up the ping-hosts */

	        rs1 = prochostsfins(pip,&phosts) ;
	        if (rs >= 0) rs = rs1 ;

	        rs1 = vechand_finish(&phosts) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (phosts) */
	    rs1 = procout_end(pip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (procout) */

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("progoutput: ret rs=%d pan=%u\n",rs,pan) ;
#endif

	return (rs >= 0) ? pan : rs ;
} /* end subroutine (progoutput) */


/* local subroutines */

local int procargs(PI *pip,ARGINFO *aip,bits *bop,vechand *php) noex {
	cint	defintmin = pip->defintminping ;
	cint	to = pip->toping ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		cl ;
	int		pan = 0 ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;
	cchar		*afn = pip->afname ;
	cchar		*cp ;

	if (rs >= 0) {
	    int	ai ;
	    int	f ;
	    for (ai = 1 ; ai < aip->argc ; ai += 1) {

	        f = (ai <= aip->ai_max) && (bits_test(bop,ai) > 0) ;
	        f = f || ((ai > aip->ai_pos) && (aip->argv[ai] != nullptr)) ;
	        if (f) {
	            cp = aip->argv[ai] ;
	            if (cp[0] != '\0') {
	                pan += 1 ;
	                rs = loadhost(pip,php,cp,-1,defintmin,to) ;
	            }
	        }

	        if (rs < 0) break ;
	    } /* end for (looping through requested circuits) */
	} /* end if (ok) */

/* process any host names that are in the argument filename list file */

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("progoutput: afile arguments rs=%d pan=%u\n",
	        rs,pan) ;
#endif

	if ((rs >= 0) && (afn != nullptr) && (afn[0] != '\0')) {
	    bfile	afile, *afp = &afile ;

	    if (strcmp(afn,"-") == 0) afn = BFILE_STDIN ;

	    if ((rs = bopen(afp,afn,"r",0666)) >= 0) {
	        cint	dto = defintmin ;
	        cint	llen = LINEBUFLEN ;
	        int		len ;
	        char		lbuf[LINEBUFLEN + 1] ;

	        while ((rs = breadln(afp,lbuf,llen)) > 0) {
	            len = rs ;

	            if (lbuf[len - 1] == '\n') len -= 1 ;
	            lbuf[len] = '\0' ;

	            if ((cl = sfshrink(lbuf,len,&cp)) > 0) {
	                if (cp[0] == '#') {
	                    pan += 1 ;
	                    rs = loadhost(pip,php,cp,cl,dto,to) ;
	                }
	            }

	            if (rs < 0) break ;
	        } /* end while (reading lines) */

	        rs1 = bclose(afp) ;
	        if (rs >= 0) rs = rs1 ;
	    } else {
	        fmt = "%s: inaccessible arg-list (%d)\n" ;
	        bprintf(pip->efp,fmt,pn,rs) ;
	        fmt = "%s: afile=%s\n" ;
	        bprintf(pip->efp,fmt,pn,afn) ;
	    } /* end if */

	} /* end if (processing file argument file list) */

	return (rs >= 0) ? pan : rs ;
} /* end subroutine (procargs) */

local int procout_begin(PI *pip,bfile *ofp,cchar *ofname) noex {
	int		rs = SR_OK ;
	if (! pip->fl.nooutput) {
	    if (pip->ofp == nullptr) {
	        if ((rs = bopen(ofp,ofname,"wct",0666)) >= 0) {
	            pip->ofp = ofp ;
	        }
	    }
	}
	return rs ;
} /* end subroutine (procout_begin) */

local int procout_end(PI *pip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (pip->ofp != nullptr) {
	    rs1 = bclose(pip->ofp) ;
	    if (rs >= 0) rs = rs1 ;
	    pip->ofp = nullptr ;
	}
	return rs ;
} /* end subroutine (procout_end) */

local int procdefpingtab(PI *pip) noex {
	int		rs ;
	cchar	*dn = DEFPTFNAME ;

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("progoutput/procdefpingtab: ent\n") ;
#endif

	rs = progpingtabadd(pip,dn,-1) ;

	return rs ;
} /* end subroutine (procdefpingtab) */

/* process all of the ping-hosts */
local int prochosts(PI *pip,VH *phlp) noex {
	PINGHOST	*php ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;
	int		f = true ;

	for (i = 0 ; vechand_get(phlp,i,&php) >= 0 ; i += 1) {
	    if (php != nullptr) {

#if	CF_DEBUG
	        if (DEBUGLEVEL(4)) {
	            debugprintf("progoutput/prochosts: host=%s\n",php->name) ;
	            debugprintf("progoutput/prochosts: ->intminping=%d\n",
	                php->intminping) ;
	            debugprintf("progoutput/prochosts: to=%d\n",php->to) ;
	        }
#endif

	        pip->c_hosts += 1 ;
	        rs1 = proghost(pip,php->name,php->intminping,php->to) ;

#if	CF_DEBUG
	        if (DEBUGLEVEL(4))
	            debugprintf("progoutput/prochosts: proghost() rs=%d\n",
	                rs1) ;
#endif

	        if ((rs1 >= 0) || (rs1 == SR_HOSTDOWN))
	            pip->c_processed += 1 ;

	        if (rs1 >= 0)
	            pip->c_up += 1 ;

	        if (rs1 == SR_HOSTDOWN) {
	            f = false ;
	            pip->fl.hostdown = true ;
	        }

	        if ((rs1 < 0) && (rs1 != SR_HOSTDOWN)) {
	            rs = rs1 ;
		}

#if	CF_DEBUG
	        if (DEBUGLEVEL(4))
	            debugprintf("progoutput/prochosts: processed=%d up=%d\n",
	                pip->c_processed , pip->c_up) ;
#endif

	    }
	    if (rs < 0) break ;
	} /* end for */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progoutput/prochosts: ret rs=%d f=%u\n",rs,f) ;
#endif

	return (rs >= 0) ? f : rs ;
} /* end subroutine (prochosts) */

/* free up all of the ping-hosts */
local int prochostsfins(PI *pip,VH *phlp) noex {
	PINGHOST	*php ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;

	if (pip == nullptr) return SR_FAULT ;
	for (int i = 0 ; vechand_get(phlp,i,&php) >= 0 ; i += 1) {
	    if (php) {
	        c += 1 ;
	        rs1 = pinghost_finish(php) ;
	        if (rs >= 0) rs = rs1 ;
	        rs1 = uc_free(php) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progoutput/prochostsfins: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
} /* end subroutine (prochostsfins) */

/* munge up the pingtab file names */
local int mungepingtab(PI *pip,char *rbuf,cchar *ptfname) noex {
	int		rs ;

	if (strchr(ptfname,'/') == nullptr) {
	    if (ustat sb ; (rs = uc_stat(ptfname,&sb)) >= 0) {
	        rs = mkpath1(rbuf,ptfname) ;
	    } else if (isNotPresent(rs)) {
	        cchar	*sn = pip->progname ;
	        cchar	*etc = ETCDNAME ;
	        rs = mkpath5(rbuf,pip->pr,etc,sn,PTDNAME,ptfname) ;
	    }
	} else {
	    rs = mkpath1(rbuf,ptfname) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progoutput/mungepingtab: ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (mungepingtab) */

/* load the ping-hosts from a pingtab file */
local int loadpingtabs(PI *pip,VH *phlp) {
	VS		*ptp = &pip->pingtabs ;
	int		rs = SR_OK ;
	int		c = 0 ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;
	cchar	*ptname ;
	char		tbuf[MAXPATHLEN + 1] ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4)) {
	    debugprintf("progoutput/loadpingtabs: defintmin=%d\n",
	        pip->defintminping) ;
	    debugprintf("progoutput/loadpingtabs: toping=%d\n", pip->toping) ;
	}
#endif

	for (int i = 0 ; vecstr_get(ptp,i,&ptname) >= 0 ; i += 1) {
	    if (ptname) {

#if	CF_DEBUG
	        if (DEBUGLEVEL(4))
	            debugprintf("progoutput/loadpingtabs: ptname=%s\n",
	                ptname) ;
#endif

	        if ((rs = mungepingtab(pip,tbuf,ptname)) >= 0) {

#if	CF_DEBUG
	            if (DEBUGLEVEL(4))
	                debugprintf("progoutput/loadpingtabs: tbuf=%s\n",
	                    tbuf) ;
#endif

	            if ((rs = loadpingtab(pip,phlp,tbuf)) >= 0) {
	                c += rs ;
	            } else {

#if	CF_DEBUG
	                if (DEBUGLEVEL(3))
	                    debugprintf("progoutput/loadpingtabs: "
				"open-fail (%d)\n", rs) ;
#endif

	                if (pip->debuglevel > 0) {
	                    fmt = "%s: inaccessible ping-tab (%d)\n" ;
	                    bprintf(pip->efp,fmt,pn,rs) ;
	                    fmt = "%s: pt=%s\n" ;
	                    bprintf(pip->efp,fmt,pn,ptname) ;
	                }

	                fmt = "inaccessible pt=%s" ;
	                proglog_printf(pip,fmt,ptname) ;

	            } /* end if (loadpingtab) */

	        } /* end if (mungepingtab) */

	    }
	    if (rs < 0) break ;
	} /* end for */

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("progoutput/loadpingtabs: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
} /* end subroutine (loadpingtabs) */

local int loadpingtab(PI *pip,VH *phlp,cchar *ptfname) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (char *ebuf ; (rs = mem.hostname(&ebuf)) >= 0) {
	    cint elen = rs ;
	    if (PT pt ; (rs = pingtab_open(&pt,ptfname)) >= 0) {
	        PT_ENT	pte ;
	        int		min, to ;
	        cchar	*pn = pip->progname ;
	        cchar	*fmt ;
    
	        if (pip->debuglevel > 0) {
	            fmt = "%s: pt=%s\n" ;
	            bprintf(pip->efp,fmt,pn,ptfname) ;
	        }
    
	        proglog_printf(pip,"pt=%s",ptfname) ;
    
	        pip->c_pingtabs += 1 ;
	        while ((rs = pingtab_read(&pt,&pte)) > 0) {
    
	            min = pte.intminping ;
	            to = pte.timeout ;
    
#if	CF_DEBUG
	            if (DEBUGLEVEL(4))
	                debugprintf("progoutput/loadpingtabs: "
	                    "host=%s min=%d to=%d\n",
	                    pte.hostname,min,to) ;
#endif

	            if (pip->intminping >= 0) {
	                if ((min < 0) || (pip->intminping < min))
	                min = pip->intminping ;
	            }
	            if (min < 0) min = pip->defintminping ;
    
#if	CF_DEBUG
	            if (DEBUGLEVEL(4))
	                debugprintf("progoutput/loadpingtabs: "
	                    "final min=%d\n",min) ;
#endif
    
	            c += 1 ;
	            rs = loadhost(pip,phlp,pte.hostname,-1,min,to) ;
    
	            if (rs < 0) break ;
	        } /* end while */
    
	        rs1 = pingtab_close(&pt) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (pingtab) */
	    rs1 = mem.free(ebuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (loadpingtab) */

/* load a single ping-host */
local int loadhost(PI *pip,VH *phlp,cc *hp,int hl,int min,int to) noex {
	PINGHOST	*php ;
	cint	sz = szof(PINGHOST) ;
	int		rs ;

	if (hp == nullptr) return SR_FAULT ;

	if ((to <= 0) && (pip->toping >= 0))
	    to = pip->toping ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progoutput/loadhost: host=%r min=%d to=%d\n",
	        hp,hl,min,to) ;
#endif

	if ((rs = uc_malloc(sz,&php)) >= 0) {
	    if ((rs = pinghost_start(php,hp,hl,min,to)) >= 0) {
	        rs = vechand_add(phlp,php) ;
	        if (rs < 0) pinghost_finish(php) ;
	    } /* end if (pinghost) */
	    if (rs < 0) uc_free(php) ;
	} /* end if (allocation) */

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("progoutput/loadhost: ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (loadhost) */


