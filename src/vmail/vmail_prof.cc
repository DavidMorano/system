/* vmail_prof SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* profile options */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* run-time debug print-outs */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<curses.h>		/* POSIX® */
#include	<pwd.h>			/* POSIX® */
#include	<grp.h>			/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<csignal>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdio>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<logfile.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<bfile.h>		/* LIBB */

#include	"vmail_config.h"
#include	"vmail_prof.h"
#include	"defs.h"


/* external subroutines */


/* external vaiables */

extern struct proginfo	g ;



/* boolean function which determines whether the current user profile 
   has set the specified option (desires certain action).  
   returns 1 if set, 0 if not.
   references the global profile structure "userprof".
 */

int profile(opt)
cchar	opt[];
{
	int	i ;


	i = 0 ;
	while ((userprof[i].name != NULL) && (userprof[i].name[0] != '\0')) {

	    if (strcmp(userprof[i].name,opt) == 0)
	        return (userprof[i].value) ;

	    i += 1 ;

	} /* end while */

/* if we did not have the option specified at all, consider it OFF */

	return -1 ;
}
/* end subroutine (profile) */

/****
	initializes the various profile options.
   any user-specified ones are contained in the shell variable RDMAILOPTS.
   these options then over-ride the defaults in the global profile structure
    "userprof".  this structure is defined in "defs.c" and declared
    in "config.h" (which is included into every function file).
   set options (+) have value 1, not set (-) have value 0.
****/

int profinit() noex {
	int i,val;
	char 	opts[MAXPATHLEN + 1],*opt;
	/* check for user over-rides */
	opt = getenv("RDMAILOPTS");
	if ((opt == NULL) || (*opt == '\0')) {
	    strcpy(opts,DRDMAILOPTS);
	} else {
	    strcpy(opts, opt );
	}
	opt = strtok(opts,":");
	if (opt != NULL) {
	    do {

#if	CF_DEBUG
		debugprintf("profinit: given \"%s\"\n",opt) ;
#endif

		val = 1 ;
	        if (*opt == '-') {

	            val = 0;     		/* turn off */
	        	opt += 1 ;

		} else if (*opt == '+')
			opt += 1 ;

#if	CF_DEBUG
		debugprintf("profinit: given \"%s\"\n",opt) ;
#endif

	        i=0;
	        while ((int) strlen(userprof[i].name) > 0) {

#if	CF_DEBUG
		debugprintf("profinit: checking for \"%s\"\n",userprof[i].name) ;
#endif

	            if (strcmp(opt,userprof[i].name) == 0) {

#if	CF_DEBUG
		debugprintf("profinit: got \"%s\"\n",opt) ;
#endif

	                userprof[i].value = val;
			break ;

	            }
	            i += 1 ;

	        } /* end while */

	        opt = strtok(0,":");

	    } while (opt != NULL) ;

	} /* end if */

	return OK ;
} /* end subroutine (profinit) */


