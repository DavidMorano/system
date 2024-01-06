/* envs_subs SUPPORT */
/* lang=C++20 */

/* process the cookie substitutions for environment variables */
/* version %I% last-modified %G% */

#define	CF_EXPAND	0		/* perform expand-cookie */

/* revision history:

	= 1998-09-10, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	envs_subs

	Description:
	This subroutine takes raw strings that are supposed to be
	environment variables and processes any cookie substitutions
	that are found in those strings. It also cleans up those
	environment variables that are supposed to be directory
	paths of some kind.

	Synopsis:
	int envs_subs(envs *nlp,expcook *clp,vecstr *pvp,vecstr *evp) noex

	Arguments:
	nlp		new-list-pointer, new (forming) environment list
	clp		cookies list pointer
	pvp		pointer to VECSTR of path-vars
	evp		pointer to VECSTR of exportable environment

	Returns:
	>=0		count of environment variables
	<0		bad (system-return)

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<mallocxx.h>
#include	<vecstr.h>
#include	<buffer.h>
#include	<sbuf.h>
#include	<expcook.h>
#include	<strn.h>
#include	<pathclean.h>
#include	<localmisc.h>

#include	"envs.h"


/* local defines */

#define	EBUFLEN		1024		/* environment-variable buf-length */

#ifndef	DEFNPATHS
#define	DEFNPATHS	20
#endif

#define	SUBINFO		struct subinfo


/* local namespaces */

using std::min ;
using std::max ;


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int vecstr_envadd(vecstr *,cchar *,cchar *,int) noex ;
}


/* external variables */


/* local structures */

struct subinfo {
	envs		*nlp ;
	expcook		*clp ;
	vecstr		*pvp ;
	vecstr		*evp ;
} ;

struct vars {
	int		maxpathlen ;
} ;


/* forward references */

static int	procvar(subinfo *,cchar *,int) noex ;
static int	procvarnorm(subinfo *,cchar *,int) noex ;
static int	procvarpath(subinfo *,cchar *,int) noex ;
static int	procpathsplit(subinfo *,vecstr *,cchar *,int) noex ;
static int	procsub(subinfo *,buffer *,cchar *,int) noex ;
static int	pathjoin(vecstr *,char *,int) noex ;

static int	mkvars() noex ;


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;

static vars		var ;


/* exported subroutines */

