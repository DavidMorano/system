/* uclibsubs HEADER */
/* lang=C20 */

/* version %I% last-modified %G% */
/* UNIX� C-language library (standard) subroutines */


/* revision history:

	= 1998-03-21, David A�D� Morano
	This module was originally written.

	= 2017-08-01, David A�D� Morano
	Updated for lack of interfaces in MacOS Darwin

*/

/* Copyright � 1998,2017 David A�D� Morano.  All rights reserved. */

#ifndef	UCLIBSUBS_INCLUDE
#define	UCLIBSUBS_INCLUDE


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
#include	<ucontext.h>

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
#include	<dirent.h>
#include	<netdb.h>
#include	<pwd.h>
#include	<grp.h>
#include	<secdb.h>

#if	defined(SYSHAS_XTI) && (SYSHAS_XTI > 0)
#include	<xti.h>
#endif

#include	<usys.h>	/* <- auxilllary OS support */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>

#include	<ucsys.h>
#include	<ucent.h>
#include	<ucget.h>
#include	<uclibmemalloc.h>
#include	<ucmemalloc.h>
#include	<ucmallocx.h>
#include	<ucstrtox.h>
#include	<ucopeninfo.h>
#include	<ucfdmanip.h>
#include	<ucuserattr.h>
#include	<uctimer.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int	uc_exit(int) noex ;
extern int	uc_atexit(void_f) noex ;
extern int	uc_atforkrecord(void_f,void_f,void_f) noex ;
extern int	uc_atforkexpunge(void_f,void_f,void_f) noex ;
extern int	uc_atfork(void_f,void_f,void_f) noex ;
extern int	uc_ftime(TIMEB *) noex ;
extern int	uc_clockset(clockid_t,const TIMESPEC *) noex ;
extern int	uc_clockget(clockid_t,TIMESPEC *) noex ;
extern int	uc_clockres(clockid_t,TIMESPEC *) noex ;
extern int	uc_gethostid(unsigned int *) noex ;
extern int	uc_gethostname(char *,int) noex ;
extern int	uc_gettimeofday(struct timeval *,void *) noex ;
extern int	uc_getloadavg(double *,int) noex ;
extern int	uc_nprocessors(int) noex ;
extern int	uc_syspages(int) noex ;
extern int	uc_nprocs(int) noex ;
extern int	uc_confsys(int,long *) noex ;
extern int	uc_confstr(char *,int,int) noex ;
extern int	uc_confmaxline() noex ;

#if	SYSHAS_TIMER && (SYSHAS_TIMER > 0)
extern int uc_timercreate(clockid_t,struct sigevent *,timer_t *) noex ;
extern int uc_timerdestroy(timer_t) noex ;
extern int uc_timerdelete(timer_t) noex ;
extern int uc_timerset(timer_t,int,ITIMERSPEC *,ITIMERSPEC *) noex ;
extern int uc_timerget(timer_t,ITIMERSPEC *) noex ;
extern int uc_timerover(timer_t) noex ;
#endif /* SYSHAS_TIMER */

extern int	uc_fork() noex ;
extern int	uc_forklockbegin(int) noex ;
extern int	uc_forklockend() noex ;
extern int	uc_execve(cchar *,cchar **,cchar **) noex ;
extern int	uc_isaexecve(cchar *,cchar **,cchar **) noex ;
extern int	uc_initgroups(cchar *,gid_t) noex ;

extern int	uc_getcwd(char *,int) noex ;
extern int	uc_swapcontext(ucontext_t *,const ucontext_t *) noex ;
extern int	uc_msync(caddr_t,size_t,int) noex ;
extern int	uc_getauid() noex ;
extern int	uc_getpriority(int,id_t,int *) noex ;
extern int	uc_setpriority(int,id_t,int) noex ;
extern int	uc_getpuid(pid_t) noex ;
extern int	uc_procpid(cchar *,uid_t) noex ;

