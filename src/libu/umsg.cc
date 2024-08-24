/* umsg SUPPORT (UNIX® System V IPC - Message Queues) */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	u_msgget
	u_msgsnd
	u_msgrcv
	u_msgctl

	Descroption:
	UNIX® System V Inter-Process-Communication (IPC) - Message Queues

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/ipc.h>
#include	<sys/msg.h>
#include	<unistd.h>
#include	<climits>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<localmisc.h>

#include	"uipc.h"


/* local defines */


/* imported namespaces */

using namespace	uipc ;			/* namespace */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local variables */


/* exported variables */


/* exported subroutines */

int u_msgget(key_t key,int msgflag) noex {
	int		rs ;
	int		to_nospc = utimeout[uto_nospc] ;
	bool		f_exit = false ;
	repeat {
	    if ((rs = msgget(key,msgflag)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOSPC:
	            if (to_nospc-- > 0) {
	                msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
		    break ;
	        case SR_AGAIN:
	        case SR_INTR:
	            break ;
		default:
		    f_exit = true ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;
	return rs ;
}
/* end subroutine (u_msgget) */

int u_msgsnd(int msqid,void *msgp,int msgsz,int msgflag) noex {
	int		rs ;
	repeat {
	    if ((rs = msgsnd(msqid,msgp,msgsz,msgflag)) < 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;
	return rs ;
}
/* end subroutine (u_msgsnd) */

int u_msgrcv(int msqid,void *msgp,int msgsz,long msgtype,int msgflag) noex {
	int		rs ;
	repeat {
	    if ((rs = msgrcv(msqid,msgp,msgsz,msgtype,msgflag)) < 0) {
	        rs = (- errno) ;
	    }
	} until (rs != SR_INTR) ;
	return rs ;
}
/* end subroutine (u_msgrcv) */

/*******************************************************************************

	Name:
	u_msgctl
	
	Note: We have no reason to believe that this subroutine can
	generate an SR_NOSPC or even a SR_INTR.  But we have code
	included to handle these (with retries).  If these error
	types never occur, then no harm was done.

*******************************************************************************/

int u_msgctl(int msgid,int cmd,MSQIDDS *buf) noex {
	int		rs ;
	int		to_nospc = utimeout[uto_nospc] ;
	bool		f_exit = false ;
	repeat {
	    if ((rs = msgctl(msgid,cmd,buf)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOSPC:
	            if (to_nospc-- > 0) {
	        	msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
		    break ;
	        case SR_AGAIN:
	        case SR_INTR:
	            break ;
		default:
		    f_exit = true ;
		    break ;
	       } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;
	return rs ;
}
/* end subroutine (u_msgctl) */


