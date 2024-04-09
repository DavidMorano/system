/* pcsuserfile SUPPORT */
/* lang=C++20 */

/* update a record in a PCS userfile */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	pcsuserfile

	Description:
	This subroutine maintains PCS "users" files.  These files
	are used to maintain the names and counts of the users that
	invoke certain PCS progams.  This is generally for development
	debugging purposes for the programs that log the user
	invocations.

	Synopsis:
	int pcsuserfile(cc *pcs,cc *urfn,cc *nn,cc *un,cc *name) noex

	Arguments:
	pcs		PCS programroot
	ufn		name of file relative to programroot
	nn		machine node name
	un		users login username
	name		real or fullname of the user

	Returns:
	==1		record created
	==0		record written
	<0		error code (system-return)


	Format of file records:

	- eight digits of a decimal count number field
	- the time of the last entry update
	- the nodename and username
	- an optional real or fullname of the user

	Format example:

     989 111115_1137:06_EST      TOTAL
     775 111115_1137:06_EST      rca!local (LOCAL)
     200 111001_2015:04_EDT      rca!dam (David A­D­ Morano)
      14 110103_0922:32_EST      rca!genserv (GENSERV)


	Note that a special user field with the value of "TOTAL"
	maintains a total count of all program invocations separately
	from the user counts.  Oh, and finally, since file record
	locking sucks (that is: it is very often BROKEN) on the
	stupid Sun platforms, we attempt to lock the file to be a
	nice guy but we have relatively small timeouts after which
	we proceed anyway !  This strategy is applied to all record
	locks used throughout this subroutine.


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<nulstr.h>
#include	<useraccdb.h>
#include	<localmisc.h>


/* local defines */

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN	2048
#endif
#endif

#define	USERBUFLEN	(NODENAMELEN + 1 + USERNAMELEN)

#define	TO_LOCK		4		/* seconds */

#ifndef	TIMEBUFLEN
#define	TIMEBUFLEN	80
#endif


/* external subroutines */

extern int	sncpy3(char *,int,cchar *,cchar *,cchar *) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	mkpath3(char *,cchar *,cchar *,cchar *) ;
extern int	sfbasename(cchar *,int,cchar **) ;
extern int	cfdeci(cchar *,int,int *) ;

#if	CF_DEBUGS
extern int	debugprintf(cchar *,...) ;
extern int	debugclose() ;
extern int	strlinelen(cchar *,int,int) ;
#endif

extern char	*strdcpy3(char *,int,cchar *,cchar *,cchar *) ;
extern char	*strwset(char *,int,int) ;
extern char	*strnchr(cchar *,int,int) ;
extern char	*strnrchr(cchar *,int,int) ;


/* external variables */


/* forward references */


/* local variables */


/* exported subroutines */


int pcsuserfile(prpcs,userfname,nodename,username,name)
cchar	prpcs[] ;
cchar	userfname[] ;
cchar	nodename[] ;
cchar	username[] ;
cchar	name[] ;
{
	int		rs = SR_OK ;
	int		rs1 ;
	int		bnl ;
	int		f_created = FALSE ;
	cchar	*bnp ;

	if (prpcs == NULL) return SR_FAULT ;
	if (userfname == NULL) return SR_FAULT ;
	if (nodename == NULL) return SR_FAULT ;
	if (username == NULL) return SR_FAULT ;

#if	CF_DEBUGS
	debugprintf("pcsuserfile: prpcs=%s\n",prpcs) ;
	debugprintf("pcsuserfile: userfile=%s\n",userfname) ;
	debugprintf("pcsuserfile: nn=%s\n",nodename) ;
	debugprintf("pcsuserfile: un=%s\n",username) ;
#endif

	if (prpcs[0] == '\0') return SR_INVALID ;

	if ((nodename[0] == '\0') || (username[0] == '\0'))
	    return SR_INVALID ;

/* get into it */

	if ((bnl = sfbasename(userfname,-1,&bnp)) > 0) {
	    const int	ulen = USERBUFLEN ;
	    cchar	*tp = strnrchr(bnp,bnl,'.') ;
	    char	ubuf[USERBUFLEN + 1] ;
	    if (tp != NULL) bnl = (tp-bnp) ;
	    if ((rs = sncpy3(ubuf,ulen,nodename,"!",username)) >= 0) {
		NULSTR		dbs ;
		cchar	*dbname ;
	        if ((rs = nulstr_start(&dbs,bnp,bnl,&dbname)) >= 0) {
	            USERACCDB	udb ;
	            if ((rs = useraccdb_open(&udb,prpcs,dbname)) >= 0) {
			{
		            f_created = (rs > 0) ;
		            rs = useraccdb_update(&udb,ubuf,name) ;
			}
	                rs1 = useraccdb_close(&udb) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (useraccdb) */
#if	CF_DEBUGS
	debugprintf("pcsuserfile: useraccdb_open-out rs=%d\n",rs) ;
#endif
	            nulstr_finish(&dbs) ;
	        } /* end if (nulstr) */
	    } /* end if (sncpy) */
#if	CF_DEBUGS
	debugprintf("pcsuserfile: sncpy-out rs=%d\n",rs) ;
#endif
	} else
	    rs = SR_INVALID ;

#if	CF_DEBUGS
	debugprintf("pcsuserfile: ret rs=%d f_created=%u\n",rs,f_created) ;
#endif

	return (rs >= 0) ? f_created : rs ;
}
/* end subroutine (pcsuserfile) */


