/* tmz SUPPORT */
/* lang=C++20 */

/* time and timezone parsing */
/* version %I% last-modified %G% */

#define	CF_MULTIZONE	1		/* allow fragmented time zone names */

/* revision history:

	= 1999-05-01, David A-D- Morano
	This was created along with the DATE object.  This code was
	based on older code I wrote to handle the "STRDIG" form for
	the BBNEWS facility.  That goes back in time to possibly
	as far back as 1992!

	= 2014-07-15, David A-D- Morano
	I laugh sometimes as how long some of these objects (or
	subroutines) last without change.  Yes, most of this code
	looks pretty bad by standards of today.  But to the point,
	I am enhancing this object (specially the |tmz_std()|
	subroutine) so that it recognizes a date-string without
	either a time-zone abbreviation or a year but which does
	have a time-zone offset.  In the past, the case of an offset
	without a year was supposed to be an error in conversion.
	But now-a-days, in an effort to make almost all date-strings
	work similarly, we enhance this subroutine to make this
	above case allowable.  This case never has occurred (because
	having zone-offsets is so relatively new in itself) and
	really never can happen (because we always now have fully
	qualified date-strings with the only exception possibly
	having the zone-offset excluded).  But we will make this
	enhancement and just to round out the symmetry of the various
	date-strings.

*/

/* Copyright (c) 1999,2014 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	This object is used to parse date strings that are in ASCII
	and are human-readable. There are tons of places where ASCII
	strings that represent a date are used. I will leave it as
	an exercise for the reader to think up some of those!

	Note for STD type dates:

	Note that our |tmz_std()| will parse both the old style
	standard date strings (a la |ctime(3c)|) as well as the new
	style date strings (|date(1)|).  An old style standard date
	string looked like:

		[Mon] Nov 26 21:48[:25] 1999

	The new style standard date string looks like:

		[Mon] Nov 26 21:48[:25] [EST] 1999

	Both are handled.  The newer style, used in newer email
	envelopes, is prefered since it includes the timezone name.

	Updated note:
	It is reported that some envelopes of the form:

		[Mon] Nov 26 21:48[:25] [EST] 1999 [-0500]

	are being used in the world (and particularly on Sun Solaris
	boxes).  We also handle this variant.

*******************************************************************************/

#include	<envstandards.h>
#include	<cstdlib>
#include	<cstring>
#include	<tzfile.h>		/* for TM_YEAR_BASE */
#include	<usystem.h>
#include	<usupport.h>		/* for |memclear(3u)| */
#include	<estrings.h>
#include	<field.h>
#include	<tmstrs.h>
#include	<mkchar.h>
#include	<char.h>
#include	<cfdec.h>
#include	<localmisc.h>

#include	"tmz.h"


/* local defines */

#ifndef	NYEARS_CENTURY
#define	NYEARS_CENTURY	100
#endif

#ifndef	CENTURY_BASE
#define	CENTURY_BASE	19
#endif


/* external subroutines */


/* local structures */


/* forward references */

static int	tmz_timeparts(TMZ *,cchar *,int) noex ;
static int	tmz_stdtrailing(TMZ *,cchar *,int) noex ;
static int	tmz_procday(TMZ *,cchar *,int) noex ;
static int	tmz_procmonth(TMZ *,cchar *,int) noex ;
static int	tmz_procyear(TMZ *,cchar *,int) noex ;
static int	tmz_proczoff(TMZ *,cchar *,int) noex ;
static int	tmz_proczname(TMZ *,cchar *,int) noex ;
static int	tmz_yearadj(TMZ *,int) noex ;

static int	getzoff(int *,cchar *,int) noex ;
static int	val(cchar *) noex ;
static int	silogend(cchar *,int) noex ;

static int	isplusminus(int) noex ;

#if	defined(CF_MULTIZONE) && (CF_MULTIZONE == 0)
static int	isgoodname(cchar *,int) ;
#endif

static cchar	*strnzone(cchar *,int) noex ;


/* local variables */

