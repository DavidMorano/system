/* tmz SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

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
	I am enhancing this object (specially the |tmz_xstd()|
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
	and are human-readable.  There are tons of places where ASCII
	strings that represent a date are used.  I will leave it as
	an exercise for the reader to think up some of those!

	Note for STD type dates:

	Note that our |tmz_xstd()| will parse both the old style
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
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<estrings.h>
#include	<field.h>
#include	<tmstrs.h>
#include	<cfdec.h>
#include	<mkchar.h>
#include	<char.h>
#include	<hasx.h>
#include	<ischarx.h>
#include	<localmisc.h>		/* |NYEARS_CENTURY| */

#include	"tmz.hh"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		znlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
}


/* forward references */

static int	mkvars() noex ;

template<typename ... Args>
static int tmz_ctor(tmz *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
    	    static cint		rsv = mkvars() ;
	    if ((rs = rsv) >= 0) {
	        if (op->zname == nullptr) {
	            if (char *a ; (rs = malloc_zn(&a)) >= 0) {
		        op->zname = a ;
		        memclear(a,rs) ;
		        a[rs] = '\0' ;
	            } /* end if (memory-allocation) */
	        } /* end if (zname was NULL) */
	    } /* end if (mkvars) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tmz_ctor) */

static int	tmz_timeparts(tmz *,cchar *,int) noex ;
static int	tmz_xstdtrailing(tmz *,cchar *,int) noex ;
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

static vars		var ;

constexpr int		nyears = NYEARS_CENTURY ;
constexpr bool		f_comment = false ;


/* exported variables */


/* exported subroutines */

int tmz_init(tmz *op) noex {
	int		rs ;
	if ((rs = tmz_ctor(op)) >= 0) {
	    rs = op->clear() ;
	    op->zoff = SHORT_MIN ;
	    if (rs < 0) {
		op->dtor() ;
	    }
	} /* end if (tmz_ctor) */
	return rs ;
}
/* end subroutine (tmz_init) */

/* format> [Wed] Nov 14 19:24[:04] [EST] [[19]99] [±0400] */
int tmz_xstd(tmz *op,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = tmz_ctor(op,sp)) >= 0) {
	    if (sl < 0) sl = xstrlen(sp) ;
	    rs = op->clear() ;
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
	        rs = tmz_xstdtrailing(op,sp,sl) ;
	        if (rs == 0) break ;
	        sp += rs ;
	        sl -= rs ;
	    } /* end for */
	    if (rs >= 0) {
		cint	znl = var.znlen ;
	        rs = xstrnlen(op->zname,znl) ;
	    }
	    if (rs < 0) {
		op->dtor() ;
	    }
	} /* end if (tmz_ctor) */
	return rs ;
}
/* end subroutine (tmz_xstd) */

/* format> [Weekday,] DD MMM [CC]YY hh:mm[:ss] [Â±hhmm] [zname] */
int tmz_xmsg(tmz *op,cchar *sp,int sl) noex {
	int		rs ;
	int		zl = 0 ;
	if ((rs = tmz_ctor(op,sp)) >= 0) {
	    if (sl < 0) sl = xstrlen(sp) ;
	    rs = op->clear() ;
	    op->zoff = SHORT_MIN ;
	    op->st.tm_year = -1 ;
	    op->st.tm_wday = -1 ;
	    op->st.tm_isdst = -1 ;
	    if (cchar *tp ; (tp = strnchr(sp,sl,',')) != nullptr) {
		cint	tl = intconv(tp - sp) ;
	        cchar	*cp{} ;
	        if (int cl ; (cl = sfnext(sp,tl,&cp)) > 0) {
	            rs = tmstrsday(cp,cl) ;
	            op->st.tm_wday = rs ;
	        }
	        sl -= intconv((tp + 1) - sp) ;
	        sp = (tp + 1) ;
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
		cint	znl = var.znlen ;
	        rs = xstrnlen(op->zname,znl) ;
	        zl = rs ; /* return value for subroutine */
	    }
	    if (rs < 0) {
		op->dtor() ;
	    }
	} /* end if (tmz_ctor) */
	return (rs >= 0) ? zl : rs ;
}
/* end subroutine (tmz_xmsg) */

