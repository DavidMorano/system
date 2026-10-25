/* vt4200 SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* discriminate among VT52x models */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debug print-outs */

/* revision history:

	= 2000-07-19, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	Discrminate among VTxx0 models.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<sncpyx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */


/* local defines */


/* external subroutines */


/* forward references */


/* local variables */

enum names {
	name_vt100,
	name_vt101,
	name_vt102,
	name_vt220,
	name_vt220int,
	name_vt320,
	name_vt320int,
	name_vt420,
	name_vt420int,
	name_vt520,
	name_vt520pc,
	name_vt525,
	name_vt525pc,
	name_overlast
} ; /* end enum */

constexpr cpcchar	names[] = {
	"vt100",
	"vt101",
	"vt102",
	"vt220",
	"vt220int",
	"vt320",
	"vt320int",
	"vt420",
	"vt420int",
	"vt520",
	"vt520pc",
	"vt525",
	"vt525pc",
	NULL
} ; /* end array */


/* exported variables */


/* exported subroutines */

int vt100(char *rbuf,int rlen,ushort *pp, int pl) noex {
	int		rs = SR_OK ;
	int		si = 0 ;
	if (rbuf == NULL) return SR_FAULT ;
	if (pp == NULL) return SR_FAULT ;
	rbuf[0] = 0 ;
	for (int i = 0 ; (i < pl) && (pp[i] != USHORT_MAX) ; i += 1) {
	    if (pp[i] == 2) {
		break ;
	    } else if (pp[i] == 0) {
		si = name_vt101 ;
		break ;
	    }
	} /* end for */
	rs = sncpy1(rbuf,rlen,names[si]) ;
	return rs ;
} /* end subroutine (vt100) */

/* ARGSUSED */
int vt102(char *rbuf,int rlen,ushort *pp, int pl) noex {
	int		si = name_vt102 ;
	return sncpy1(rbuf,rlen,names[si]) ;
} /* end subroutine (vt102) */

int vt220(char *rbuf,int rlen,uint *pp, int pl) noex {
	int		rs = SR_OK ;
	int		si = name_vt220 ;
	if (rbuf == NULL) return SR_FAULT ;
	if (pp == NULL) return SR_FAULT ;
	rbuf[0] = 0 ;
	for (int i = 0 ; (i < pl) && (pp[i] != USHORT_MAX) ; i += 1) {
	    if (pp[i] == 9) {
		si = name_vt220int ;
		break ;
	    }
	} /* end for */
	rs = sncpy1(rbuf,rlen,names[si]) ;
	return rs ;
} /* end subroutine (vt220) */

int vt320(char *rbuf,int rlen,uint *pp, int pl) noex {
	int		rs = SR_OK ;
	int		si = name_vt320 ;
	if (rbuf == NULL) return SR_FAULT ;
	if (pp == NULL) return SR_FAULT ;
	rbuf[0] = 0 ;
	for (int i = 0 ; (i < pl) && (pp[i] != USHORT_MAX) ; i += 1) {
	    if (pp[i] == 9) {
		si = name_vt320int ;
		break ;
	    }
	} /* end for */
	rs = sncpy1(rbuf,rlen,names[si]) ;
	return rs ;
} /* end subroutine (vt320) */

/* ARGSUSED */
/* I am not aware of a way to discriminate out a vt420int from a vt420 */
int vt420(char *rbuf,int rlen,ushort *pp, int pl) noex {
	int		si = name_vt420 ;
	return sncpy1(rbuf,rlen,names[si]) ;
} /* end subroutine (vt102) */

int vt520(char *rbuf,int rlen,uint *pp, int pl) noex {
	int		rs = SR_OK ;
	int		si = name_vt520 ;
	if (rbuf == NULL) return SR_FAULT ;
	if (pp == NULL) return SR_FAULT ;
	rbuf[0] = 0 ;
	for (int i = 0 ; (i < pl) && (pp[i] != USHORT_MAX) ; i += 1) {
	    if (pp[i] == 8) {
		si = name_vt520 ;
		break ;
	    } else if (pp[i] == 22) {
		si = name_vt525 ;
		break ;
	    }
	} /* end for */
	rs = sncpy1(rbuf,rlen,names[si]) ;
	return rs ;
} /* end subroutine (vt520) */


