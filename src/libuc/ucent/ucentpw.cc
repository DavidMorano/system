/* ucentpw SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

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
	These subroutines facilitate read-nnly access to the the
	system PASSWD database.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<new>			/* C++STD |nothrow(3c++)| */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<intceil.h>		/* LIBU */
#include	<storeitem.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<vechand.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<six.h>			/* LIBUC */
#include	<cfdec.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"ucgetpw.h"		/* |uc_getpw{x}(3uc)| */
#include	"ucentpw.h"
#include	"ucentxx.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	SI		storeitem


/* imported namespaces */

using ucent::si_copystr ;		/* local group support subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int ucentpw_parseone(ucentpw *,SI *,int,cchar *,int) noex ;
local int ucentpw_parsedefs(ucentpw *,SI *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int ucentpw::parse(char *pwbuf,int pwlen,cc *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if ( pwbuf && sp) ylikely {
	    PASSWD *pep = this ;
	    memclear(pep) ;
	    if (sl < 0) sl = lenstr(sp) ;
	    if (storeitem si ; (rs = si.start(pwbuf,pwlen)) >= 0) ylikely {
	        int	fi = 0 ;
	        for (int idx ; (idx = sichr(sp,sl,':')) >= 0 ; ) ylikely {
	            rs = ucentpw_parseone(this,&si,fi++,sp,idx) ;
	            sl -= (idx +1) ;
	            sp += (idx +1) ;
	            if (rs < 0) break ;
	        } /* end for */
	        if ((rs >= 0) && sl && sp[0]) {
	            rs = ucentpw_parseone(this,&si,fi++,sp,sl) ;
	        }
	        if (rs >= 0) ylikely {
	            rs = ucentpw_parsedefs(this,&si,fi) ;
		    fi = rs ;
	        }
	        if ((rs >= 0) && (fi < 6)) {
		    rs = SR_BADFMT ;
		}
	        rs1 = si.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ucentpw::parse) */

int ucentpw::load(char *pwbuf,int pwlen,const ucentpw *spwp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (pwbuf && spwp) ylikely {
	    PASSWD *pep = this ;
	    *pep = *spwp ;	/* <- copy over opaque values */
	    if (storeitem si ; (rs = si.start(pwbuf,pwlen)) >= 0) ylikely {
		{
	            si_copystr(&si,&pw_name,spwp->pw_name) ;
	            si_copystr(&si,&pw_passwd,spwp->pw_passwd) ;
	            si_copystr(&si,&pw_gecos,spwp->pw_gecos) ;
	            si_copystr(&si,&pw_dir,spwp->pw_dir) ;
	            si_copystr(&si,&pw_shell,spwp->pw_shell) ;
		} /* end block */
	        rs1 = si.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ucentpw::load) */

int ucentpw::format(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && rbuf) ylikely {
	    if (sbuf b ; (rs = b.start(rbuf,rlen)) >= 0) ylikely {
	        for (int i = 0 ; i < 7 ; i += 1) {
	            if (i > 0) rs = b.chr(':') ;
	            if (rs >= 0) ylikely {
	                switch (int v ; i) {
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
} /* end subroutine (ucentpw::format) */

int ucentpw::size() noex {
	int		rs = SR_OK ;
	    int		sz = 1 ;
	    if (pw_name) {
	        sz += (lenstr(pw_name) + 1) ;
	    }
	    if (pw_passwd) {
	        sz += (lenstr(pw_passwd) + 1) ;
	    }
	    if (pw_gecos) {
	        sz += (lenstr(pw_gecos) + 1) ;
	    }
	    if (pw_dir) {
	        sz += (lenstr(pw_dir) + 1) ;
	    }
	    if (pw_shell) {
	        sz += (lenstr(pw_shell) + 1) ;
	    }
	    rs = sz ;
	return rs ;
} /* end subroutine (ucentpw::size) */

int ucentpw::getent(char *pwbuf,int pwlen) noex {
    	return uc_getpwent(this,pwbuf,pwlen) ;
} /* end method */

int ucentpw::getnam(char *pwbuf,int pwlen,cchar *name) noex {
    	return uc_getpwnam(this,pwbuf,pwlen,name) ;
} /* end method */

int ucentpw::getuid(char *pwbuf,int pwlen,uid_t uid) noex {
    	return uc_getpwuid(this,pwbuf,pwlen,uid) ;
} /* end method */


/* local subroutines */

local int ucentpw_parseone(ucentpw *pwp,SI *sip,int fi,cc *vp,int vl) noex {
	int		rs = SR_OK ;
	cchar		**vpp = nullptr ;
	switch (int v = -1 ; fi) {
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
	} /* end if */
	return rs ;
} /* end subroutine (ucentpw_parseone) */

local int ucentpw_parsedefs(ucentpw *pwp,storeitem *sip,int sfi) noex {
	int		rs = SR_OK ;
	if (sfi == 6) {
	    cchar	**vpp = ccharpp(&pwp->pw_shell) ;
	    cchar	*sp = pwp->pw_name ;
	    cchar	*vp ;
	    vp = (sp + lenstr(sp)) ;
	    sfi += 1 ;
	    rs = sip->strw(vp,0,vpp) ;
	} /* end if */
	return (rs >= 0) ? sfi : rs ;
} /* end subroutine (ucentpw_parsedefs) */


