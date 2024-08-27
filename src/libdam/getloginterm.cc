/* getloginterm SUPPORT */
/* lang=C++20 */

/* get the name of the controlling terminal for the current session */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-01-10, David A­D­ Morano
	This subroutine was originally written. It was prompted by
	the failure of other terminal message programs from finding
	the proper controlling terminal.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getloginterm

	Description:
	This subroutine will find and return the name of the controlling
	terminal for the given session ID.

	Synopsis:
	int getloginterm(char *dbuf,int dlen,pid_t sid) noex

	Arguments:
	dbuf	user buffer to receive name of controlling terminal
	dlen	length of user supplied buffer
	sid	session ID to find controlling terminal for

	Returns :
	>=	length of name of controlling terminal
	<0	error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<tmpx.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"getloginterm.h"


/* local defines */

#define	DEVDIR		"/dev/"


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr cchar		devdir[] = DEVDIR ;


/* exported variables */


/* exported subroutines */

int getloginterm(char *dbuf,int dlen,pid_t sid) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (dbuf) {
	    rs = SR_INVALID ;
	    if (dlen > 0) {
		cnullptr	np{} ;
	        tmpx		utmp ;
	        tmpx_ent	ue ;
		cint		of = O_RDONLY ;
	        if (sid <= 0) sid = u_getsid((pid_t) 0) ;
	        if ((rs = tmpx_open(&utmp,np,of)) >= 0) {
	            if ((rs = tmpx_fetchpid(&utmp,&ue,sid)) >= 0) {
	                if (dlen >= 0) {
	                    if (dlen >= 6) {
				int	i ; /* used-afterwards */
	                        strcpy(dbuf,devdir) ;
	                        for (i = 0 ; i < MIN(32,dlen) ; i += 1) {
			            if (ue.ut_line[i] == '\0') break ;
	                            dbuf[5 + i] = ue.ut_line[i] ;
			        } /* end for */
	                        if ((i < 32) && (ue.ut_line[i] != '\0')) {
	                            rs = SR_TOOBIG ;
	                        } else {
	                            dbuf[5 + i] = '\0' ;
			        }
	                        len = 5 + i ;
	                    } else {
	                        rs = SR_TOOBIG ;
		            }
	                } else {
	                    strcpy(dbuf,devdir) ;
	                    len = strwcpy(dbuf + 5,ue.ut_line,32) - dbuf ;
	                }
	            } /* end if (got it) */
	            rs1 = tmpx_close(&utmp) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (UTMPX open) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getloginterm) */


