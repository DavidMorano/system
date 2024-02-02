/* prgetprogpath SUPPORT */
/* lang=C++20 */

/* get the path to a program that is used within the PCS system */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine is originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	prgetprogpath

	Description:
	This subroutine is used to find a PCS related program and
	to verify that it is executable.

	Important:
	This subroutine is different from 'pcsgetprog(3pcs)' in
	that this will return a full path of the found program
	whenever it is different than what was supplied.  In contrast,
	the 'pcsgetprog(3pcs)' subroutine only returns the full
	path of the found program when it is not absolute and it
	is found in the PCS distribution.

	Synopsis:
	int prgetprogpath(cchar *pr,char *rbuf,cchar *np,int nl) noex

	Arguments:
	pr		program-root path
	rbuf		returned file path if not the same as input
	np		name
	nl		name-length

	Returns:
	>0		found the program path and this is the length
	==0		program was found w/o a path prefix
	<0		program was not found

	programpath	returned file path if it was not in the PWD

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<vecstr.h>
#include	<ids.h>
#include	<mkpathx.h>
#include	<mkpathxw.h>
#include	<storebuf.h>
#include	<strn.h>
#include	<xperm.h>
#include	<isoneof.h>
#include	<isnot.h>
#include	<localmisc.h>


/* local typedefs */


/* local defines */

#ifndef	VARPATH
#define	VARPATH		"PATH"
#endif

#define	NDF		"prgetprogpath.deb"


/* external subroutines */

extern "C" {
    extern int	pathaddw(char *,int,cchar *,int) noex ;
}


/* external variables */


/* local structures */

struct subinfo {
	cchar		*pr ;
	ids		id ;
	vecstr		dirs ;
	uint		f_dirs:1 ;
	uint		f_changed:1 ;
	uint		f_done:1 ;
} ; /* end struct (subinfo) */


/* forward references */

static int	subinfo_start(subinfo *,cchar *) noex ;
static int	subinfo_xfile(subinfo *,cchar *) noex ;
static int	subinfo_record(subinfo *,cchar *,int) noex ;
static int	subinfo_finish(subinfo *) noex ;

static int	subinfo_tryfull(subinfo *,char *,cchar *,int) noex ;
static int	subinfo_tryroot(subinfo *,char *,cchar *,int) noex ;
static int	subinfo_tryother(subinfo *,char *,cchar *,int) noex ;
static int	subinfo_tryothercheck(subinfo *,cchar *,int,
			char *,cchar *,int) noex ;

static int	mkdfname(char *,cchar *,int,cchar *,int) noex ;
static int	isOverNoEntAcc(int) noex ;


/* local variables */

static int 	(*tries[])(subinfo *,char *,cchar *,int) = {
	subinfo_tryfull,
	subinfo_tryroot,
	subinfo_tryother,
	nullptr
} ;

static cchar	*prbins[] = {
	"bin",
	"sbin",
	nullptr
} ;

static const int	rsentacc[] = {
	SR_NOENT,
	SR_ACCESS,
	SR_OVERFLOW,
	SR_NAMETOOLONG,
	SR_RANGE,
	0
} ;


/* exported subroutines */

