/* zdb SUPPORT */
/* lang=C++20 */

/* time-zone database management */
/* version %I% last-modified %G% */

#define	CF_BSEARCH	0		/* use |bsearch(3c)| */

/* revision history:

	= 1998-05-01, David A­D­ Morano
        Originally created due to frustration with various other "fuzzy" date
        conversion subroutines.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************
 
	This object deals with the mangement of the time-zone database.

*******************************************************************************/

#include	<envstandards.h>
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| */
#include	<tzfile.h>		/* for TM_YEAR_BASE */
#include	<usystem.h>
#include	<estrings.h>
#include	<nulstr.h>
#include	<strwcpy.h>
#include	<strwcmp.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"zdb.h"


/* local defines */

#ifndef	TZO_EMPTY
#define	TZO_EMPTY	SHORT_MAX
#endif
#ifndef	TZO_MAXZOFF
#define	TZO_MAXZOFF	(14*60)
#endif


/* imported namespaces */

using std::min ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	findname(cchar *,int) noex ;

#if	CF_BSEARCH
static int reccmp(cvoid *,cvoid *) noex ;
#endif


/* local variables */

static constexpr ZDB	zones[] = {
	{ "acsst", -630, 1 },	/* Cent. Australia */
	{ "acst", -570, 0 },	/* Cent. Australia */
	{ "adt", 3*60, 1 },	/* Atlantic */
	{ "aesst", -660, 1 },	/* E. Australia */
	{ "aest", -600, 1 },	/* Australia Eastern ST */
	{ "akdt", 8*60, 1 },	/* Alaska (US) */
	{ "akst", 9*60, 0 },	/* Alaska (US) */
	{ "ast", 240, 0 },	/* Atlantic ST (Canada) */
	{ "awst", -480, 0 },	/* W. Australia */
	
	{ "brt", -3*60, -1 },	/* XXX unknown XXX */
	{ "bst", -1*60, 1 },	/* British Summer */
	
	{ "cadt", -630, 1 },	/* Central Australian DT */
	{ "cast", -570, 0 },	/* Central Australian ST */
	{ "cct", -480, 0 },	/* China Coast */
	{ "cdt", 5*60, 1 },	/* central daylight time (US) */
	{ "cest", -120, 1 },	/* Central Europe Summer Time */
	{ "cet", -1*60, 0 },	/* Central Europe */
	{ "cetdst", -120, 1 },	/* Central European DT */
	{ "cst", 6*60, 0 },	/* central standard time (US) */
	{ "cut", 0*60, 0 },	/* coordinated universal time */
	
	{ "dnt", -60, 0 },	/* Dansk Normal Tid */
	
	{ "eadt", -660, 1 },	/* East Australian DT */
	{ "east", -600, 0 },	/* East Australian ST */
	{ "edt", 4*60, 1 },	/* eastern daylight time (US) */
	{ "eest", -180, 1 },	/* Eastern Europe Summer */
	{ "eet", -2*60, 0 },	/* Eastern Europe */
	{ "eetdst", -180, 1 },	/* Eastern Europe */
	{ "est", 5*60, 0 },	/* eastern standard time (US) */
	
	{ "fst", -120, 1 },	/* French Summer Time */
	{ "fwt", -60, 0 },	/* French Winter Time  */
	
	{ "gmt", 0*60, 0 },	/* Greenwich Mean Time */
	{ "gst", -600, 0 },	/* Guam ST */
	
	{ "hadt", 9*60, 1 },	/* Hawaii-Aleutian */
	{ "hast", 10*60, 0 },	/* Hawaii-Aleutian */
	{ "hkt", -480, 0 },	/* Hong Kong Time */
	{ "hst", 10*60, 0 },	/* Hawaii */
	
	{ "idle", -720, 0 },	/* Intl. Date Line, East */
	{ "idlw", 720, 0 },	/* Intl. Date Line, West */
	{ "idt", -180, 1 },	/* Israel DT */
	{ "ist", -120, 0 },	/* Israel */
	
	{ "jdt", -10*60, 1 },	/* Japan */
	{ "jst", -9*60, 0 },	/* Japan */
	
	{ "kdt", -10*60, 1 },	/* Korea */
	{ "kst", -9*60, 0 },	/* Korea */
	
	{ "ligt", -600, 0 },	/* From Melbourne, Australia */
	{ "lt", -1*60, 0 },
	
	{ "mdt", 6*60, 1 },	/* mountain daylight time (US) */
	{ "mest", -120, 1 },	/* Middle Europe Summer Time */
	{ "mesz", -120, 1 },	/* Mittel-Europaeische Sommerzeit */
	{ "met", -1*60, 0 },	/* Middle Europe */
	{ "metdst", -120, 1 },	/* Middle Europe DT */
	{ "mewt", -60, 0 },	/* Middle Europe Winter Time */
	{ "mez", -60, 0 },	/* Mittel-Europaeische Zeit */
	{ "mst", 7*60, 0 },	/* mountain standard time (US) */
	
	{ "ndt", 150, 1 },	/* Newfoundland */
	{ "nft", 210, 0 },	/* Newfoundland ST */
	{ "nor", -60, 0 },	/* Norway ST */
	{ "nst", 210, 0 },	/* Newfoundland */
	{ "nzdt", -13*60, 1 },	/* New Zealand */
	{ "nzst", -12*60, 0 },	/* New Zealand */
	{ "nzt", -720, 0 },	/* New Zealand Time */
	
	{ "pdt", 7*60, 1 },	/* pacific daylight time (US) */
	{ "pst", 8*60, 0 },	/* pacific standard time (US) */
	
	{ "sadt", -630, 1 },	/* S. Australian DT */
	{ "sast", -570, 0 },	/* S. Australian ST */
	{ "sst", -120, 1 },	/* Swedish Summer Time */
	{ "swt", -60, 0 },	/* Swedish Winter Time  */

	{ "usakdt", 8*60, 1 },	/* Alaska daylight (US) */
	{ "usakst", 9*60, 0 },	/* Alaska standard (US) */
	{ "uscdt", 5*60, 1 },	/* central daylight time (US) */
	{ "uscst", 6*60, 0 },	/* central standard time (US) */
	{ "usedt", 4*60, 1 },	/* eastern daylight time (US) */
	{ "usest", 5*60, 0 },	/* eastern standard time (US) */
	{ "ushst", 10*60, 0 },	/* Hawaii (no "daylight" time) */
	{ "usmdt", 6*60, 1 },	/* mountain daylight time (US) */
	{ "usmst", 7*60, 0 },	/* mountain standard time (US) */
	{ "uspdt", 7*60, 1 },	/* pacific daylight time (US) */
	{ "uspst", 8*60, 0 },	/* pacific standard time (US) */

	{  "ut", 0*60, 0 },	/* universal time? */
	{ "utc", 0*60, 0 },	/* UTC */
	
	{ "wadt", -540, 0 },	/* West Australian DT (DAM) */
	{ "wast", -480, 0 },	/* West Australian ST */
	{ "wat", 60, 0 },	/* West Africa Time */
	{ "west", -60, 1 },	/* Western Europe Summer */
	{ "wet", 0, 0 },	/* Western Europe */
	{ "wetdst", -60, 1 },	/* Western Europe */
	{ "wst", -480, 0 },	/* West Australian ST */

	{ "ydt", 480, 1 },	/* Yukon DT */
	{ "yst", 540, 0 },	/* Yukon ST */

	{ "z", 0, 0 },		/* Zulu */

	{ nullptr, 0, 0 }
} ;