extern int	uc_createfile(cchar *,mode_t) noex ;
extern int	uc_openpt(int) noex ;
extern int	uc_realpath(cchar *,char *) noex ;
extern int	uc_truncate(cchar *,offset_t) noex ;
extern int	uc_open(cchar *,int,mode_t) noex ;
extern int	uc_opene(cchar *,int,mode_t,int) noex ;
extern int	uc_openex(cchar *,int,mode_t,int,int) noex ;
extern int	uc_openenv(cchar *,int,mode_t,cchar **,int) noex ;
extern int	uc_openinfo(struct ucopeninfo *) noex ;
extern int	uc_create(cchar *,mode_t) noex ;
extern int	uc_fpassfd(int,int) noex ;
extern int	uc_ftruncate(int,offset_t) noex ;
extern int	uc_fstat(int,USTAT *) noex ;
extern int	uc_stat(cchar *,USTAT *) noex ;
extern int	uc_lstat(cchar *,USTAT *) noex ;
extern int	uc_readlink(cchar *,char *,int) noex ;
extern int	uc_pipe2(int *,int) noex ;
extern int	uc_chmod(cchar *,mode_t) noex ;
extern int	uc_chown(cchar *,uid_t,gid_t) noex ;
extern int	uc_rename(cchar *,cchar *) noex ;
extern int	uc_utime(cchar *,const struct utimbuf *) noex ;

extern int	uc_isatty(int) noex ;
extern int	uc_fsync(int) noex ;
extern int	uc_fdatasync(int) noex ;
extern int	uc_fattach(int,cchar *) noex ;
extern int	uc_fdetach(cchar *) noex ;
extern int	uc_minmod(cchar *,mode_t) noex ;
extern int	uc_fminmod(int,mode_t) noex ;
extern int	uc_fsize(int) noex ;
extern int	uc_fuid(int) noex ;
extern int	uc_fgid(int) noex ;

extern int	uc_opensocket(cchar *,int,int) noex ;
extern int	uc_openproto(cchar *,int,int,int) noex ;
extern int	uc_openpass(cchar *,int,int,int) noex ;
extern int	uc_openuser(cchar *,cchar *,int,mode_t,int) noex ;
extern int	uc_openuserinfo(struct ucopeninfo *) noex ;
extern int	uc_openprog(cchar *,int,
			cchar **,cchar **) noex ;
extern int	uc_opendialer(cchar *,cchar *,int,mode_t,
			cchar **,cchar **,int) noex ;
extern int	uc_openfsvc(cchar *,cchar *,cchar *,int,mode_t,
			cchar **,cchar **,int) noex ;
extern int	uc_openusvc(cchar *,cchar *,cchar *,int,mode_t,
			cchar **,cchar **,int) noex ;
extern int	uc_openfint(cchar *,cchar *,cchar *,
			cchar *, cchar *, int,mode_t,
			cchar **,cchar **,int) noex ;
extern int	uc_opensys(cchar *,int,mode_t,cchar **,int,int) noex ;
extern int	uc_opendev(cchar *,int,mode_t,cchar **,int,int) noex ;

extern int	uc_accepte(int,cvoid *,int *,int) noex ;
extern int	uc_connecte(int,cvoid *,int,int) noex ;

extern int	uc_copy(int,int,int) noex ;

extern int	uc_readn(int,void *,int) noex ;
extern int	uc_readline(int,char *,int) noex ;
extern int	uc_readlinetimed(int,char *,int,int) noex ;

extern int	uc_write(int,cvoid *,int,int) noex ;
extern int	uc_writen(int,cvoid *,int) noex ;
extern int	uc_writedesc(int,int,int) noex ;
extern int	uc_writefile(int,cchar *) noex ;

extern int	uc_reade(int,void *,int,int,int) noex ;
extern int	uc_recve(int,void *,int,int,int,int) noex ;
extern int	uc_recvfrome(int,void *,int,int,void *,int *,int,int) noex ;
extern int	uc_recvmsge(int,struct msghdr *,int,int,int) noex ;
extern int	uc_sockatmark(int) noex ;
extern int	uc_peek(int,void *,int) noex ;
extern int	uc_getsocktype(int) noex ;

