/* dater_main SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* general dater object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	Originally created due to frustration with various other
	"fuzzy" date conversion subroutines.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************
 
  	Object:
	dater

  	Description:
	This object can be used to create daters from various input
	data including strings.

	Note:
	The timezone offset value in 'TIMEB' is the minutes
	west of GMT.  This is a positive value for timezones that
	are west of Greenwich. It is negative for timezones east
	of Greenwich. This is opposite from what you will see in
	email headers (for example). Our number here must be
	subtracted from GMT in order to get the local time.

	Frustration note:
	What an incredible pain this time-dater handling stuff all
	is?  This file doesn't even do justice to a small fraction
	of the real problems associated with date management!  The
	problem is that the dater changes as time progresses.
	Changes are due to timezone differences and year leaps and
	leap seconds.  The fact that timezone data is not a part
	of many daters only complicates matters worse as we then
	have to try and figure out a reasonable timezone when they
	are not supplied.

	Comment parseing for RFC-822 dates:
	Note that dates given to us for MSG (RFC-822) processing,
	might have comments in them.  These comments are those
	specified in RFC-822.  Note that the tmz object does some
	comment processing on MSG dates but only in the same way
	that NetNews does ; namely, only for a comment at the end
	of the string and which contains a time-zone-name.

	Full comment parsing is done on MSG dates using a comparse
	object. With comparse processing, we still try to divine a
	time-zone-name from the leading part of the resulting
	comment.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/timeb.h>
#include	<climits>		/* |SHORT_MIN| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<ctime>
#include	<tzfile.h>		/* for TM_YEAR_BASE */
#include	<usystem.h>
#include	<estrings.h>
#include	<calstrs.h>
#include	<sbuf.h>
#include	<getdefzinfo.h>
#include	<tmtime.h>		/* CHRONO */
#include	<zos.h>			/* CHRONO */
#include	<comparse.h>		/* CHRONO */
#include	<tmz.h>			/* CHRONO */
#include	<zdb.h>			/* CHRONO */
#include	<mkchar.h>
#include	<hasx.h>		/* |hasalldig(3uc)| */
#include	<ischarx.h>		/* |isdigitlatin(3uc)| */
#include	<localmisc.h>		/* |TIMEBUFLEN| */

#include	"dater.h"


/* local defines */

#ifndef	TZO_EMPTY
#define	TZO_EMPTY	SHORT_MIN
#endif

#ifndef	TZO_MAXZOFF
#define	TZO_MAXZOFF	(14*60)
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct knownzone {
	cchar		*zname ;
	int		zoff ;
	int		isdst ;
} ;


/* forward references */

static int	dater_initcur(dater *) noex ;
static int	dater_ldname(dater *,cchar *,int) noex ;
static int	dater_pname(dater *) noex ;
static int	dater_pnum(dater *) noex ;
static int	dater_findzname(dater *) noex ;
static int	dater_findzoff(dater *,TM *) noex ;
static int	dater_mkptime(dater *,TM *,int) noex ;
static int	dater_mktime(dater *,TM *) noex ;
static int	dater_mkpzoff(dater *,TM *,int) noex ;

static int	dater_initbase(dater *) noex ;
static int	dater_ldtmz(dater *,tmz *) noex ;
static int	dater_ldcomzone(dater *,comparse *) noex ;
static int	dater_ldzname(dater *,cchar *,int) noex ;
static int	dater_defs(dater *,tmz *) noex ;


/* local variables */

constexpr int		znamelen = DATER_ZNAMELEN ;


/* exported variables */


/* exported subroutines */

