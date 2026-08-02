/* getdefzdata SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* this is supposed to provide an OS-independent time management operation */
/* version %I% last-modified %G% */


/* revision history:

	- 1998-10-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getdefzdata

	Description:
	We return some default time-zone data (name and other).

	Sysnopsis:
	int getdefzdata(defzdata *zip,char *zbuf,int zlen,int isdst) noex

	Arguments:
	zip		DEFZDATA object pointer
	zbuf		time-zone-name abbreviation result buffer pointer
	zlen		time-zone-name abbreviation result buffer length
	isdst		is-daylight-savings-time value

	Returns:
	>=0		length of result buffer
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<ctime>			/* CSTD |time(2)| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usysflag.h>		/* LIBU */
#include	<uctimeconv.h>		/* LIBUC uc_time{xx}(3uc)| */
#include	<bufsizevar.hh>		/* LIBUC |zn| */
#include	<snwcpy.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"getdefzdata.h"


/* local defines */


/* external subroutines */


/* external varaibles */


/* local structures */


/* forward references */


/* local variables */

static bufsizevar	znlen(bufsize_zn) ;	/* time-zone-name (abbr) */

constexpr bool		f_darwin	= F_DARWIN ;
constexpr bool		f_linux		= F_LINUX ;


/* exported variables */


/* exported subroutines */

int getdefzdata(defzdata *zip,char *zbuf,int zlen,int isdst) noex {
	int		rs = SR_FAULT ;
	int		znl = 0 ;
	(void) isdst ;			/* <- ?? */
	if (zip && zbuf) ylikely {
	    cchar	*zp{} ;
	    if_constexpr (f_darwin || f_linux) {
		custime		dt = time(nullptr) ;
		if (TM tmo ; (rs = uc_timelocal(&dt,&tmo)) >= 0) ylikely {
	            zip->zoff = intconv(tmo.tm_gmtoff / 60) ;
	            zp = tmo.tm_zone ;
		}
	    } else {
		custime		dt = time(nullptr) ;
		if (TM tmo ; (rs = uc_timelocal(&dt,&tmo)) >= 0) ylikely {
	            zip->zoff = intconv(tmo.tm_gmtoff / 60) ;
	            zp = tmo.tm_zone ;
		}
	    } /* end if_constexpr (f_darwin) */
	    if ((rs >= 0) && zp) ylikely {
		if ((rs = znlen) >= 0) ylikely {
	            rs = snwcpy(zbuf,zlen,zp,rs) ;
		    znl = rs ;
		}
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? znl : rs ;
} /* end subroutine (getdefzdata) */

int defzdata::get(char *zbuf,int zlen,int dst) noex {
	return getdefzdata(this,zbuf,zlen,dst) ;
} /* end method */


