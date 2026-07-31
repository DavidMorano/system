/* progopts */

/* get the PCS-wide options */


#define	CF_DEBUGS	0		/* compile-time debugging */
#define	CF_DEBUG	0		/* switchable debug print-outs */


/* revision history:

	= 1998-05-01, David A­D­ Morano
        This code module is new and represents completely new function for this
        program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine parses out options from the environment and arguments.

	Synopsis:

	int progopts(pip,kop)
	PROGINFO	*pip ;
	keyopt		*kop ;

	Arguments:

	pip		pointer to program information
	kop		pointer to keyopt object

	Returns:

	>=0		OK
	<0		some error


*******************************************************************************/


#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>

#include	<usystem.h>
#include	<vecstr.h>
#include	<keyopt.h>
#include	<localmisc.h>

#include	"vmail_config.h"
#include	"defs.h"


/* local defines */


/* external subroutines */

#if	CF_DEBUGS || CF_DEBUG
extern int	debugprintf(const char *,...) ;
extern int	strlinelen(const char *,int,int) ;
#endif


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static const char *akopts[] = {
	"cf",
	"pcspoll",
	"clen",
	"clines",
	"useclen",
	"useclines",
	"getmail",
	"mailget",
	"mailcheck",
	"maildir",
	"folderdir",
	"clock",
	"scanline",
	"scanjump",
	"nextdel",
	"nextmov",
	"shell",
	"winadj",
	"deldup",
	"nosysconf",
	NULL
} ;

enum akopts {
	akopt_cf,
	akopt_pcspoll,
	akopt_clen,
	akopt_clines,
	akopt_useclen,
	akopt_useclines,
	akopt_getmail,
	akopt_mailget,
	akopt_mailcheck,
	akopt_maildir,
	akopt_folderdir,
	akopt_clock,
	akopt_scanline,
	akopt_scanjump,
	akopt_nextdel,
	akopt_nextmov,
	akopt_shell,
	akopt_winadj,
	akopt_deldup,
	akopt_nosysconf,
	akopt_overlast
} ;


/* exported subroutines */


