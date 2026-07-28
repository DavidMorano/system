/* pingstatdb_com1 MODULE (implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* common subroutines for PINGSTATDB */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	pingstatdb_com

  	Description:
	These are support subroutines for the PINGSTATDB object.

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
#include	<bufsizeget.h>		/* LIBUC */
#include	<vechand.h>		/* LIBUC */
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
import pingstatdb_rec ;

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

#define	DTR		dater

#define	RLENMULT	2		/* HostNameLen multiplier */

#define	TO_LOCK		30		/* seconds */
#define	TO_MINUPDATE	3		/* minimum time between updates */

#define	LASTUPDATE	"*LAST_UPDATE*"


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int	hostnamelen ;
	int	rlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */


/* local variables */

static vars		var ;


/* exported variables */

int pingstatdb_cominit(PSD *op) noex {
    	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
    	    static cint rsv = var ;
	    rs = rsv ;
	} /* end if (non-null) */
    	return rs ;
} /* end subroutine */

int pingstatdb_comfini(PSD *op) noex {
    	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    rs = SR_OK ;
	} /* end if (non-null) */
    	return rs ;
} /* end subroutine */

int pingstatdb_checkcache(PSD *op) noex {
	int		rs ;
	bool		f_cached = op->fl.cached ;
	DEBUGPRINTF("ent f_cached=%d\n", f_cached) ;
	if (ustat sb ; (rs = bcontrol(op->pfp,BC_STAT,&sb)) >= 0) ylikely {
	    if (f_cached) {
	        if (sb.st_mtime > op->mtime) {
	            f_cached = false ;
	            pingstatdb_fes(op) ;
	        }
	    } /* end if */
	    if (! f_cached) {
	        if ((rs = pingstatdb_readrecs(op)) >= 0) {
	            op->mtime = sb.st_mtime ;
	            op->fl.cached = true ;
	        }
	    } else {
	        rs = vechand_count(op->rlp) ;
	    }
	} /* end if (bcontrol) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (pingstatdb_checkcache) */

local int pingstatdb_readrecln(PSD *op,uint roff,cchar *rbuf,int rl) noex {
    	vechand		*rlp = op->rlp ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
    	int		rs ;
	int		c = 0 ; /* return-value */
	if (PSD_REC *rep = new(nt) PSD_REC ; rep) ylikely {
	    timeb	*nowp = op->nowp ;
	    cchar	*zn = op->znbuf ;
	    if ((rs = record_start(rep,nowp,zn,roff,np)) >= 0) ylikely {
	        if ((rs = record_load(rep,roff,rbuf,rl)) >= 0) ylikely {
	            c += 1 ;
	            rs = rlp->add(rep) ;
	        } /* end if (record_load) */
	        if (rs < 0) {
		    record_finish(rep) ;
	    	} /* end if (error) */
	    } /* end if (record_start) */
	    if (rs < 0) {
		delete rep ;
	    } /* end if (error) */
	} /* end if (new-pingstatdb_rec) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (pingstatdb_readrecln) */

int pingstatdb_readrecs(PSD *op) noex {
    	cint		rsz = var.rlen ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	DEBUGPRINTF("ent\n") ;
	if (char *rbuf ; (rs = mem.mall(rsz,&rbuf)) >= 0) ylikely {
	    cint rlen = rs ;
	    if ((rs = brewind(op->pfp)) >= 0) ylikely {
	        uint	roff = 0 ;
	        int	line = 1 ;
	        int	f_eol ;
	        int	f_bol = true ;
	        while ((rs = breadln(op->pfp,rbuf,rlen)) > 0) {
	            cint len = rs ;
	            cint rl = (rs - 1) ;
	            f_eol = (rbuf[rl] == '\n') ;
	            rbuf[rl] = '\0' ;
	            if (f_bol && (rl > RF_LEAD)) {
		        DEBUGPRINTF("line=%u\n",line) ;
		        rs = pingstatdb_readrecln(op,roff,rbuf,rl) ;
		        c += rs ;
	            } /* end if (a live one) */
	            roff += len ;
	            line += 1 ;
	            f_bol = f_eol ;
	            if (rs < 0) break ;
	        } /* end while (reading file records) */
	    } /* end if (brewind) */
	    rs1 = mem.free(rbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	DEBUGPRINTF("ret rs=%d c=%u\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (pingstatdb_readrecs) */

int pingstatdb_fes(PSD *op) noex {
	vechand		*rlp = op->rlp ;
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	void *vp ;
	for (int i = 0 ; (rs1 = rlp->get(i,&vp)) >= 0 ; i += 1) {
	    if (PSD_REC *rep = resumelife<PSD_REC>(vp) ; rep) {
	        record_finish(rep) ;
	        vechand_del(op->rlp,i) ;
		delete rep ;
	    }
	} /* end for */
	if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (pingstatdb_fes) */

local bool ishostmat(cc *h1,cc *h2) noex {
    	return (strcmp(h1,h2) == 0) ;
} /* end subroutine (ishostmat) */

int pingstatdb_recget(PSD *op,cchar *hn,PSD_REC **rpp) noex {
	int		rs = SR_BUGCHECK ;
	int		i = 0 ; /* return-value */
	if (op && hn && rpp) ylikely {
    	    vechand	*rlp = op->rlp ;
	    rs = SR_OK ;
	    void *vp ;
	    for (i = 0 ; (rs = rlp->get(i,&vp)) >= 0 ; i += 1) {
	        if (PSD_REC *rep = resumelife<PSD_REC>(vp) ; rep) {
	            if (ishostmat(hn,rep->hostbuf)) {
		        *rpp = rep ;
		        break ;
		    } /* end if (ishostmat) */
	        }
	    } /* end for */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
} /* end subroutine (pingstatdb_get) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_hostname)) >= 0) ylikely {
	    hostnamelen = rs ;
	    rlen = (rs * RLENMULT) ;
	} /* end if (bufsizeget) */
	return rs ;
} /* end method (vars::operator) */


