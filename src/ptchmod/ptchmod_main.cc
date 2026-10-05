/* ptchmod_main SUPPORT (ptchmod) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* change owner and mode on terminal device */
/* version %I% last-modified %G% */


/* updated (enhanced):

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	This program changes the owner and mode of the pseudo terminal 
	slave device.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<grp.h>			/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */


/* local defines */

#define	DEFAULT_TTY_GROUP	"tty"


/* external subroutines */


/* exported subroutines */

int main(int argc,con mainv argv,con mainv envv) {
    	cnullptr	np{} ;
	int		ex = EXIT_FAILURE ;
	if (argc >= 2) {
	    group	*gr_name_ptr ;
	    gid_t	gid ;
	    int		fd ;
	    if ((gr_name_ptr = getgrnam(DEFAULT_TTY_GROUP)) != np) {
		gid = gr_name_ptr->gr_gid ;
	    } else {
		gid = getgid() ;
	    }
	    fd = atoi(argv[1]) ;
	    if (cchar *cp = ptsname(fd)) {
	        const uid_t	uid = getuid() ;
	        if (chown(cp,uid,gid) == 0) {
	            if (chmod(cp, 00620) == 0) {
	                ex = EXIT_SUCCESS ;
	            }
	        }
	    } /* end if (ptsname) */
	} /* end if (good arguments) */
	return ex ;
} /* end subroutine (main) */


