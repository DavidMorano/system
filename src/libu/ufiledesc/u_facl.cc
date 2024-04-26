/* u_facl */

/* translation layer interface for UNIX� equivalents */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */

#include	<sys/types.h>
#if	defined(SYSHAS_ACL) && (SYSHAS_ACL > 0)
#include	<sys/acl.h>
#endif
#include	<fcntl.h>
#include	<poll.h>
#include	<errno.h>

#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#define	TO_NOSPC	5


/* external subroutines */

extern int	msleep(int) ;


/* exported subroutines */


#if	defined(SYSHAS_ACL) && (SYSHAS_ACL > 0)

int u_facl(int fd,int cmd,int nentries,aclent_t *ap)
{
	int		rs ;
	int		to_nospc = TO_NOSPC ;
	int		f_exit = FALSE ;

	if (ap == NULL) return SR_FAULT ;

	repeat {
	    if ((rs = facl(fd,cmd,nentries,ap)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOSPC:
	            if (to_nospc-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            msleep(1000) ;
		    break ;
	        case SR_INTR:
	            break ;
		default:
		    f_exit = TRUE ;
		    break ;
		} /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_facl) */

#else

int u_facl(fd,cmd,nentries,ap)
int		fd ;
int		cmd ;
int		nentries ;
aclent_t	*ap ;
{

	return SR_NOSYS ;
}
/* end subroutine (u_facl) */

#endif /* defined(SYSHAS_ACL) && (SYSHAS_ACL > 0) */



