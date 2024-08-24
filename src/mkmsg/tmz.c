/* tmz */

/* time and timezone parsing */


#define	CF_DEBUGS	0		/* non-switchable debug print-outs */	
#define	CF_SAFE		1		/* safe mode */
#define	CF_MULTIZONE	1		/* allow fragmented time zone names */


/* revision history:

	= 1999-05-01, David A�D� Morano

	This was created along with the DATE object.  This code was based on
	older code I wrote to handle the "STRDIG" form for the BBNEWS
	facility.  That goes back in time to possibly as far back as 1992!

	= 2014-07-15, David A�D� Morano

	I laugh sometimes as how long some of these objects (or subroutines)
	last without change.  Yes, most of this code looks pretty bad by
	today's standards.  But to the point, I am enhancing this object
	(specially the 'tmz_std()' subroutine) so that it recognizes a
	date-string without either a time-zone abbreviation or a year but which
	does have a time-zone offset.  In the past, the case of an offset
	without a year was supposed to be an error in conversion.  But
	now-a-days, in an effort to make almost all date-strings work similarly,
	we enhance this subroutine to make this above case allowable.  This
	case never has occurred (because having zone-offsets is so relatively
	new in itself) and really never can happen (because we always now have
	fully qualified date-strings with the only exception possibly having
	the zone-offset excluded).  But we will make this enhancement and just
	to round out the symmetry of the various date-strings.

*/

/* Copyright � 1999 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This object is used to parse date strings that are in ASCII and are
	human-readable.  There are tons of places where ASCII strings that
	represent a date are used.  I will leave it as an exercise for the
	reader to think up some of those!


	Note for STD type dates:

	Note that our 'tmz_std()' will parse both the old style standard date
	strings (a la 'ctime(3c)') as well as the new style date strings
	('date(1)').  An old style standard date string looked like:

		[Mon] Nov 26 21:48[:25] 1999

	The new style standard date string looks like:

		[Mon] Nov 26 21:48[:25] [EST] 1999

	Both are handled.  The newer style, used in newer email envelopes, is
	prefered since it includes the timezone name.

	Updated note:
	It is reported that some envelopes of the form:

		[Mon] Nov 26 21:48[:25] [EST] 1999 [-0500]

	are being used in the world (and particularly on Sun Solaris boxes).
	We also handle this variant.


*******************************************************************************/


#define	TMZ_MASTER	1


#include	<envstandards.h>

#include	<sys/types.h>
#include	<stdlib.h>
#include	<string.h>
#include	<tzfile.h>		/* for TM_YEAR_BASE */

#include	<usystem.h>
#include	<estrings.h>
#include	<char.h>
#include	<field.h>
#include	<tmstrs.h>
#include	<localmisc.h>

#include	"tmz.h"


/* local defines */

#ifndef	NYEARS_CENTURY
#define	NYEARS_CENTURY	100
#endif

#ifndef	CENTURY_BASE
#define	CENTURY_BASE	19
#endif

#ifndef	MKCHAR
#define	MKCHAR(c)	(c & 0xff)
#endif


/* external subroutines */

extern int	nextfield(cchar *,int,cchar **) ;
extern int	siskipwhite(cchar *,int) ;
extern int	sialpha(cchar *,int) ;
extern int	cfdeci(cchar *,int,int *) ;
extern int	hasalldig(cchar *,int) ;
extern int	isalphalatin(int) ;
extern int	isalnumlatin(int) ;
extern int	isdigitlatin(int) ;

#if	CF_DEBUGS
extern int	debugprintf(cchar *,...) ;
extern int	strlinelen(cchar *,int,int) ;
#endif

extern char	*strnwcpy(char *,int,cchar *,int) ;
extern char	*strnchr(cchar *,int,int) ;
extern char	*strdcpy1w(char *,int,cchar *,int) ;


/* local structures */


/* forward references */

static int	tmz_getmonth(TMZ *,cchar *,int) ;
static int	tmz_getday(TMZ *,cchar *,int) ;
static int	tmz_timeparts(TMZ *,cchar *,int) ;
static int	tmz_stdtrailing(TMZ *,cchar *,int) ;
static int	tmz_getzname(TMZ *op,cchar *sp,int sl) ;
static int	tmz_getyear(TMZ *op,cchar *sp,int sl) ;
static int	tmz_getzoff(TMZ *op,cchar *sp,int sl) ;

static int	getzoff(int *,cchar *,int) ;
static int	val(cchar *) ;

static int	isplusminus(int) ;

#if	defined(CF_MULTIZONE) && (CF_MULTIZONE == 0)
static int	isgoodname(cchar *,int) ;
#endif

static cchar	*strnzone(cchar *,int) ;


/* local variables */