/* convert from a TOUCH (original) format> MMDDhhmm[YY] */
int tmz_xtouch(tmz *op,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = tmz_ctor(op,sp)) >= 0) {
	    TM		*stp = &op->st ;
	    if (sl < 0) sl = xstrlen(sp) ;
	    rs = op->clear() ;
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
	            op->fl.year = true ;
	            if ((stp->tm_year >= 0) && (stp->tm_year <= 38)) {
	                stp->tm_year += nyears ;
	            }
	        } else if (i < (n-1)) {
	            rs = SR_INVALID ;
	        }
	    } else {
	        rs = SR_INVALID ;
	    }
	    if (rs < 0) {
		op->dtor() ;
	    }
	} /* end if (tmz_ctor) */
	return rs ;
}
/* end subroutine (tmz_xtouch) */

/* convert from a TOUCH-t (new '-t') format> [[CC]YY]MMDDhhmm[.SS] */
int tmz_xtoucht(tmz *op,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = tmz_ctor(op,sp)) >= 0) {
            TM		*stp = &op->st ;
            if (sl < 0) sl = xstrlen(sp) ;
	    rs = op->clear() ;
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
                cint        cl = intconv(sl-((tp+1)-sp)) ;
                cchar       *cp = (tp+1) ;
                if (cl >= 2) {
                    cint    tch = mkchar(*cp) ;
                    if (isdigitlatin(tch)) {
                        stp->tm_sec = val(cp) ;
                    } else {
                        rs = SR_INVALID ;
                    }
                }
                sl = intconv(tp - sp) ;
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
                        op->fl.year = true ;
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
	    if (rs < 0) {
		op->dtor() ;
	    }
	} /* end if (tmz_ctor) */
	return rs ;
}
/* end subroutine (date_toucht) */

/* format> [[CC]]YYMMDDhhmm[ss][Â±hhmm][zname] */
int tmz_xstrdig(tmz *op,cchar *sp,int sl) noex {
	int		rs ;
	int		zl = 0 ;
	if ((rs = tmz_ctor(op,sp)) >= 0) {
	    TM		*stp = &op->st ;
	    if (sl < 0) sl = xstrlen(sp) ;
	    rs = op->clear() ;
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
	        int	cl = intconv(sl - (tp - sp)) ;
	        if ((cl >= 1) && isplusminus(*cp)) { /* ok */
	            int		zol = cl ;
	            int		zo ;
	            cchar	*zop = cp ;
	            if (int si ; (si = sialpha(cp,cl)) > 0) {
	                zol = si ;
	            }
	            if ((rs = getzoff(&zo,zop,zol)) >= 0) {
	                op->zoff = shortconv(zo) ;
	                op->fl.zoff = true ;
	                cp += rs ;
	                cl -= rs ;
	            }
	        }
	        if ((rs >= 0) && (cl > 0)) {
	            cint	ch = mkchar(*cp) ;
	            if (isalphalatin(ch)) {
		        cint	znl = var.znlen ;
			cchar	*znp = op->zname ;
	                rs = intconv(strnwcpy(op->zname,znl,cp,cl) - znp) ;
	                zl = rs ;
	            } else {
	                rs = SR_INVALID ;
	            }
	        }
	        sl = intconv(tp - sp) ;
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
	    if (rs < 0) {
		op->dtor() ;
	    }
	} /* end if (tmz_ctor) */
	return (rs >= 0) ? zl : rs ;
}
/* end subroutine (tmz_xstrdig) */