extern int	uc_lockf(int,int,offset_t) noex ;
extern int	uc_lockfile(int,int,offset_t,offset_t,int) noex ;
extern int	uc_tcsetattr(int,int,struct termios *) noex ;
extern int	uc_tcgetattr(int,struct termios *) noex ;
extern int	uc_tcgetsid(int) noex ;
extern int	uc_tcsetpgrp(int,pid_t) noex ;
extern int	uc_tcgetpgrp(int) noex ;
extern int	uc_keepalive(int,int) noex ;
extern int	uc_reuseaddr(int) noex ;
extern int	uc_moveup(int,int) noex ;
extern int	uc_ndelay(int,int) noex ;
extern int	uc_nonblock(int,int) noex ;
extern int	uc_msgdiscard(int) noex ;
extern int	uc_setappend(int,int) noex ;
extern int	uc_closeonexec(int,int) noex ;
extern int	uc_close(int) noex ;

extern int	uc_remove(cchar *) noex ;
extern int	uc_link(cchar *,cchar *) noex ;
extern int	uc_symlink(cchar *,cchar *) noex ;
extern int	uc_unlink(cchar *) noex ;
extern int	uc_lockend(int,int,int,int) noex ;
extern int	uc_mkdir(cchar *,mode_t) noex ;
extern int	uc_rmdir(cchar *) noex ;
extern int	uc_access(cchar *,int) noex ;
extern int	uc_pathconf(cchar *,int,long *) noex ;

extern int	uc_raise(int) noex ;
extern int	uc_sigdefault(int) noex ;
extern int	uc_sigignore(int) noex ;
extern int	uc_sighold(int) noex ;
extern int	uc_sigrelease(int) noex ;
extern int	uc_sigpause(int) noex ;
extern int	uc_sigsetempty(sigset_t *) noex ;
extern int	uc_sigsetfill(sigset_t *) noex ;
extern int	uc_sigsetadd(sigset_t *,int) noex ;
extern int	uc_sigsetdel(sigset_t *,int) noex ;
extern int	uc_sigsetismem(sigset_t *,int) noex ;

/* special */
extern int	uc_sigqueue(pid_t,int,const union sigval) noex ;
extern int	uc_sigwaitinfo(const sigset_t *,siginfo_t *) noex ;
extern int	uc_sigtimedwait(const sigset_t *,siginfo_t *,
			const TIMESPEC *) noex ;
extern int	uc_sigwaitinfoto(const sigset_t *,siginfo_t *,
			const TIMESPEC *) noex ;
/* special */

extern int	uc_safesleep(int) noex ;
extern int	uc_mktime(struct tm *,time_t *) noex ;
extern int	uc_gmtime(const time_t *,struct tm *) noex ;
extern int	uc_localtime(const time_t *,struct tm *) noex ;
extern int	uc_ttyname(int,char *,int) noex ;
extern int	uc_mkfifo(cchar *,mode_t) noex ;
extern int	uc_piper(int *,int) noex ;

extern int	uc_plock(int) noex ;
extern int	uc_mlock(cvoid *,size_t) noex ;
extern int	uc_munlock(cvoid *,size_t) noex ;
extern int	uc_mlockall(int) noex ;
extern int	uc_munlockall() noex ;
extern int	uc_madvise(cvoid *,size_t,int) noex ;

extern int	uc_getrandom(void *,int,int) noex ;
extern int	uc_getentropy(void *,int) noex ;

/* memory allocation (user-space) */

#ifdef	COMMENT
extern int	uc_malloc(int,void *) noex ;
extern int	uc_calloc(int,int,void *) noex ;
extern int	uc_valloc(int,void *) noex ;
extern int	uc_realloc(cvoid *,int,void *) noex ;
extern int	uc_free(cvoid *) noex ;
extern int	uc_mallset(int) noex ;
extern int	uc_mallout(ulong *) noex ;
extern int	uc_mallinfo(unsigned int *,int) noex ;
extern int	uc_mallpresent(cvoid *) noex ;
extern int	uc_mallocstrw(cchar *,int,cchar **) noex ;
extern int	uc_mallocsys(int,char **) noex ;
extern int	uc_mallocbuf(cvoid *,int,cvoid **) noex ;
#endif /* COMMENT */

/* OS PASSWD database */
extern int	uc_getpwbegin() noex ;
extern int	uc_getpwent(ucentpw *,char *,int) noex ;
extern int	uc_getpwnam(ucentpw *,char *,int,cchar *) noex ;
extern int	uc_getpwgid(ucentpw *,char *,int,uid_t) noex ;
extern int	uc_getpwend() noex ;