#define	ZDB_NZONES	((sizeof(zones) / sizeof(ZDB)) - 1)


/* exported variables */


/* exported subroutines */

/* set from 'name' and 'offset' */
int zdb_nameoff(zdb *zrp,cchar *sp,int sl,int zoff) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (zrp && sp) {
	    int		zi = -1 ;
	    cint	zlen = ZDB_ZNAMESIZE ;
	    char	zbuf[ZDB_ZNAMESIZE + 1] ;
	    bool	f = false ;
	    rs = SR_NOTFOUND ;
	    memclear(zrp) ;
	    if (sl < 0) sl = strnlen(sp,zlen) ;
	    if (hasuc(sp,sl)) {
	        cint	ml = (sl >= 0) ? min(sl,zlen) : zlen ;
	        sl = strwcpylc(zbuf,sp,ml) - zbuf ;
	        sp = zbuf ;
	    }
    /* lookup by name and offset (if we have an offset) */
	    if (zoff != TZO_EMPTY) {
	        for (i = 0 ; zones[i].name != nullptr ; i += 1) {
	            f = (strwcmp(zones[i].name,sp,sl) == 0) ;
		    if (f && (zi < 0)) zi = i ;
	            f = f && (zones[i].off == zoff) ;
		    if (f) break ;
	        } /* end for */
	    } /* end if */
    /* if not match yet => lookup by name only */
	    if ((! f) && (zi >= 0)) {
	        i = zi ;
	        f = true ;
	    } /* end if (looking up name only) */
	    if (f) {
		rs = SR_OK ;
	        zrp->name = zones[i].name ;
	        zrp->off = zones[i].off ;
	        zrp->isdst = zones[i].isdst ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (zdb_nameoff) */

/* set from 'name' only */
int zdb_name(zdb *zrp,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (zrp && sp) {
	    cint	zlen = ZDB_ZNAMESIZE ;
	    char	zbuf[ZDB_ZNAMESIZE + 1] ;
	    rs = SR_NOTFOUND ;
	    memclear(zrp) ;
	    if (sl < 0) sl = strnlen(sp,zlen) ;
	    if (hasuc(sp,sl)) {
	        cint	ml = (sl >= 0) ? min(sl,zlen) : zlen ;
	        sl = strwcpylc(zbuf,sp,ml) - zbuf ;
	        sp = zbuf ;
	    }
	    if ((i = findname(sp,sl)) >= 0) {
		rs = SR_OK ;
	        zrp->name = zones[i].name ;
	        zrp->off = zones[i].off ;
	        zrp->isdst = zones[i].isdst ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (zdb_name) */

/* set from 'offset' only */
int zdb_off(zdb *zrp,int zoff) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (zrp) {
	    bool	f = false ;
	    rs = SR_NOTFOUND ;
	    memclear(zrp) ;
	    for (i = 0 ; zones[i].name != nullptr ; i += 1) {
	        f = (zones[i].off == zoff) ;
	        if (f) break ;
	    } /* end for */
	    if (f) {
	 	rs = SR_OK ;
	        zrp->name = zones[i].name ;
	        zrp->off = zones[i].off ;
	        zrp->isdst = zones[i].isdst ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (zdb_off) */

/* set from 'offset' and 'isdst' */
int zdb_offisdst(zdb *zrp,int zoff,int isdst) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (zrp) {
	    bool	f = false ;
	    rs = SR_NOTFOUND ;
	    memclear(zrp) ;
	    if (isdst >= 0) {
	        for (i = 0 ; zones[i].name != nullptr ; i += 1) {
	            f = (zones[i].off == zoff) ;
		    f = f && (zones[i].isdst == isdst) ;
	            if (f) break ;
	        } /* end for */
	    } else {
	        for (i = 0 ; zones[i].name != nullptr ; i += 1) {
	            f = (zones[i].off == zoff) ;
	            if (f) break ;
	        } /* end for */
	    } /* end if */
	    if (f) {
	 	rs = SR_OK ;
	        zrp->name = zones[i].name ;
	        zrp->off = zones[i].off ;
	        zrp->isdst = zones[i].isdst ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (zdb_off) */

int zdb_count(zdb *zrp) noex {
	int		rs = SR_FAULT ;
	cint		n = ZDB_NZONES ;
	if (zrp) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (zdb_count) */


/* local subroutines */

#if	CF_BSEARCH

/****
struct zdb_e {
	cchar		*name ;
	short		off ;
	short		isdst ;
} ;
****/

static int findname(cchar *sp,int sl) noex {
	ZDB		ke, *rp ;
	nulstr		s ;
	cint		n = ZDB_NZONES ;
	cint		size = sizeof(struct zdb_e) ;
	int		rs ;
	int		rs1 ;
	int		i = 0 ;
	cchar		*kp{} ;
	if ((rs = nulstr_start(&s,sp,sl,&kp)) >= 0) {
	    {
	        ke.name = kp ;
	        rp = bsearch(&ke,zones,n,size,reccmp) ;
	        i = (rp != nullptr) ? (rp - zones) : -1 ;
	    }
	    rs1 = nulstr_finish(&s) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (nulstr) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (findname) */

static int reccmp(cvoid *v1p,cvoid *v2p) noex {
	struct zdb_e	*e1p = (struct zdb_e *) v1p ;
	struct zdb_e	*e2p = (struct zdb_e *) v2p ;
	return strcmp(e1p->name,e2p->name) ;
}
/* end subroutine (reccmp) */

#else /* CF_BSEARCH */

/* do a little binary search to find the zone-name (if we have it) */
static int findname(cchar *sp,int sl) noex {
	int		i = 0 ;
	int		rc = 1 ;
	int		first = 0 ;
	int		last = (ZDB_NZONES - 1) ;
	cchar		*znp ;
	while (last >= first) {
	    i = first + ((last - first) >> 1) ;
	    znp = zones[i].name ;
	    if ((rc = (sp[0] - znp[0])) == 0) {
	        rc = -strwcmp(znp,sp,sl) ;
	        if (rc == 0) break ;
	    } /* end if (equality comparison) */
	    if (rc < 0) {
	        last = i - 1 ;
	    } else {
	        first = i + 1 ;
	    }
	} /* end while */
	return (rc == 0) ? i : -1 ;
}
/* end subroutine (findname) */

#endif /* CF_BSEARCH */


