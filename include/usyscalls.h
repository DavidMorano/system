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
#include	<sys/time.h>		/* for 'u_adjtime(3u)' */
#include	<sys/timeb.h>		/* for 'uc_ftime(3uc)' */
#include	<sys/resource.h>
#include	<sys/stat.h>
#include	<sys/statvfs.h>
#include	<sys/socket.h>
#include	<sys/poll.h>

#if	defined(SYSHAS_ACL) && (SYSHAS_ACL > 0)
#include	<sys/acl.h>
#endif

#include	<signal.h>
#include	<limits.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<utime.h>		/* for 'u_utime(2)' */
#include	<pthread.h>
#include	<termios.h>
#include	<time.h>
#include	<errno.h>
#include	<ucontext.h>
#include	<dirent.h>
#include	<netdb.h>
#include	<pwd.h>
#include	<grp.h>

#if	defined(SYSHAS_XTI) && (SYSHAS_XTI > 0)
#include	<xti.h>
#endif

#if	defined(OSNAME_SunOS) && (OSNAME_SunOS > 0)
#include	<thread.h>
#include	<shadow.h>
#include	<project.h>
#include	<user_attr.h>
#endif

#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>

#include	<usys.h>	/* <- auxilllary OS support */

#include	<um.h>		/* UNIX® memory-management */
#include	<ustr.h>	/* UNIX® STREAMS */
#include	<udup.h>


EXTERNC_begin

extern int	u_brk(cvoid *) noex ;
extern int	u_sbrk(int,void **) noex ;

extern int	u_uname(UTSNAME *) noex ;
extern int	u_getloadavg(uint *,int) noex ;
extern int	u_sysauxinfo(char *,int,int) noex ;
extern int	u_adjtime(TIMEVAL *,TIMEVAL *) noex ;
extern int	u_ulimit(int,int) noex ;

extern int	u_getpgid(pid_t) noex ;
extern int	u_getsid(pid_t) noex ;
extern int	u_getgroups(int,gid_t *) noex ;
extern int	u_setgroups(int,const gid_t *) noex ;
extern int	u_setuid(uid_t) noex ;
extern int	u_seteuid(uid_t) noex ;
extern int	u_setreuid(uid_t,uid_t) noex ;
extern int	u_setgid(gid_t) noex ;
extern int	u_setegid(gid_t) noex ;
extern int	u_setregid(gid_t,gid_t) noex ;
extern int	u_setpgid(pid_t,pid_t) noex ;
extern int	u_setsid() noex ;

extern int	u_sigaction(int,SIGACTION *,SIGACTION *) noex ;
extern int	u_sigprocmask(int,sigset_t *,sigset_t *) noex ;
extern int	u_sigsuspend(const sigset_t *) noex ;
extern int	u_sigwait(const sigset_t *,int *) noex ;
extern int	u_sigpending(sigset_t *) noex ;
extern int	u_pause() noex ;
extern int	u_sigaltstack(const stack_t *,stack_t *) noex ;

extern int	u_sigsend(idtype_t,id_t,int) noex ;
extern int	u_sigsendset(procset_t *,int) noex ;

extern int	u_setcontext(const ucontext_t *) noex ;

extern int	u_fork() noex ;
extern int	u_vfork() noex ;
extern int	u_execve(cchar *,mainv,mainv) noex ;
extern int	u_execv(cchar *,mainv) noex ;
extern int	u_execvp(cchar *,mainv) noex ;
extern int	u_exit(int) noex ;
extern int	u_kill(pid_t,int) noex ;
extern int	u_waitpid(pid_t,int *,int) noex ;
extern int	u_getrlimit(int,RLIMIT *) noex ;
extern int	u_setrlimit(int,const RLIMIT *) noex ;
extern int	u_nice(int) noex ;

extern int	u_mknod(cchar *,mode_t,dev_t) noex ;
extern int	u_mkdir(cchar *,mode_t) noex ;
extern int	u_chdir(cchar *) noex ;
extern int	u_readlink(cchar *,char *,int) noex ;
extern int	u_pathconf(cchar *,int,long *) noex ;
extern int	u_statvfs(cchar *,USTATVFS *) noex ;
extern int	u_stat(cchar *,USTAT *) noex ;
extern int	u_lstat(cchar *,USTAT *) noex ;
extern int	u_creat(cchar *,mode_t) noex ;
extern int	u_open(cchar *,int,mode_t) noex ;

#if	defined(OSNAME_SunOS) && (OSNAME_SunOS > 0)
extern int	u_getdents(int,dirent_t *,int) noex ;
#endif

extern int	u_fchdir(int) noex ;
extern int	u_fsync(int) noex ;
extern int	u_pipe(int *) noex ;
extern int	u_socket(int,int,int) noex ;
extern int	u_getsockopt(int,int,int,void *,int *) noex ;
extern int	u_setsockopt(int,int,int,cvoid *,int) noex ;
extern int	u_socketpair(int,int,int,int *) noex ;
extern int	u_connect(int,cvoid *,int) noex ;
extern int	u_accept(int,cvoid *,int *) noex ;

extern int	u_poll(POLLFD *,int,int) noex ;
extern int	u_read(int,void *,int) noex ;
extern int	u_readv(int,IOVEC *,int) noex ;
extern int	u_pread(int,void *,int,off_t) noex ;
extern int	u_write(int,cvoid *,int) noex ;
extern int	u_writev(int,const IOVEC *,int) noex ;
extern int	u_pwrite(int,cvoid *,int,off_t) noex ;

extern int	u_fpathconf(int,int,long *) noex ;
extern int	u_fstat(int,USTAT *) noex ;
extern int	u_fstatvfs(int,USTATVFS *) noex ;

extern int	u_fchown(int,uid_t,gid_t) noex ;
extern int	u_fcntl(int,int,...) noex ;
extern int	u_ioctl(int,int,...) noex ;
extern int	u_fchmod(int,mode_t) noex ;
extern int	u_rewind(int) noex ;
extern int	u_seek(int,off_t,int) noex ;
extern int	u_tell(int,off_t *) noex ;
extern int	u_seeko(int,off_t,int,off_t *) noex ;

extern int	u_bind(int,void *,int) noex ;
extern int	u_getsockname(int,void *,int *) noex ;
extern int	u_getpeername(int,void *,int *) noex ;
extern int	u_listen(int,int) noex ;
extern int	u_send(int,cvoid *,int,int) noex ;
extern int	u_sendto(int,cvoid *,int,int,void *,int) noex ;
extern int	u_sendmsg(int,MSGHDR *,int) noex ;
extern int	u_recv(int,void *,int,int) noex ;
extern int	u_recvfrom(int,void *,int,int,void *,int *) noex ;
extern int	u_recvmsg(int,MSGHDR *,int) noex ;
extern int	u_shutdown(int,int) noex ;
extern int	u_close(int) noex ;

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


