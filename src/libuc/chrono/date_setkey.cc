/* date_setkey SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* handle parsing a supplied date string using a key format identifier */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* non-switchable debug print-outs */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	Although there was probably something that could have done
	with parsing job (with a different argument syntax) I wrote
	this from scratch (sigh).  Yes, we try and avoid writing
	whatever we can. This subroutine extends the DATE object
	to parse strings with the date type indicated by a key name.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Description:
	This subroutine will parse out a date that has been specified
	with a syntax like:

		type=string

	where:

		type		is a name of a date string type
		string		is the string that is supposed to be the date


	Arguments:
	dp		pointer to DATE object (already initialized)
	datestr		string containing the specified key=value pair
	dlen		length of supplied date string
	nowp		pointer to a timeb structure representing NOW
	zn		time zone name


	Returns:
	>=0		it all worked out OK
	<0		the date could not be parsed (system-returned)

******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<sys/utsname.h>		/* POSIX® */
#include	<sys/timeb.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<csignal>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<baops.h>		/* LIBU */
#include	<field.h>		/* LIBUC */
#include	<date.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */


/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

#define	DATETYPE_CURRENT	0
#define	DATETYPE_NOW		1
#define	DATETYPE_TOUCH		2
#define	DATETYPE_TT		3
#define	DATETYPE_TTOUCH		4
#define	DATETYPE_TOUCHT		5
#define	DATETYPE_LOG		6
#define	DATETYPE_LOGZ		7
#define	DATETYPE_STRDIG		8

constexpr cpcchar	datetypes[] = {
	"current",
	"now",
	"touch",
	"tt",
	"ttouch",
	"toucht",
	"log",
	"logz",
	"strdig",
	nullptr
} ; /* end array */


/* exported variables */


/* exported subroutine */

int date_setkey(dae *dp,char *datestr,int dlen,TIMEB *nowp,cchar *zn) noex {
	int	rs ;
	int	sl, cl ;
	int	tlen ;
	int	ti ;
	cchar	*sp, *cp ;
	cchar	*strtype = nullptr ;

	if (datestr == nullptr) return SR_FAULT ;

/* get the key name first (if it has one) */

	ti = -1 ;
	tlen = 0 ;
	if ((cp = strchr(datestr,'=')) != nullptr) {

	    sp = cp + 1 ;
	    tlen = sfshrink(datestr,(cp - datestr),&strtype) ;

#if	CF_DEBUG
	    eprintf("date_setkey: explicit key=%W\n",strtype,tlen) ;
#endif

	} else {
	    bool f = false ;
	    sp = datestr ;
	    tlen = sfshrink(datestr,-1,&strtype) ;

#if	CF_DEBUG
	    eprintf("date_setkey: possible key=%W\n",strtype,tlen) ;
#endif

	    if ((tolower(strtype[0]) == 'c') ||
	        (tolower(strtype[0]) == 'n'))
	        f = ((ti = matstr(datetypes,strtype,tlen)) >= 0) ;

	    if (! f)
	        tlen = -1 ;

	} /* end if (getting possible strtype) */

#if	CF_DEBUG
	eprintf("date_setkey: ti=%d\n",ti) ;
	eprintf("date_setkey: tlen=%d strtype=%W\n",tlen,strtype,tlen) ;
#endif

/* if there was no type name, assume it was a TOUCHT type date string */

	if (ti < 0) {

	    if (tlen > 0) {

	        ti = matstr(datetypes,strtype,tlen) ;
	        if (ti < 0)
	            return SR_INVALID ;

	    } else {
	        ti = DATETYPE_TOUCHT ;
	    }

	} /* end if (had to find the type index) */

#if	CF_DEBUG
	eprintf("date_setkey: type[%d]=%s\n",ti,datetypes[ti]) ;
#endif

	switch (ti) {

	case DATETYPE_TOUCH:
	    rs = date_settouch(dp,sp,-1) ;

	    break ;

	case DATETYPE_TT:
	case DATETYPE_TTOUCH:
	case DATETYPE_TOUCHT:
	    rs = date_settoucht(dp,sp,-1) ;

	    break ;

	case DATETYPE_LOG:
	case DATETYPE_LOGZ:
	    rs = date_setlogz(dp,sp,-1) ;

	    break ;

	case DATETYPE_STRDIG:
	    rs = date_setstrdig(dp,sp,-1) ;

	    break ;

	case DATETYPE_CURRENT:
	case DATETYPE_NOW:
	    rs = date_settimezn(dp,nowp->time,zn,-1) ;

	    break ;

	} /* end switch */


	return rs ;
}
/* end subroutine (date_setkey) */


