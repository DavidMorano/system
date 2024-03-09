/* getax SUPPORT */
/* lang=C++20 */

/* UNIX® System databases (ucentpw, SHADOW, ucentgr, and ucentpj) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	These subroutines were written so that we could use a single
	interface to access the 'passwd' database on all UNIX®
	platforms. This code module provides a platform independent
	implementation of UNIX® 'passwd' database access subroutines.

	These are the preferred interfaces:

	preferred interfaces: getpw_name(), getpw_uid() ;
	preferred interfaces: getgr_name(), getgr_gid() ;

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>

#include	"getax.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int		rsn = SR_NOTFOUND ;


/* exported variables */


/* exported subroutines */

int getpw_begin() noex {
	return uc_getpwbegin() ;
}
/* end subroutine (getpw_begin) */

int getpw_end() noex {
	return uc_getpwend() ;
}
/* end subroutine (getpw_end) */

int getpw_ent(ucentpw *pwp,char *pwbuf,int pwlen) noex {
	int		rs ;
	if ((rs = uc_getpwent(pwp,pwbuf,pwlen)) == rsn) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (getpw_ent) */

int getpw_name(ucentpw *pwp,char *pwbuf,int pwlen,cchar *name) noex {
	return uc_getpwnam(pwp,pwbuf,pwlen,name) ;
}
/* end subroutine (getpw_name) */

int getpw_uid(ucentpw *pwp,char *pwbuf,int pwlen,uid_t uid) noex {
	return uc_getpwuid(pwp,pwbuf,pwlen,uid) ;
}
/* end subroutine (getpw_uid) */


int getsp_begin() noex {
	return uc_getspbegin() ;
}
/* end subroutine (getsp_begin) */

int getsp_end() noex {
	return uc_getspend() ;
}
/* end subroutine (getsp_end) */

int getsp_ent(ucentsp *spp,char *rbuf,int rlen) noex {
	int		rs ;
	if ((rs = uc_getspent(spp,rbuf,rlen)) == rsn) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (getsp_ent) */

int getsp_name(ucentsp *spp,char *rbuf,int rlen,cchar *name) noex {
	return uc_getspnam(spp,rbuf,rlen,name) ;
}
/* end subroutine (getsp_name) */


int getgr_begin() noex {
	return uc_getgrbegin() ;
}
/* end subroutine (getgr_begin) */

int getgr_end() noex {
	return uc_getgrend() ;
}
/* end subroutine (getgr_end) */

int getgr_ent(ucentgr *grp,char *grbuf,int grlen) noex {
	int		rs ;
	if ((rs = uc_getgrent(grp,grbuf,grlen)) == rsn) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (getgr_ent) */

int getgr_name(ucentgr *grp,char *rbuf,int rlen,cchar *name) noex {
	return uc_getgrnam(grp,rbuf,rlen,name) ;
}
/* end subroutine (getgr_name) */

int getgr_gid(ucentgr *grp,char *rbuf,int rlen,gid_t gid) noex {
	return uc_getgrgid(grp,rbuf,rlen,gid) ;
}
/* end subroutine (getgr_gid) */


int getpj_begin() noex {
	return uc_getpjbegin() ;
}
/* end subroutine (getpj_begin) */

int getpj_end() noex {
	return uc_getpjend() ;
}
/* end subroutine (getpj_end) */

int getpj_ent(ucentpj *pjp,char *rbuf,int rlen) noex {
	int		rs ;
	if ((rs = uc_getpjent(pjp,rbuf,rlen)) == rsn) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (getpj_ent) */

int getpj_name(ucentpj *pjp,char *rbuf,int rlen,cchar *name) noex {
	return uc_getpjnam(pjp,rbuf,rlen,name) ;
}
/* end subroutine (getpj_name) */

int getpj_pid(ucentpj *pjp,char *rbuf,int rlen,projid_t pid) noex {
	return uc_getpjpid(pjp,rbuf,rlen,pid) ;
}
/* end subroutine (getpj_pid) */

int getpj_def(ucentpj *pjp,char *rbuf,int rlen,cchar *name) noex {
	return uc_getpjdef(pjp,rbuf,rlen,name) ;
}
/* end subroutine (getpj_def) */


