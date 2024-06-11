/* gethename SUPPORT */
/* lang=C++20 */

/* subroutine to get a single host entry by name (raw) */
/* version %I% last-modified %G% */

#define	CF_LOG		0

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	gethename

	Description:
	This subroutine is a platform independent subroutine to get
	an INET host address entry, but does it dumbly on purpose.

	Synopsis:
	int gethename(HOSTENT *hep,char *hbuf,int hlen,cc *name) noex

	Arguments:
	- hep		pointer to 'hostent' structure
	- hbuf		user supplied buffer to hold result
	- hlen		length of user supplied buffer
	- name		name to lookup

	Returns:
	0		host was found OK
	-1		host could not be found
	-2		request timed out (bad network someplace)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<ctime>
#include	<netdb.h>
#include	<usystem.h>
#include	<getxx.h>
#include	<ucgetpid.h>
#include	<bfile.h>
#include	<snx.h>
#include	<localmisc.h>

#include	"gethename.h"

#if	CF_LOG
#include	<logfile.h>
#endif


/* local defines */

#ifndef	LOGIDLEN
#define	LOGIDLEN	80
#endif

#define	LOGFNAME	"/tmp/gethostbyname.log"
#define	SERIALFILE1	"/tmp/serial"
#define	SERIALFILE2	"/tmp/gethename.serial"
#define	DEFLOGSIZE	80000


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int gethename(HOSTENT *hep,char *hebuf,int helen,cchar *name) noex {

#if	CF_LOG
	logfile	lh ;
#endif

#if	CF_LOG
	pid_t	pid ;
#endif

#if	CF_LOG
	int	f_log = false ;
#endif

	int	rs ;

#if	CF_LOG
	char	logid[LOGIDLEN + 1] ;
#endif


	if ((hep == NULL) || (hebuf == NULL) || (name == nullptr))
	    return SR_FAULT ;

/* do we want logging performed? */

#if	CF_LOG
	if (u_access(LOGFNAME,W_OK) >= 0) {
	    pid = uc_getpid() ;
	    int		serial = -1 ;
	    char	*cp ;

	    if (serial < 0) {
	        cp = SERIALFILE1 ;
	        serial = getserial(cp) ;
	    }
	    if (serial < 0) {
	        cp = SERIALFILE2 ;
	        serial = getserial(cp) ;
	    }

	    if (serial == 0)
	        u_chmod(cp,0666) ;

	    if (serial < 0)
	        serial = 0 ;

	    snddd(logid,LOGIDLEN, pid,serial) ;

	    rs = logfile_open(&lh,LOGFNAME,0,0666,logid) ;
	    f_log = (rs >= 0) ;

	    if (f_log)
	        logfile_printf(&lh,
			"name=%s\n",(name == NULL) ? STDFNNULL : name) ;

	} /* end if */
#endif /* CF_LOG */

/* do the real work */

	rs = uc_gethostbyname(hep,hebuf,helen,name) ;

#if	CF_LOG
	if (f_log) {
	    if (rs == SR_TIMEDOUT) {
	        logfile_printf(&lh,"network timeout (rs=%d)\n",rs) ;
	    } else if (rs == SR_NOTFOUND) {
	        logfile_printf(&lh,"entry not found (rs=%d)\n",rs) ;
	    } else {
	        logfile_printf(&lh,"result=%s (rs=%d)\n",
	            ((rs >= 0) ? hep->h_name : STDFNNULL),rs) ;
	    }
	}
#endif /* CF_LOG */

ret1:

#if	CF_LOG
	if (f_log) {
	    logfile_checksize(&lh,DEFLOGSIZE) ;
	    logfile_close(&lh) ;
	}
#endif /* CF_LOG */

ret0:

	return rs ;
}
/* end subroutine (gethename) */


