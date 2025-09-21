/* libmallocxx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-15, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	libmallocxx

	Description:
	This subroutine is similar to |uc_malloc(3uc)| except that
	it takes a string argument and copies it into the newly
	allocated space.

	Synopsis:
	int libmallocxx(int w,char **rpp) noex

	Arguments:
	w		which type of system bffer
	rpp		resulting pointer

	Returns:
	>=0		nominally -> (strlen(string) + 1)
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<sysval.hh>
#include	<getbufsize.h>

#include	"libmallocxx.h"


/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */

extern "C" {
    int lm_mallsys(int w,charpp rpp) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static sysval		pagesize(sysval_ps) ;


/* exported variables */


/* exported subroutines */

int libmalloc_ma(char **rpp) noex {
	cint	w = getbufsize_ma ;
	return lm_mallsys(w,rpp) ;
}
int libmalloc_mn(char **rpp) noex {
	cint	w = getbufsize_mn ;
	return lm_mallsys(w,rpp) ;
}
int libmalloc_mp(char **rpp) noex {
	cint	w = getbufsize_mp ;
	return lm_mallsys(w,rpp) ;
}
int libmalloc_ml(char **rpp) noex {
	cint	w = getbufsize_ml ;
	return lm_mallsys(w,rpp) ;
}
int libmalloc_mm(char **rpp) noex {
	cint	w = getbufsize_mm ;
	return lm_mallsys(w,rpp) ;
}
int libmalloc_nn(char **rpp) noex {
	cint	w = getbufsize_nn ;
	return lm_mallsys(w,rpp) ;
}
int libmalloc_hn(char **rpp) noex {
	cint	w = getbufsize_hn ;
	return lm_mallsys(w,rpp) ;
}
int libmalloc_un(char **rpp) noex {
	cint	w = getbufsize_un ;
	return lm_mallsys(w,rpp) ;
}
int libmalloc_gn(char **rpp) noex {
	cint	w = getbufsize_gn ;
	return lm_mallsys(w,rpp) ;
}
int libmalloc_pn(char **rpp) noex {
	cint	w = getbufsize_pn ;
	return lm_mallsys(w,rpp) ;
}
int libmalloc_pw(char **rpp) noex {
	cint	w = getbufsize_pw ;
	return lm_mallsys(w,rpp) ;
}
int libmalloc_sp(char **rpp) noex {
	cint	w = getbufsize_sp ;
	return lm_mallsys(w,rpp) ;
}
int libmalloc_ua(char **rpp) noex {
	cint	w = getbufsize_ua ;
	return lm_mallsys(w,rpp) ;
}
int libmalloc_gr(char **rpp) noex {
	cint	w = getbufsize_gr ;
	return lm_mallsys(w,rpp) ;
}
int libmalloc_pj(char **rpp) noex {
	cint	w = getbufsize_pj ;
	return lm_mallsys(w,rpp) ;
}
int libmalloc_pr(char **rpp) noex {
	cint	w = getbufsize_pr ;
	return lm_mallsys(w,rpp) ;
}
int libmalloc_nw(char **rpp) noex {
	cint	w = getbufsize_nw ;
	return lm_mallsys(w,rpp) ;
}
int libmalloc_ho(char **rpp) noex {
	cint	w = getbufsize_ho ;
	return lm_mallsys(w,rpp) ;
}
int libmalloc_sv(char **rpp) noex {
	cint	w = getbufsize_sv ;
	return lm_mallsys(w,rpp) ;
}
int libmalloc_fs(char **rpp) noex {
	cint	w = getbufsize_fs ;
	return lm_mallsys(w,rpp) ;
}
int libmalloc_sn(char **rpp) noex {
	cint	w = getbufsize_sn ;
	return lm_mallsys(w,rpp) ;
}
int libmalloc_zn(char **rpp) noex {
	cint	w = getbufsize_zn ;
	return lm_mallsys(w,rpp) ;
}
int libmalloc_zi(char **rpp) noex {
	cint	w = getbufsize_zi ;
	return lm_mallsys(w,rpp) ;
}

int libmalloc_ps(char **rpp) noex {
	int             rs ;
        if ((rs = pagesize) >= 0) {
            cint        sz = (rs + 1) ;
            rs = libmem.vall(sz,rpp) ;
        }
        return rs ;
} /* end if (libmalloc_ps) */

int libmalloc_strw(cchar *sp,int sl,cchar **rpp) noex {
    	return libmem.strw(sp,sl,rpp) ;
}

int libmalloc_free(void *p) noex {
    	return libmem.free(p) ;
}


