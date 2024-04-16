/* uc_procpid */
/* lang=C20 */

/* interface component for UNIX® library-3c */
/* get a process ID by searching for its command string */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_procpid

	Description:
	We search for a process (first one) with a given command
	string and UID.

	Synopsis:
	int uc_procpid(cchar *name,uid_t uid) noex

	Arguments:
	name		string with name of process (command-string) to search
	uid		process must have this UID

	Returns:
	>=0		proces s foudnd with this PID
	==0		no such process
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<usystem.h>
#include	<sfx.h>
#include	<cfdec.h>
#include	<ctdec.h>
#include	<localmisc.h>


/* local defines */

#define	READBUFLEN	(MAXPATHLEN+DIGBUFLEN+2)


/* external subroutines */


/* exported subroutines */

int uc_procpid(cchar *name,uid_t uid) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		pid = 0 ;
	if (name) {
	    cint	v = uid ;
	    rs = SR_INVALID ;
	    if (name[0]) {
	        cint		dlen = DIGBUFLEN ;
	        char		dbuf[DIGBUFLEN+1] ;
	        if ((rs = ctdeci(dbuf,dlen,v)) >= 0) {
	            cint	of = O_RDONLY ;
	            int		i = 0 ;
	            cchar	*pfname = "sys:pgrep" ;
	            cchar	*argv[6] ;
	            argv[i++] = "PGREP" ;
	            if (v >= 0) {
	                argv[i++] = "-U" ;
	                argv[i++] = dbuf ;
	            }
	            argv[i++] = name ;
	            argv[i] = NULL ;
	            if ((rs = uc_openprog(pfname,of,argv,NULL)) >= 0) {
	                cint	fd = rs ;
	                cint	llen = READBUFLEN ;
	                char	lbuf[READBUFLEN+1] ;
	                if ((rs = u_read(fd,lbuf,llen)) > 0) {
	                    cint	sl = rs ;
	                    int		cl ;
	                    cchar	*sp = lbuf ;
	                    cchar	*cp ;
	                    if ((cl = sfnext(sp,sl,&cp)) > 0) {
	                        rs = cfdeci(cp,cl,&v) ;
	                        pid = v ;
	                    }
	                } /* end if (u_read) */
	                rs1 = uc_close(fd) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (open) */
	        } /* end if (ctdeci) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? pid : rs ;
}
/* end subroutine (uc_procpid) */

