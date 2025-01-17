/* tmstrs SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* TM structure strings processing */
/* version %I% last-modified %G% */

#define	CF_THREEYEAR	1		/* use RFC2822 3-digit years */

/* revision history:

	= 1999-05-01, David A­D­ Morano
	This was created along with the DATE object.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	tmstrs{x}

	Description:
	These subroutines provide support for converting various
	date strings to their TM-structure equivalents.

	Notes:
	Note on year calculations for MSG-type year strings:

	RFC2822 says that all three digit years should be interpreted
	as being added to 2000 to get the actual year.  This would
	seem to be an unfortunate choice since the only way that
	three digit years were ever used was due to buggy software
	from the Y2K problem (the year 2000 was represented as 100
	in some time-date software).  Therefore a better interpretation
	of three digit years would have been to add it to 1900 in
	order to get the actual year, but who are we, just dumb
	software designers that make the world work! See the
	compile-time switch CF_THREEYEAR for our options on this.

	Also, RFC2822 says that two digit years less than (whatever)
	about 50 should be interpreted as being added to 2000 to
	get the actual year, while two digit years greater than
	about that should be added to 1900 to get the actual year.
	Prior practice in UNIX® was to use the year 69 or 70 for
	this purpose.  Better compatibility is probably achieved
	using 69, but we go with 70 for avant-gauard reasons!

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<tzfile.h>		/* for |TM_YEAR_BASE| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<usystem.h>
#include	<cfdec.h>
#include	<localmisc.h>


/* local defines */

#define	TWOCHARS(a,b)		(((a) << 8) + (b))

#ifndef	TOUPPER
#define	TOUPPER(c)		((c) & (~ 0x20))
#endif
#ifndef	TOLOWER
#define	TOLOWER(c)		((c) | 0x20)
#endif

#ifndef	CF_THREEYEAR
#define	CF_THREEYEAR	1		/* use RFC2822 3-digit years */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr bool		f_threeyear = CF_THREEYEAR ;


/* exported variables */


/* exported subroutines */

int tmstrsday(cchar *sp,int sl) noex {
	int		rs = SR_INVALID ;
	if (sl < 0) sl = strlen(sp) ;
	if ((sl >= 1) && (sl <= 9)) {
	    switch (TWOCHARS(TOUPPER(sp[0]),TOLOWER(sp[1]))) {
	    case TWOCHARS('S', 'u'):
	        rs = 0 ;
	        break ;
	    case TWOCHARS('M', 'o'):
	        rs = 1 ;
	        break ;
	    case TWOCHARS('T', 'u'):
	        rs = 2 ;
	        break ;
	    case TWOCHARS('W', 'e'):
	        rs = 3 ;
	        break ;
	    case TWOCHARS('T', 'h'):
	        rs = 4 ;
	        break ;
	    case TWOCHARS('F', 'r'):
	        rs = 5 ;
	        break ;
	    case TWOCHARS('S', 'a'):
	        rs = 6 ;
	        break ;
	    } /* end switch */
	} /* end if (possible) */
	return rs ;
}
/* end subroutine (tmstrsday) */

int tmstrsmonth(cchar *sp,int sl) noex {
	int		rs = SR_INVALID ;
	if (sl < 0) sl = strlen(sp) ;
	if (sl >= 3) {
	    switch (TWOCHARS(TOUPPER(sp[0]),TOLOWER(sp[1]))) {
	    case TWOCHARS('J', 'a'):
	        rs = 0 ;
	        break ;
	    case TWOCHARS('F', 'e'):
	        rs = 1 ;
	        break ;
	    case TWOCHARS('M', 'a'):		/* March - May */
	        rs = ((TOLOWER(sp[2]) == 'r') ? 2 : 4) ;
	        break ;
	    case TWOCHARS('A', 'p'):
	        rs = 3 ;
	        break ;
	    case TWOCHARS('J', 'u'):		/* June - July */
	        rs = ((TOLOWER(sp[2]) == 'n') ? 5 : 6) ;
	        break ;
	    case TWOCHARS('A', 'u'):
	        rs = 7 ;
	        break ;
	    case TWOCHARS('S', 'e'):
	        rs = 8 ;
	        break ;
	    case TWOCHARS('O', 'c'):
	        rs = 9 ;
	        break ;
	    case TWOCHARS('N', 'o'):
	        rs = 10 ;
	        break ;
	    case TWOCHARS('D', 'e'):
	        rs = 11 ;
	        break ;
	    } /* end switch */
	} /* end if (possible) */
	return rs ;
}
/* end subroutine (tmstrsmonth) */

/* calclate the year based on the number of digits given */
int tmstrsyear(cchar *sp,int sl) noex {
	int		rs = SR_INVALID ;
	int		year = 0 ;
	if (sl < 0) sl = strlen(sp) ;
	if ((sl >= 1) && (sl <= 5)) {
	    if ((rs = cfdeci(sp,sl,&year)) >= 0) {
	        switch (sl) {
	        case 1:
	            year += 100 ;
	            break ;
	        case 2:
	            if (year < 70) year += 100 ;
	            break ;
	        case 3:
		    if_constexpr (f_threeyear) {
	                year += 100 ;
		    } else {
	                if (year < 70) year += 100 ;
		    }
	            break ;
	        case 4:
	        case 5:
	            year -= TM_YEAR_BASE ;
	            break ;
	        } /* end switch */
	    } /* end if (cfdec) */
	} /* end if (possible) */
	return (rs >= 0) ? year : rs ;
}
/* end subroutine (tmstrsyear) */


