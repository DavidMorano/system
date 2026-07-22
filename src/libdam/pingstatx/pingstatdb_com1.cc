/* pingstatdb_com1 MODULE (implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* object to manipulate a PINGSTATDB file */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#define	CF_CREATE	0		/* always create the file? */
#define	CF_UNLOCK	1		/* always unlock after an operation */

/* revision history:

	= 1998-04-13, David A-D- Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	pingstatdb_rec

  	Description:
	This subroutine maintains a PINGSTATDB file. These files
	are used to maintain the names and status of a PING event.

	Synopsis:

	Arguments:
	op		ping status_file pointer
	fname		filename

	Returns:
	<0		error
	==0		record written
	==1		record created

	Format of file records:

	- eight digits of a decimal count number field
	- the time of the last status change
	- the time of the last entry update
	- the UP/DOWN status
	- the hostname

	Format example:

       3 000505_1018:55_EDT 000505_2233:28_EDT U rcb
       5 000505_1018:55_EDT 000505_2234:43_EDT U *LAST_UPDATE*
       1 000505_2234:37_EDT 000505_2234:37_EDT U bars
       1 000505_2234:40_EDT 000505_2234:40_EDT U amps
       1 000505_2234:43_EDT 000505_2234:43_EDT U farads
       1 000505_2234:46_EDT 000505_2234:46_EDT U ergs

	Oh, and finally, since file record locking sucks, that is: it is
	very often BROKEN.  On the stupid Sun platforms, we attempt to
	lock to be a nice guy but we have relatively small timeouts
	after which we proceed anyway!  This strategy is applied to
	all record lockes used throughout this subroutine.

******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<vecitem.h>		/* LIBUC */
#include	<dater.h>		/* LIBUC */
#include	<timestr.h>		/* LIBUC */
#include	<ismisc.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU |TIMEBUFLEN| */
#include	<bfile.h>		/* LIBB */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"pingstatdb.h"
#include	"pingstatdb_rec.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

module pingstatdb_com ;

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	PSD		pingstatdb
#define	PSD_FL		pingstatdb_fl
#define	PSD_ENT		pingstatdb_ent
#define	PSD_CUR		pingstatdb_cur
#define	PSD_UP		pingstatdb_up
#define	PSD_REC		pingstatdb_rec

/* record-format (RF) paramters */
#define	RF_NUMDIGITS	8		/* RF count field width */
#define	RF_LOGZLEN	23		/* RF time log field width */
#define	RF_UPSTAT	1		/* RF up status field width */
#define	RF_LEAD0	(RF_NUMDIGITS + (2*RF_LOGZLEN))
#define	RF_LEAD1	(RF_UPSTAT + 4)
#define	RF_LEAD		(RF_LEAD0 + RF_LEAD1)

#define	BUFLEN		(RF_LEAD + MAXHOSTNAMELEN + 3)

#define	TO_LOCK		30		/* seconds */
#define	TO_MINUPDATE	3		/* minimum time between updates */

#define	LASTUPDATE	"*LAST_UPDATE*"

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#endif
#ifndef	CF_CREATE
#define	CF_CREATE	0		/* always create the file? */
#endif
#ifndef	CF_UNLOCK
#define	CF_UNLOCK	1		/* always unlock after an operation */
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */

int pingstatdb_updrec(PSB *op,time_t dt,dater *dp,
		cchar *hostname,int f_up,time_t timestamp) noex {
	PSD_REC	*rp ;
	off_t	boff ;
	int		rs ;
	int		f_changed = false ;

	{
	    char	timebuf[TIMEBUFLEN+1] ;
	    DEBUGPRINTF("hostname=%s\n",hostname) ;
	    DEBUGPRINTF("f_up=%u\n",f_up) ;
	    DEBUGPRINTF("timestamp=%s\n",
	        timestr_logz(timestamp,timebuf)) ;
	}

	if ((rs = pingstatdb_getrec(op,hostname,&rp)) >= 0) {
	    time_t	ptime ;
	    int		f_greater = (! LEQUIV(f_up,rp->f_up)) ;

	    DEBUGPRINTF("found match rs=%d\n",rs) ;

	    rs = dater_gettime(&rp->pdate,&ptime) ;

	    f_greater = ((rs >= 0) && (timestamp > ptime)) ;

	    if (ptime > timestamp)
	        timestamp = ptime ;

	    dater_settimezn(dp,timestamp,op->zname,-1) ;

	    {
	        char	timebuf[TIMEBUFLEN + 1] ;
	        DEBUGPRINTF("dater_gettime() ptime=%s\n",
	            timestr_logz(ptime,timebuf)) ;
	    }

	    if ((rs < 0) || f_changed ||
	        (((dt - ptime) > TO_MINUPDATE) && f_greater)) {

	        boff = rp->roff ;
	        bseek(op->pfp,boff,SEEK_SET) ;

	        rs = record_update(rp,op->pfp,dp,f_up) ;
	        DEBUGPRINTF("record_updrec() rs=%d\n",rs) ;
	    } /* end if (did the update) */

	} else if (rs == SR_NOTFOUND) {
	    PSD_REC	r ;
	    uint	roff ;
	    cint	sz = szof(PSD_REC) ;
	    int		f_rec = false ;

	    DEBUGPRINTF("no match found rs=%d\n",rs) ;
	    DEBUGPRINTF("zname=%s\n",op->zname) ;
	    if ((rs = dater_settimezn(dp,timestamp,op->zname,-1)) >= 0) {
		TIMEB	*nowp = &op->now ;
		cchar	*zn = op->zname ;
		cchar	*hn = hostname ;

	        f_changed = true ;
	        bseek(op->pfp,0L,SEEK_END) ;

	        btell(op->pfp,&boff) ;
	        roff = boff ;

	        if ((rs = record_start(&r,nowp,zn,roff,hn,dp)) >= 0) {
	            f_rec = true ;
		}

	        DEBUGPRINTF("record_start() rs=%d hn=%s\n",
	            rs,hostname) ;

	    } /* end if */

	    if (rs >= 0) {
	        rs = record_update(&r,op->pfp,dp,f_up) ;

	        DEBUGPRINTF("record_update() rs=%d\n",
	            rs) ;

	    } /* end if */

	    if (rs >= 0) {
	        rs = vecitem_add(op->elp,&r,sz) ;
	    }

	    if ((rs < 0) && f_rec) {
	        record_finish(&r) ;
	    } /* end if (error) */
	} /* end if (target entry) */
	DEBUGPRINTF("ret rs=%d f_changed=%u\n", rs,f_changed) ;
	return (rs >= 0) ? f_changed : rs ;
} /* end subroutine (pingstatdb_updrec) */

int pingstatdb_getrec(PSD *op,cchar *hostname,PSD_REC **rpp) noex {
    	vecitem *elp = op->elp ;
	int		rs ;
	int		i ; /* return-value */
	void *vp ;
	for (i = 0 ; (rs = elp->get(i,&vp)) >= 0 ; i += 1) {
	    if (vp == nullptr) continue ;
	    if (strcmp(hostname,(*rpp)->hostname) == 0) break ;
	} /* end for */
	return (rs >= 0) ? i : rs ;
} /* end subroutine (pingstatdb_getrecord) */


