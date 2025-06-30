/* ucentpw SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UCENTPW object management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	uc_entpw{x}

	Description:
	These subroutines perform some PASSWD-structure management
	functions.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>			/* |nothrow(3c++)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<storeitem.h>
#include	<sbuf.h>
#include	<vechand.h>
#include	<strn.h>
#include	<sfx.h>
#include	<six.h>
#include	<intceil.h>
#include	<cfdec.h>
#include	<localmisc.h>

#include	"ucentpw.h"
#include	"ucentxx.hh"

import libutil ;

/* local defines */

#define	SI		storeitem


/* imported namespaces */

using ucent::si_copystr ;		/* local group support subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int ucentpw_parseone(ucentpw *,SI *,int,cchar *,int) noex ;
static int ucentpw_parsedefs(ucentpw *,SI *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int ucentpw::parse(char *pwbuf,int pwlen,cc *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && pwbuf && sp) {
	    PASSWD *pep = this ;
	    memclear(pep) ;
	    if (sl < 0) sl = xstrlen(sp) ;
	    if (storeitem si ; (rs = si.start(pwbuf,pwlen)) >= 0) {
	        int		fi = 0 ;
	        for (int idx ; (idx = sichr(sp,sl,':')) >= 0 ; ) {
	            rs = ucentpw_parseone(this,&si,fi++,sp,idx) ;
	            sl -= (idx +1) ;
	            sp += (idx +1) ;
	            if (rs < 0) break ;
	        } /* end for */
	        if ((rs >= 0) && sl && sp[0]) {
	            rs = ucentpw_parseone(this,&si,fi++,sp,sl) ;
	        }
	        if (rs >= 0) {
	            rs = ucentpw_parsedefs(this,&si,fi) ;
		    fi = rs ;
	        }
	        if ((rs >= 0) && (fi < 6)) rs = SR_BADFMT ;
	        rs1 = si.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentpw::parse) */

int ucentpw::load(char *pwbuf,int pwlen,const ucentpw *spwp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && pwbuf && spwp) {
	    PASSWD *pep = this ;
	    *pep = *spwp ;	/* <- copy over opaque values */
	    if (storeitem si ; (rs = si.start(pwbuf,pwlen)) >= 0) {
		{
	            si_copystr(&si,&pw_name,spwp->pw_name) ;
	            si_copystr(&si,&pw_passwd,spwp->pw_passwd) ;
	            si_copystr(&si,&pw_gecos,spwp->pw_gecos) ;
	            si_copystr(&si,&pw_dir,spwp->pw_dir) ;
	            si_copystr(&si,&pw_shell,spwp->pw_shell) ;
		}
	        rs1 = si.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentpw::load) */

int ucentpw::format(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && rbuf) {
	    if (sbuf b ; (rs = b.start(rbuf,rlen)) >= 0) {
	        for (int i = 0 ; i < 7 ; i += 1) {
	            if (i > 0) rs = b.chr(':') ;
	            if (rs >= 0) {
	                int		v ;
	                switch (i) {
	                case 0:
	                    rs = b.str(pw_name) ;
	                    break ;
	                case 1:
	                    rs = b.str(pw_passwd) ;
	                    break ;
	                case 2:
	                    v = int(pw_uid) ;
	                    rs = b.dec(v) ;
	                    break ;
	                case 3:
	                    v = int(pw_gid) ;
	                    rs = b.deci(v) ;
	                    break ;
	                case 4:
	                    rs = b.str(pw_gecos) ;
	                    break ;
	                case 5:
	                    rs = b.str(pw_dir) ;
	                    break ;
	                case 6:
	                    rs = b.strw(pw_shell) ;
	                    break ;
	                } /* end switch */
	            } /* end if */
	            if (rs < 0) break ;
	        } /* end for */
	        rs1 = b.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sbuf) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentpw::format) */

int ucentpw::size() noex {
	int		rs = SR_FAULT ;
	if (this) {
	    int		sz = 1 ;
	    if (pw_name) {
	        sz += (xstrlen(pw_name)+1) ;
	    }
	    if (pw_passwd) {
	        sz += (xstrlen(pw_passwd)+1) ;
	    }
	    if (pw_gecos) {
	        sz += (xstrlen(pw_gecos)+1) ;
	    }
	    if (pw_dir) {
	        sz += (xstrlen(pw_dir)+1) ;
	    }
	    if (pw_shell) {
	        sz += (xstrlen(pw_shell)+1) ;
	    }
	    rs = sz ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentpw::size) */

int ucentpw::getent(char *pwbuf,int pwlen) noex {
    	return uc_getpwent(this,pwbuf,pwlen) ;
}

int ucentpw::getnam(char *pwbuf,int pwlen,cchar *name) noex {
    	return uc_getpwnam(this,pwbuf,pwlen,name) ;
}

int ucentpw::getuid(char *pwbuf,int pwlen,uid_t uid) noex {
    	return uc_getpwuid(this,pwbuf,pwlen,uid) ;
}


/* local subroutines */

static int ucentpw_parseone(ucentpw *pwp,SI *sip,int fi,cc *vp,int vl) noex {
	int		rs = SR_OK ;
	int		v = -1 ;
	cchar		**vpp = nullptr ;
	switch (fi) {
	case 0:
	    vpp = ccharpp(&pwp->pw_name) ;
	    break ;
	case 1:
	    vpp = ccharpp(&pwp->pw_passwd) ;
	    break ;
	case 2:
	    rs = cfdeci(vp,vl,&v) ;
	    pwp->pw_uid = v ;
	    break ;
	case 3:
	    rs = cfdeci(vp,vl,&v) ;
	    pwp->pw_gid = v ;
	    break ;
	case 4:
	    vpp = ccharpp(&pwp->pw_gecos) ;
	    break ;
	case 5:
	    vpp = ccharpp(&pwp->pw_dir) ;
	    break ;
	case 6:
	    vpp = ccharpp(&pwp->pw_shell) ;
	    break ;
	} /* end switch */
	if ((rs >= 0) && vpp) {
	    cchar	*cp{} ;
	    if (int cl ; (cl = sfshrink(vp,vl,&cp)) >= 0) {
	        rs = sip->strw(cp,cl,vpp) ;
	    }
	}
	return rs ;
}
/* end subroutine (ucentpw_parseone) */

static int ucentpw_parsedefs(ucentpw *pwp,storeitem *sip,int sfi) noex {
	int		rs = SR_OK ;
	if (sfi == 6) {
	    cchar	**vpp = ccharpp(&pwp->pw_shell) ;
	    cchar	*sp = pwp->pw_name ;
	    cchar	*vp ;
	    vp = (sp + xstrlen(sp)) ;
	    sfi += 1 ;
	    rs = sip->strw(vp,0,vpp) ;
	}
	return (rs >= 0) ? sfi : rs ;
}
/* end subroutine (ucentpw_parsedefs) */


