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
#include	<getdefzinfo.h>
#include	<bufsizevar.hh>
#include	<mallocxx.h>
#include	<estrings.h>
#include	<calstrs.h>
#include	<sbuf.h>
#include	<tmtime.hh>		/* CHRONO */
#include	<zos.h>			/* CHRONO */
#include	<comparse.h>		/* CHRONO */
#include	<tmz.hh>			/* CHRONO */
#include	<zdb.h>			/* CHRONO */
#include	<sncpyx.h>
#include	<snwcpyx.h>
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

static inline int znwcpy(char *zb,int zl,cc *zsp,int zsl = -1) noex {
	cint rs = strnwcpy(zb,zl,zsp,zsl) - zb ;
	zb[zl] = '0' ;
	return rs ;
}


/* local variables */

static bufsizevar	zalen(getbufsize_za) ;


/* exported variables */


/* exported subroutines */

int dater_start(dater *op,TIMEB *nowp,cchar *zsp,int zsl) noex {
    	DATER		*hop = op ;
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = memclear(hop)) >= 0) {
		if ((rs = zalen) >= 0) {
		    cint	znl = rs ;
		    cint	sz = ((rs + 1) * 2) ;
		    int		ai = 0 ;
		    op->znamelen = rs ;
		    if (char *a ; (rs = uc_malloc(sz,&a)) >= 0) {
			op->cname = (a + ((zalen + 1) * ai++)) ;
			op->zname = (a + ((zalen + 1) * ai++)) ;
			op->cname[znl] = '\0' ;
			op->zname[znl] = '\0' ;
	                op->b.timezone = TZO_EMPTY ;
	                op->b.dstflag = -1 ;
	                if (nowp != nullptr) {
	                    op->fl.cb = true ;
	                    op->cb = *nowp ;
	                } /* end if (time-offset) */
	                if ((zsp != nullptr) && (zsl != 0)) {
	                    op->fl.czn = true ;
	                    znwcpy(op->cname,zalen,zsp,zsl) ;
	                }
	                op->magic = DATER_MAGIC ;
		    } /* end if (memory-allocation) */
		} /* end if (zalen) */
	    } /* end if (memclear) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dater_start) */

int dater_startcopy(dater *op,dater *d2p) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && d2p) {
	    if ((rs = dater_start(op,np,np,0)) >= 0) {
	        cint	znl = op->znamelen ;
	        strncpy(op->cname,d2p->cname,znl) ; op->cname[znl] = '\0' ;
	        strncpy(op->zname,d2p->zname,znl) ; op->zname[znl] = '\0' ;
	        op->cb = d2p->cb ;
	        op->b = d2p->b ;
	        op->fl = d2p->fl ;
	        op->cyear = d2p->cyear ;
	    } /* end if (dater_start) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_startcopy) */

int dater_finish(dater *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = dater_magic(op)) >= 0) {
	    if (op->cname) {
		rs1 = uc_free(op->cname) ;
		if (rs >= 0) rs = rs1 ;
		op->cname = nullptr ;
		op->zname = nullptr ;
	    }
	    op->b.time = 0 ;
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_finish) */