/* format> [CC]YYMMDD_hhmm[:ss][_][zname] */
int tmz_xlogz(tmz *op,cchar *sp,int sl) noex {
	int		rs ;
	int		zl = 0 ;
	if ((rs = tmz_ctor(op,sp)) >= 0) {
	    TM		*stp = &op->st ;
            if (sl < 0) sl = xstrlen(sp) ;
	    rs = op->clear() ;
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
                int         si = intconv(tp - sp) ;
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
                                zl = xstrlen(op->zname) ;
                            }
                        } else {
                            rs = SR_INVALID ;
                        } /* end if (hasalldig) */
                    } /* end if (silogend) */
                } else {
                    rs = SR_INVALID ;
                }
            } /* end if (ok) */
	    if (rs < 0) {
		op->dtor() ;
	    }
	} /* end if (tmz_ctor) */
	return (rs >= 0) ? zl : rs ;
}
/* end subroutine (tmz_xlogz) */

/* format> [CC]YYMMDD (like abbreviated variation of "touch") */
int tmz_xday(tmz *op,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = tmz_ctor(op,sp)) >= 0) {
	    TM		*stp = &op->st ;
	    if (sl < 0) sl = xstrlen(sp) ;
	    rs = op->clear() ;
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
	    if (rs < 0) {
		op->dtor() ;
	    }
	} /* end if (tmz_ctor) */
	return rs ;
}
/* end subroutine (tmz_xday) */

int tmz_isset(tmz *op) noex {
	int		rs ;
	if ((rs = tmz_ctor(op)) >= 0) {
	    rs = op->st.tm_mday ;
	    if (rs < 0) {
		op->dtor() ;
	    }
	} /* end if (tmz_ctor) */
	return rs ;
}
/* end subroutine (tmz_isset) */

int tmz_hasyear(tmz *op) noex {
	int		rs ;
	if ((rs = tmz_ctor(op)) >= 0) {
	    rs = op->fl.year ;
	    if (rs < 0) {
		op->dtor() ;
	    }
	} /* end if (tmz_ctor) */
	return rs ;
}
/* end subroutine (tmz_hasyear) */

