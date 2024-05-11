/* usyscalls HEADER */
/* lang=C20 */

/* version %I% last-modified %G% */
/* UNIX® system-calls */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in MacOS Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

#ifndef	USYSCALLS_INCLUDE
#define	USYSCALLS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<sys/utsname.h>
#include	<sys/uio.h>
#include	<sys/time.h>		/* |u_adjtime(2u)| */
#include	<sys/resource.h>
#include	<sys/stat.h>
#include	<sys/statvfs.h>
#include	<sys/socket.h>
#include	<sys/poll.h>

#include	<signal.h>
#include	<limits.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<utime.h>		/* for old UNIX® |u_utime(2)| */
#include	<pthread.h>
#include	<termios.h>
#include	<time.h>
#include	<errno.h>
#include	<dirent.h>
#include	<netdb.h>
#include	<pwd.h>
#include	<grp.h>

#if	defined(SYSHAS_XTI) && (SYSHAS_XTI > 0)
#include	<xti.h>
#endif

#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>

#include	<usys.h>	/* <- auxilllary OS support */

#include	<usysop.h>	/* UNIX® system-operations */
#include	<usysdata.h>	/* UNIX® system-data */
#include	<um.h>		/* UNIX® memory-management */
#include	<uipc.h>	/* UNIX® System V IPC */
#include	<ustr.h>	/* UNIX® STREAMS® */
#include	<uopen.h>
#include	<ufiledesc.h>	/* file-descriptor */
#include	<ufileop.h>	/* file-operations */
#include	<usig.h>
#include	<uprocess.h>
#include	<ugetloadavg.h>


EXTERNC_begin

extern int	u_resolvepath(cchar *,char *,int) noex ;
extern int	u_access(cchar *,int) noex ;
extern int	u_link(cchar *,cchar *) noex ;
extern int	u_unlink(cchar *) noex ;
extern int	u_rmdir(cchar *) noex ;
extern int	u_rename(cchar *,cchar *) noex ;
extern int	u_symlink(cchar *,cchar *) noex ;
extern int	u_chown(cchar *,uid_t,gid_t) noex ;
extern int	u_lchown(cchar *,uid_t,gid_t) noex ;
extern int	u_chmod(cchar *,mode_t) noex ;
extern int	u_utime(cchar *,const UTIMBUF *) noex ;
extern int	u_utimes(cchar *,const TIMEVAL *) noex ;
extern int	u_sync() noex ;

#if	defined(SYSHAS_ACL) && (SYSHAS_ACL > 0)
extern int	u_acl(cchar *,int,int,aclent_t *) noex ;
extern int	u_facl(int,int,int,aclent_t *) noex ;
#endif /* SYSHAS_ACL */

EXTERNC_end


#endif /* USYSCALLS_INCLUDE */


