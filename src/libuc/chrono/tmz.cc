/* tmz SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* time and timezone parsing */
/* version %I% last-modified %G% */


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

  	Object:
	tmz

	Description:
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

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<tzfile.h>		/* for |TM_YEAR_BASE| */
#include	<usystem.h>
#include	<estrings.h>
#include	<field.h>
#include	<tmstrs.h>
#include	<cfdec.h>
#include	<mkchar.h>
#include	<char.h>
#include	<hasx.h>
#include	<ischarx.h>
#include	<localmisc.h>		/* |NYEARS_CENTURY| */

#include	"tmz.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */

static int	tmz_timeparts(tmz *,cchar *,int) noex ;
static int	tmz_stdtrailing(tmz *,cchar *,int) noex ;
static int	tmz_procday(tmz *,cchar *,int) noex ;
static int	tmz_procmonth(tmz *,cchar *,int) noex ;
static int	tmz_procyear(tmz *,cchar *,int) noex ;
static int	tmz_proczoff(tmz *,cchar *,int) noex ;
static int	tmz_proczname(tmz *,cchar *,int) noex ;
static int	tmz_yearadj(tmz *,int) noex ;

static int	getzoff(int *,cchar *,int) noex ;
static int	val(cchar *) noex ;
static int	silogend(cchar *,int) noex ;

static cchar	*strnzone(cchar *,int) noex ;


/* local variables */

