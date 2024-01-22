/* ulogerror SUPPORT */
/* lang=C++20 */

/* log an error in the error buffer */
/* version %I% last-modified %G% */


/* revision history:

	= 2023-10-08, David A.D. Morano
	This is a new thing, because, why now?
	
*/

/* Copyright © 2023 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ulogerror

	Description:
	This is a facility to log a small number of error reports.
	These reports can optionally be written out (in formatted
	form) to STDERR on program exit.

	Synopsis:
	int logerror(cchar *id,int err,cchar *msg) noex

	Arguments:
	id		the ID of the reporter
	err		the numerial error from reporter
	msg		optional message being reported

	Returns:
	0		your error report was not recorded
	1		your error report was recorded

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstdio>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>
#include	<getfdfile.h>		/* <- for |FD_STDxxx| */
#include	<strdcpy.h>
#include	<localmisc.h>

#include	"ulogerror.h"		/* <- redundant */


/* local defines */

#define	ULOGERRMGR_IDLEN	16
#define	ULOGERRMGR_MSGLEN	80
#define	ULOGERRMGR_NENTS	10
#define	ULOGERRMGR_VARNAME	"ULOGERROR_OUTPUT"


/* local typedefs */


/* external subroutines */


/* local structures */

namespace {
   struct ulogerrmgr_ent {
	int		err ;
	char		id[ULOGERRMGR_IDLEN+1] ;
	char		msg[ULOGERRMGR_IDLEN+1] ;
   } ;
   struct ulogerrmgr {
	ulogerrmgr_ent	ents[ULOGERRMGR_NENTS] = {} ;
	int		c = 0 ;
	int record(cchar *,int,cchar * = nullptr) noex ;
	void fini() noex ;
	~ulogerrmgr() noex {
	    fini() ;
	} ;
    } ; /* end struct ulogerrmgr) */
}


/* forward references */


/* local variables */

static constexpr int		wlen = (ULOGERRMGR_MSGLEN*2) ;

static constexpr char		var[] = ULOGERRMGR_VARNAME ;

static constinit ulogerrmgr	ulogerrmgr_data ;


/* exported variables */


/* exported subroutines */

int ulogerror(cchar *id,int err,cchar *msg) noex {
	int		f = false ;
	if (id) {
	    f = ulogerrmgr_data.record(id,err,msg) ;
	}
	return f ;
}


/* local subroutines */

int ulogerrmgr::record(cchar *id,int err,cchar *msg) noex {
	int		f = false ;
	if (c < ULOGERRMGR_NENTS) {
	    f = true ;
	    ents[c].err = err ;
	    strdcpy(ents[c].id,ULOGERRMGR_IDLEN,id) ;
	    if (msg) {
	        strdcpy(ents[c].msg,ULOGERRMGR_MSGLEN,msg) ;
	    }
	    c += 1 ;
	} /* end if (recorded) */
	return f ;
}
/* end subroutine (ulogerrmgr::record) */

void ulogerrmgr::fini() noex {
	static constexpr int	fd = FD_STDERR ;
	cchar	*announce = "ulogerror: dump\n" ;
	cchar	*fmt = "ulogerror: %s (%3d) %s\n" ;
	cchar	*vp ;
	if ((vp = getenv(var)) != nullptr) {
	    if (vp[0] && (c > 0)) {
		int	wl ;
		char	wbuf[wlen+1] ;
		write(fd,announce,strlen(announce)) ;
	        for (int i = 0 ; i < c ; i += 1) {
	            cint	err = ents[i].err ;
	            cchar	*id = ents[c].id ;
	            cchar	*msg = ents[c].msg ;
		    if ((wl = snprintf(wbuf,wlen,fmt,id,err,msg)) > 0) {
		        write(fd,wbuf,wl) ;
		    } /* end if (snprintf) */
		} /* end for */
	    } /* end if (have valid) */
	} /* end if (have variable) */
}
/* end subroutine (ulogerrmgr::fini) */