int tmz_haszoff(tmz *op) noex {
	int		rs ;
	if ((rs = tmz_ctor(op)) >= 0) {
	    rs = op->fl.zoff ;
	    if (rs < 0) {
		op->dtor() ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tmz_haszoff) */

int tmz_haszone(tmz *op) noex {
	int		rs ;
	if ((rs = tmz_ctor(op)) >= 0) {
	    rs = (op->zname[0] != '\0') ;
	    if (rs < 0) {
		op->dtor() ;
	    }
	} /* end if (tmz_ctor) */
	return rs ;
}
/* end subroutine (tmz_haszone) */

int tmz_setday(tmz *op,int y,int m,int d) noex {
	int		rs ;
	if ((rs = tmz_ctor(op)) >= 0) {
	    TM		*stp = &op->st ;
	    cint	sc = -1 ;
	    if (y >= TM_YEAR_BASE) {
	        y -= TM_YEAR_BASE ;
	    }
	    stp->tm_year = y ;
	    stp->tm_mon = m ;
	    stp->tm_mday = d ;
	    rs = tmz_yearadj(op,sc) ;
	    if (rs < 0) {
		op->dtor() ;
	    }
	} /* end if (tmz_ctor) */
	return rs ;
}
/* end subroutine (tmz_setday) */

int tmz_setyear(tmz *op,int year) noex {
	int		rs ;
	if ((rs = tmz_ctor(op)) >= 0) {
	    op->st.tm_year = year ;
	    op->fl.year = true ;
	    if (rs < 0) {
		op->dtor() ;
	    }
	} /* end if (tmz_ctor) */
	return rs ;
}
/* end subroutine (tmz_setyear) */

int tmz_setzone(tmz *op,cchar *zp,int zl) noex {
	int		rs ;
	if ((rs = tmz_ctor(op,zp)) >= 0) {
	    cint	znl = var.znlen ;
	    cchar	*znp = op->zname ;
	    rs = intconv(strnwcpy(op->zname,znl,zp,zl) - znp) ;
	    if (rs < 0) {
		op->dtor() ;
	    }
	} /* end if (tmz_ctor) */
	return rs ;
}
/* end subroutine (tmz_setzone) */

int tmz_gettm(tmz *op,TM *tmp) noex {
	int		rs ;
	if ((rs = tmz_ctor(op,tmp)) >= 0) {
	    *tmp = op->st ;
	    if (rs < 0) {
		op->dtor() ;
	    }
	} /* end if (tmz_ctor) */
	return rs ;
}
/* end subroutine (tmz_gettm) */

int tmz_getdst(tmz *op) noex {
    	int		rs ;
	if ((rs = tmz_ctor(op)) >= 0) {
	    rs = op->st.tm_isdst ;
	    if (rs < 0) {
		op->dtor() ;
	    }
	} /* end if (tmz_ctor) */
	return rs ;
}
/* end subroutine (tmz_getdst) */

int tmz_getzoff(tmz *op) noex {
    	int		rs ;
	if ((rs = tmz_ctor(op)) >= 0) {
	    rs = op->zoff ;
	    if (rs < 0) {
		op->dtor() ;
	    }
	} /* end if (tmz_ctor) */
	return rs ;
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
	    si = intconv(lp - sp) ;
	    rs1 = fsb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (field) */
	return (rs >= 0) ? si : rs ;
}
/* end subroutine (tmz_timeparts) */

static int tmz_xstdtrailing(tmz *op,cchar *sp,int sl) noex {
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
	        } else if (isdigitlatin(ch) && (! op->fl.year)) {
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
/* end subroutine (tmz_xstdtrailing) */

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
	        	si = intconv((cp + cl) - sp) ;
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
	        si += intconv((cp + cl) - sp) ;
	        sp += si ;
	        sl -= si ;
	        if ((cl = sfnext(sp,sl,&cp)) > 0) {
	            ch = mkchar(*cp) ;
	            if (isalphalatin(ch)) {
	                rs = tmstrsday(mp,ml) ;
	                op->st.tm_wday = rs ;
	                mp = cp ;
	                ml = cl ;
	                si += intconv((cp + cl) - sp) ;
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
	        op->fl.year = true ;
	        si = intconv((cp + cl) - sp) ;
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
	        if (int v ; (rs = getzoff(&v,cp,cl)) >= 0) {
	            op->zoff = shortconv(v) ;
	            op->fl.zoff = true ;
	            si = intconv((cp + cl) - sp) ;
		}
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
	        cint	znl = var.znlen ;
		cchar	*znp = op->zname ;
	        rs = intconv(strnwcpy(op->zname,znl,cp,cl)  - znp) ;
	        si = intconv((cp + cl) - sp) ;
	    }
	} /* end if (sfnext) */
	return (rs >= 0) ? si : rs ;
}
/* end subroutine (tmz_proczname) */

static int tmz_yearadj(tmz *op,int sc) noex {
	TM		*stp = &op->st ;
	if (stp->tm_year >= 0) {
	    op->fl.year = true ;
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
	int		ch = mkchar(*sp) ;
	bool		f = false ;
	f = f || isplusminus(ch) ;
	f = f || isdigitlatin(ch) ;
	if ((sl >= 2) && f) {
	    int		i{} ; /* used-afterwards */
	    int		zoff ;
	    int		sign ;
	    int		hours ;
	    int		mins ;
	    int		cl = sl ;
	    cchar	*cp = sp ;
	    ch = mkchar(*sp) ;
	    rs = SR_OK ;
	    sign = ((*sp == '+') || isdigitlatin(ch)) ? -1 : 1 ;
	    if ((*sp == '-') || (*sp == '+')) {
	        cp += 1 ;
	        cl -= 1 ;
	    }
	    auto lamb = [&cp,&cl] (int &idx) {
		cint	ich = mkchar(cp[idx]) ;
		bool	lf = true ;
		lf = lf && (idx < cl) ;
		lf = lf && ich ;
		lf = lf && (! CHAR_ISWHITE(ich)) ;
		lf = lf && (ich != ',') ; 
		return (lf) ? ich : 0  ;
	    } ; /* end lambda (lamb) */
	    for (i = 0 ; (ch = lamb(i)) > 0 ; i += 1) {
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
	    /* reportedly, there are timezones at up to 14 hours off of GMT! */
	    if_constexpr (f_comment) {
	        if (zoff > (14 * 60)) {
	            rs = SR_INVALID ;
	        }
	    } /* end if_constexpr (f_comment) */
	    {
	        zoff *= sign ;
	        if (zop) {
	            *zop = zoff ;
	        }
	    }
	    if (rs >= 0) {
	        rs = intconv(cp - sp) ;
	    }
	} /* end if (getting timezone offset) */
	return rs ;
}
/* end subroutine (getzoff) */

static int tmz_clear(tmz *op) noex {
	op->st = {} ;
	op->fl = {} ;
	op->zoff = 0 ;
	return SR_OK ;
}
/* end method (tmz::clæar) */

int tmz::xstd(cchar *sp,int sl) noex {
	return tmz_xstd(this,sp,sl) ;
}

int tmz::xmsg(cchar *sp,int sl) noex {
	return tmz_xmsg(this,sp,sl) ;
}

int tmz::xtouch(cchar *sp,int sl) noex {
	return tmz_xtouch(this,sp,sl) ;
}

int tmz::xtoucht(cchar *sp,int sl) noex {
	return tmz_xtoucht(this,sp,sl) ;
}

int tmz::xstrdig(cchar *sp,int sl) noex {
	return tmz_xstrdig(this,sp,sl) ;
}

int tmz::xlogz(cchar *sp,int sl) noex {
	return tmz_xlogz(this,sp,sl) ;
}

int tmz::xday(cchar *sp,int sl) noex {
	return tmz_xday(this,sp,sl) ;
}

int tmz::setday(int y,int m,int d) noex {
	return tmz_setday(this,y,m,d) ;
}

int tmz::setyear(int year) noex {
	return tmz_setyear(this,year) ;
}

int tmz::setzone(cchar *sp,int sl) noex {
	return tmz_setzone(this,sp,sl) ;
}

int tmz::gettm(TM *tmp) noex {
	return tmz_gettm(this,tmp) ;
}

void tmz::dtor() noex {
    int		rs = SR_OK ;
    int		rs1 ;
    if (zname) {
	rs1 = uc_free(zname) ;
	if (rs >= 0) rs = rs1 ;
	zname = nullptr ;
    }
    if (rs < 0) {
	ulogerror("tmx",rs,"dtor-ucfree") ;
    }
}
/* end method (tmz::dtor) */

tmz_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case tmzmem_clear:
	        rs = tmz_clear(op) ;
	        break ;
	    case tmzmem_init:
	        rs = tmz_init(op) ;
	        break ;
	    case tmzmem_isset:
	        rs = tmz_isset(op) ;
	        break ;
	    case tmzmem_hasyear:
	        rs = tmz_hasyear(op) ;
	        break ;
	    case tmzmem_haszoff:
	        rs = tmz_haszoff(op) ;
	        break ;
	    case tmzmem_haszone:
	        rs = tmz_haszone(op) ;
	        break ;
	    case tmzmem_getdst:
	        rs = tmz_getdst(op) ;
	        break ;
	    case tmzmem_getzoff:
	        rs = tmz_getzoff(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (tmz_co::operator) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = getbufsize(getbufsize_zn)) >= 0) {
	    znlen = rs ;
	}
	return rs ;
}

static int mkvars() noex {
    	return var ;
}

static int val(cchar *sp) noex {
	int		v = 0 ;
	v += (10 * (sp[0] - '0')) ;
	v += ( 1 * (sp[1] - '0')) ;
	return v ;
}
/* end subroutine (val) */

static int silogend(cchar *sp,int sl) noex {
	int		i ; /* used-afterwards */
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