static constexpr cchar	tpterms[] = {
	0x00, 0x1F, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x04,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;


/* exported variables */


/* exported subroutines */

int tmz_init(TMZ *op) noex {
	if (op == NULL) return SR_FAULT ;
	memclear(op) ;
	op->zoff = SHORT_MIN ;
	return SR_OK ;
}
/* end subroutine (tmz_init) */

/* format> [Wed] Nov 14 19:24[:04] [EST] [[19]99] [±0400] */
int tmz_std(TMZ *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (op && sp) {
	    rs = SR_OK ;
	    if (sl < 0) sl = strlen(sp) ;
	    memclear(op) ;
	    op->zoff = SHORT_MIN ;
	    op->st.tm_year = -1 ;
	    op->st.tm_wday = -1 ;
	    op->st.tm_isdst = -1 ;
	    if ((rs >= 0) && ((rs = tmz_procmonth(op,sp,sl)) > 0)) {
	        sp += rs ;
	        sl -= rs ;
	    }
	    if ((rs >= 0) && ((rs = tmz_procday(op,sp,sl)) > 0)) {
	        sp += rs ;
	        sl -= rs ;
	    }
	    if ((rs >= 0) && ((rs = tmz_timeparts(op,sp,sl)) > 0)) {
	        sp += rs ;
	        sl -= rs ;
	    }
	    for (int i = 0 ; (rs >= 0) && (i < 3) ; i += 1) {
	        rs = tmz_stdtrailing(op,sp,sl) ;
	        if (rs == 0) break ;
	        sp += rs ;
	        sl -= rs ;
	    } /* end for */
	    if (rs >= 0) {
	        rs = strnlen(op->zname,TMZ_ZNAMESIZE) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tmz_std) */

/* format> [Weekday,] DD MMM [CC]YY hh:mm[:ss] [±hhmm] [zname] */
int tmz_msg(TMZ *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		cl ;
	int		zl = 0 ;
	cchar		*tp ;
	cchar		*cp ;

	if (op == NULL) return SR_FAULT ;

#if	CF_SAFE
	if (sp == NULL) return SR_FAULT ;
#endif

	if (sl < 0)
	    sl = strlen(sp) ;

	memset(op,0,sizeof(TMZ)) ;
	op->zoff = SHORT_MIN ;
	op->st.tm_year = -1 ;
	op->st.tm_wday = -1 ;
	op->st.tm_isdst = -1 ;

	if ((tp = strnchr(sp,sl,',')) != NULL) {
	    if ((cl = nextfield(sp,(tp-sp),&cp)) > 0) {
	        rs = tmstrsday(cp,cl) ;
	        op->st.tm_wday = rs ;
	    }
	    sl -= ((tp+1)-sp) ;
	    sp = (tp+1) ;
	}

	if ((rs >= 0) && ((rs = tmz_procday(op,sp,sl)) > 0)) {
	    sp += rs ;
	    sl -= rs ;
	}

	if ((rs >= 0) && ((rs = tmz_procmonth(op,sp,sl)) > 0)) {
	    sp += rs ;
	    sl -= rs ;
	}

	if ((rs >= 0) && ((rs = tmz_procyear(op,sp,sl)) > 0)) {
	    sp += rs ;
	    sl -= rs ;
	}

	if ((rs >= 0) && ((rs = tmz_timeparts(op,sp,sl)) > 0)) {
	    sp += rs ;
	    sl -= rs ;
	}

	if ((rs >= 0) && ((rs = tmz_proczoff(op,sp,sl)) > 0)) {
	    sp += rs ;
	    sl -= rs ;
	}

	if ((rs >= 0) && ((rs = tmz_proczname(op,sp,sl)) > 0)) {
	    sp += rs ;
	    sl -= rs ;
	}

	if (rs >= 0) {
	    rs = strnlen(op->zname,TMZ_ZNAMESIZE) ;
	    zl = rs ; /* return value for subroutine */
	}

	return (rs >= 0) ? zl : rs ;
}
/* end subroutine (tmz_msg) */

/* convert from a TOUCH (original) format> MMDDhhmm[YY] */
int tmz_touch(TMZ *op,cchar *sp,int sl) noex {
	TM		*stp ;
	cint		n = 5 ;
	int		rs = SR_OK ;
	int		i = 0 ;

	if (op == NULL) return SR_FAULT ;

#if	CF_SAFE
	if (sp == NULL) return SR_FAULT ;
#endif

	if (sl < 0)
	    sl = strlen(sp) ;

	stp = &op->st ;
	memset(op,0,sizeof(TMZ)) ;
	op->zoff = SHORT_MIN ;
	stp->tm_year = -1 ;
	stp->tm_wday = -1 ;
	stp->tm_isdst = -1 ;

/* skip leading white space */

	while (sl && CHAR_ISWHITE(*sp)) {
	    sp += 1 ;
	    sl -= 1 ;
	}
	while (sl && CHAR_ISWHITE(sp[sl-1])) sl -= 1 ;

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
	} else {
	    rs = SR_INVALID ;
	}

	if (rs >= 0) {
	    if (i >= n) {
	        op->f.year = true ;
	        if ((stp->tm_year >= 0) && (stp->tm_year <= 38)) {
	            stp->tm_year += NYEARS_CENTURY ;
	        }
	    } else if (i < (n-1)) {
	        rs = SR_INVALID ;
	    }
	} /* end if */

	return rs ;
}
/* end subroutine (tmz_touch) */

/* convert from a TOUCH-t (new '-t') format> [[CC]YY]MMDDhhmm[.SS] */
int tmz_toucht(TMZ *op,cchar *sp,int sl) noex {
	TM		*stp ;
	cint	n = 4 ;
	int		rs = SR_OK ;
	int		i = 0 ;
	int		cc = -1 ;
	cchar		*tp ;

	if (op == NULL) return SR_FAULT ;

#if	CF_SAFE
	if (sp == NULL) return SR_FAULT ;
#endif

	if (sl < 0)
	    sl = strlen(sp) ;

	stp = &op->st ;
	memset(op,0,sizeof(TMZ)) ;
	op->zoff = SHORT_MIN ;
	stp->tm_year = -1 ;
	stp->tm_wday = -1 ;
	stp->tm_isdst = -1 ;

/* skip leading white space */

	while (sl && CHAR_ISWHITE(*sp)) {
	    sp += 1 ;
	    sl -= 1 ;
	}
	while (sl && CHAR_ISWHITE(sp[sl-1])) sl -= 1 ;

	if ((tp = strnchr(sp,sl,'.')) != NULL) {
	    cint	cl = (sl-((tp+1)-sp)) ;
	    cchar	*cp = (tp+1) ;
	    if (cl >= 2) {
	        cint	tch = mkchar(*cp) ;
	        if (isdigitlatin(tch)) {
	            stp->tm_sec = val(cp) ;
	        } else {
	            rs = SR_INVALID ;
	        }
	    }
	    sl = (tp-sp) ;
	} /* end if (tried for seconds) */

	if (rs >= 0) {
	    if (hasalldig(sp,sl)) {

	        if ((rs >= 0) && (sl >= 12)) {
	            cc = val(sp) ;
	            sp += 2 ;
	            sl -= 2 ;
	        } /* end if (CC) */

	        if (sl >= 10) {
	            op->f.year = true ;
	            stp->tm_year = val(sp) ;
	            sp += 2 ;
	            sl -= 2 ;
	        } /* end if (YY) */

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

	    } else {
	        rs = SR_INVALID ;
	    }
	} /* end if (ok) */

	if (rs >= 0) {
	    if (i >= n) {
	        tmz_yearadj(op,cc) ;
	    } else {
	        rs = SR_INVALID ;
	    }
	} /* end if */

	return rs ;
}
/* end subroutine (date_toucht) */


/* format> [[CC]]YYMMDDhhmm[ss][±hhmm][zname] */
int tmz_strdig(TMZ *op,cchar *sp,int sl)
{
	TM		*stp ;
	cint	n = 6 ;
	int		rs = SR_OK ;
	int		i = 0 ;
	int		cc = -1 ;
	int		zl = 0 ;
	cchar		*tp ;

	if (op == NULL) return SR_FAULT ;

#if	CF_SAFE
	if (sp == NULL) return SR_FAULT ;
#endif

	if (sl < 0)
	    sl = strlen(sp) ;

	stp = &op->st ;
	memset(op,0,sizeof(TMZ)) ;
	op->zoff = SHORT_MIN ;
	stp->tm_year = -1 ;
	stp->tm_wday = -1 ;
	stp->tm_isdst = -1 ;

/* skip leading and trailing white space */

	while (sl && CHAR_ISWHITE(*sp)) {
	    sp += 1 ;
	    sl -= 1 ;
	}
	while (sl && CHAR_ISWHITE(sp[sl-1])) sl -= 1 ;

	if ((tp = strnzone(sp,sl)) != NULL) {
	    cchar	*cp = tp ;
	    int		cl = (sl-(tp-sp)) ;
	    if ((cl >= 1) && isplusminus(*cp)) { /* ok */
	        int	zol = cl ;
	        int	zo ;
	        int	si ;
	        cchar	*zop = cp ;
	        if ((si = sialpha(cp,cl)) > 0) {
	            zol = si ;
	        }
	        if ((rs = getzoff(&zo,zop,zol)) >= 0) {
	            op->zoff = zo ;
	            op->f.zoff = true ;
	            cp += rs ;
	            cl -= rs ;
	        }
	    }
	    if ((rs >= 0) && (cl > 0)) {
	        cint	ch = mkchar(*cp) ;
	        if (isalphalatin(ch)) {
		    cint	zlen = TMZ_ZNAMESIZE ;
	            rs = strnwcpy(op->zname,zlen,cp,cl) - op->zname ;
	            zl = rs ;
	        } else {
	            rs = SR_INVALID ;
	        }
	    }
	    sl = (tp-sp) ;
	} /* end if (tried for ZOFF and ZNAME) */

	if (rs >= 0) {
	    i = 0 ;
	    if (hasalldig(sp,sl)) {

	        if ((rs >= 0) && (sl >= 14)) {
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

	        if (i >= (n-1)) {
	            tmz_yearadj(op,cc) ;
	        } else {
	            rs = SR_INVALID ;
	        }

	    } else {
	        rs = SR_INVALID ;
	    }
	} /* end if (ok) */

	return (rs >= 0) ? zl : rs ;
}
/* end subroutine (tmz_strdig) */


/* format> [CC]YYMMDD_hhmm[:ss][_][zname] */
int tmz_logz(TMZ *op,cchar *sp,int sl)
{
	TM		*stp ;
	int		rs = SR_OK ;
	int		zl = 0 ;
	cchar		*tp ;

	if (op == NULL) return SR_FAULT ;

#if	CF_SAFE
	if (sp == NULL) return SR_FAULT ;
#endif

	if (sl < 0)
	    sl = strlen(sp) ;

	stp = &op->st ;
	memset(op,0,sizeof(TMZ)) ;
	op->zoff = SHORT_MIN ;
	stp->tm_year = -1 ;
	stp->tm_wday = -1 ;
	stp->tm_isdst = -1 ;

/* skip leading and trailing white space */

	while (sl && CHAR_ISWHITE(*sp)) {
	    sp += 1 ;
	    sl -= 1 ;
	}
	while (sl && CHAR_ISWHITE(sp[sl-1])) sl -= 1 ;

	if ((tp = strnchr(sp,sl,'_')) != NULL) {
	    int		si = (tp-sp) ;
	    if (hasalldig(sp,si)) {
	        int	cc = -1 ;
	        int	i = 0 ;
	        if (si >= 8) {
	            cc = val(sp) ;
	            sp += 2 ;
	            sl -= 2 ;
	        } /* end if (CC) */
	        while ((sl >= 2) && (i < 3)) {
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
	        sp += 1 ;
	        sl -= 1 ;
	        if ((si = silogend(sp,sl)) >= 4) {
		    if (hasalldig(sp,4)) {
			int	ch ;
	                while ((sl >= 2) && (i < 6)) {
	                    switch (i++) {
	                    case 3:
	                        stp->tm_hour = val(sp) ;
	                        break ;
	                    case 4:
	                        stp->tm_min = val(sp) ;
	                        break ;
	                    case 5:
	                        if ((sl >= 3) && (sp[0] == ':')) {
	                            sp += 1 ;
	                            sl -= 1 ;
	                        }
	                        stp->tm_sec = val(sp) ;
	                        break ;
	                    } /* end switch */
	                    sp += 2 ;
	                    sl -= 2 ;
	                } /* end while */
	                tmz_yearadj(op,cc) ;
	                if (sl && (*sp == '_')) {
	                    sp += 1 ;
	                    sl -= 1 ;
	                }
	                if (sl && ((ch = mkchar(*sp)),isalphalatin(ch))) {
	                    rs = tmz_proczname(op,sp,sl) ;
	                    zl = strlen(op->zname) ;
	                }
		    } else {
			rs = SR_INVALID ;
		    } /* end if (hasalldig) */
	        } /* end if (silogend) */
	    } else {
	        rs = SR_INVALID ;
	    }
	} /* end if (ok) */

	return (rs >= 0) ? zl : rs ;
}
/* end subroutine (tmz_logz) */


/* format> [CC]YYMMDD */
int tmz_day(TMZ *op,cchar *sp,int sl)
{
	TM		*stp ;
	int		rs = SR_OK ;
	int		cc = -1 ;

	if (op == NULL) return SR_FAULT ;

#if	CF_SAFE
	if (sp == NULL) return SR_FAULT ;
#endif

	if (sl < 0)
	    sl = strlen(sp) ;

	stp = &op->st ;
	memset(op,0,sizeof(TMZ)) ;
	op->zoff = SHORT_MIN ;
	stp->tm_year = -1 ;
	stp->tm_wday = -1 ;
	stp->tm_isdst = -1 ;

	while (sl && CHAR_ISWHITE(*sp)) {
	    sp += 1 ;
	    sl -= 1 ;
	}
	while (sl && CHAR_ISWHITE(sp[sl-1])) sl -= 1 ;

	if (hasalldig(sp,sl)) {
	    cint	n = 3 ;
	    int		i = 0 ;

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

	    if (i >= n) {
	        tmz_yearadj(op,cc) ;
	    } else {
	        rs = SR_INVALID ;
	    }

	} else {
	    rs = SR_INVALID ;
	}

	return rs ;
}
/* end subroutine (tmz_day) */


int tmz_isset(TMZ *op)
{
	if (op == NULL) return SR_FAULT ;
	return (op->st.tm_mday) ;
}
/* end subroutine (tmz_isset) */


int tmz_hasyear(TMZ *op)
{
	if (op == NULL) return SR_FAULT ;
	return (op->f.year) ;
}
/* end subroutine (tmz_hasyear) */


int tmz_haszoff(TMZ *op)
{
	if (op == NULL) return SR_FAULT ;
	return op->f.zoff ;
}
/* end subroutine (tmz_haszoff) */


int tmz_haszone(TMZ *op)
{
	if (op == NULL) return SR_FAULT ;
	return (op->zname[0] != '\0') ;
}
/* end subroutine (tmz_haszone) */


int tmz_setday(TMZ *op,int y,int m,int d)
{
	TM		*stp ;
	cint	cc = -1 ;
	if (op == NULL) return SR_FAULT ;
	stp = &op->st ;
	if (y >= TM_YEAR_BASE) {
	    y -= TM_YEAR_BASE ;
	}
	stp->tm_year = y ;
	stp->tm_mon = m ;
	stp->tm_mday = d ;
	tmz_yearadj(op,cc) ;
	return SR_OK ;
}
/* end subroutine (tmz_setday) */


int tmz_setyear(TMZ *op,int year)
{
	if (op == NULL) return SR_FAULT ;
	op->st.tm_year = year ;
	op->f.year = true ;
	return SR_OK ;
}
/* end subroutine (tmz_setyear) */


int tmz_setzone(TMZ *op,cchar *zp,int zl)
{
	if (op == NULL) return SR_FAULT ;
	return (strnwcpy(op->zname,TMZ_ZNAMESIZE,zp,zl) - op->zname) ;
}
/* end subroutine (tmz_setzone) */


int tmz_gettm(TMZ *op,TM	 *tmp)
{
	*tmp = op->st ;
	return SR_OK ;
}
/* end subroutine (tmz_gettm) */


int tmz_getdst(TMZ *op)
{
	return op->st.tm_isdst ;
}
/* end subroutine (tmz_getdst) */


int tmz_getzoff(TMZ *op)
{
	return op->zoff ;
}
/* end subroutine (tmz_getzoff) */


/* private subroutines */


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

/* get minutes */

	        if ((fl = field_get(&fsb,tpterms,&fp)) > 0) {
	            lp = (fp + fl) ;
	            rs = cfdeci(fp,fl,&v) ;
	            op->st.tm_min = v ;
	        }

	    } /* end if */

	    if ((rs >= 0) && (fsb.term == ':')) {

/* get seconds */

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

	if ((wi = siskipwhite(sp,sl)) >= 0) {
	    si += wi ;
	    sp += wi ;
	    sl -= wi ;
	    if (sl > 0) {
	        cint	ch = mkchar(*sp) ;
	        if (isalphalatin(ch)) {
	            rs = tmz_proczname(op,sp,sl) ;
	        } else if (isdigitlatin(ch) && (! op->f.year)) {
	            rs = tmz_procyear(op,sp,sl) ;
	        } else if (isplusminus(ch) || isdigitlatin(ch)) {
	            rs = tmz_proczoff(op,sp,sl) ;
	        } else {
	            rs = SR_INVALID ;
	        }
	        si = rs ;
	    } /* end if (non-zero string) */
	} /* end if (siskipwhite) */

	return (rs >= 0) ? si : rs ;
}
/* end subroutine (tmz_stdtrailing) */

/* parse out> dd */
static int tmz_procday(TMZ *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		si = 0 ;
	int		cl ;
	cchar		*cp ;
	if ((cl = nextfield(sp,sl,&cp)) > 0) {
	    cint	tch = mkchar(*cp) ;
	    if (isdigitlatin(tch)) {
	        int	v ;
	        if ((rs = cfdeci(cp,cl,&v)) >= 0) {
		    if (v <= 31) {
	        	op->st.tm_mday = v ;
	        	si = ((cp+cl)-sp) ;
		    } else {
			rs = SR_INVALID ;
		    }
		}
	    } else {
	        rs = SR_INVALID ;
	    }
	} else {
	    rs = SR_INVALID ;
	}
	return (rs >= 0) ? si : rs ;
}
/* end subroutine (tmz_procday) */

/* parse out> [DDD] MMM */
static int tmz_procmonth(TMZ *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		si = 0 ;
	int		cl ;
	cchar		*cp ;
	if ((cl = nextfield(sp,sl,&cp)) > 0) {
	    int		ch = mkchar(*cp) ;
	    if (isalphalatin(ch)) {
	        int	ml = cl ;
	        cchar	*mp = cp ;
	        si += ((cp+cl)-sp) ;
	        sp += si ;
	        sl -= si ;
	        if ((cl = nextfield(sp,sl,&cp)) > 0) {
	            ch = mkchar(*cp) ;
	            if (isalphalatin(ch)) {
	                rs = tmstrsday(mp,ml) ;
	                op->st.tm_wday = rs ;
	                mp = cp ;
	                ml = cl ;
	                si += ((cp+cl)-sp) ;
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
/* end subroutine (tmz_procmonth) */

static int tmz_procyear(TMZ *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		si = 0 ;
	int		cl ;
	cchar		*cp ;
	if ((cl = nextfield(sp,sl,&cp)) > 0) {
	    cint	ch = mkchar(*cp) ;
	    if (isdigitlatin(ch)) {
	        rs = tmstrsyear(cp,cl) ;
	        op->st.tm_year = rs ;
	        op->f.year = true ;
	        si = ((cp+cl)-sp) ;
	    }
	} /* end if (nextfield) */
	return (rs >= 0) ? si : rs ;
}
/* end subroutine (tmz_procyear) */

static int tmz_proczoff(TMZ *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		si = 0 ;
	int		cl ;
	cchar		*cp ;
	if ((cl = nextfield(sp,sl,&cp)) > 0) {
	    cint	ch = mkchar(*cp) ;
	    int		f = false ;
	    f = f || isplusminus(ch) ;
	    f = f || isdigitlatin(ch) ;
	    if (f) {
	        int	v ;
	        rs = getzoff(&v,cp,cl) ;
	        op->zoff = v ;
	        op->f.zoff = true ;
	        si = ((cp+cl)-sp) ;
	    }
	} /* end if (nextfield) */
	return (rs >= 0) ? si : rs ;
}
/* end subroutine (tmz_proczoff) */

static int tmz_proczname(TMZ *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		si = 0 ;
	int		cl ;
	cchar		*cp ;

	if ((cl = nextfield(sp,sl,&cp)) > 0) {
	    cint	ch = mkchar(*cp) ;
	    if (isalphalatin(ch)) {
	        cint	znl = TMZ_ZNAMESIZE ;
	        rs = strnwcpy(op->zname,znl,cp,cl)  - op->zname ;
	        si = ((cp+cl)-sp) ;
	    }
	} /* end if (nextfield) */
	return (rs >= 0) ? si : rs ;
}
/* end subroutine (tmz_proczname) */

static int tmz_yearadj(TMZ *op,int cc) noex {
	TM		*stp = &op->st ;
	if (stp->tm_year >= 0) {
	    op->f.year = true ;
	    if (cc >= 0) {
	        cint	yy = ((cc*NYEARS_CENTURY)-TM_YEAR_BASE) ;
	        stp->tm_year += yy ;
	    } else {
	        if ((stp->tm_year >= 0) && (stp->tm_year <= 38)) {
	            stp->tm_year += NYEARS_CENTURY ;
	        } else if (stp->tm_year >= TM_YEAR_BASE) {
	            stp->tm_year -= TM_YEAR_BASE ;
		}
	    }
	} /* end if (had a year) */
	return SR_OK ;
}
/* end subroutine (tmz_yearadj) */


#if	defined(CF_MULTIZONE) && (CF_MULTIZONE == 0)

/* do we have a valid time-zone name */
static int isgoodname(cchar *sp,int sl) noex {
	int		ch ;
	int		f = false ;
	while ((sl != 0) && (sp[0] != '\0')) {
	    ch = mkchar(*sp) ;
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
static int getzoff(int *zop,cchar *sp,int sl) noex {
	int		rs = SR_INVALID ;
	int		cl ;
	int		zoff ;
	int		ch = mkchar(*sp) ;
	int		f = false ;
	cchar		*cp ;

	f = f || isplusminus(ch) ;
	f = f || isdigitlatin(ch) ;
	if ((sl >= 2) && f) {
	    int		i, sign ;
	    int		hours, mins ;

	    rs = SR_OK ;
	    ch = mkchar(*sp) ;
	    sign = ((*sp == '+') || isdigitlatin(ch)) ? -1 : 1 ;

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
		ch = mkchar(cp[i]) ;
	        if (! isdigitlatin(ch)) {
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

	    if (rs >= 0) {
	        rs = (cp - sp) ;
	    }

	} /* end if (getting timezone offset) */

	return rs ;
}
/* end subroutine (getzoff) */

static int val(cchar *sp) noex {
	int		v = 0 ;
	v += (10 * (sp[0] - '0')) ;
	v += ( 1 * (sp[1] - '0')) ;
	return v ;
}
/* end subroutine (val) */

static int silogend(cchar *sp,int sl) noex {
	int		i = 0 ;
	int		f = false ;
	for (i = 0 ; sl-- && *sp ; i += 1) {
	    cint	ch = mkchar(sp[i]) ;
	    f = f || (ch == '_') ;
	    f = f || isalphalatin(ch) ;
	    if (f) break ;
	} /* end while */
	return i ;
}
/* end subroutine (silogend) */

static int isplusminus(int ch) noex {
	return ((ch == '+') || (ch == '-')) ;
}
/* end subroutine (isplusminus) */

static cchar *strnzone(cchar *sp,int sl) noex {
	int		f = false ;
	while (sl && *sp) {
	    cint	ch = mkchar(*sp) ;
	    f = f || isplusminus(ch) ;
	    f = f || isalphalatin(ch) ;
	    if (f) break ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	return sp ;
}
/* end subroutine (strnzone) */