/* OS SHADOW database */
extern int	uc_getspbegin() noex ;
extern int	uc_getspent(ucentsp *,char *,int) noex ;
extern int	uc_getspnam(ucentsp *,char *,int,cchar *) noex ;
extern int	uc_getspend() noex ;

/* OS GROUP database */
extern int	uc_getgrbegin() noex ;
extern int	uc_getgrent(ucentgr *,char *,int) noex ;
extern int	uc_getgrnam(ucentgr *,char *,int,cchar *) noex ;
extern int	uc_getgrgid(ucentgr *,char *,int,gid_t) noex ;
extern int	uc_getgrpend() noex ;

/* OS PROJECT database */
extern int	uc_getpjbegin() noex ;
extern int	uc_getpjent(ucentpj *,char *,int) noex ;
extern int	uc_getpjbyid(ucentpj *,char *,int,projid_t) noex ;
extern int	uc_getpjbyname(ucentpj *,char *,int,cchar *) noex ;
extern int	uc_getdefaultproj(ucentpj *,char *,int,cchar ) noex ;
extern int	uc_getpjend() noex ;
extern int	uc_inproj(cchar *,cchar *,char *,int) noex ;

/* POSIX shared memory operations */
extern int	uc_openshm(cchar *,int,mode_t) noex ;
extern int	uc_openshmto(cchar *,int,mode_t,int) noex ;
extern int	uc_unlinkshm(cchar *) noex ;

/* miscellaneous */
extern int	uc_inetnetpton(void *,int,int,cchar *) noex ;
extern int	uc_inetpton(void *,int,cchar *) noex ;
extern int	uc_inetntop(char *,int,int,cvoid *) noex ;
extern int	uc_strtod(cchar *,char **,double *) noex ;
extern int	uc_getnetname(char *) noex ;

/* NETWORK PROTOCOL database */
extern int	uc_setprotoent(int) noex ;
extern int	uc_endprotoent() noex ;
extern int	uc_getprotoent(PROTOENT *,char *,int) noex ;
extern int	uc_getprotobyname(PROTOENT *,char *,int, cchar *) noex ;
extern int	uc_getprotobynumber(PROTOENT *,char *,int,int) noex ;

/* NETWORK NET database */
extern int	uc_setnetent(int) noex ;
extern int	uc_endnetent() noex ;
extern int	uc_getnetent(NETENT *,char *,int) noex ;
extern int	uc_getnetbyname(NETENT *,char *,int,cchar *) noex ;
extern int	uc_getnetbyaddr(NETENT *,char *,int,long,int) noex ;

/* NETWORK HOST database */
extern int	uc_sethostent(int) noex ;
extern int	uc_endhostent() noex ;
extern int	uc_gethostent(HOSTENT *,char *,int) noex ;
extern int	uc_gethostbyname(HOSTENT *,char *,int,cchar *) noex ;
extern int	uc_gethostbyaddr(HOSTENT *,char *,int,cchar *,int,int) noex ;

/* NETWORK SERVICE database */
extern int	uc_setservent(int) noex ;
extern int	uc_endservent() noex ;
extern int	uc_getservent(SERVENT *,char *,int) noex ;
extern int	uc_getservbyname(SERVENT *,char *,int,cchar *,cchar *) noex ;
extern int	uc_getservbyport(SERVENT *,char *,int,int,cchar *) noex ;

/* NETWORK IPNODE database */
extern int	uc_getipnodebyname(HOSTENT **,cchar *,int,int) noex ;
extern int	uc_freehostent(HOSTENT *) noex ;

/* NETWORK ADDRINFO database */
extern int uc_getaddrinfo(cchar *,cchar *,const ADDRINFO *,ADDRINFO **) noex ;
extern int uc_freeaddrinfo(ADDRINFO *) noex ;

/* NETWORK SOCKADDR combined database */
extern int	uc_getnameinfo(const SOCKADDR *,int,
			char *,int,char *,int,int) noex ;

/* USER-SHELL database */
extern int	uc_getusbegin() noex ;
extern int	uc_getusent(char *,int) noex ;
extern int	uc_getusend() noex ;

/* miscellaneous */
extern int	uc_ztime(struct tm *,const time_t *,int) noex ;
extern int	uc_kvamatch(kva_t *,cchar *,cchar **) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* UCLIBSUBS_INCLUDE */