int envs_subs(envs *nlp,expcook *clp,vecstr *pvp,vecstr *evp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (nlp && clp && pvp && evp) {
	    static int	srs = mkvars() ;
	    if ((rs = srs) >= 0) {
	        SUBINFO		si, *sip = &si ;
	        envs_cur	cur ;
	        sip->evp = evp ;
	        sip->clp = clp ;
	        sip->pvp = pvp ;
	        sip->nlp = nlp ;
	        if ((rs = envs_curbegin(nlp,&cur)) >= 0) {
	            int		kl ;
	            cchar	*kp{} ;
	            while (rs >= 0) {
	                kl = envs_enumkey(nlp,&cur,&kp) ;
	                if (kl == SR_NOTFOUND) break ;
	                rs = kl ;
	                if (rs >= 0) {
	                    c += 1 ;
	                    rs = procvar(sip,kp,kl) ;
	                }
	            } /* end while */
	            rs1 = envs_curend(nlp,&cur) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (cursor) */
	    } /* end if (mkvars) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (envs_subs) */


/* local subroutines */

static int procvar(subinfo *sip,cchar *kp,int kl) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if ((rs1 = vecstr_findn(sip->pvp,kp,kl)) >= 0) {
	    rs = procvarpath(sip,kp,kl) ;
	} else if (rs1 == SR_NOTFOUND) {
	    rs = procvarnorm(sip,kp,kl) ;
	} else {
	    rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (procvar) */

static int procvarnorm(subinfo *sip,cchar *kp,int kl) noex {
	buffer		b ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = buffer_start(&b,EBUFLEN)) >= 0) {
	    envs_cur	cur ;
	    int		bl ;
	    cchar	*bp ;
	    if ((rs = envs_curbegin(sip->nlp,&cur)) >= 0) {
	        int	vl ;
	        cchar	*vp ;
	        while (rs >= 0) {
	            vl = envs_fetch(sip->nlp,kp,kl,&cur,&vp) ;
	            if (vl == SR_NOTFOUND) break ;
	            rs = vl ;
	            if ((rs >= 0) && (vl > 0)) {
	                if (rs >= 0) {
	                    rs = procsub(sip,&b,vp,vl) ;
			}
	            } /* end if */
	        } /* end while */
	        rs1 = envs_curend(sip->nlp,&cur) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (cursor) */
	    if (rs >= 0) {
	        if ((bl = buffer_get(&b,&bp)) >= 0) {
	            rs = vecstr_envadd(sip->evp,kp,bp,bl) ;
		}
	    } /* end if */
	    rs1 = buffer_finish(&b) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (buffer) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procvarnorm) */

static int procvarpath(subinfo *sip,cchar *kp,int kl) noex {
	vecstr		paths ;
	cint		vo = VECSTR_OCOMPACT ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if ((rs = vecstr_start(&paths,DEFNPATHS,vo)) >= 0) {
	    buffer	b ;
	    cint	bslen = var.maxpathlen ;
	    if ((rs = buffer_start(&b,bslen)) >= 0) {
		envs_cur	cur ;
	        if ((rs = envs_curbegin(sip->nlp,&cur)) >= 0) {
		    int		vl, bl ;
		    cchar	*vp, *bp ;
	            while (rs >= 0) {
	                vl = envs_fetch(sip->nlp,kp,kl,&cur,&vp) ;
	                if (vl == SR_NOTFOUND) break ;
	                rs = vl ;
	                if ((rs >= 0) && (vp[0] != ':')) {
	                    buffer_reset(&b) ;
	                    if ((rs = procsub(sip,&b,vp,vl)) >= 0) {
	                        if ((bl = buffer_get(&b,&bp)) >= 0) {
	                            rs = procpathsplit(sip,&paths,bp,bl) ;
				}
	                    } /* end if */
	                } /* end if */
	            } /* end while */
	            rs1 = envs_curend(sip->nlp,&cur) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (cursor) */
	        if (rs >= 0) {
	            cint	size = vecstr_strsize(&paths) ;
	            cint	npaths = vecstr_count(&paths) ;
	            int		bufl ;
	            char	*bufp{} ;
	            bufl = (size + npaths + 1) ;
	            if ((rs = uc_malloc(bufl,&bufp)) >= 0) {
			cchar	*sp = bufp ;
			int	sl = 0 ;
	                if ((rs = pathjoin(&paths,bufp,bufl)) > 0) {
			    sl = rs ;
			    while (sl && (sp[0] == ':')) {
				sp += 1 ;
				sl -= 1 ;
			    } /* end while */
			    len = sl ;
	                    rs = vecstr_envadd(sip->evp,kp,sp,sl) ;
			} /* end if (pathjoin) */
	                rs1 = uc_free(bufp) ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (allocation) */
	        } /* end if (ok) */
	        rs1 = buffer_finish(&b) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (buffer) */
	    rs1 = vecstr_finish(&paths) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecstr-paths) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (procvarpath) */

static int procpathsplit(subinfo *sip,vecstr *plp,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (sip) {
	    char	*pathbuf{} ;
	    if ((rs = malloc_mp(&pathbuf)) >= 0) {
	        int	pl, cl ;
	        cchar	*tp, *cp ;
	        while ((tp = strnpbrk(vp,vl,":;")) != nullptr) {
	            cp = vp ;
	            cl = (tp - vp) ;
	            c += 1 ;
	            if ((pl = pathclean(pathbuf,cp,cl)) >= 0) {
	                rs = vecstr_adduniq(plp,pathbuf,pl) ;
		    }
	            if ((rs >= 0) && (tp[0] == ';')) {
	                rs = vecstr_adduniq(plp,";",1) ;
	            }
	            vl -= ((tp + 1) - vp) ;
	            vp = (tp + 1) ;
	            if (rs < 0) break ;
	        } /* end while */
	        if ((rs >= 0) && ((c == 0) || (vl > 0))) {
	            c += 1 ;
	            if ((pl = pathclean(pathbuf,vp,(tp - vp))) >= 0) {
	                rs = vecstr_adduniq(plp,pathbuf,pl) ;
	            }
	        } /* end if */
		rs1 = uc_free(pathbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procpathsplit) */

#if	CF_EXPAND
static int procsub(subinfo *sip,buffer *bp,cchar *vp,int vl) noex {
	int		rs ;
	int		rs1 = SR_NOTFOUND ;
	int		i ;
	int		buflen = max(EBUFLEN,vl) ;
	char		*buf = nullptr ;
	char		*p ;
	for (i = 0 ; (rs = uc_malloc(buflen,&p)) >= 0 ; i += 1) {
	    buf = p ;
	    rs1 = expcook_exp(sip->clp,0,buf,buflen,vp,vl) ;
	    if (rs1 >= 0) break ;
	    buflen += EBUFLEN ;
	    uc_free(buf) ;
	    buf = nullptr ;
	} /* end while */
	if ((rs >= 0) && (rs1 > 0)) {
	    rs = buffer_strw(bp,buf,rs1) ;
	}
	if (buf != nullptr) {
	    uc_free(buf) ;
	}
	return rs ;
}
/* end subroutine (procsub) */
#else /* CF_EXPAND */
static int procsub(subinfo *sip,buffer *bp,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	if (sip) {
	    rs = buffer_strw(bp,vp,vl) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (procsub) */
#endif /* CF_EXPAND */

static int pathjoin(vecstr *plp,char jbuf[],int jlen) noex {
	SBUF		b ;
	int		rs ;
	int		c = 0 ;
	int		bl = 0 ;
	int		f_semi = false ;
	if ((rs = sbuf_start(&b,jbuf,jlen)) >= 0) {
	    int		sc ;
	    cchar	*cp ;
	    for (int i = 0 ; vecstr_get(plp,i,&cp) >= 0 ; i += 1) {
	        if (cp) {
	        if (cp[0] != ';') {
	            if (c++ > 0) {
	                if (f_semi) {
	                    f_semi = false ;
	                    sc = ';' ;
	                } else {
	                    sc = ':' ;
			}
	                rs = sbuf_char(&b,sc) ;
	            }
	            if (rs >= 0) {
	                rs = sbuf_strw(&b,cp,-1) ;
		    }
	        } else {
	            f_semi = true ;
		}
		} /* end if (non-null) */
	        if (rs < 0) break ;
	    } /* end for */
	    bl = sbuf_finish(&b) ;
	    if (rs >= 0) rs = bl ;
	} /* end if (sbuf) */

	return (rs >= 0) ? bl : rs ;
}
/* end subroutine (pathjoin) */

static int mkvars() noex {
	int		rs ;
	if ((rs = maxpathlen) >= 0) {
	    var.maxpathlen = rs ;
	}
	return rs ;
}
/* end subroutine (mkvars) */