static const uchar	tpterms[] = {
	0x00, 0x1F, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x04,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;


/* exported subroutines */


/* format> [Wed] Nov 14 19:24[:04] [EST] [[19]99] [�0400] */
int tmz_std(TMZ *op,cchar *sp,int sl)
{
	int		rs = SR_OK ;
	int		i ;

	if (op == NULL)
	    return SR_FAULT ;

#if	CF_SAFE
	if (sp == NULL)
	    return SR_FAULT ;
#endif

	if (sl < 0)
	    sl = strlen(sp) ;

	memset(op,0,sizeof(TMZ)) ;
	op->zoff = SHORT_MAX ;
	op->st.tm_year = -1 ;
	op->st.tm_wday = -1 ;
	op->st.tm_isdst = -1 ;

	if ((rs >= 0) && ((rs = tmz_getmonth(op,sp,sl)) > 0)) {
	    sp += rs ;
	    sl -= rs ;
	}

	if ((rs >= 0) && ((rs = tmz_getday(op,sp,sl)) > 0)) {
	    sp += rs ;
	    sl -= rs ;
	}

	if ((rs >= 0) && (op->st.tm_mday > 31)) {
	    rs = SR_INVALID ;
	}

	if ((rs >= 0) && ((rs = tmz_timeparts(op,sp,sl)) > 0)) {
	    sp += rs ;
	    sl -= rs ;
	}

	for (i = 0 ; (rs >= 0) && (i < 3) ; i += 1) {
	    rs = tmz_stdtrailing(op,sp,sl) ;
	    if (rs == 0) break ;
	    sp += rs ;
	    sl -= rs ;
	} /* end for */

	if (rs >= 0) rs = strnlen(op->zname,TMZ_ZNAMESIZE) ;

#if	CF_DEBUGS
	debugprintf("tmz_std: ret rs=%d zname=%t\n",
	    rs,op->zname,strnlen(op->zname,TMZ_ZNAMESIZE)) ;
#endif

	return rs ;
}
/* end subroutine (tmz_std) */


/* format> [Weekday,] DD MMM [CC]YY hh:mm[:ss] [�hhmm] [zname] */
int tmz_msg(TMZ *op,cchar *sp,int sl)
{
	int		rs = SR_OK ;
	int		cl ;
	cchar	*tp ;
	cchar	*cp ;

	if (op == NULL)
	    return SR_FAULT ;

#if	CF_SAFE
	if (sp == NULL)
	    return SR_FAULT ;
#endif

	if (sl < 0)
	    sl = strlen(sp) ;

	memset(op,0,sizeof(TMZ)) ;
	op->zoff = SHORT_MAX ;
	op->st.tm_year = -1 ;
	op->st.tm_wday = -1 ;
	op->st.tm_isdst = -1 ;

#if	CF_DEBUGS
	debugprintf("tmz_msg: sl=%d\n",sl) ;
	debugprintf("tmz_msg: >%t<\n",sp,strlinelen(sp,sl,76)) ;
#endif

	if ((tp = strnchr(sp,sl,',')) != NULL) {
	    if ((cl = nextfield(sp,(tp-sp),&cp)) > 0) {
	        rs = tmstrsday(cp,cl) ;
	        op->st.tm_wday = rs ;
	    }
	    sl -= ((tp+1)-sp) ;
	    sp = (tp+1) ;
	}

	if ((rs >= 0) && ((rs = tmz_getday(op,sp,sl)) > 0)) {
	    sp += rs ;
	    sl -= rs ;
	}

	if ((rs >= 0) && ((rs = tmz_getmonth(op,sp,sl)) > 0)) {
	    sp += rs ;
	    sl -= rs ;
	}

	if ((rs >= 0) && (op->st.tm_mday > 31)) {
	    rs = SR_INVALID ;
	}

	if ((rs >= 0) && ((rs = tmz_getyear(op,sp,sl)) > 0)) {
	    sp += rs ;
	    sl -= rs ;
	}

	if ((rs >= 0) && ((rs = tmz_timeparts(op,sp,sl)) > 0)) {
	    sp += rs ;
	    sl -= rs ;
	}

	if ((rs >= 0) && ((rs = tmz_getzoff(op,sp,sl)) > 0)) {
	    sp += rs ;
	    sl -= rs ;
	}

	if ((rs >= 0) && ((rs = tmz_getzname(op,sp,sl)) > 0)) {
	    sp += rs ;
	    sl -= rs ;
	}

	if (rs >= 0) rs = strnlen(op->zname,TMZ_ZNAMESIZE) ;

#if	CF_DEBUGS
	debugprintf("tmz_msg: ret f_zoff=%d zoff=%dm\n",
	    op->f.zoff,op->zoff) ;
	debugprintf("tmz_msg: ret zname=%t\n",
	    op->zname,strnlen(op->zname,TMZ_ZNAMESIZE)) ;
#endif

	return rs ;
}
/* end subroutine (tmz_msg) */


/* convert from a TOUCH (original) format> MMDDhhmm[YY] */
int tmz_touch(TMZ *op,cchar *sp,int sl)
{
	struct tm	*stp ;
	const int	n = 5 ;
	int		rs = SR_OK ;
	int		i = 0 ;

	if (op == NULL)
	    return SR_FAULT ;

#if	CF_SAFE
	if (sp == NULL)
	    return SR_FAULT ;
#endif

	if (sl < 0)
	    sl = strlen(sp) ;

	stp = &op->st ;
	memset(op,0,sizeof(TMZ)) ;
	op->zoff = SHORT_MAX ;
	stp->tm_year = -1 ;
	stp->tm_wday = -1 ;
	stp->tm_isdst = -1 ;

/* skip leading white space */

	while (sl && CHAR_ISWHITE(*sp)) {
	    sp += 1 ;
	    sl -= 1 ;
	}
	while (sl && CHAR_ISWHITE(sp[sl-1])) sl -= 1 ;

#if	CF_DEBUGS
	debugprintf("tmz_touch: stripped string=%t\n",sp,sl) ;
#endif

	if (hasalldig(sp,sl)) {
	    while ((sl >= 2) && (i < n)) {
	        switch (i++) {
	        case 0:
	            stp->tm_mon = (val(sp) - 1) ;
	            break ;
	        case 1:
	            stp->tm_mday = val(sp) ;
	            break ;
	        case 2:
	            stp->tm_hour = val(sp) ;
	            break ;
	        case 3:
	            stp->tm_min = val(sp) ;
	            break ;
	        case 4:
	            stp->tm_year = val(sp) ;
	            break ;
	        } /* end switch */
	        sp += 2 ;
	        sl -= 2 ;
	    } /* end while */
	} else
	    rs = SR_INVALID ;

	if (rs >= 0) {
	    if (i >= n) {
	        op->f.year = TRUE ;
	        if ((stp->tm_year >= 0) && (stp->tm_year <= 38))
	            stp->tm_year += NYEARS_CENTURY ;
	        if (stp->tm_year < 69) rs = SR_INVALID ;
	    } else if (i < (n-1))
	        rs = SR_INVALID ;
	} /* end if */

	return rs ;
}
/* end subroutine (tmz_touch) */


/* convert from a TOUCH-t (new '-t') format> [[CC]YY]MMDDhhmm[.SS] */
int tmz_toucht(TMZ *op,cchar *sp,int sl)
{
	struct tm	*stp ;
	const int	n = 4 ;
	int		rs = SR_OK ;
	int		i = 0 ;
	int		cc = -1 ;
	cchar		*tp ;

	if (op == NULL)
	    return SR_FAULT ;

#if	CF_SAFE
	if (sp == NULL)
	    return SR_FAULT ;
#endif

	if (sl < 0)
	    sl = strlen(sp) ;

	stp = &op->st ;
	memset(op,0,sizeof(TMZ)) ;
	op->zoff = SHORT_MAX ;
	stp->tm_year = -1 ;
	stp->tm_wday = -1 ;
	stp->tm_isdst = -1 ;

/* skip leading white space */

	while (sl && CHAR_ISWHITE(*sp)) {
	    sp += 1 ;
	    sl -= 1 ;
	}
	while (sl && CHAR_ISWHITE(sp[sl-1])) sl -= 1 ;

#if	CF_DEBUGS
	debugprintf("tmz_toucht: s=%t\n",sp,sl) ;
#endif

	if ((tp = strnchr(sp,sl,'.')) != NULL) {
	    cchar	*cp = (tp+1) ;
	    const int	cl = (sl-((tp+1)-sp)) ;
	    if (cl >= 2) {
	        if (isdigit(MKCHAR(*cp))) {
	            stp->tm_sec = val(cp) ;
	        } else
	            rs = SR_INVALID ;
	    }
	    sl = (tp-sp) ;
	} /* end if (tried for seconds) */

#if	CF_DEBUGS
	debugprintf("tmz_toucht: mid1 rs=%d\n",rs) ;
	debugprintf("tmz_toucht: s=%t\n",sp,sl) ;
#endif

	if (rs >= 0) {
	    if (hasalldig(sp,sl)) {

	        if ((rs >= 0) && (sl >= 12)) {
	            cc = val(sp) ;
	            sp += 2 ;
	            sl -= 2 ;
	        } /* end if (CC) */

#if	CF_DEBUGS
	        debugprintf("tmz_toucht: mid2 rs=%d\n",rs) ;
	        debugprintf("tmz_toucht: s=%t\n",sp,sl) ;
#endif

	        if (sl >= 10) {
	            op->f.year = TRUE ;
	            stp->tm_year = val(sp) ;
	            sp += 2 ;
	            sl -= 2 ;
	        } /* end if (YY) */

#if	CF_DEBUGS
	        debugprintf("tmz_toucht: mid3 rs=%d\n",rs) ;
	        debugprintf("tmz_toucht: s=%t\n",sp,sl) ;
#endif

	        while ((sl >= 2) && (i < n)) {
	            switch (i++) {
	            case 0:
	                stp->tm_mon = (val(sp) - 1) ;
	                break ;
	            case 1:
	                stp->tm_mday = val(sp) ;
	                break ;
	            case 2:
	                stp->tm_hour = val(sp) ;
	                break ;
	            case 3:
	                stp->tm_min = val(sp) ;
	                break ;
	            } /* end switch */
	            sp += 2 ;
	            sl -= 2 ;
	        } /* end while */

	    } else
	        rs = SR_INVALID ;
	} /* end if (ok) */

	if (rs >= 0) {
	    if (i >= n) {
	        if (stp->tm_year >= 0) {
	            if (cc >= 0) {
	                cc = ((cc * NYEARS_CENTURY) - TM_YEAR_BASE) ;
	                stp->tm_year += cc ;
	            } else {
	                if ((stp->tm_year >= 0) && (stp->tm_year <= 38))
	                    stp->tm_year += NYEARS_CENTURY ;
	            }
	            if (stp->tm_year < 69) rs = SR_INVALID ;
	        } /* end if (had a year) */
	    } else
	        rs = SR_INVALID ;
	} /* end if */

#if	CF_DEBUGS
	debugprintf("tmz_toucht: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (date_toucht) */


/* format> [[CC]]YYMMDDhhmm[ss][�hhmm][zname] */
int tmz_strdig(TMZ *op,cchar *sp,int sl)
{
	struct tm	*stp ;
	const int	n = 6 ;
	int		rs = SR_OK ;
	int		i = 0 ;
	int		cc = -1 ;
	int		zlen = 0 ;
	cchar		*tp ;

	if (op == NULL)
	    return SR_FAULT ;

#if	CF_SAFE
	if (sp == NULL)
	    return SR_FAULT ;
#endif

	if (sl < 0)
	    sl = strlen(sp) ;

	stp = &op->st ;
	memset(op,0,sizeof(TMZ)) ;
	op->zoff = SHORT_MAX ;
	stp->tm_year = -1 ;
	stp->tm_wday = -1 ;
	stp->tm_isdst = -1 ;

/* skip leading white space */

	while (sl && CHAR_ISWHITE(*sp)) {
	    sp += 1 ;
	    sl -= 1 ;
	}
	while (sl && CHAR_ISWHITE(sp[sl-1])) sl -= 1 ;

#if	CF_DEBUGS
	debugprintf("tmz_strdig: s=%t\n",sp,sl) ;
#endif

	if ((tp = strnzone(sp,sl)) != NULL) {
	    cchar	*cp = tp ;
	    int		cl = (sl-(tp-sp)) ;
	    if ((cl >= 1) && isplusminus(*cp)) {
		int	zol = cl ;
		int	zo ;
		int	si ;
		cchar	*zop = cp ;
		if ((si = sialpha(cp,cl)) > 0) {
		    zol = si ;
		}
		if ((rs = getzoff(&zo,zop,zol)) >= 0) {
			op->zoff = zo ;
			op->f.zoff = TRUE ;
			cp += rs ;
			cl -= rs ;
		}
	    }
	    if ((rs >= 0) && (cl > 0)) {
	        if (isalphalatin(MKCHAR(*cp))) {
		    rs = strnwcpy(op->zname,TMZ_ZNAMESIZE,cp,cl) - op->zname ;
		    zlen = rs ;
	        } else {
	            rs = SR_INVALID ;
	 	}
	    }
	    sl = (tp-sp) ;
	} /* end if (tried for ZOFF and ZNAME) */

#if	CF_DEBUGS
	debugprintf("tmz_strdig: mid1 rs=%d\n",rs) ;
	debugprintf("tmz_strdig: s=%t\n",sp,sl) ;
#endif

	if (rs >= 0) {
	    i = 0 ;
	    if (hasalldig(sp,sl)) {

	        if ((rs >= 0) && (sl >= 14)) {
	            cc = val(sp) ;
	            sp += 2 ;
	            sl -= 2 ;
	        } /* end if (CC) */

#if	CF_DEBUGS
	        debugprintf("tmz_strdig: mid2 rs=%d\n",rs) ;
	        debugprintf("tmz_strdig: s=%t\n",sp,sl) ;
#endif

#ifdef	COMMENT
	        if (sl >= 12) {
	            op->f.year = TRUE ;
	            stp->tm_year = val(sp) ;
	            sp += 2 ;
	            sl -= 2 ;
	        } /* end if (YY) */
#endif /* COMMENT */

#if	CF_DEBUGS
	        debugprintf("tmz_strdig: mid3 rs=%d\n",rs) ;
	        debugprintf("tmz_strdig: s=%t\n",sp,sl) ;
#endif

	        while ((sl >= 2) && (i < n)) {
	            switch (i++) {
	            case 0:
	                stp->tm_year = val(sp) ;
	                break ;
	            case 1:
	                stp->tm_mon = (val(sp) - 1) ;
	                break ;
	            case 2:
	                stp->tm_mday = val(sp) ;
	                break ;
	            case 3:
	                stp->tm_hour = val(sp) ;
	                break ;
	            case 4:
	                stp->tm_min = val(sp) ;
	                break ;
	            case 5:
	                stp->tm_sec = val(sp) ;
	                break ;
	            } /* end switch */
	            sp += 2 ;
	            sl -= 2 ;
	        } /* end while */

	    } else
	        rs = SR_INVALID ;
	} /* end if (ok) */

	if (rs >= 0) {
	    if (i >= (n-1)) {
	        if (stp->tm_year >= 0) {
	            op->f.year = TRUE ;
	            if (cc >= 0) {
	                cc = ((cc * NYEARS_CENTURY) - TM_YEAR_BASE) ;
	                stp->tm_year += cc ;
	            } else {
	                if ((stp->tm_year >= 0) && (stp->tm_year <= 38))
	                    stp->tm_year += NYEARS_CENTURY ;
	            }
	            if (stp->tm_year < 69) rs = SR_INVALID ;
	        } /* end if (had a year) */
	    } else
	        rs = SR_INVALID ;
	} /* end if (ok) */

#if	CF_DEBUGS
	debugprintf("tmz_strdig: ret rs=%d zlen=%u\n",rs,zlen) ;
	debugprintf("tmz_strdig: f_zoff=%u zoff=%d\n",op->f.zoff,op->zoff) ;
	debugprintf("tmz_strdig: zname=%t\n",op->zname,8) ;
#endif

	return (rs >= 0) ? zlen : rs ;
}
/* end subroutine (tmz_strdig) */


/* format> [CC]YYMMDD[_]hhmm[ss][_][zname] */
int tmz_logz(TMZ *op,cchar *sp,int sl)
{
	struct tm	*stp ;
	const int	zsize = TMZ_ZNAMESIZE ;
	int		rs = SR_OK ;
	int		ch ;
	int		i ;

	if (op == NULL) return SR_FAULT ;

#if	CF_SAFE
	if (sp == NULL) return SR_FAULT ;
#endif

	if (sl < 0) sl = strlen(sp) ;

	stp = &op->st ;
	memset(op,0,sizeof(TMZ)) ;
	op->zoff = SHORT_MAX ;
	stp->tm_year = -1 ;
	stp->tm_wday = -1 ;
	stp->tm_isdst = -1 ;

/* skip leading white space */

	while (sl && CHAR_ISWHITE(*sp)) {
	    sp += 1 ;
	    sl -= 1 ;
	}
	while (sl && CHAR_ISWHITE(sp[sl-1])) sl -= 1 ;

#if	CF_DEBUGS
	debugprintf("tmz_logz: stripped string=%t\n",sp,sl) ;
#endif

/* get year */

	if (sl < (5 * 2))
	    rs = SR_INVALID ;

	for (i = 0 ; (rs >= 0) && (i < 6) ; i += 1) {
	    if (! isdigit(MKCHAR(sp[i]))) {
	        rs = SR_INVALID ;
		break ;
	    }
	} /* end for */
	if (rs < 0) goto done ;

	stp->tm_year = 10 * (*sp++ - '0') ;
	stp->tm_year += (*sp++ - '0') ;
	if (stp->tm_year < 70)
	    stp->tm_year += NYEARS_CENTURY ;

	stp->tm_mon = 10 * (*sp++ - '0') ;
	stp->tm_mon += (*sp++ - '0') ;
	stp->tm_mon -= 1 ;		/* for 'mktime' */

	stp->tm_mday = 10 * (*sp++ - '0') ;
	stp->tm_mday += (*sp++ - '0') ;

	sl -= 6 ;
	op->f.year = TRUE ;

/* skip a possible underscore character */

	if (sl && (! isdigit(MKCHAR(*sp)))) {
	    sp += 1 ;
	    sl -= 1 ;
	}

/* continue */

	for (i = 0 ; i < 4 ; i += 1) {
	    if (! isdigit(MKCHAR(sp[i]))) {
	        rs = SR_INVALID ;
		break ;
	    }
	} /* end for */
	if (rs < 0) goto done ;

	stp->tm_hour = 10 * (*sp++ - '0') ;
	stp->tm_hour += (*sp++ - '0') ;

	stp->tm_min = 10 * (*sp++ - '0') ;
	stp->tm_min += (*sp++ - '0') ;

	sl -= 4 ;

/* skip a possible colon also */

	if (sl && (! isdigit(MKCHAR(sp[0])))) {
	    sp += 1 ;
	    sl -= 1 ;
	}

/* the next two characters must be numeric digits */

	if (sl > 0) {

	    if ((! isdigit(MKCHAR(sp[0]))) || (! isdigit(MKCHAR(sp[1])))) {

#if	CF_DEBUGS
	        debugprintf("date_setlogz: next two were not digits\n") ;
#endif

	        rs = SR_INVALID ;
	    }

	    stp->tm_sec = 10 * (*sp++ - '0') ;
	    stp->tm_sec += (*sp++ - '0') ;

	    sl -= 2 ;

	} /* end if (optional seconds field) */
	if (rs < 0) goto done ;

/* skip a possible underscore character */

	ch = MKCHAR(*sp) ;
	if (sl && (! isalphalatin(ch))) {
	    sp += 1 ;
	    sl -= 1 ;
	}

/* get possible timezone name */

	for (i = 0 ; sl && isalphalatin(MKCHAR(*sp)) && (i < zsize) ; i += 1) {
	    op->zname[i] = *sp++ ;
	    sl -= 1 ;
	} /* end for */

	if (i < TMZ_ZNAMESIZE)
	    op->zname[i] = '\0' ;

done:

#if	CF_DEBUGS
	debugprintf("tmz_logz: ret zname=%t\n",
	    op->zname,strnlen(op->zname,TMZ_ZNAMESIZE)) ;
#endif

	return (rs >= 0) ? i : rs ;
}
/* end subroutine (tmz_logz) */


/* format> [CC]YYMMDD */
int tmz_day(TMZ *op,cchar *sp,int sl)
{
	struct tm	*stp ;
	const int	n = 3 ;
	int		rs = SR_OK ;
	int		i = 0 ;
	int		cc = -1 ;

	if (op == NULL)
	    return SR_FAULT ;

#if	CF_SAFE
	if (sp == NULL)
	    return SR_FAULT ;
#endif

	if (sl < 0)
	    sl = strlen(sp) ;

#if	CF_DEBUGS
	debugprintf("tmz_day: ent string=%t\n",sp,sl) ;
#endif

	stp = &op->st ;
	memset(op,0,sizeof(TMZ)) ;
	op->zoff = SHORT_MAX ;
	stp->tm_year = -1 ;
	stp->tm_wday = -1 ;
	stp->tm_isdst = -1 ;

	while (sl && CHAR_ISWHITE(*sp)) {
	    sp += 1 ;
	    sl -= 1 ;
	}
	while (sl && CHAR_ISWHITE(sp[sl-1])) sl -= 1 ;

#if	CF_DEBUGS
	debugprintf("tmz_day: stripped string=%t\n",sp,sl) ;
#endif

	if (hasalldig(sp,sl)) {

	    if (sl >= 8) {
	        cc = val(sp) ;
	        sp += 2 ;
	        sl -= 2 ;
	    } /* end if (CC) */

	    while ((sl >= 2) && (i < n)) {
	        switch (i++) {
	        case 0:
	            stp->tm_year = val(sp) ;
	            break ;
	        case 1:
	            stp->tm_mon = (val(sp) - 1) ;
	            break ;
	        case 2:
	            stp->tm_mday = val(sp) ;
	            break ;
	        } /* end switch */
	        sp += 2 ;
	        sl -= 2 ;
	    } /* end while */

	} else
	    rs = SR_INVALID ;

	if (rs >= 0) {
	    if (i >= n) {
	        op->f.year = TRUE ;
	        if (cc >= 0) {
	            cc = ((cc * NYEARS_CENTURY) - TM_YEAR_BASE) ;
	            stp->tm_year += cc ;
	        } else {
	            if ((stp->tm_year >= 0) && (stp->tm_year <= 38))
	                stp->tm_year += NYEARS_CENTURY ;
	        }
	        if (stp->tm_year < 69) rs = SR_INVALID ;
	    } else
	        rs = SR_INVALID ;
	} /* end if (ok) */

#if	CF_DEBUGS
	debugprintf("tmz_day: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (tmz_day) */


int tmz_isset(TMZ *op)
{
	if (op == NULL) return SR_FAULT ;
	return (op->st.tm_mday) ;
}
/* end subroutine (tmz_isset) */


int tmz_setyear(TMZ *op,int year)
{

	if (op == NULL) return SR_FAULT ;

	op->st.tm_year = year ;
	op->f.year = TRUE ;
	return SR_OK ;
}
/* end subroutine (tmz_setyear) */


int tmz_setzone(TMZ *op,cchar *zp,int zl)
{
	int		rs ;

	if (op == NULL) return SR_FAULT ;

	rs = strnwcpy(op->zname,TMZ_ZNAMESIZE,zp,zl) - op->zname ;

	return rs ;
}
/* end subroutine (tmz_setzone) */


int tmz_gettm(TMZ *op,struct tm *tmp)
{
	*tmp = op->st ;
	return SR_OK ;
}
/* end subroutine (tmz_gettm) */


/* private subroutines */


/* parse out> [DDD] MMM */
static int tmz_getmonth(TMZ *op,cchar *sp,int sl)
{
	int		rs = SR_OK ;
	int		si = 0 ;
	int		cl ;
	cchar		*cp ;

	if ((cl = nextfield(sp,sl,&cp)) > 0) {
	    int		ch = MKCHAR(*cp) ;
	    if isalphalatin(ch)) {
	        int	ml = cl ;
	        cchar	*mp = cp ;
	        si += ((cp+cl)-sp) ;
	        sp += si ;
	        sl -= si ;
	        if (((cl = nextfield(sp,sl,&cp)) > 0) {
	            ch = MKCHAR(*cp) ;
		    if (isalphalatin(ch)) {
	                rs = tmstrsday(mp,ml) ;
	                op->st.tm_wday = rs ;
	                mp = cp ;
	                ml = cl ;
	                si += ((cp+cl)-sp) ;
	                sp += si ;
	                sl -= si ;
		    }
	        }
	        if (rs >= 0) {
	            rs = tmstrsmonth(mp,ml) ;
	            op->st.tm_mon = rs ;
	        }
	    } else {
	        rs = SR_INVALID ;
	    }
	} else {
	    rs = SR_INVALID ;
	}

	return (rs >= 0) ? si : rs ;
}
/* end subroutine (tmz_getmonth) */


/* parse out> dd */
static int tmz_getday(TMZ *op,cchar *sp,int sl)
{
	int		rs = SR_OK ;
	int		ch ;
	int		si = 0 ;
	int		cl ;
	cchar		*cp ;

	if (((cl = nextfield(sp,sl,&cp)) > 0) {
	    ch = MKCHAR(*cp) ;
	    if (isdigitlatin(ch)) {
	        int		v ;
	        rs = cfdeci(cp,cl,&v) ;
	        op->st.tm_mday = v ;
	        si += ((cp+cl)-sp) ;
	        sp += si ;
	        sl -= si ;
	    } else {
	        rs = SR_INVALID ;
	    }
	} else {
	    rs = SR_INVALID ;
	}

	return (rs >= 0) ? si : rs ;
}
/* end subroutine (tmz_getday) */


/* format> hh:mm[:ss] */
static int tmz_timeparts(TMZ *op,cchar *sp,int sl)
{
	FIELD		fsb ;
	int		rs ;
	int		si = 0 ;

	if ((rs = field_start(&fsb,sp,sl)) >= 0) {
	    int		v ;
	    int		fl ;
	    cchar	*lp = sp ;
	    cchar	*fp ;

/* get hours */

	    if ((fl = field_get(&fsb,tpterms,&fp)) > 0) {
	        lp = (fp + fl) ;
	        rs = cfdeci(fp,fl,&v) ;
	        op->st.tm_hour = v ;
	    }

	    if ((rs >= 0) && (fsb.term == ':')) {

	        if ((fl = field_get(&fsb,tpterms,&fp)) > 0) {
	            lp = (fp + fl) ;
	            rs = cfdeci(fp,fl,&v) ;
	            op->st.tm_min = v ;
	        }

	    } /* end if */

	    if ((rs >= 0) && (fsb.term == ':')) {

	        if ((fl = field_get(&fsb,tpterms,&fp)) > 0) {
	            lp = (fp + fl) ;
	            rs = cfdeci(fp,fl,&v) ;
	            op->st.tm_sec = v ;
	        }

	    } /* end if */

	    si = (lp - sp) ;

	    field_finish(&fsb) ;
	} /* end if (field) */

	return (rs >= 0) ? si : rs ;
}
/* end subroutine (tmz_timeparts) */


static int tmz_stdtrailing(TMZ *op,cchar *sp,int sl)
{
	int		rs = SR_OK ;
	int		si = 0 ;
	int		wi ;

	if ((rs >= 0) && ((wi = siskipwhite(sp,sl)) >= 0)) {
	    si += wi ;
	    sp += wi ;
	    sl -= wi ;
#if	CF_DEBUGS
	    debugprintf("tmz_stdtrailing: wi=%u s=>%t<\n",wi,sp,sl) ;
#endif
	    if (sl > 0) {
	        int	ch = MKCHAR(*sp) ;
	        if (isalphalatin(ch)) {
	            rs = tmz_getzname(op,sp,sl) ;
	        } else if (isdigitlatin(ch) && (! op->f.year)) {
	            rs = tmz_getyear(op,sp,sl) ;
	        } else if (isplusminus(ch) || isdigitlatin(ch)) {
	            rs = tmz_getzoff(op,sp,sl) ;
	        } else {
	            rs = SR_INVALID ;
		}
	        si += rs ;
	        sp += si ;
	        sl -= si ;
	    } /* end if (non-zero string) */
	} /* end if */

#if	CF_DEBUGS
	debugprintf("tmz_stdtrailing: ret rs=%d si=%u\n",rs,si) ;
#endif

	return (rs >= 0) ? si : rs ;
}
/* end subroutine (tmz_stdtrailing) */


static int tmz_getzname(TMZ *op,cchar *sp,int sl)
{
	const int	znl = TMZ_ZNAMESIZE ;
	int		rs = SR_OK ;
	int		si = 0 ;
	int		cl ;
	cchar		*cp ;

	if ((cl = nextfield(sp,sl,&cp)) > 0) {
	    int		ch = MKCHAR(*cp) ;
	    if (isalphalatin(ch)) {
	        rs = strnwcpy(op->zname,znl,cp,cl)  - op->zname ;
	        si += ((cp+cl)-sp) ;
	        sp += si ;
	        sl -= si ;
	    }
	} /* end if (nextfield) */

#if	CF_DEBUGS
	debugprintf("tmz_getzname: ret rs=%d si=%u\n",rs,si) ;
#endif

	return (rs >= 0) ? si : rs ;
}
/* end subroutine (tmz_getzname) */


static int tmz_getyear(TMZ *op,cchar *sp,int sl)
{
	int		rs = SR_OK ;
	int		si = 0 ;
	int		cl ;
	cchar		*cp ;

	if ((cl = nextfield(sp,sl,&cp)) > 0) {
	    int	f = FALSE ;
	    f = f || isdigitlatin(MKCHAR(*cp)) ;
	    if (f) {
	        rs = tmstrsyear(cp,cl) ;
	        op->st.tm_year = rs ;
	        op->f.year = TRUE ;
	        si += ((cp+cl)-sp) ;
	        sp += si ;
	        sl -= si ;
	    }
	} /* end if (nextfield) */

#if	CF_DEBUGS
	debugprintf("tmz_getyear: ret rs=%d si=%u\n",rs,si) ;
	debugprintf("tmz_getyear: year=%u\n",op->st.tm_year) ;
#endif

	return (rs >= 0) ? si : rs ;
}
/* end subroutine (tmz_getyear) */


static int tmz_getzoff(TMZ *op,cchar *sp,int sl)
{
	int		rs = SR_OK ;
	int		si = 0 ;
	int		cl ;
	cchar		*cp ;

	if ((cl = nextfield(sp,sl,&cp)) > 0) {
	    int	ch = MKCHAR(*cp) ;
	    int	f = FALSE ;
	    f = f || isplusminus(ch) ;
	    f = f || isdigitlatin(ch) ;
	    if (f) {
	        int	v ;
	        rs = getzoff(&v,cp,cl) ;
	        op->zoff = v ;
	        op->f.zoff = TRUE ;
	        si += ((cp+cl)-sp) ;
	        sp += si ;
	        sl -= si ;
	    }
	} /* end if (nextfield) */

#if	CF_DEBUGS
	debugprintf("tmz_getzoff: ret rs=%d si=%u\n",rs,si) ;
	debugprintf("tmz_getzoff: zoff=%u\n",op->zoff) ;
#endif

	return (rs >= 0) ? si : rs ;
}
/* end subroutine (tmz_getzoff) */


#if	defined(CF_MULTIZONE) && (CF_MULTIZONE == 0)

/* do we have a valid time-zone name */
static int isgoodname(cchar *sp,int sl)
{
	int		ch ;
	int		f = FALSE ;

	while ((sl != 0) && (sp[0] != '\0')) {
	    ch = (*sp & 0xff) ;
	    f = isalnumlatin(ch) ;
	    if (! f) break ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */

	return f ;
}
/* end subroutine (isgoodname) */

#endif /* defined(CF_MULTIZONE) && (CF_MULTIZONE == 0) */


/* parse minutes west of GMT */
static int getzoff(int *zop,cchar *sp,int sl)
{
	int		rs = SR_INVALID ;
	int		cl ;
	int		zoff ;
	int		f = FALSE ;
	cchar		*cp ;

	f = f || isplusminus(MKCHAR(*sp)) ;
	f = f || isdigitlatin(MKCHAR(*sp)) ;
	if ((sl >= 2) && f) {
	    int	i, sign ;
	    int	hours, mins ;

	    rs = SR_OK ;
	    sign = ((*sp == '+') || isdigitlatin(MKCHAR(*sp))) ? -1 : 1 ;

	    cp = sp ;
	    cl = sl ;
	    if ((*sp == '-') || (*sp == '+')) {
	        cp += 1 ;
	        cl -= 1 ;
	    }

	    for (i = 0 ; 
	        (i < cl) && cp[i] && 
	        (! CHAR_ISWHITE(cp[i])) && (cp[i] != ',') ; 
	        i += 1) {

	        if (! isdigitlatin(MKCHAR(cp[i]))) {
	            rs = SR_INVALID ;
	            break ;
	        }

	    } /* end for (extra sanity check) */

/* skip over extra leading digits (usually '0' but whatever) */

	    if (i > 4) {
	        cp += (i - 4) ;
	        cl -= (i - 4) ;
	    }

/* extract hours and minutes from remaining 3 or 4 digits */

	    hours = (*cp++ - '0') ;
	    if (cl > 3) {
	        hours *= 10 ;
	        hours += (*cp++ - '0') ;
	    }

	    mins = (*cp++ - '0') * 10 ;
	    mins += (*cp++ - '0') ;

	    zoff = ((hours * 60) + mins) ;

/* reportedly, there are time zones at up to 14 hours off of GMT! */

#ifdef	COMMENT
	    if (zoff > (14 * 60))
	        rs = SR_INVALID ;
#endif

	    zoff *= sign ;
	    if (zop != NULL)
	        *zop = zoff ;

	    if (rs >= 0)
	        rs = (cp - sp) ;

	} /* end if (getting timezone offset) */

#if	CF_DEBUGS
	debugprintf("getzoff: ret rs=%d zo=%d\n",rs,zoff) ;
#endif

	return rs ;
}
/* end subroutine (getzoff) */


static int val(cchar *sp)
{
	int		v ;
	v = (10 * (*sp++ - '0')) ;
	v += (*sp - '0') ;
	return v ;
}
/* end subroutine (val) */


static int isplusminus(int ch)
{
	return ((ch == '+') || (ch == '-')) ;
}
/* end subroutine (isplusminus) */


static cchar *strnzone(cchar *sp,int sl)
{
	int		f = FALSE ;
	while (sl && sp[0]) {
	    cchar	ch = MKCHAR(*sp) ;
	    f = f || (ch == '+') ;
	    f = f || (ch == '-') ;
	    f = f || isalphalatin(ch) ;
	    if (f) break ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	return sp ;
}
/* end subroutine (strnzone) */