/* copy one dater to another */
int dater_setcopy(dater *op,dater *d2p) noex {
	int		rs ;
	if ((rs = dater_magic(op,d2p)) >= 0) {
	    cint	znl = op->znamelen ;
	    strncpy(op->cname,d2p->cname,znl) ; op->cname[znl] = '\0' ;
	    strncpy(op->zname,d2p->zname,znl) ; op->zname[znl] = '\0' ;
	    op->cb = d2p->cb ;
	    op->b = d2p->b ;
	    op->fl = d2p->fl ;
	    op->cyear = d2p->cyear ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_setcopy) */

int dater_setstd(dater *op,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = dater_magic(op,sp)) >= 0) {
	    if (sl < 0) sl = strlen(sp) ;
	    if (tmz stz ; (rs = tmz_std(&stz,sp,sl)) >= 0) {
	        TM	dst = stz.st ;
	        dater_ldtmz(op,&stz) ;
	        if ((rs = dater_defs(op,&stz)) >= 0) {
	            rs = dater_mktime(op,&dst) ;
	        }
	    } /* end if (tmz_std) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_setstd) */

/* set the dater from a message-type string */
int dater_setmsg(dater *op,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = dater_magic(op,sp)) >= 0) {
	    if (sl < 0) sl = strlen(sp) ;
	    if (comparse vc ; (rs = comparse_start(&vc,sp,sl)) >= 0) {
	        int	vl ;
	        cchar	*vp{} ;
	        if ((rs = comparse_getval(&vc,&vp)) >= 0) {
	            vl = rs ;
	            if (tmz stz ; (rs = tmz_msg(&stz,vp,vl)) >= 0) {
	                TM	dst = stz.st ;
	                dater_ldtmz(op,&stz) ;
	                dater_ldcomzone(op,&vc) ;
	                if ((rs = dater_defs(op,&stz)) >= 0) {
	                    rs = dater_mktime(op,&dst) ;
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
int dater_setstrdig(dater *op,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = dater_magic(op,sp)) >= 0) {
	    if (sl < 0) sl = strlen(sp) ;
	    if (tmz stz ; (rs = tmz_strdig(&stz,sp,sl)) >= 0) {
	        TM	dst = stz.st ;
	        dater_ldtmz(op,&stz) ;
	        if ((rs = dater_defs(op,&stz)) >= 0) {
	            rs = dater_mktime(op,&dst) ;
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

int dater_setlogz(dater *op,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = dater_magic(op,sp)) >= 0) {
	    if (sl < 0) sl = strlen(sp) ;
	    if (tmz stz ; (rs = tmz_logz(&stz,sp,sl)) >= 0) {
	        TM	dst = stz.st ;
	        dater_ldtmz(op,&stz) ;
	        if ((rs = dater_defs(op,&stz)) >= 0) {
	            rs = dater_mktime(op,&dst) ;
	        }
	    } /* end if (tmz_logz) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_setlogz) */

/* set from TOUCH (original) time string */
int dater_settouch(dater *op,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = dater_magic(op,sp)) >= 0) {
	    if (sl < 0) sl = strlen(sp) ;
	    if (tmz stz ; (rs = tmz_touch(&stz,sp,sl)) >= 0) {
	        TM	dst = stz.st ;
	        dater_ldtmz(op,&stz) ;
	        if ((rs = dater_defs(op,&stz)) >= 0) {
	            rs = dater_mktime(op,&dst) ;
	        }
	    } /* end if (tmz_touch) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_settouch) */

/* set from TOUCH-t (new '-t' version) time string */
int dater_settoucht(dater *op,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = dater_magic(op,sp)) >= 0) {
	    if (sl < 0) sl = strlen(sp) ;
	    if (tmz stz ; (rs = tmz_toucht(&stz,sp,sl)) >= 0) {
	        TM	dst = stz.st ;
	        dater_ldtmz(op,&stz) ;
	        if ((rs = dater_defs(op,&stz)) >= 0) {
	            rs = dater_mktime(op,&dst) ;
	        }
	    } /* end if (tmz_toucht) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_settoucht) */

/* set from a split-out time, zone offset, and zone-name */
int dater_settmzon(dater *op,TM *stp,int zoff,cchar *zstr,int zlen) noex {
	int		rs ;
	if ((rs = dater_magic(op,stp)) >= 0) {
	    rs = SR_INVALID ;
	    if ((zoff == TZO_EMPTY) || (abs(zoff) <= TZO_MAXZOFF)) {
	        rs = dater_initbase(op) ;
		/* load the zone-offset (authoritative) */
	        if ((rs >= 0) && (zoff != TZO_EMPTY)) {
	            op->fl.zoff = true ;
	            op->b.timezone = zoff ;
	        }
		/* lookup the zone-name */
	        if ((rs >= 0) && (zstr != nullptr) && (zstr[0] != '\0')) {
		    if ((rs = zalen) >= 0) {
	                zdb	zr ;
	                op->fl.zname = true ;
	                znwcpy(op->zname,zalen,zstr,zlen) ;
	                if ((rs = zdb_nameoff(&zr,zstr,zlen,zoff)) >= 0) {
	                    if (stp->tm_isdst < 0) stp->tm_isdst = zr.isdst ;
	                    if (zoff == TZO_EMPTY) zoff = zr.off ;
	                } else if (rs == SR_NOTFOUND) {
	                    rs = SR_OK ;
	                } /* end if (got a match) */
		    } /* end if (zalen) */
	        } /* end if (name lookup) */
		/* calculate the time */
	        if (rs >= 0) {
	            rs = dater_mkptime(op,stp,zoff) ;
	        }
		/* now do something with the name */
	        if ((rs >= 0) && (op->zname[0] == '\0')) {
	            rs = dater_mkpzoff(op,stp,zoff) ;
	        }
	        if (rs >= 0) op->magic = DATER_MAGIC ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_settmzon) */

/* set from split-out time and zone offset only (minutes west of GMT) */
int dater_settmzo(dater *op,TM *stp,int zoff) noex {
	int		rs ;
	if ((rs = dater_magic(op,stp)) >= 0) {
	    rs = SR_INVALID ;
	    if ((zoff == TZO_EMPTY) || (abs(zoff) <= TZO_MAXZOFF)) {
	        rs = dater_initbase(op) ;
/* calculate the time */
	        if (rs >= 0) {
	            rs = dater_mkptime(op,stp,zoff) ;
	        }
/* try to find a time zone-name for the information we have */
	        if (rs >= 0) op->magic = DATER_MAGIC ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_settmzo) */

/* set from a split-out time and zone-name only */
int dater_settmzn(dater *op,TM *stp,cchar *zstr,int zlen) noex {
	int		rs ;
	if ((rs = dater_magic(op,stp)) >= 0) {
	    int		zoff = TZO_EMPTY ;
	    /* initialize */
	    rs = dater_initbase(op) ;
	    /* lookup the zone-name */
	    if ((rs >= 0) && (zstr != nullptr) && (zstr[0] != '\0')) {
		if ((rs = zalen) >= 0) {
	            zdb		zr ;
	            op->fl.zname = true ;
	            znwcpy(op->zname,zalen,zstr,zlen) ;
	            if ((rs = zdb_name(&zr,zstr,zlen)) >= 0) {
	                if (stp->tm_isdst < 0) stp->tm_isdst = zr.isdst ;
	                if (zoff == TZO_EMPTY) zoff = zr.off ;
	            } else if (rs == SR_NOTFOUND) {
	                rs = SR_OK ;
	            } /* end if (zdb_name) */
		} /* end if (zalen) */
	    } /* end if (name lookup) */
	    /* calculate the time */
	    if (rs >= 0) {
	        rs = dater_mkptime(op,stp,zoff) ;
	    }
	    if (rs >= 0) op->magic = DATER_MAGIC ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_settmzn) */

/* set from ( time, timezone_name, DST_indication ) */
int dater_settimezn(dater *op,time_t t,cchar *zname,int isdst) noex {
	int		rs ;
	if ((rs = dater_magic(op)) >= 0) {
	    rs = dater_initbase(op) ;
	    op->b.time = t ;
	    op->b.dstflag = isdst ;
	    if ((zname != nullptr) && (zname[0] != '\0')) {
	        rs = dater_ldname(op,zname,-1) ;
	    } else {
	        op->fl.tzset = true ;
	        if (tmtime tmt ; (rs = tmtime_localtime(&tmt,t)) >= 0) {
	            zname = tmt.zname ;
	            op->fl.zname = true ;
		    if ((rs = zalen) >= 0) {
	                strncpylc(op->zname,zname,zalen) ;
		        op->zname[zalen] = '\0' ;
		    }
	            if (isdst < 0) {
			isdst = tmt.isdst ;
		    }
	            if (isdst >= 0) {
	                op->b.timezone = short(tmt.gmtoff / 60) ;
	            }
	        } /* end if */
	        op->b.dstflag = isdst ;
	        op->fl.zoff = true ;
	        op->b.timezone = 0 ;
	    } /* end if */
	    if (rs >= 0) op->magic = DATER_MAGIC ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_settimezn) */

/* set from ( time, tz-offset, tz-name, tz-DST_indication ) */
int dater_settimezon(dater *op,time_t t,int zoff,cchar *zname,int isdst) noex {
	int		rs ;
	if ((rs = dater_magic(op)) >= 0) {
	    if ((rs = zalen) >= 0) {
	        op->b.time = t ;
	        op->b.timezone = zoff ;
	        op->b.dstflag = isdst ;
	        op->zname[0] = '\0' ;
	        op->fl.zname = true ;
	        op->fl.zoff = true ;
	        /* continue */
	        if ((zname != nullptr) && (zname[0] != '\0')) {
	            strncpylc(op->zname,zname,zalen) ;
		    op->zname[zalen] = '\0' ;
	        } else {
	            op->fl.tzset = true ;
	            if (tmtime tmt ; (rs = tmtime_localtime(&tmt,t)) >= 0) {
	                zname = tmt.zname ;
	                op->fl.zname = true ;
	                strncpylc(op->zname,zname,zalen) ;
		        op->zname[zalen] = '\0' ;
	                if (isdst < 0) {
			    isdst = tmt.isdst ;
			}
	                if (isdst >= 0) {
	                    op->b.timezone = short(tmt.gmtoff / 60) ;
	                }
	            } /* end if */
	            op->b.dstflag = isdst ;
	            op->fl.zoff = true ;
	            op->b.timezone = 0 ;
	        } /* end if */
	    if (rs >= 0) op->magic = DATER_MAGIC ;
	    } /* end if (zalen) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_settimezon) */

int dater_mkdatestr(dater *op,int type,char *dbuf,int dlen) noex {
	int		rs = SR_OK ;
	int		sl = 0 ;
	if ((rs = dater_magic(op,dbuf)) >= 0) {
	    rs = SR_INVALID ;
	    if ((type >= 0) && (type < DATER_DTSEND)) {
	        time_t	t ;
	        int	zoff ;
	        cchar	*znp ;
	        cchar	*fmt ;
	        if (dlen < 0) dlen = TIMEBUFLEN ;
		/* go */
	        t = op->b.time ;
	        zoff = op->b.timezone ;
	        znp = op->zname ;
	        if ((op->b.timezone == TZO_EMPTY) || (! op->fl.zoff)) {
	            zoff = 0 ;
	            znp = "GMT" ;
	        }
		/* selective adjustment */
	        switch (type) {
	        case DATER_DTSENV:
	        case DATER_DTSLOGZ:
	            if (op->zname[0] == '\0') {
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
	        if (tmtime tmt ; (rs = tmtime_gmtime(&tmt,t)) >= 0) {
	            tmt.gmtoff = (zoff * 60) ;
		    if ((rs = zalen) >= 0) {
	                strwcpyuc(tmt.zname,znp,zalen) ;
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
		    } /* end if (zalen) */
	        } /* end if (tmtime_gmtime) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (dater_mkdatestr) */

/* make a mail envelope-type dater string */
int dater_mkstd(dater *op,char *dbuf,int dlen) noex {
	return dater_mkdatestr(op,DATER_DTSENV,dbuf,dlen) ;
}
/* end subroutine (dater_mkstd) */

/* make a mail envelope-type dater string */
int dater_mkenv(dater *op,char *dbuf,int dlen) noex {
	return dater_mkdatestr(op,DATER_DTSENV,dbuf,dlen) ;
}
/* end subroutine (dater_mkenv) */

/* make a mail envelope-type dater string */
int dater_mkmsg(dater *op,char *dbuf,int dlen) noex {
	return dater_mkdatestr(op,DATER_DTSMSG,dbuf,dlen) ;
}
/* end subroutine (dater_mkmsg) */

/* make a mail header dater string */
int dater_mkhdr(dater *op,char *dbuf,int dlen) noex {
	return dater_mkdatestr(op,DATER_DTSHDR,dbuf,dlen) ;
}
/* end subroutine (dater_mkhdr) */

/* make the old STRDIG type of dater string */
int dater_mkstrdig(dater *op,char *dbuf,int dlen) noex {
	return dater_mkdatestr(op,DATER_DTSSTRDIG,dbuf,dlen) ;
}
/* end subroutine (dater_mkstrdig) */

/* make the familiar LOGZ-type dater string */
int dater_mklogz(dater *op,char *dbuf,int dlen) noex {
	return dater_mkdatestr(op,DATER_DTSLOGZ,dbuf,dlen) ;
}
/* end subroutine (dater_mklogz) */

/* make the familiar LOGZ-type dater string */
int dater_mkgmtlogz(dater *op,char *dbuf,int dlen) noex {
	return dater_mkdatestr(op,DATER_DTSGMLOGZ,dbuf,dlen) ;
}
/* end subroutine (dater_mkgmlogz) */

int dater_zinfoset(dater *op,dater_zi *zip,cc *zn,int zl) noex {
	int		rs ;
	if ((rs = dater_magic(op,zip,zn)) >= 0) {
	    if ((rs = zalen) >= 0) {
	        op->b.timezone = zip->zoff ;
	        op->b.dstflag = zip->isdst ;
	        rs = snwcpy(op->zname,zalen,zn,zl) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_zinfoset) */

/* return the UNIX® time out of dater object */
int dater_gettime(dater *op,time_t *tp) noex {
	int		rs ;
	if ((rs = dater_magic(op,tp)) >= 0) {
	    *tp = op->b.time ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_gettime) */

/* return the timezone in minutes west of GMT */
int dater_getzoneoff(dater *op,int *zp) noex {
	int		rs ;
	if ((rs = dater_magic(op,zp)) >= 0) {
	    if (! op->fl.zoff) {
	        rs = SR_NOENT ;
	    }
	    if (zp) {
	        *zp = op->b.timezone ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_getzoneoff) */

/* return the timezone in minutes west of GMT */
int dater_getzonename(dater *op,char *rbuf,int rlen) noex {
	int		rs ;
	int		zl = 0 ;
	if ((rs = dater_magic(op,rbuf)) >= 0) {
	    rbuf[0] = '\0' ;
	    if (op->fl.zname) {
	        rs = sncpy(rbuf,rlen,op->zname) ;
		zl = rs ;
	    } else {
	        rs = SR_NOTFOUND ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? zl : rs ;
}
/* end subroutine (dater_getzonename) */

int dater_zinfoget(dater *op,dater_zi *zip,char *zbuf,int zlen) noex {
	int		rs ;
	int		zl = 0 ;
	if ((rs = dater_magic(op,zip,zbuf)) >= 0) {
	    zip->zoff = op->b.timezone ;
	    zip->isdst = op->b.dstflag ;
	    if ((rs = sncpy(zbuf,zlen,op->zname)) >= 0) {
		zl = rs ;
		if (! op->fl.zoff) {
	            rs = SR_NOTFOUND ;
		}
	    }
	} /* end if (magic) */
	return (rs >= 0) ? zl : rs ;
}
/* end subroutine (dater_zinfoget) */

/* get the difference in seconds between two daters */
int dater_diff(dater *op,dater *d2p,time_t *rp) noex {
	int		rs ;
	if ((rs = dater_magic(op,d2p)) >= 0) {
	    if (rp) {
	        *rp = (op->b.time > d2p->b.time) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_diff) */

#ifdef	COMMENT

/* return the number of time zones that we have */
int dater_nzones(dater *op) noex {
	int		rs ;
	if ((rs = dater_magic(op)) >= 0) {
	    rs  = zdb_count(&zr) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dater_nzones) */

int dater_zi(dater *op,dater_zi *zip,int ei) noex {
	cint		n = DATER_NZONES ;
	int		rs ;
	int		zl = 0 ;
	if ((rs = dater_magic(op,zip)) >= 0) {
	    memclear(zip) ;
	    rs = SR_NOTFOUND ;
	    if ((ei > 0) && (ei < n)) {
		char	*zbuf = zip->zname ;
		if ((rs = zalen) >= 0) {
	            zip->isdst = zones[ei].isdst ;
	            zip->zoff = zones[ei].off ;
	            znl = strwcpy(zbuf,zones[ei].zname,zalen) - zbuf ;
		}
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? zl : rs ;
}
/* end subroutine (dater_zi) */

#endif /* COMMENT */


/* private subroutines */

static int dater_initcur(dater *op) noex {
	int		rs = SR_OK ;
	if (! op->fl.cyear) {
	    if (! op->fl.cb) {
	        op->cb.time = time(nullptr) ;
	    }
	    op->fl.tzset = true ;
	    if (tmtime tmt ; (rs = tmtime_localtime(&tmt,op->cb.time)) >= 0) {
		if ((rs = zalen) >= 0) {
	            cint	zo = tmt.gmtoff ; /* seconds west of GMT */
	            op->cyear = tmt.year ;
	            op->cb.timezone = (zo / 60) ; /* minutes west of GMT */
	            op->cb.dstflag = tmt.isdst ;
	            strncpylc(op->cname,tmt.zname,zalen) ;
		    op->cname[zalen] = '\0' ;
	            op->fl.cb = true ;
	            op->fl.czn = true ;
	            op->fl.cyear = true ;
		} /* end if (zalen) */
	    } /* end if (tmtime_localtime) */
	} /* end if (need current time) */
	return rs ;
}
/* end subroutine (dater_initcur) */

/* get any possible zone-name information from the zone offset */
static int dater_mkpzoff(dater *op,TM *stp,int zoff) noex {
	zdb		zr ;
	int		rs ;
	op->zname[0] = '\0' ;
	if ((rs = zdb_offisdst(&zr,zoff,stp->tm_isdst)) >= 0) {
	    if ((rs = zalen) >= 0) {
	        rs = znwcpy(op->zname,zalen,zr.name) ;
	    }
	} else {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (dater_mkpzoff) */

static int dater_ldtmz(dater *op,tmz *tp) noex {
    	int		rs = SR_OK ;
	op->b.dstflag = tmz_getdst(tp) ;
	op->b.timezone = tmz_getzoff(tp) ;
	op->fl.zoff = tp->fl.zoff ;
	if (tp->zname[0] != '\0') {
	    if ((rs = zalen) >= 0) {
	        strncpylc(op->zname,tp->zname,zalen) ;
		op->zname[zalen] = '\0' ;
	        op->fl.zname = (op->zname[0] != '\0') ;
	    }
	}
	return rs ;
}
/* end subroutine (dater_ldtmz) */

static int dater_ldzname(dater *op,cchar *sp,int sl) noex {
    	int		rs = SR_OK ;
	int		len = 0 ;
	if (cchar *tp ; (tp = strnpbrk(sp,sl," \t")) != nullptr) {
	    sl = (tp - sp) ;
	}
	if ((rs = zalen) >= 0) {
	    len = znwcpy(op->zname,zalen,sp,sl) ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (dater_ldzname) */

static int dater_ldcomzone(dater *op,comparse *cpp) noex {
	int		rs = SR_OK ;
	if (op->zname[0] == '\0') {
	    cchar	*cp{} ;
	    if (int cl ; (cl = comparse_getcom(cpp,&cp)) > 0) {
	        dater_ldzname(op,cp,cl) ;
	    }
	}
	return rs ;
}
/* end subroutine (dater_ldcomzone) */

static int dater_defs(dater *op,tmz *tp) noex {
	cbool		f_year = tmz_hasyear(tp) ;
	cbool		f_zoff = tmz_haszoff(tp) ;
	cbool		f_zone = tmz_haszone(tp) ;
	int		rs = SR_OK ;
	bool		f = false ;
	f = f || (! f_year) ;
	f = f || (! f_zoff) ;
	f = f || (! f_zone) ;
	if (f) rs = dater_initcur(op) ;
	if (rs >= 0) {
	    if (! f_year) {
	        tmz_setyear(tp,op->cyear) ;
	    }
	    if ((! f_zoff) && (! f_zone) && (op->zname[0] == '\0')) {
	        op->b.dstflag = op->cb.dstflag ;
	        op->b.timezone = op->cb.timezone ;
	        op->fl.zoff = true ;
	        dater_ldzname(op,op->cname,-1) ;
	        op->fl.zname = true ;
	    } else if (! f_zoff) {
	        rs = dater_findzoff(op,&tp->st) ;
	    }
	} /* end if (ok) */
	return rs ;
}
/* end subroutine (dater_defs) */

/* store a time zone-name into the object */
static int dater_ldname(dater *op,cchar *znp,int znl) noex {
	int		rs ;
	if ((rs = zalen) >= 0) {
	    char	*dnp = op->zname ;
	    op->fl.zname = true ;
	    znwcpy(dnp,zalen,znp,znl) ;
	    if ((! op->fl.zoff) || (op->b.dstflag < 0)) {
	        cint	ch = mkchar(dnp[0]) ;
	        if (isplusminus(ch) || isdigitlatin(ch)) {
	            rs = dater_pnum(op) ;
	        } else {
	            rs = dater_pname(op) ;
	        }
	    } /* end if (needed zone offset from name) */
	} /* end if (zalen) */
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

static int dater_pname(dater *op) noex {
	zdb		zr ;
	int		rs ;
	int		zl = -1 ;
	cchar		*zp = op->zname ;
	if (! op->fl.zoff) {
	    op->b.timezone = 0 ;
	}
	if ((rs = zdb_name(&zr,zp,zl)) >= 0) {
	    if (! op->fl.zoff) {
	        op->b.timezone = zr.off ;
	    }
	    if (op->b.dstflag < 0) {
	        op->b.dstflag = zr.isdst ;
	    }
	    op->fl.zname = true ;
	    op->fl.zoff = true ;
	} /* end if (zdb_name) */
	return rs ;
}
/* end subroutine (dater_pname) */

/* get the timezone information out of an RFC822-type number string */
static int dater_pnum(dater *op) noex {
	int		rs = SR_OK ;
	if (!op->fl.zoff) {
	    int		zo{} ;
	    if ((rs = zos_get(op->zname,-1,&zo)) >= 0) {
		op->fl.zname = false ;	/* XXX why is this? */
	        op->b.timezone = zo ;
	        op->fl.zoff = true ;
	    }
	} /* end if (already extracted) */
	return rs ;
}
/* end subroutine (dater_pnum) */

static int dater_findzname(dater *op) noex {
	int		rs = SR_OK ;
	if (op->zname[0] == '\0') {
	    if (! op->fl.cyear) {
		rs = dater_initcur(op) ;
	    }
	    if (rs >= 0) {
		if (op->fl.zoff && (op->b.timezone == op->cb.timezone)) {
		    if ((rs = zalen) >= 0) {
	                op->b.dstflag = op->cb.dstflag ;
	                op->fl.zname = true ;
	                rs = znwcpy(op->zname,zalen,op->cname) ;
		    } /* end if (zalen) */
		} /* end if */
	    } /* end if (ok) */
	} /* end if */
	return rs ;
}
/* end subroutine (dater_findzname) */

static int dater_findzoff(dater *op,TM *stp) noex {
	int		rs = SR_FAULT ;
	int		zoff = 0 ;
	if (stp) {
	    rs = SR_OK ;
	    if (! op->fl.zoff) {
	        if (! op->fl.cyear) {
		    rs = dater_initcur(op) ;
		}
	        if (op->zname[0] != '\0') {
		    if ((rs = zalen) >= 0) {
	                if (zdb d ; (rs = zdb_name(&d,op->zname,zalen)) >= 0) {
	                    op->b.timezone = d.off ;
	                    op->b.dstflag = d.isdst ;
	                    op->fl.zoff = true ;
	                } else if (rs == SR_NOTFOUND) {
	                    rs = SR_OK ;
	                }
		    } /* end if (zalen) */
	        } /* end if (have a time-zone name) */
	    } /* end if (needed) */
	    zoff = op->fl.zoff ;
	} /* end if (non-null) */
	return (rs >= 0) ? zoff : rs ;
}
/* end subroutine (dater_findzoff) */

/* try to make a time for the given date */
static int dater_mkptime(dater *op,TM *stp,int zoff) noex {
	tmtime tmt{} ;
	int		rs = SR_OK ;
	if ((zoff == TZO_EMPTY) || (! op->fl.zoff)) {
	    if (! op->fl.cb) rs = dater_initcur(op) ;
	    zoff = op->cb.timezone ;
	}
	if (rs >= 0) {
	    rs = tmtime_insert(&tmt,stp) ;
	    tmt.gmtoff = (zoff*60) ;
	}
	if (rs >= 0) {
	    time_t	t ;
	    op->fl.tzset = true ;		/* mktime() calls it! */
	    rs = tmtime_mktime(&tmt,&t) ;
	    op->b.time = t ;
	    op->b.timezone = (short) zoff ;
	    op->fl.zoff = true ;
	} /* end if (ok) */
	return rs ;
}
/* end subroutine (dater_mkptime) */

/* try to make a time for the given date */
static int dater_mktime(dater *op,TM *stp) noex {
	time_t		t ;
	int		rs = SR_OK ;
	if (op->fl.zoff) {
	    if (tmtime tmt ; (rs = tmtime_insert(&tmt,stp)) >= 0) {
	        tmt.gmtoff = (op->b.timezone*60) ; /* insert time-zone-offet */
	        op->fl.tzset = true ;		/* mktime() calls it! */
	        rs = tmtime_mktime(&tmt,&t) ;
	        op->b.time = t ;
	    }
	} else { /* must be local */
	    op->fl.tzset = true ;		/* mktime() calls it! */
	    if ((rs = uc_mktime(stp,&t)) >= 0) {
	        op->b.time = t ;
		if (char *zb ; (rs = malloc_za(&zb)) >= 0) {
		    cint	zl = rs ;
		    cint	isd = stp->tm_isdst ;
	            if (DEFZINFO zi ; (rs = getdefzinfo(&zi,zb,zl,isd)) >= 0) {
	                znwcpy(op->zname,zalen,zb,rs) ;
	                op->b.timezone = zi.zoff ;
	                op->b.dstflag = stp->tm_isdst ;
	                op->fl.zoff = true ;
	                op->fl.zname = true ;
	            } /* end if (getdefzinfo) */
		    rs = rsfree(rs,zb) ;
		} /* end if (m-a-f) */
	    } /* end if (uc_mktime) */
	} /* end if (something or local) */
	if ((rs >= 0) && (op->zname[0] == '\0')) {
	    rs = dater_findzname(op) ;
	}
	return rs ;
}
/* end subroutine (dater_mktime) */

static int dater_initbase(dater *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->zname[0] = '\0' ;
	    op->b.timezone = TZO_EMPTY ;
	    op->b.dstflag = -1 ;
	    op->fl.zname = false ;
	    op->fl.zoff = false ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dater_initbase) */