int progopts(PROGINFO *pip,keyopt *kop)
{
	int		rs = SR_OK ;
	int		c = 0 ;
	const char	*cp ;

	if ((cp = getourenv(pip->envv,VAROPTS)) != NULL) {
	    rs = keyopt_loads(kop,cp,-1) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("progopts: processing the options rs=%d\n",rs) ;
#endif

	if (rs >= 0) {
	    keyopt_cur	kcur ;
	    if ((rs = keyopt_curbegin(kop,&kcur)) >= 0) {
		int	oi, v ;
		int	kl, vl ;
		cchar	*kp, *vp ;

	        while (rs >= 0) {
	            kl = keyopt_enumkeys(kop,&kcur,&kp) ;
	            if (kl == SR_NOTFOUND) break ;
	            rs = kl ;
	            if (rs < 0) break ;

	            if ((oi = matpstr(akopts,3,kp,kl)) >= 0) {

	                vl = keyopt_fetch(kop,kp,NULL,&vp) ;

	                switch (oi) {

	                case akopt_cf:
	                    if ((vl > 0) && (! pip->finval.cfname)) {
	                        const char	**vpp = &pip->cfname ;
	                        pip->finval.cfname = TRUE ;
	                        pip->have.cfname = TRUE ;
	                        rs = proginfo_setentry(pip,vpp,vp,vl) ;
	                    }
	                    break ;

	                case akopt_pcspoll:
	                    if (! pip->finval.pcspoll) {
	                        pip->finval.pcspoll = TRUE ;
	                        pip->have.pcspoll = TRUE ;
	                        pip->fl.pcspoll = TRUE ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            pip->fl.pcspoll = (rs > 0) ;
	                        }
	                    }
	                    break ;

	                case akopt_clen:
	                case akopt_useclen:
	                    if (! pip->finval.useclen) {
	                        pip->finval.useclen = TRUE ;
	                        pip->have.useclen = TRUE ;
	                        pip->fl.useclen = TRUE ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            pip->fl.useclen = (rs > 0) ;
	                        }
	                    }
	                    break ;

	                case akopt_clines:
	                case akopt_useclines:
	                    if (! pip->finval.useclines) {
	                        pip->finval.useclines = TRUE ;
	                        pip->have.useclines = TRUE ;
	                        pip->fl.useclines = TRUE ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            pip->fl.useclines = (rs > 0) ;
	                        }
	                    }
	                    break ;

	                case akopt_getmail:
	                case akopt_mailget:
	                    if (! pip->finval.mailget) {
	                        pip->finval.mailget = TRUE ;
	                        pip->have.mailget = TRUE ;
	                        pip->fl.mailget = TRUE ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            pip->fl.mailget = (rs > 0) ;
	                        }
	                    }
	                    break ;

	                case akopt_clock:
	                    if (! pip->finval.clock) {
	                        pip->finval.clock = TRUE ;
	                        pip->have.clock = TRUE ;
	                        pip->fl.clock = TRUE ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            pip->fl.clock = (rs > 0) ;
	                        }
	                    }
	                    break ;

	                case akopt_nextdel:
	                    if (! pip->finval.nextdel) {
	                        pip->finval.nextdel = TRUE ;
	                        pip->have.nextdel = TRUE ;
	                        pip->fl.nextdel = TRUE ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            pip->fl.nextdel = (rs > 0) ;
	                        }
	                    }
	                    break ;

	                case akopt_nextmov:
	                    if (! pip->finval.nextmov) {
	                        pip->finval.nextmov = TRUE ;
	                        pip->have.nextmov = TRUE ;
	                        pip->fl.nextmov = TRUE ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            pip->fl.nextmov = (rs > 0) ;
	                        }
	                    }
	                    break ;

	                case akopt_winadj:
	                    if (! pip->finval.winadj) {
	                        pip->finval.winadj = TRUE ;
	                        pip->have.winadj = TRUE ;
	                        pip->fl.winadj = TRUE ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            pip->fl.winadj = (rs > 0) ;
	                        }
	                    }
	                    break ;

	                case akopt_deldup:
	                    if (! pip->finval.deldup) {
	                        pip->finval.deldup = TRUE ;
	                        pip->have.deldup = TRUE ;
	                        pip->fl.deldup = TRUE ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            pip->fl.deldup = (rs > 0) ;
	                        }
	                    }
	                    break ;

	                case akopt_nosysconf:
	                    if (! pip->finval.nosysconf) {
	                        pip->finval.nosysconf = TRUE ;
	                        pip->have.nosysconf = TRUE ;
	                        pip->fl.nosysconf = TRUE ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            pip->fl.nosysconf = (rs > 0) ;
	                        }
	                    }
	                    break ;

	                case akopt_mailcheck:
	                    if (! pip->finval.mailcheck) {
	                        pip->finval.mailcheck = TRUE ;
	                        pip->have.mailcheck = TRUE ;
	                        if (vl > 0) {
	                            rs = cfdecti(vp,vl,&v) ;
	                            pip->mailcheck = v ;
	                        }
	                    }
	                    break ;

	                case akopt_maildir:
	                    if ((vl > 0) && (pip->maildname == NULL)) {
				const char	**vpp = &pip->maildname ;
	                        rs = proginfo_setentry(pip,vpp,vp,vl) ;
			    }
	                    break ;

	                case akopt_folderdir:
	                    if ((vl > 0) && (pip->folderdname == NULL)) {
	                        const char	**vpp = &pip->folderdname ;
	                        rs = proginfo_setentry(pip,vpp,vp,vl) ;
			    }
	                    break ;

	                case akopt_scanline:
	                    if ((vl > 0) && (! pip->finval.svspec)) {
	                        const char	**vpp = &pip->svspec ;
	                        pip->finval.svspec = TRUE ;
	                        pip->have.svspec = TRUE ;
	                        rs = proginfo_setentry(pip,vpp,vp,vl) ;
	                    }
	                    break ;

	                case akopt_scanjump:
	                    if ((vl > 0) && (! pip->finval.sjspec)) {
	                        const char	**vpp = &pip->sjspec ;
	                        pip->finval.sjspec = TRUE ;
	                        pip->have.sjspec = TRUE ;
	                        rs = proginfo_setentry(pip,vpp,vp,vl) ;
	                    }
	                    break ;

	                case akopt_shell:
	                    if ((vl > 0) && (! pip->finval.shell)) {
	                        const char	**vpp = &pip->prog_shell ;
	                        pip->finval.shell = TRUE ;
	                        pip->have.shell = TRUE ;
	                        rs = proginfo_setentry(pip,vpp,vp,vl) ;
	                    }
	                    break ;

	                } /* end switch */

			c += 1 ;
	            } else
			rs = SR_INVALID ;

	            if (rs < 0) break ;
	        } /* end while */

	        keyopt_curend(kop,&kcur) ;
	    } /* end if (keyopt-cur) */
	} /* end if (ok) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (progopts) */


