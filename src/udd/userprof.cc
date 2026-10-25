/* userprof SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* version %I% last-modified %G% */
/* version %I% last-modified %G% */

#define		CF_DEBUG	1

/* revision history:

	= 1995-06-01, David A­D­ Morano
	Originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	?

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/param.h>
#include	<sys/utsname.h>
#include	<unistd.h>
#include	<pwd.h>
#include	<grp.h>
#include	<ctime>
#include	<curses.h>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<bfile.h>
#include	<baops.h>
#include	<char.h>
#include	<mallocstuff.h>
#include	<localmisc.h>

#include	"mailbox.h"


/* local defines */


/* external subroutines */


/* external variables */

extern userprof		u ;


/* local structures */


/* forward references */


/* local variables */

static URSNAME		uts ;


/* exported variables */


/* exported subroutines */

int userprof(userprog *up) noex {
	TM		*timep ;
	PASSWD		*pp ;
	GROUP		*gp ;
	ustat		stats ;
	int		pan, i, l ;
	int		rs ;
	char	buf[BUFLEN + 1] ;
	char	*cp ;

/* get our process PID */

	up->pid = getpid() ;

/* get UNIX system name */

	uname(&uts) ;

	up->nodename = uts.nodename ;
	if ((cp = strchr(uts.nodename,'.')) != NULL) {

	    *cp++ = '\0' ;
	    up->domain = cp ;
	}

/* machine user ID */

	up->uid = getuid() ;

/* try to get a passwd entry */

	up->username = NULL ;
	if ((up->username == NULL) && ((cp = getenv("LOGNAME")) != NULL)) {

	    if ((pp = getpwnam(cp)) != NULL) {

	        if (pp->pw_uid == up->uid) up->username = cp ;

	    }
	}

	if ((up->username == NULL) && ((cp = getlogin()) != NULL)) {

	    if ((pp = getpwnam(cp)) != NULL) {

	        if (pp->pw_uid == up->uid) 
			up->username = mallocstr(cp) ;

	    }
	}

	if (up->username == NULL) {

	    if ((pp = getpwuid(up->uid)) != NULL)
	        up->username = mallocstr(pp->pw_name) ;

	    else
	        up->username = cp ;

	}

/* get the GECOS name field (leave in variable 'cp') if we can also */

	if (pp != NULL) {

#if	CF_DEBUG_MAIN
	        eprintf("got a passwd entry\n") ;
#endif

	    up->gid = pp->pw_gid ;
	    up->homedir = mallocstr(pp->pw_dir) ;

	    cp = pp->pw_gecos ;

	} else {

#if	CF_DEBUG_MAIN
	        eprintf("did NOT get a passwd entry\n") ;
#endif

	    up->gid = 1 ;
	    if ((gp = getgrnam("other")) != NULL)
	        up->gid = gp->gr_gid ;

	    up->homedir = getenv("HOME") ;

	    cp = getenv("NAME") ;

	}

/* process the GECOS name if supplied */

	up->gecosname = NULL ;
	if (cp != NULL) {

#if	CF_DEBUG_MAIN
	        eprintf("cp so far %s\n",cp) ;
#endif

	    if (fixgecos(cp,buf,BUFLEN) >= 0)
	        up->gecosname = mallocstr(buf) ;

	}

#if	CF_DEBUG_MAIN
	    eprintf("userprof: about gecos %08X\n",up->gecosname) ;
	    eprintf("userprof: gecos %s\n",up->gecosname) ;
	    eprintf("userprof: homedir=\"%s\"\n",up->homedir) ;
#endif

/* can we make some sort of perferred 'mail' name ? */

	up->mailname = up->gecosname ;
	if (up->gecosname != NULL) {

	    if (mailname(up->gecosname,buf,BUFLEN) >= 0)
	        up->mailname = mallocstr(buf) ;

	}

#if	CF_DEBUG_MAIN
	    eprintf("userprof: put name on heap\n") ;
#endif


	sprintf(buf,"%s%d",up->nodename,up->pid) ;

	up->logid = mallocstr(buf) ;

	return OK ;
}
/* end subroutine (userprof) */


