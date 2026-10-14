/* termstore SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* store terminal attributes */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debugging */
#define	CF_TCATTR	1		/* use |tcXXXattr(3termion)| */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written for use by PCS 'vmail'.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<sys/time.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<termios.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usysflag.h>		/* LIBU */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"termstore.h"


/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* compile-time debugging */
#endif
#ifndef	CF_TCATTR
#define	CF_TCATTR	1		/* use |tcXXXattr(3termion)| */
#endif


/* external subroutines */


/* external varaibles */


/* local variables */


/* forward references */

local int ta_get(int,mut TERMIOS *) noex ;
local int ta_set(int,con TERMIOS *) noex ;


/* local variables */

cbool		f_debug		= CF_DEBUG ;
cbool		f_tcattr	= CF_TCATTR ;


/* exported variables */


/* exported subroutines */

/* save the terminal mode first */
int termstore_save(termstore *tsp,int tfd) noex {
	int		rs = SR_FAULT ;
	if (tsp) ylikely {
	    rs = SR_BADF ;
	    if (tfd >= 0) ylikely {
	        tsp->f_stored = false ;
	        tsp->fd = -1 ;
	        if (ustat sb ; (rs = u_fstat(tfd,&sb)) >= 0) ylikely {
	            TERMIOS *tap = &tsp->ta ;
	            tsp->fd = tfd ;
	            tsp->mode = sb.st_mode ;
	            if ((rs = ta_get(tfd,tap)) >= 0) {
	                tsp->f_stored = true ;
	            } /* end if (ta_get) */
	        } /* end if (u_fstat) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (termstore_save) */

/* restore the terminal settings */
int termstore_restore(termstore *tsp) noex {
	int		rs = SR_FAULT ;
	if (tsp) ylikely {
	    rs = SR_NOTOPEN ;
	    if ((tsp->fd >= 0) && tsp->f_stored) ylikely {
	        TERMIOS *tap = &tsp->ta ;
		cint tfd = tsp->fd ;
		if ((rs = ta_set(tfd,tap)) >= 0) {
	            if (ustat sb ; (rs = u_fstat(tsp->fd,&sb)) >= 0) {
	                if (sb.st_mode != tsp->mode) {
		            rs = u_fchmod(tfd,tsp->mode) ;
	                }
	            } /* end if (u_fstat) */
		} /* end if */
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (termstore_restore) */


/* local subroutines */

local int ta_get(int fd,mut TERMIOS *tap) noex {
	int		rs ;
	if_constexpr (f_tcattr) {
	    rs = uc_tcattrget(fd,tap) ;
	} else {
	    if (usysflag.sunos || usysflag.linux) {
	        rs = u_ioctl(fd,TCGETS,tap) ;
	    } else {
	        rs = u_ioctl(fd,TIOCGETA,tap) ;
	    } /* end */
	} /* end if_constexpr (f_tcattr) */
	return rs ;
} /* end subroutine (ta_get) */

local int ta_set(int fd,con TERMIOS *tap) noex {
    	int		rs ;
	if_constexpr (f_tcattr) {
	    rs = uc_tcattrset(fd,TCSADRAIN,tap) ;
	} else {
	    if (usysflag.sunos || usysflag.linux) {
	        rs = u_ioctl(fd,TCSETSW,tap) ;
	    } else {
	        rs = u_ioctl(fd,TIOCSETAW,tap) ;
	    } /* end */
	} /* end if_constexpr (f_tcattr) */
	return rs ;
} /* end subroutine (ta_set) */


