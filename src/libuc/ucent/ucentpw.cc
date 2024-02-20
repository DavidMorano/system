/* ucentpw SUPPORT */
/* lang=C++20 */

/* UCENTPW object management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	These subroutines perform some PASSWD-structure management
	functions.


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
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


/* local defines */

#define	SI		storeitem


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int ucentpw_parseone(ucentpw *,SI *,int,cchar *,int) noex ;
static int ucentpw_parsedefs(ucentpw *,SI *,int) noex ;

static int si_copystr(SI *,char **,cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int ucentpw::parse(char *pwbuf,int pwlen,cc *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && pwbuf && sp) {
	    rs = SR_INVALID ;
	    memclear(this,sizeof(ucentpw)) ;
	    if (pwlen > 0) {
	        storeitem	ib, *ibp = &ib ;
	        if (sl < 0) sl = strlen(sp) ;
	        if ((rs = storeitem_start(ibp,pwbuf,pwlen)) >= 0) {
	            int		fi = 0 ;
	            int		si ;
	            while ((si = sichr(sp,sl,':')) >= 0) {
	                rs = ucentpw_parseone(this,ibp,fi++,sp,si) ;
	                sl -= (si+1) ;
	                sp += (si+1) ;
	                if (rs < 0) break ;
	            } /* end while */
	            if ((rs >= 0) && sl && sp[0]) {
	                rs = ucentpw_parseone(this,ibp,fi++,sp,sl) ;
	            }
	            if (rs >= 0) {
	                rs = ucentpw_parsedefs(this,ibp,fi) ;
		        fi = rs ;
	            }
	            if ((rs >= 0) && (fi < 6)) rs = SR_BADFMT ;
	            rs1 = storeitem_finish(ibp) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (storeitem) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentpw::parse) */

int ucentpw::load(char *pwbuf,int pwlen,const ucentpw *spwp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && pwbuf && spwp) {
	    rs = SR_INVALID ;
	    if (pwlen > 0) {
	        storeitem	ib ;
	        (*this) = *spwp ;	/* <- copy over opaque values */
	        if ((rs = storeitem_start(&ib,pwbuf,pwlen)) >= 0) {
	            si_copystr(&ib,&pw_name,spwp->pw_name) ;
	            si_copystr(&ib,&pw_passwd,spwp->pw_passwd) ;
	            si_copystr(&ib,&pw_gecos,spwp->pw_gecos) ;
	            si_copystr(&ib,&pw_dir,spwp->pw_dir) ;
	            si_copystr(&ib,&pw_shell,spwp->pw_shell) ;
	            rs1 = storeitem_finish(&ib) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (storeitem) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentpw::load) */

int ucentpw::format(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && rbuf) {
	    rs = SR_INVALID ;
	    if (rlen > 0) {
	        sbuf	b ;
	        if ((rs = sbuf_start(&b,rbuf,rlen)) >= 0) {
	            int	v ;
	            for (int i = 0 ; i < 7 ; i += 1) {
	                if (i > 0) rs = sbuf_char(&b,':') ;
	                if (rs >= 0) {
	                    switch (i) {
	                    case 0:
	                        rs = sbuf_strw(&b,pw_name,-1) ;
	                        break ;
	                    case 1:
	                        rs = sbuf_strw(&b,pw_passwd,-1) ;
	                        break ;
	                    case 2:
	                        v = int(pw_uid) ;
	                        rs = sbuf_deci(&b,v) ;
	                        break ;
	                    case 3:
	                        v = int(pw_gid) ;
	                        rs = sbuf_deci(&b,v) ;
	                        break ;
	                    case 4:
	                        rs = sbuf_strw(&b,pw_gecos,-1) ;
	                        break ;
	                    case 5:
	                        rs = sbuf_strw(&b,pw_dir,-1) ;
	                        break ;
	                    case 6:
	                        rs = sbuf_strw(&b,pw_shell,-1) ;
	                        break ;
	                    } /* end switch */
	                } /* end if */
	                if (rs < 0) break ;
	            } /* end for */
	            rs1 = sbuf_finish(&b) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (sbuf) */
	    } /* end if (valid) */	
	} /* endif (non-null) */
	return rs ;
}
/* end subroutine (ucentpw::format) */

int ucentpw::size() noex {
	int		rs = SR_FAULT ;
	if (this) {
	    int		size = 1 ;
	    if (pw_name) {
	        size += (strlen(pw_name)+1) ;
	    }
	    if (pw_passwd) {
	        size += (strlen(pw_passwd)+1) ;
	    }
	    if (pw_gecos) {
	        size += (strlen(pw_gecos)+1) ;
	    }
	    if (pw_dir) {
	        size += (strlen(pw_dir)+1) ;
	    }
	    if (pw_shell) {
	        size += (strlen(pw_shell)+1) ;
	    }
	    rs = size ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentpw::size) */


/* local subroutines */

static int ucentpw_parseone(ucentpw *pwp,SI *ibp,int fi,cc *vp,int vl) noex {
	int		rs = SR_OK ;
	int		v = -1 ;
	cchar		**vpp = nullptr ;
	switch (fi) {
	case 0:
	    vpp = (cchar **) &pwp->pw_name ;
	    break ;
	case 1:
	    vpp = (cchar **) &pwp->pw_passwd ;
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
	    vpp = (cchar **) &pwp->pw_gecos ;
	    break ;
	case 5:
	    vpp = (cchar **) &pwp->pw_dir ;
	    break ;
	case 6:
	    vpp = (cchar **) &pwp->pw_shell ;
	    break ;
	} /* end switch */
	if ((rs >= 0) && vpp) {
	    int		cl ;
	    cchar	*cp ;
	    if ((cl = sfshrink(vp,vl,&cp)) >= 0) {
	        rs = storeitem_strw(ibp,cp,cl,vpp) ;
	    }
	}
	return rs ;
}
/* end subroutine (ucentpw_parseone) */

static int ucentpw_parsedefs(ucentpw *pwp,storeitem *ibp,int sfi) noex {
	int		rs = SR_OK ;
	if (sfi == 6) {
	    cchar	**vpp = (cchar **) &pwp->pw_shell ;
	    cchar	*np = pwp->pw_name ;
	    cchar	*vp ;
	    vp = (np + strlen(np)) ;
	    sfi += 1 ;
	    rs = storeitem_strw(ibp,vp,0,vpp) ;
	}
	return (rs >= 0) ? sfi : rs ;
}
/* end subroutine (ucentpw_parsedefs) */

static int si_copystr(SI *ibp,char **pp,cchar *p1) noex {
	int		rs = SR_OK ;
	cchar		**cpp = (cchar **) pp ;
	*cpp = nullptr ;
	if (p1) {
	    rs = storeitem_strw(ibp,p1,-1,cpp) ;
	}
	return rs ;
}
/* end subroutine (si_copystr) */