int dater_start(dater *dp,TIMEB *nowp,cchar *znp,int znl) noex {
	int		rs = SR_FAULT ;
	if (dp) {
	    rs = memclear(dp) ;
	    dp->b.timezone = TZO_EMPTY ;
	    dp->b.dstflag = -1 ;
	    if (nowp != nullptr) {
	        dp->f.cb = true ;
	        dp->cb = *nowp ;
	    } /* end if (time-offset) */
	    if ((znp != nullptr) && (znl != 0)) {
	        dp->f.czn = true ;
	        strnwcpy(dp->cname,DATER_ZNAMELEN,znp,znl) ;
	    }
	    dp->magic = DATER_MAGIC ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dater_start) */

int dater_startcopy(dater *dp,dater *d2p) noex {
	int		rs ;
	if ((rs = dater_magic(d2p,dp)) >= 0) {
	    memcpy(dp,d2p) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_startcopy) */

int dater_finish(dater *dp) noex {
	int		rs ;
	if ((rs = dater_magic(dp)) >= 0) {
	    dp->b.time = 0 ;
	    dp->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_finish) */

/* copy one dater to another */
int dater_setcopy(dater *dp,dater *d2p) noex {
	int		rs ;
	if ((rs = dater_magic(d2p,dp)) >= 0) {
	    memcpy(dp,d2p) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_setcopy) */

int dater_setstd(dater *dp,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = dater_magic(dp,sp)) >= 0) {
	    tmz		stz ;
	    if (sl < 0) sl = strlen(sp) ;
	    if ((rs = tmz_std(&stz,sp,sl)) >= 0) {
	        TM	dst = stz.st ;
	        dater_ldtmz(dp,&stz) ;
	        if ((rs = dater_defs(dp,&stz)) >= 0) {
	            rs = dater_mktime(dp,&dst) ;
	        }
	    } /* end if (tmz_std) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_setstd) */

/* set the dater from a message-type string */
int dater_setmsg(dater *dp,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = dater_magic(dp,sp)) >= 0) {
	    comparse	vc ;
	    if (sl < 0) sl = strlen(sp) ;
	    if ((rs = comparse_start(&vc,sp,sl)) >= 0) {
	        int	vl ;
	        cchar	*vp{} ;
	        if ((rs = comparse_getval(&vc,&vp)) >= 0) {
	            tmz		stz ;
	            vl = rs ;
	            if ((rs = tmz_msg(&stz,vp,vl)) >= 0) {
	                TM	dst = stz.st ;
	                dater_ldtmz(dp,&stz) ;
	                dater_ldcomzone(dp,&vc) ;
	                if ((rs = dater_defs(dp,&stz)) >= 0) {
	                    rs = dater_mktime(dp,&dst) ;
	                }
	            } /* end if (tmz_msg) */
	        } /* end if (comparse_getval) */
	        rs1 = comparse_finish(&vc) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (comparse) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_setmsg) */

/* set from a dater-like or decimal digit time string */
/* format> [CC]YYMMDDhhmm[ss][±<zoff>][<zname>] */
int dater_setstrdig(dater *dp,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = dater_magic(dp,sp)) >= 0) {
	    tmz		stz ;
	    if (sl < 0) sl = strlen(sp) ;
	    if ((rs = tmz_strdig(&stz,sp,sl)) >= 0) {
	        TM	dst = stz.st ;
	        dater_ldtmz(dp,&stz) ;
	        if ((rs = dater_defs(dp,&stz)) >= 0) {
	            rs = dater_mktime(dp,&dst) ;
	        }
	    } /* end if tmz_strdig) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_setstrdig) */


/* set from a LOGZ-type string */

/*
This amounts to a conversion from the string:
      990704_1647:33_EDT
to the dater object.
*/

int dater_setlogz(dater *dp,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = dater_magic(dp,sp)) >= 0) {
	    tmz		stz ;
	    if (sl < 0) sl = strlen(sp) ;
	    if ((rs = tmz_logz(&stz,sp,sl)) >= 0) {
	        TM	dst = stz.st ;
	        dater_ldtmz(dp,&stz) ;
	        if ((rs = dater_defs(dp,&stz)) >= 0) {
	            rs = dater_mktime(dp,&dst) ;
	        }
	    } /* end if (tmz_logz) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_setlogz) */

/* set from TOUCH (original) time string */
int dater_settouch(dater *dp,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = dater_magic(dp,sp)) >= 0) {
	    tmz		stz ;
	    if (sl < 0) sl = strlen(sp) ;
	    if ((rs = tmz_touch(&stz,sp,sl)) >= 0) {
	        TM	dst = stz.st ;
	        dater_ldtmz(dp,&stz) ;
	        if ((rs = dater_defs(dp,&stz)) >= 0) {
	            rs = dater_mktime(dp,&dst) ;
	        }
	    } /* end if (tmz_touch) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_settouch) */

/* set from TOUCH-t (new '-t' version) time string */
int dater_settoucht(dater *dp,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = dater_magic(dp,sp)) >= 0) {
	    tmz		stz ;
	    if (sl < 0) sl = strlen(sp) ;
	    if ((rs = tmz_toucht(&stz,sp,sl)) >= 0) {
	        TM	dst = stz.st ;
	        dater_ldtmz(dp,&stz) ;
	        if ((rs = dater_defs(dp,&stz)) >= 0) {
	            rs = dater_mktime(dp,&dst) ;
	        }
	    } /* end if (tmz_toucht) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_settoucht) */

/* set from a split-out time, zone offset, and zone-name */
int dater_settmzon(dater *dp,TM *stp,int zoff,cchar *zstr,int zlen) noex {
	int		rs ;
	if ((rs = dater_magic(dp,stp)) >= 0) {
	    rs = SR_INVALID ;
	    if ((zoff == TZO_EMPTY) || (abs(zoff) <= TZO_MAXZOFF)) {
	        rs = dater_initbase(dp) ;
/* load the zone-offset (authoritative) */
	        if ((rs >= 0) && (zoff != TZO_EMPTY)) {
	            dp->f.zoff = true ;
	            dp->b.timezone = zoff ;
	        }
/* lookup the zone-name */
	        if ((rs >= 0) && (zstr != nullptr) && (zstr[0] != '\0')) {
	            ZDB		zr ;
	            dp->f.zname = true ;
	            strnwcpy(dp->zname,DATER_ZNAMELEN,zstr,zlen) ;
	            if ((rs = zdb_nameoff(&zr,zstr,zlen,zoff)) >= 0) {
	                if (stp->tm_isdst < 0) stp->tm_isdst = zr.isdst ;
	                if (zoff == TZO_EMPTY) zoff = zr.off ;
	            } else if (rs == SR_NOTFOUND) {
	                rs = SR_OK ;
	            } /* end if (got a match) */
	        } /* end if (name lookup) */
/* calculate the time */
	        if (rs >= 0) {
	            rs = dater_mkptime(dp,stp,zoff) ;
	        }
/* now do something with the name */
	        if ((rs >= 0) && (dp->zname[0] == '\0')) {
	            rs = dater_mkpzoff(dp,stp,zoff) ;
	        }
	        if (rs >= 0) dp->magic = DATER_MAGIC ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_settmzon) */

/* set from split-out time and zone offset only (minutes west of GMT) */
int dater_settmzo(dater *dp,TM *stp,int zoff) noex {
	int		rs ;
	if ((rs = dater_magic(dp,stp)) >= 0) {
	    rs = SR_INVALID ;
	    if ((zoff == TZO_EMPTY) || (abs(zoff) <= TZO_MAXZOFF)) {
	        rs = dater_initbase(dp) ;
/* calculate the time */
	        if (rs >= 0) {
	            rs = dater_mkptime(dp,stp,zoff) ;
	        }
/* try to find a time zone-name for the information we have */
	        if (rs >= 0) dp->magic = DATER_MAGIC ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_settmzo) */

/* set from a split-out time and zone-name only */
int dater_settmzn(dater *dp,TM *stp,cchar *zstr,int zlen) noex {
	int		rs ;
	if ((rs = dater_magic(dp,stp)) >= 0) {
	    int		zoff = TZO_EMPTY ;
/* initialize */
	    rs = dater_initbase(dp) ;
/* lookup the zone-name */
	    if ((rs >= 0) && (zstr != nullptr) && (zstr[0] != '\0')) {
	        ZDB		zr ;
	        dp->f.zname = true ;
	        strnwcpy(dp->zname,DATER_ZNAMELEN,zstr,zlen) ;
	        if ((rs = zdb_name(&zr,zstr,zlen)) >= 0) {
	            if (stp->tm_isdst < 0) stp->tm_isdst = zr.isdst ;
	            if (zoff == TZO_EMPTY) zoff = zr.off ;
	        } else if (rs == SR_NOTFOUND) {
	            rs = SR_OK ;
	        } /* end if (zdb_name) */
	    } /* end if (name lookup) */
/* calculate the time */
	    if (rs >= 0) {
	        rs = dater_mkptime(dp,stp,zoff) ;
	    }
	    if (rs >= 0) dp->magic = DATER_MAGIC ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_settmzn) */

/* set from ( time, timezone_name, DST_indication ) */
int dater_settimezn(dater *dp,time_t t,cchar *zname,int isdst) noex {
	int		rs ;
	if ((rs = dater_magic(dp)) >= 0) {
	    rs = dater_initbase(dp) ;
	    dp->b.time = t ;
	    dp->b.dstflag = isdst ;
	    if ((zname != nullptr) && (zname[0] != '\0')) {
	        rs = dater_ldname(dp,zname,-1) ;
	    } else {
	        TMTIME	tmt ;
	        dp->f.tzset = true ;
	        if ((rs = tmtime_localtime(&tmt,t)) >= 0) {
	            zname = tmt.zname ;
	            dp->f.zname = true ;
	            strncpylc(dp->zname,zname,DATER_ZNAMELEN) ;
	            if (isdst < 0) isdst = tmt.isdst ;
	        } /* end if */
	        dp->b.dstflag = isdst ;
	        dp->f.zoff = true ;
	        dp->b.timezone = 0 ;
	        if (isdst >= 0) {
	            dp->b.timezone = (short) (tmt.gmtoff / 60) ;
	        }
	    } /* end if */
	    if (rs >= 0) dp->magic = DATER_MAGIC ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_settimezn) */

/* set from ( time, tz-offset, tz-name, tz-DST_indication ) */
int dater_settimezon(dater *dp,time_t t,int zoff,cchar *zname,int isdst) noex {
	int		rs ;
	if ((rs = dater_magic(dp)) >= 0) {
/* initialize */
	    dp->b.time = t ;
	    dp->b.timezone = zoff ;
	    dp->b.dstflag = isdst ;
	    dp->zname[0] = '\0' ;
	    dp->f.zname = true ;
	    dp->f.zoff = true ;
/* continue */
	    if ((zname != nullptr) && (zname[0] != '\0')) {
	        strncpylc(dp->zname,zname,DATER_ZNAMELEN) ;
	    } else {
	        TMTIME	tmt ;
	        dp->f.tzset = true ;
	        if ((rs = tmtime_localtime(&tmt,t)) >= 0) {
	            zname = tmt.zname ;
	            dp->f.zname = true ;
	            strncpylc(dp->zname,zname,DATER_ZNAMELEN) ;
	            if (isdst < 0) isdst = tmt.isdst ;
	        } /* end if */
	        dp->b.dstflag = isdst ;
	        dp->f.zoff = true ;
	        dp->b.timezone = 0 ;
	        if (isdst >= 0) {
	            dp->b.timezone = (short) (tmt.gmtoff / 60) ;
	        }
	    } /* end if */
	    if (rs >= 0) dp->magic = DATER_MAGIC ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_settimezon) */

int dater_mkdatestr(dater *dp,int type,char *dbuf,int dlen) noex {
	int		rs = SR_OK ;
	int		sl = 0 ;
	if ((rs = dater_magic(dp,dbuf)) >= 0) {
	    rs = SR_INVALID ;
	    if ((type >= 0) && (type < DATER_DTSEND)) {
	        TMTIME	tmt ;
	        time_t	t ;
	        int	zoff ;
	        cchar	*znp ;
	        cchar	*fmt ;
	        if (dlen < 0) dlen = TIMEBUFLEN ;
/* go */
	        t = dp->b.time ;
	        zoff = dp->b.timezone ;
	        znp = dp->zname ;
	        if ((dp->b.timezone == TZO_EMPTY) || (! dp->f.zoff)) {
	            zoff = 0 ;
	            znp = "GMT" ;
	        }
/* selective adjustment */
	        switch (type) {
	        case DATER_DTSENV:
	        case DATER_DTSLOGZ:
	            if (dp->zname[0] == '\0') {
	                zoff = 0 ;
	                znp = "Z" ;
	            }
	            break ;
	        case DATER_DTSGMLOGZ:
	            zoff = 0 ;
	            znp = "GMT" ;
	            break ;
	        } /* end switch (selective adjustment) */
/* conversion */
	        t -= (zoff * 60) ;
	        if ((rs = tmtime_gmtime(&tmt,t)) >= 0) {
	            tmt.gmtoff = (zoff * 60) ;
	            strwcpyuc(tmt.zname,znp,TMTIME_ZNAMELEN) ;
	            switch (type) {
/* UNIX® envelope */
	            case DATER_DTSENV:
	                fmt = "%a %b %d %T %Z %Y %O" ;
	                rs = sntmtime(dbuf,dlen,&tmt,fmt) ;
	                sl = rs ;
	                break ;
/* message header */
	            case DATER_DTSHDR:
	                fmt = "%d %b %Y %T %O (%Z)" ;
	                if (tmt.zname[0] == '\0') fmt = "%d %b %Y %T %O" ;
	                rs = sntmtime(dbuf,dlen,&tmt,fmt) ;
	                sl = rs ;
	                break ;
	            case DATER_DTSSTRDIG:
	                fmt = "%y%m%d%H%M%S%O%Z" ;
	                if (tmt.zname[0] == '\0') fmt = "%y%m%d%H%M%S%O" ;
	                rs = sntmtime(dbuf,dlen,&tmt,fmt) ;
	                sl = rs ;
	                break ;
	            case DATER_DTSLOGZ:
	            case DATER_DTSGMLOGZ:
	                fmt = "%y%m%d_%H%M:%S_%Z" ;
	                if (tmt.zname[0] == '\0') fmt = "%y%m%d_%H%M:%S" ;
	                rs = sntmtime(dbuf,dlen,&tmt,fmt) ;
	                sl = rs ;
	                break ;
	            default:
	                sl = -1 ;
	                strwcpy(dbuf,"** invalid type **",dlen) ;
	                break ;
	            } /* end switch */
	        } /* end if (tmtime_gmtime) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (dater_mkdatestr) */

/* make a mail envelope-type dater string */
int dater_mkstd(dater *dp,char *dbuf,int dlen) noex {
	return dater_mkdatestr(dp,DATER_DTSENV,dbuf,dlen) ;
}
/* end subroutine (dater_mkstd) */

/* make a mail envelope-type dater string */
int dater_mkenv(dater *dp,char *dbuf,int dlen) noex {
	return dater_mkdatestr(dp,DATER_DTSENV,dbuf,dlen) ;
}
/* end subroutine (dater_mkenv) */

/* make a mail envelope-type dater string */
int dater_mkmsg(dater *dp,char *dbuf,int dlen) noex {
	return dater_mkdatestr(dp,DATER_DTSMSG,dbuf,dlen) ;
}
/* end subroutine (dater_mkmsg) */

/* make a mail header dater string */
int dater_mkhdr(dater *dp,char *dbuf,int dlen) noex {
	return dater_mkdatestr(dp,DATER_DTSHDR,dbuf,dlen) ;
}
/* end subroutine (dater_mkhdr) */

/* make the old STRDIG type of dater string */
int dater_mkstrdig(dater *dp,char *dbuf,int dlen) noex {
	return dater_mkdatestr(dp,DATER_DTSSTRDIG,dbuf,dlen) ;
}
/* end subroutine (dater_mkstrdig) */

/* make the familiar LOGZ-type dater string */
int dater_mklogz(dater *dp,char *dbuf,int dlen) noex {
	return dater_mkdatestr(dp,DATER_DTSLOGZ,dbuf,dlen) ;
}
/* end subroutine (dater_mklogz) */

/* make the familiar LOGZ-type dater string */
int dater_mkgmtlogz(dater *dp,char *dbuf,int dlen) noex {
	return dater_mkdatestr(dp,DATER_DTSGMLOGZ,dbuf,dlen) ;
}
/* end subroutine (dater_mkgmlogz) */

int dater_setzinfo(dater *dp,dater_zinfo *zip) noex {
	int		rs ;
	if ((rs = dater_magic(dp,zip)) >= 0) {
	    dp->b.timezone = zip->zoff ;
	    dp->b.dstflag = zip->isdst ;
	    rs = strwcpy(dp->zname,zip->zname,znamelen) - dp->zname ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_setzinfo) */

/* return the UNIX® time out of dater object */
int dater_gettime(dater *dp,time_t *tp) noex {
	int		rs ;
	if ((rs = dater_magic(dp,tp)) >= 0) {
	    *tp = dp->b.time ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_gettime) */

/* return the timezone in minutes west of GMT */
int dater_getzoneoff(dater *dp,int *zp) noex {
	int		rs ;
	if ((rs = dater_magic(dp,zp)) >= 0) {
	    if (! dp->f.zoff) {
	        rs = SR_NOENT ;
	    }
	    if (zp) {
	        *zp = dp->b.timezone ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_getzoneoff) */

/* return the timezone in minutes west of GMT */
int dater_getzonename(dater *dp,char *rbuf,int rlen) noex {
	int		rs ;
	if ((rs = dater_magic(dp,rbuf)) >= 0) {
	    rbuf[0] = '\0' ;
	    if (dp->f.zname) {
	        rs = snwcpy(rbuf,rlen,dp->zname,znamelen) ;
	    } else {
	        rs = SR_NOTFOUND ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_getzonename) */

int dater_getzinfo(dater *dp,dater_zinfo *zip) noex {
	int		rs ;
	if ((rs = dater_magic(dp,zip)) >= 0) {
	    zip->zoff = dp->b.timezone ;
	    zip->isdst = dp->b.dstflag ;
	    rs = strwcpy(zip->zname,dp->zname,znamelen) - zip->zname ;
	    if ((rs >= 0) && (! dp->f.zoff)) {
	        rs = SR_NOTFOUND ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_getzinfo) */

/* get the difference in seconds between two daters */
int dater_diff(dater *dp,dater *d2p,time_t *rp) noex {
	int		rs ;
	if ((rs = dater_magic(dp,d2p)) >= 0) {
	    if (rp) {
	        *rp = (dp->b.time > d2p->b.time) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_diff) */

#ifdef	COMMENT

/* return the number of time zones that we have */
int dater_nzones(dater *dp) noex {
	int		rs ;
	if ((rs = dater_magic(dp)) >= 0) {
	    rs  = zdb_count(&zr) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dater_nzones) */

int dater_zinfo(dater *dp,dater_zinfo *zip,int ei) noex {
	cint		n = DATER_NZONES ;
	int		rs ;
	int		zl = 0 ;
	if ((rs = dater_magic(dp,zip)) >= 0) {
	    rs = SR_NOTFOUND ;
	    if ((ei > 0) && (ei < n)) {
		char	*zbuf = zip->zname ;
	        rs = memclear(zip) ;
	        zip->isdst = zones[ei].isdst ;
	        zip->zoff = zones[ei].off ;
	        znl = strwcpy(zbuf,zones[ei].zname,znamelen) - zbuf ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? zl : rs ;
}
/* end subroutine (dater_zinfo) */

#endif /* COMMENT */


/* private subroutines */

static int dater_initcur(dater *dp) noex {
	int		rs = SR_OK ;
	if (! dp->f.cyear) {
	    TMTIME	tmt ;
	    int		zo ;
	    if (! dp->f.cb) {
	        dp->cb.time = time(nullptr) ;
	    }
	    dp->f.tzset = true ;
	    rs = tmtime_localtime(&tmt,dp->cb.time) ;
	    zo = tmt.gmtoff ; 		/* seconds west of GMT */
	    dp->cyear = tmt.year ;
	    dp->cb.timezone = (zo / 60) ;	/* minutes west of GMT */
	    dp->cb.dstflag = tmt.isdst ;
	    strncpylc(dp->cname,tmt.zname,znamelen) ;
	    dp->f.cb = true ;
	    dp->f.czn = true ;
	    dp->f.cyear = true ;
	} /* end if (need current time) */
	return rs ;
}
/* end subroutine (dater_initcur) */

/* get any possible zone-name information from the zone offset */
static int dater_mkpzoff(dater *dp,TM *stp,int zoff) noex {
	ZDB		zr ;
	int		rs ;
	dp->zname[0] = '\0' ;
	if ((rs = zdb_offisdst(&zr,zoff,stp->tm_isdst)) >= 0) {
	    rs = strnwcpy(dp->zname,znamelen,zr.name,-1) - dp->zname ;
	} else {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (dater_mkpzoff) */

static int dater_ldtmz(dater *dp,tmz *tp) noex {
	dp->b.dstflag = tmz_getdst(tp) ;
	dp->b.timezone = tmz_getzoff(tp) ;
	dp->f.zoff = tp->f.zoff ;
	if (tp->zname[0] != '\0') {
	    strncpylc(dp->zname,tp->zname,DATER_ZNAMELEN) ;
	    dp->f.zname = (dp->zname[0] != '\0') ;
	}
	return SR_OK ;
}
/* end subroutine (dater_ldtmz) */

static int dater_ldzname(dater *dp,cchar *sp,int sl) noex {
	int		len = 0 ;
	if (cchar *tp ; (tp = strnpbrk(sp,sl," \t")) != nullptr) {
	    sl = (tp - sp) ;
	}
	len = strnwcpy(dp->zname,znamelen,sp,sl) - dp->zname ;
	return len ;
}
/* end subroutine (dater_ldzname) */

static int dater_ldcomzone(dater *dp,comparse *cpp) noex {
	int		rs = SR_OK ;
	if (dp->zname[0] == '\0') {
	    cchar	*cp{} ;
	    if (int cl ; (cl = comparse_getcom(cpp,&cp)) > 0) {
	        dater_ldzname(dp,cp,cl) ;
	    }
	}
	return rs ;
}
/* end subroutine (dater_ldcomzone) */

static int dater_defs(dater *dp,tmz *tp) noex {
	cbool		f_year = tmz_hasyear(tp) ;
	cbool		f_zoff = tmz_haszoff(tp) ;
	cbool		f_zone = tmz_haszone(tp) ;
	int		rs = SR_OK ;
	bool		f = false ;
	f = f || (! f_year) ;
	f = f || (! f_zoff) ;
	f = f || (! f_zone) ;
	if (f) rs = dater_initcur(dp) ;
	if (rs >= 0) {
	    if (! f_year) {
	        tmz_setyear(tp,dp->cyear) ;
	    }
	    if ((! f_zoff) && (! f_zone) && (dp->zname[0] == '\0')) {
	        dp->b.dstflag = dp->cb.dstflag ;
	        dp->b.timezone = dp->cb.timezone ;
	        dp->f.zoff = true ;
	        dater_ldzname(dp,dp->cname,-1) ;
	        dp->f.zname = true ;
	    } else if (! f_zoff) {
	        rs = dater_findzoff(dp,&tp->st) ;
	    }
	} /* end if (ok) */
	return rs ;
}
/* end subroutine (dater_defs) */

/* store a time zone-name into the object */
static int dater_ldname(dater *dp,cchar *znp,int znl) noex {
	int		rs = SR_OK ;
	char		*dnp = dp->zname ;
	if ((znl < 0) || (znl > DATER_ZNAMELEN)) {
	    znl = DATER_ZNAMELEN ;
	}
	dp->f.zname = true ;
	strnwcpy(dnp,DATER_ZNAMELEN,znp,znl) ;
	if ((! dp->f.zoff) || (dp->b.dstflag < 0)) {
	    cint	ch = mkchar(dnp[0]) ;
	    if (isplusminus(ch) || isdigitlatin(ch)) {
	        rs = dater_pnum(dp) ;
	    } else {
	        rs = dater_pname(dp) ;
	    }
	} /* end if (needed zone offset from name) */
	return rs ;
}
/* end subroutine (dater_ldname) */

/* parse a time zone-name */

#ifdef	COMMENT
TIMEB {
	time_t	time ;		/* time, seconds since the epoch */
	ushort	millitm ;	/* 1000 msec of additional accuracy */
	short	timezone ;	/* timezone, minutes west of GMT */
	short	dstflag ;	/* daylight savings when appropriate? */
} ;
#endif /* COMMENT */

static int dater_pname(dater *dp) noex {
	ZDB		zr ;
	int		rs ;
	int		zl = -1 ;
	cchar		*zp = dp->zname ;
	if (! dp->f.zoff) {
	    dp->b.timezone = 0 ;
	}
	if ((rs = zdb_name(&zr,zp,zl)) >= 0) {
	    if (! dp->f.zoff) {
	        dp->b.timezone = zr.off ;
	    }
	    if (dp->b.dstflag < 0) {
	        dp->b.dstflag = zr.isdst ;
	    }
	    dp->f.zname = true ;
	    dp->f.zoff = true ;
	} /* end if (zdb_name) */
	return rs ;
}
/* end subroutine (dater_pname) */

/* get the timezone information out of an RFC822-type number string */
static int dater_pnum(dater *dp) noex {
	int		rs = SR_OK ;
	if (!dp->f.zoff) {
	    int		zo{} ;
	    if ((rs = zos_get(dp->zname,-1,&zo)) >= 0) {
		dp->f.zname = false ;	/* XXX why is this? */
	        dp->b.timezone = zo ;
	        dp->f.zoff = true ;
	    }
	} /* end if (already extracted) */
	return rs ;
}
/* end subroutine (dater_pnum) */

static int dater_findzname(dater *dp) noex {
	int		rs = SR_OK ;
	if (dp->zname[0] == '\0') {
	    cint	znl = DATER_ZNAMELEN ;
	    if (! dp->f.cyear) rs = dater_initcur(dp) ;
	    if (dp->f.zoff && (dp->b.timezone == dp->cb.timezone)) {
	        cchar	*lp = strnwcpy(dp->zname,znl,dp->cname,-1) ;
	        dp->b.dstflag = dp->cb.dstflag ;
	        rs = (lp - dp->zname) ;
	        dp->f.zname = true ;
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (dater_findzname) */

static int dater_findzoff(dater *dp,TM *stp) noex {
	int		rs = SR_FAULT ;
	int		zoff = 0 ;
	if (stp) {
	    rs = SR_OK ;
	    if (! dp->f.zoff) {
	        if (! dp->f.cyear) rs = dater_initcur(dp) ;
	        if (dp->zname[0] != '\0') {
	            ZDB		d ;
	            cint	znl = DATER_ZNAMELEN ;
	            if ((rs = zdb_name(&d,dp->zname,znl)) >= 0) {
	                dp->b.timezone = d.off ;
	                dp->b.dstflag = d.isdst ;
	                dp->f.zoff = true ;
	            } else if (rs == SR_NOTFOUND) {
	                rs = SR_OK ;
	            }
	        } /* end if (have a time-zone name) */
	    } /* end if (needed) */
	    zoff = dp->f.zoff ;
	} /* end if (non-null) */
	return (rs >= 0) ? zoff : rs ;
}
/* end subroutine (dater_findzoff) */

/* try to make a time for the given date */
static int dater_mkptime(dater *dp,TM *stp,int zoff) noex {
	TMTIME		tmt{} ;
	int		rs = SR_OK ;
	if ((zoff == TZO_EMPTY) || (! dp->f.zoff)) {
	    if (! dp->f.cb) rs = dater_initcur(dp) ;
	    zoff = dp->cb.timezone ;
	}
	if (rs >= 0) {
	    rs = tmtime_insert(&tmt,stp) ;
	    tmt.gmtoff = (zoff*60) ;
	}
	if (rs >= 0) {
	    time_t	t ;
	    dp->f.tzset = true ;		/* mktime() calls it! */
	    rs = tmtime_mktime(&tmt,&t) ;
	    dp->b.time = t ;
	    dp->b.timezone = (short) zoff ;
	    dp->f.zoff = true ;
	} /* end if (ok) */
	return rs ;
}
/* end subroutine (dater_mkptime) */

/* try to make a time for the given date */
static int dater_mktime(dater *dp,TM *stp) noex {
	time_t		t ;
	int		rs = SR_OK ;
	if (dp->f.zoff) {
	    TMTIME	tmt ;
	    if ((rs = tmtime_insert(&tmt,stp)) >= 0) {
	        tmt.gmtoff = (dp->b.timezone*60) ; /* insert time-zone-offet */
	        dp->f.tzset = true ;		/* mktime() calls it! */
	        rs = tmtime_mktime(&tmt,&t) ;
	        dp->b.time = t ;
	    }
	} else { /* must be local */
	    dp->f.tzset = true ;		/* mktime() calls it! */
	    if ((rs = uc_mktime(stp,&t)) >= 0) {
	        DEFZINFO	zi ;
	        dp->b.time = t ;
	        if ((rs = getdefzinfo(&zi,stp->tm_isdst)) >= 0) {
	            cint	znl = DEFZINFO_ZNAMELEN ;
	            strnwcpy(dp->zname,DATER_ZNAMELEN,zi.zname,znl) ;
	            dp->b.timezone = zi.zoff ;
	            dp->b.dstflag = stp->tm_isdst ;
	            dp->f.zoff = true ;
	            dp->f.zname = true ;
	        }
	    } /* end if (uc_mktime) */
	} /* end if (something or local) */
	if ((rs >= 0) && (dp->zname[0] == '\0')) {
	    rs = dater_findzname(dp) ;
	}
	return rs ;
}
/* end subroutine (dater_mktime) */

static int dater_initbase(dater *dp) noex {
	int		rs = SR_FAULT ;
	if (dp) {
	    rs = SR_OK ;
	    dp->zname[0] = '\0' ;
	    dp->b.timezone = TZO_EMPTY ;
	    dp->b.dstflag = -1 ;
	    dp->f.zname = false ;
	    dp->f.zoff = false ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dater_initbase) */


