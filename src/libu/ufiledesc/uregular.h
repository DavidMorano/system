/* uregular HEADER (UNIX® file operations) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:

	Description:
	All of the UNIX® system calls that use (operate on) a
	file-descroptor.

*******************************************************************************/

#ifndef	UREGULAR_INCLUDE
#define	UREGULAR_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>		/* system types */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usupport.h>

EXTERNC_begin

extern int u_closeonexec(int,int) noex ;
extern int u_nonblock(int,int) noex ;
extern int u_readn(int,void *,int) noex ;
extern int u_writen(int,cvoid *,int) noex ;
extern int u_fchdir(int) noex ;
extern int u_fchmod(int,mode_t) noex ;
extern int u_fchmodmin(int,mode_t) noex ;
extern int u_fchown(int,uid_t,gid_t) noex ;
extern int u_ftimes(int,CTIMEVAL *) noex ;
extern int u_close(int) noex ;
extern int u_fstat(int,ustat *) noex ;
extern int u_fstatfs(int,ustatfs *) noex ;
extern int u_fstatvfs(int,ustatvfs *) noex ;
extern int u_fpathconf(int,int,long *) noex ;
extern int u_fstype(int,char *,int) noex ;
extern int u_fsync(int) noex ;
extern int u_fsyncdata(int) noex ;
extern int u_fsize(int) noex ;
extern int u_ftruncate(int,off_t) noex ;
extern int u_ioctl(int,int,...) noex ;
extern int u_lockf(int,int,off_t) noex ;
extern int u_pread(int,void *,int,off_t) noex ;
extern int u_pwrite(int,cvoid *,int,off_t) noex ;
extern int u_read(int,void *,int) noex ;
extern int u_readv(int,IOVEC *,int) noex ;
extern int u_seeko(int,off_t,int,off_t *) noex ;
extern int u_write(int,cvoid *,int) noex ;
extern int u_writev(int,CIOVEC *,int) noex ;

static inline int u_seek(int fd,off_t wo,int w) noex {
	return u_seeko(fd,wo,w,nullptr) ;
}
static inline int u_seekoff(int fd,off_t wo,int w,off_t *offp) noex {
	return u_seeko(fd,wo,w,offp) ;
}
static inline int u_tell(int fd,off_t *rp) noex {
	return u_seeko(fd,0l,SEEK_CUR,rp) ;
}
static inline int u_seekable(int fd) noex {
	return u_seek(fd,0l,SEEK_CUR) ;
}
static inline int u_rewind(int fd) noex {
	return u_seek(fd,0l,SEEK_SET) ;
}

extern int u_poll(POLLFD *,int,int) noex ;	/* <- special case */

EXTERNC_end


#endif /* UREGULAR_INCLUDE */