constexpr char		tpterms[] = {
	0x00, 0x1F, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x04,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;

constexpr int		nyears = NYEARS_CENTURY ;


/* exported variables */


/* exported subroutines */

int tmz_init(tmz *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = memclear(op) ;
	    op->zoff = SHORT_MIN ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tmz_init) */

/* format> [Wed] Nov 14 19:24[:04] [EST] [[19]99] [�0400] */
int tmz_std(tmz *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (op && sp) {
	    if (sl < 0) sl = strlen(sp) ;
	    rs = memclear(op) ;
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
	        rs = strnlen(op->zname,TMZ_ZNAMELEN) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tmz_std) */

/* format> [Weekday,] DD MMM [CC]YY hh:mm[:ss] [±hhmm] [zname] */
int tmz_msg(tmz *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		zl = 0 ;
	if (op && sp) {
	    if (sl < 0) sl = strlen(sp) ;
	    rs = memclear(op) ;
	    op->zoff = SHORT_MIN ;
	    op->st.tm_year = -1 ;
	    op->st.tm_wday = -1 ;
	    op->st.tm_isdst = -1 ;
	    if (cchar *tp ; (tp = strnchr(sp,sl,',')) != nullptr) {
	        cchar	*cp{} ;
	        if (int cl ; (cl = sfnext(sp,(tp-sp),&cp)) > 0) {
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
	        rs = strnlen(op->zname,TMZ_ZNAMELEN) ;
	        zl = rs ; /* return value for subroutine */
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? zl : rs ;
}
/* end subroutine (tmz_msg) */

/* convert from a TOUCH (original) format> MMDDhhmm[YY] */
int tmz_touch(tmz *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (op && sp) {
	    TM		*stp = &op->st ;
	    if (sl < 0) sl = strlen(sp) ;
	    rs = memclear(op) ;
	    op->zoff = SHORT_MIN ;
	    stp->tm_year = -1 ;
	    stp->tm_wday = -1 ;
	    stp->tm_isdst = -1 ;
    	    /* skip leading white space */
	    while (sl && CHAR_ISWHITE(*sp)) {
	        sp += 1 ;
	        sl -= 1 ;
	    }
	    while (sl && CHAR_ISWHITE(sp[sl-1])) {
	        sl -= 1 ;
	    }
	    if (hasalldig(sp,sl)) {
	        cint	n = 5 ;
	        int	i = 0 ;
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
	        if (i >= n) {
	            op->f.year = true ;
	            if ((stp->tm_year >= 0) && (stp->tm_year <= 38)) {
	                stp->tm_year += nyears ;
	            }
	        } else if (i < (n-1)) {
	            rs = SR_INVALID ;
	        }
	    } else {
	        rs = SR_INVALID ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tmz_touch) */

/* convert from a TOUCH-t (new '-t') format> [[CC]YY]MMDDhhmm[.SS] */
int tmz_toucht(tmz *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (op && sp) {
            TM		*stp = &op->st ;
            if (sl < 0) sl = strlen(sp) ;
            rs = memclear(op) ;
            op->zoff = SHORT_MIN ;
            stp->tm_year = -1 ;
            stp->tm_wday = -1 ;
            stp->tm_isdst = -1 ;
    	    /* skip leading white space */
            while (sl && CHAR_ISWHITE(*sp)) {
                sp += 1 ;
                sl -= 1 ;
            }
            while (sl && CHAR_ISWHITE(sp[sl-1])) {
                sl -= 1 ;
            }
            if (cchar *tp ; (tp = strnchr(sp,sl,'.')) != nullptr) {
                cint        cl = (sl-((tp+1)-sp)) ;
                cchar       *cp = (tp+1) ;
                if (cl >= 2) {
                    cint    tch = mkchar(*cp) ;
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
                    cint    n = 4 ;
                    int     i = 0 ;
                    int     sc = -1 ;
                    if ((rs >= 0) && (sl >= 12)) {
                        sc = val(sp) ;
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
                    if (i >= n) {
                        tmz_yearadj(op,sc) ;
                    } else {
                        rs = SR_INVALID ;
                    }
                } else {
                    rs = SR_INVALID ;
                }
	    } /* end if (ok) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (date_toucht) */

/* format> [[CC]]YYMMDDhhmm[ss][±hhmm][zname] */
int tmz_strdig(tmz *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		zl = 0 ;
	if (op && sp) {
	    TM		*stp = &op->st ;
	    if (sl < 0) sl = strlen(sp) ;
	    rs = memclear(op) ;
	    op->zoff = SHORT_MIN ;
	    stp->tm_year = -1 ;
	    stp->tm_wday = -1 ;
	    stp->tm_isdst = -1 ;
            /* skip leading and trailing white space */
	    while (sl && CHAR_ISWHITE(*sp)) {
	        sp += 1 ;
	        sl -= 1 ;
	    }
	    while (sl && CHAR_ISWHITE(sp[sl-1])) {
	        sl -= 1 ;
	    }
	    if (cchar *tp ; (tp = strnzone(sp,sl)) != nullptr) {
	        cchar	*cp = tp ;
	        int	cl = (sl-(tp-sp)) ;
	        if ((cl >= 1) && isplusminus(*cp)) { /* ok */
	            int		zol = cl ;
	            int		zo ;
	            cchar	*zop = cp ;
	            if (int si ; (si = sialpha(cp,cl)) > 0) {
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
		        cint	zlen = TMZ_ZNAMELEN ;
	                rs = strnwcpy(op->zname,zlen,cp,cl) - op->zname ;
	                zl = rs ;
	            } else {
	                rs = SR_INVALID ;
	            }
	        }
	        sl = (tp-sp) ;
	    } /* end if (tried for ZOFF and ZNAME) */
	    if (rs >= 0) {
	        if (hasalldig(sp,sl)) {
		    cint	n = 6 ;
	            int		i = 0 ;
		    int		sc = -1 ;
	            if ((rs >= 0) && (sl >= 14)) {
	                sc = val(sp) ;
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
	                tmz_yearadj(op,sc) ;
	            } else {
	                rs = SR_INVALID ;
	            }
	        } else {
	            rs = SR_INVALID ;
	        }
	    } /* end if (ok) */
	} /* end if (non-null) */
	return (rs >= 0) ? zl : rs ;
}
/* end subroutine (tmz_strdig) */

/* format> [CC]YYMMDD_hhmm[:ss][_][zname] */
int tmz_logz(tmz *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		zl = 0 ;
	if (op && sp) {
	    TM		*stp = &op->st ;
            if (sl < 0) sl = strlen(sp) ;
            rs = memclear(op) ;
            op->zoff = SHORT_MIN ;
            stp->tm_year = -1 ;
            stp->tm_wday = -1 ;
            stp->tm_isdst = -1 ;
    	    /* skip leading and trailing white space */
            while (sl && CHAR_ISWHITE(*sp)) {
                sp += 1 ;
                sl -= 1 ;
            }
            while (sl && CHAR_ISWHITE(sp[sl-1])) {
                sl -= 1 ;
            }
            if (cchar *tp ; (tp = strnchr(sp,sl,'_')) != nullptr) {
                int         si = (tp-sp) ;
                if (hasalldig(sp,si)) {
                    int     sc = -1 ;
                    int     i = 0 ;
                    if (si >= 8) {
                        sc = val(sp) ;
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
                            int     ch ;
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
                            tmz_yearadj(op,sc) ;
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
	} /* end if (non-null) */
	return (rs >= 0) ? zl : rs ;
}
/* end subroutine (tmz_logz) */

/* format> [CC]YYMMDD */
int tmz_day(tmz *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	if (op && sp) {
	    TM		*stp = &op->st ;
	    if (sl < 0) sl = strlen(sp) ;
	    rs = memclear(op) ;
	    op->zoff = SHORT_MIN ;
	    stp->tm_year = -1 ;
	    stp->tm_wday = -1 ;
	    stp->tm_isdst = -1 ;
	    while (sl && CHAR_ISWHITE(*sp)) {
	        sp += 1 ;
	        sl -= 1 ;
	    }
	    while (sl && CHAR_ISWHITE(sp[sl-1])) {
	        sl -= 1 ;
	    }
	    if (hasalldig(sp,sl)) {
	        cint	n = 3 ;
	        int	i = 0 ;
	        int	sc = -1 ;
	        if (sl >= 8) {
	            sc = val(sp) ;
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
	            tmz_yearadj(op,sc) ;
	        } else {
	            rs = SR_INVALID ;
	        }
	    } else {
	        rs = SR_INVALID ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tmz_day) */

int tmz_isset(tmz *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = op->st.tm_mday ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tmz_isset) */

int tmz_hasyear(tmz *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = op->f.year ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tmz_hasyear) */

int tmz_haszoff(tmz *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = op->f.zoff ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tmz_haszoff) */

int tmz_haszone(tmz *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = (op->zname[0] != '\0') ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tmz_haszone) */

int tmz_setday(tmz *op,int y,int m,int d) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    TM		*stp = &op->st ;
	    cint	sc = -1 ;
	    if (y >= TM_YEAR_BASE) {
	        y -= TM_YEAR_BASE ;
	    }
	    stp->tm_year = y ;
	    stp->tm_mon = m ;
	    stp->tm_mday = d ;
	    rs = tmz_yearadj(op,sc) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tmz_setday) */

int tmz_setyear(tmz *op,int year) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->st.tm_year = year ;
	    op->f.year = true ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tmz_setyear) */

int tmz_setzone(tmz *op,cchar *zp,int zl) noex {
	int		rs = SR_FAULT ;
	if (op && zp) {
	    cint	znamelen = TMZ_ZNAMELEN ;
	    rs = (strnwcpy(op->zname,znamelen,zp,zl) - op->zname) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tmz_setzone) */

int tmz_gettm(tmz *op,TM *tmp) noex {
	int		rs = SR_FAULT ;
	if (op && tmp) {
	    rs = SR_OK ;
	    *tmp = op->st ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tmz_gettm) */

int tmz_getdst(tmz *op) noex {
	return op->st.tm_isdst ;
}
/* end subroutine (tmz_getdst) */

int tmz_getzoff(tmz *op) noex {
	return op->zoff ;
}
/* end subroutine (tmz_getzoff) */


/* private subroutines */

/* format> hh:mm[:ss] */
static int tmz_timeparts(tmz *op,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	int		si = 0 ;
	if (field fsb ; (rs = fsb.start(sp,sl)) >= 0) {
	    int		v ;
	    int		fl ;
	    cchar	*lp = sp ;
	    cchar	*fp{} ;
	    /* get hours */
	    if ((fl = fsb.get(tpterms,&fp)) > 0) {
	        lp = (fp + fl) ;
	        rs = cfdeci(fp,fl,&v) ;
	        op->st.tm_hour = v ;
	    }
	    if ((rs >= 0) && (fsb.term == ':')) {
	        /* get minutes */
	        if ((fl = fsb.get(tpterms,&fp)) > 0) {
	            lp = (fp + fl) ;
	            rs = cfdeci(fp,fl,&v) ;
	            op->st.tm_min = v ;
	        }
	    } /* end if */
	    if ((rs >= 0) && (fsb.term == ':')) {
		/* get seconds */
	        if ((fl = fsb.get(tpterms,&fp)) > 0) {
	            lp = (fp + fl) ;
	            rs = cfdeci(fp,fl,&v) ;
	            op->st.tm_sec = v ;
	        }
	    } /* end if */
	    si = (lp - sp) ;
	    rs1 = fsb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (field) */
	return (rs >= 0) ? si : rs ;
}
/* end subroutine (tmz_timeparts) */

static int tmz_stdtrailing(tmz *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		si = 0 ;
	if (int wi ; (wi = siskipwhite(sp,sl)) >= 0) {
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
static int tmz_procday(tmz *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		si = 0 ;
	cchar		*cp{} ;
	if (int cl ; (cl = sfnext(sp,sl,&cp)) > 0) {
	    cint	tch = mkchar(*cp) ;
	    if (isdigitlatin(tch)) {
	        if (int v{} ; (rs = cfdeci(cp,cl,&v)) >= 0) {
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
static int tmz_procmonth(tmz *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		si = 0 ;
	cchar		*cp{} ;
	if (int cl ; (cl = sfnext(sp,sl,&cp)) > 0) {
	    int		ch = mkchar(*cp) ;
	    if (isalphalatin(ch)) {
	        int	ml = cl ;
	        cchar	*mp = cp ;
	        si += ((cp+cl)-sp) ;
	        sp += si ;
	        sl -= si ;
	        if ((cl = sfnext(sp,sl,&cp)) > 0) {
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

static int tmz_procyear(tmz *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		si = 0 ;
	cchar		*cp{} ;
	if (int cl ; (cl = sfnext(sp,sl,&cp)) > 0) {
	    cint	ch = mkchar(*cp) ;
	    if (isdigitlatin(ch)) {
	        rs = tmstrsyear(cp,cl) ;
	        op->st.tm_year = rs ;
	        op->f.year = true ;
	        si = ((cp+cl)-sp) ;
	    }
	} /* end if (sfnext) */
	return (rs >= 0) ? si : rs ;
}
/* end subroutine (tmz_procyear) */

static int tmz_proczoff(tmz *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		si = 0 ;
	cchar		*cp{} ;
	if (int cl ; (cl = sfnext(sp,sl,&cp)) > 0) {
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
	} /* end if (sfnext) */
	return (rs >= 0) ? si : rs ;
}
/* end subroutine (tmz_proczoff) */

static int tmz_proczname(tmz *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		si = 0 ;
	cchar		*cp{} ;
	if (int cl ; (cl = sfnext(sp,sl,&cp)) > 0) {
	    cint	ch = mkchar(*cp) ;
	    if (isalphalatin(ch)) {
	        cint	znl = TMZ_ZNAMELEN ;
	        rs = strnwcpy(op->zname,znl,cp,cl)  - op->zname ;
	        si = ((cp+cl)-sp) ;
	    }
	} /* end if (sfnext) */
	return (rs >= 0) ? si : rs ;
}
/* end subroutine (tmz_proczname) */

static int tmz_yearadj(tmz *op,int sc) noex {
	TM		*stp = &op->st ;
	if (stp->tm_year >= 0) {
	    op->f.year = true ;
	    if (sc >= 0) {
	        cint	yy = ((sc*nyears)-TM_YEAR_BASE) ;
	        stp->tm_year += yy ;
	    } else {
	        if ((stp->tm_year >= 0) && (stp->tm_year <= 38)) {
	            stp->tm_year += nyears ;
	        } else if (stp->tm_year >= TM_YEAR_BASE) {
	            stp->tm_year -= TM_YEAR_BASE ;
		}
	    }
	} /* end if (had a year) */
	return SR_OK ;
}
/* end subroutine (tmz_yearadj) */

/* parse minutes west of GMT */
static int getzoff(int *zop,cchar *sp,int sl) noex {
	int		rs = SR_INVALID ;
	int		cl ;
	int		zoff ;
	int		ch = mkchar(*sp) ;
	bool		f = false ;
	cchar		*cp ;
	f = f || isplusminus(ch) ;
	f = f || isdigitlatin(ch) ;
	if ((sl >= 2) && f) {
	    int		i{} ;
	    int		sign ;
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
	    if (zoff > (14 * 60)) {
	        rs = SR_INVALID ;
	    }
#endif
	    zoff *= sign ;
	    if (zop) {
	        *zop = zoff ;
	    }
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
	bool		f = false ;
	for (i = 0 ; sl-- && *sp ; i += 1) {
	    cint	ch = mkchar(sp[i]) ;
	    f = f || (ch == '_') ;
	    f = f || isalphalatin(ch) ;
	    if (f) break ;
	} /* end while */
	return i ;
}
/* end subroutine (silogend) */

static cchar *strnzone(cchar *sp,int sl) noex {
	bool		f = false ;
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

#ifdef	COMMENT
static bool	isgoodname(cchar *,int) noex ;
/* do we have a valid time-zone name */
static bool isgoodname(cchar *sp,int sl) noex {
	bool		f = false ;
	while ((sl != 0) && (sp[0] != '\0')) {
	    cint	ch = mkchar(*sp) ;
	    f = isalnumlatin(ch) ;
	    if (! f) break ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	return f ;
}
/* end subroutine (isgoodname) */
#endif /* COMMENT */