int prgetprogpath(cchar *pr,char *rbuf,cchar *np,int nl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rl = 0 ;
	if (pr && rbuf && np) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (np[0]) {
	        subinfo		si{}, *sip = &si ;
	        if (nl < 0) nl = strlen(np) ;
	        while ((nl > 0) && (np[nl-1] == '/')) {
	            sip->f_changed = true ;
	            nl -= 1 ;
	        }
	        if ((rs = subinfo_start(sip,pr)) >= 0) {
	            for (int i = 0 ; tries[i] ; i += 1) {
		        rs = (*tries[i])(sip,rbuf,np,nl) ;
		        rl = rs ;
		        if ((rs != 0) || sip->f_done) break ;
	            } /* end for */
	            if (rs >= 0) {
		        if (rl == 0) {
		            rs = SR_NOENT ;
 		        } else if (rl > 0) {
		            if (! sip->f_changed) rl = 0 ;
		        }
	            }
	            rs1 = subinfo_finish(sip) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (subinfo) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (prgetprogpath) */


/* local subroutines */

static int subinfo_start(subinfo *sip,cchar *pr) noex {
	sip->pr = pr ;
	return ids_load(&sip->id) ;
}
/* end subroutine (subinfo_start) */

static int subinfo_finish(subinfo *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (sip->f_dirs) {
	    sip->f_dirs = false ;
	    rs1 = vecstr_finish(&sip->dirs) ;
	    if (rs >= 0) rs = rs1 ;
	}
	rs1 = ids_release(&sip->id) ;
	if (rs >= 0) rs = rs1 ;
	return rs ;
}
/* end subroutine (subinfo_finish) */

static int subinfo_tryfull(subinfo *sip,char *rbuf,cchar *np,int nl) noex {
	int		rs = SR_OK ;
	int		rl = 0 ;
	if (strnchr(np,nl,'/') != nullptr) {
	    if ((rs = mkpath1w(rbuf,np,nl)) >= 0) {
		rl = rs ;
		rs = subinfo_xfile(sip,rbuf) ;
	        if (isNotPresent(rs)) {
		    rs = SR_OK ;
		    rl = 0 ;
		    sip->f_done = true ;
		}
	    }
	} /* end if (full-path) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (subinbfo_tryfull) */

static int subinfo_tryroot(subinfo *sip,char *rbuf,cchar *np,int nl) noex {
	int		rs = SR_OK ;
	int		i ;
	int		rl = 0 ;
	cchar		*pr = sip->pr ;
	for (i = 0 ; prbins[i] ; i += 1) {
	    rl = 0 ;
	    if ((rs = mkpath2(rbuf,pr,prbins[i])) >= 0) {
	        cint	plen = rs ;
		if ((rs = pathaddw(rbuf,plen,np,nl)) >= 0) {
		    rl = rs ;
	            if ((rs = subinfo_xfile(sip,rbuf)) >= 0) {
			break ;
		    } else if (isOverNoEntAcc(rs)) {
	                rs = subinfo_record(sip,rbuf,rl) ;
			rl = 0 ;
		    }
		}
	    } else if (isOverNoEntAcc(rs)) {
	        rs = SR_OK ;
	    }
	    if (rs < 0) break ;
	} /* end for */
	if ((rs >= 0) && (rl > 0)) {
	    sip->f_changed = true ;
	}
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (subinfo_tryroot) */

static int subinfo_tryother(subinfo *sip,char *rbuf,cchar *np,int nl) noex {
	int		rs = SR_OK ;
	int		rl = 0 ;
	cchar		*sp ;
	if ((sp = getenv(VARPATH)) != nullptr) {
	    cchar	*tp ;
	    while (((tp = strpbrk(sp,":;")) != nullptr) && (rl == 0)) {
	        rs = subinfo_tryothercheck(sip,sp,(tp - sp),rbuf,np,nl) ;
	        rl = rs ;
	        sp = (tp + 1) ;
		if (rs < 0) break ;
	    } /* end while */
	    if ((rs >= 0) && (rl == 0)) {
	        if (sp[0] != '\0') {
	            rs = subinfo_tryothercheck(sip,sp,-1,rbuf,np,nl) ;
	            rl = rs ;
	        }
	    }
	    if ((rs >= 0) && (rl > 0)) {
	        sip->f_changed = true ;
	    }
	} /* end if (getenv-path) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (subinfo_other) */

static int subinfo_tryothercheck(subinfo *sip,cchar *dp,int dl,
		char *rbuf,cchar *np,int nl) noex {
	int		rs = SR_NOENT ;
	int		rl = 0 ;
	if (sip->f_dirs) {
	    rs = vecstr_findn(&sip->dirs,dp,dl) ;
	}
	if (rs == SR_NOENT) {
	    if ((rs = mkdfname(rbuf,dp,dl,np,nl)) >= 0) {
	        rl = rs ;
	        rs = subinfo_xfile(sip,rbuf) ;
	        if ((rs == SR_NOENT) || (rs == SR_ACCESS)) {
	            rs = subinfo_record(sip,rbuf,rl) ;
		    rl = 0 ;
		}
	    } /* end if */
	} /* end if (no-entry) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (subinfo_tryothercheck) */

static int subinfo_xfile(subinfo *sip,cchar *name) noex {
	USTAT		sb ;
	int		rs ;
	if ((rs = u_stat(name,&sb)) >= 0) {
	    rs = SR_NOENT ;
	    if (S_ISREG(sb.st_mode)) {
	        rs = sperm(&sip->id,&sb,X_OK) ;
	    }
	}
	return rs ;
}
/* end subroutine (subinfo_xfile) */

static int subinfo_record(subinfo *sip,cchar *dp,int dl) noex {
	int		rs = SR_OK ;
	if (! sip->f_dirs) {
	    rs = vecstr_start(&sip->dirs,10,0) ;
	    sip->f_dirs = (rs >= 0) ;
	}
	if (rs >= 0) {
	    rs = vecstr_add(&sip->dirs,dp,dl) ;
	}
	return rs ;
}
/* end subroutine (subinfo_record) */

static int mkdfname(char *rbuf,cchar *dp,int dl,cchar *np,int nl) noex {
	const int	rlen = MAXPATHLEN ;
	int		rs = SR_OK ;
	int		i = 0 ;
	if (rs >= 0) {
	    rs = storebuf_strw(rbuf,rlen,i,dp,dl) ;
	    i += rs ;
	}
	if ((rs >= 0) && (i > 0) && (rbuf[i - 1] != '/')) {
	    rs = storebuf_char(rbuf,rlen,i,'/') ;
	    i += rs ;
	}
	if (rs >= 0) {
	    rs = storebuf_strw(rbuf,rlen,i,np,nl) ;
	    i += rs ;
	}
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mkdfname) */

static int isOverNoEntAcc(int rs) noex {
	return isOneOf(rsentacc,rs) ;
}
/* end subroutine (isOverNoEntAcc) */


