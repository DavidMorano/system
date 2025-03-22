/* ucopenuser SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* open a user file */

#define	CF_UCPWCACHE	1		/* use |ugetpw(3uc)| */

/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine opens a user file. This is a file that is
	relative (under) a user-home directory.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<getax.h>
#include	<ucpwcache.h>		/* |ucpwcache_name(3uc)| */
#include	<getusername.h>
#include	<opensysfs.h>
#include	<snwcpy.h>
#include	<mkpathx.h>
#include	<strn.h>
#include	<strwcpy.h>
#include	<strdcpy.h>
#include	<ischarx.h>
#include	<localmisc.h>


/* local defines */

#if	CF_UCPWCACHE
#define	GETPW_NAME	ucpwcache_name
#else
#define	GETPW_NAME	getpw_name
#endif /* CF_UCPWCACHE */


/* external subroutines */

extern "C" {
    extern int uc_openuserinfo(ucopeninfo *) noex ;
}

extern "C" {
    int	uc_openuserbase(struct ucopeninfo *oip) noex ;
    int	uc_openuserpath(struct ucopeninfo *oip) noex ;
}


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int uc_openuser(cchar *un,cchar *upath,int oflags,mode_t operms,int to) noex {
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;

	if (un == NULL) return SR_FAULT ;
	if (upath == NULL) return SR_FAULT ;
	if (un[0] == '\0') return SR_INVALID ;

	if ((rs = getbufsize(getbufsize_pw)) >= 0) {
	    PASSWD	pw ;
	    cint	pwlen = rs ;
	    char	*pwbuf{} ;
	    if ((rs = uc_malloc((pwlen+1),&pwbuf)) >= 0) {
	        if (un[0] == '-') {
	            rs = getpwusername(&pw,pwbuf,pwlen,-1) ;
	        } else {
	            rs = GETPW_NAME(&pw,pwbuf,pwlen,un) ;
	        }
	        if (rs >= 0) {
		    char	fname[MAXPATHLEN+1] ;
	            if ((rs = mkpath2(fname,pw.pw_dir,upath)) >= 0) {
	                struct ucopeninfo	oi{} ;
	                oi.fname = fname ;
	                oi.oflags = oflags ;
	                oi.operms = operms ;
	                oi.to = to ;
	                rs = uc_openinfo(&oi) ;
			fd = rs ;
	            } /* end if (mkpath) */
	        } /* end if (ok) */
	        rs1 = uc_free(pwbuf) ;
		if (rs >= 0) rs = rs1 ;
		if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	    } /* end if (memory-allocation) */
	} /* end if (getbufsize) */

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (uc_openuser) */

int uc_openuserinfo(struct ucopeninfo *oip) noex {
	int		rs ;
	cchar		*fp = oip->fname ;
	while (fp[0] == '/') fp += 1 ;
	if (fp[0] == '\0') {
	    rs = uc_openuserbase(oip) ;
	} else {
	    rs = uc_openuserpath(oip) ;
	}
	return rs ;
}
/* end subroutine (uc_openuserinfo) */

int uc_openuserbase(struct ucopeninfo *oip) noex {
	cint		w = OPENSYSFS_WUSERHOMES ;
	cint		of = oip->oflags ;
	cint		to = oip->to ;
	return opensysfs(w,of,to) ;
}
/* end subroutine (uc_openuserbase) */

int uc_openuserpath(struct ucopeninfo *oip) noex {
	int		rs = SR_NOENT ;
	int		rs1 ;
	int		ul = -1 ;
	int		fd = -1 ;
	cchar		*fp = oip->fname ;
	cchar		*tp ;
	cchar		*un = NULL ;

	while (fp[0] == '/') fp += 1 ;

	un = fp ;
	if ((tp = strchr(fp,'/')) != NULL) {
	    ul = (tp-fp) ;
	    fp = (tp+1) ;
	} else {
	    fp += strlen(fp) ;
	}

	if (un[0] != '\0') {
	    cint	ulen = USERNAMELEN ;
	    char	ubuf[USERNAMELEN + 1] ;
	    if ((rs = snwcpy(ubuf,ulen,un,ul)) >= 0) {
		if ((rs = getbufsize(getbufsize_pw)) >= 0) {
		    PASSWD	pw ;
		    cint	pwlen = rs ;
		    char	*pwbuf{} ;
		    if ((rs = uc_malloc((pwlen+1),&pwbuf)) >= 0) {
	                if ((rs = GETPW_NAME(&pw,pwbuf,pwlen,ubuf)) >= 0) {
	                    cchar	*ud = pw.pw_dir ;
	                    if (ud[0] != '\0') {
				char	tbuf[MAXPATHLEN + 1] ;
	                        while (fp[0] == '/') fp += 1 ;
	                        if (fp[0] != '\0') {
	                            if ((rs = mkpath2(tbuf,ud,fp)) >= 0) {
	                                oip->fname = tbuf ;
	                                rs = uc_openinfo(oip) ;
					fd = rs ;
	                            }
	                        } else {
	                            const int	of = oip->oflags ;
	                            rs = u_open(ud,of,0666) ;
				    fd = rs ;
	                        }
	                    } else {
	                        rs = SR_NOENT ;
			    }
	                } /* end if (getpw-name) */
		        rs1 = uc_free(pwbuf) ;
			if (rs >= 0) rs = rs1 ;
			if ((rs < 0) && (fd >= 0)) u_close(fd) ;
		    } /* end if (memory-allocation) */
	        } /* end if (getbufsize) */
	    } /* end if (cpy) */
	} else {
	    rs = SR_NOENT ;
	}

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (uc_openuserpath) */


