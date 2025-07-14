/* usys_xxx HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* miscelllaneous (XXX) operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in MacOS Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	usys_xxx

  	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

*******************************************************************************/

#ifndef	USYSXXX_INCLUDE
#define	USYSXXX_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>		/* |stat| */
#include	<sys/wait.h>		/* <- type |idtype_t| is there */
#include	<sys/time.h>		/* <- |TIMESPEC| is there */
#include	<unistd.h>		/* |dup2(2)| + |pipe2(2)| */
#include	<signal.h>		/* <- |SIGEVENT| */
#include	<pthread.h>
#include	<time.h>
#include	<string.h>		/* |strpbrk(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>

/*----------------------------------------------------------------------------*/
/* SECDB begin */
#if	(!defined(SYSHAS_SECDB)) || (SYSHAS_SECDB == 0)

#include	<secdb.h>		/* <- money shot (at least partly) */

#endif /* (!defined(SYSHAS_SECDB)) || (SYSHAS_SECDB == 0) */
/* SECDB end */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* USERATTR begin */
#if	(!defined(SYSHAS_USERATTR)) || (SYSHAS_USERATTR == 0)

#ifndef	STRUCT_USERATTR
#define	STRUCT_USERATTR	
#define	TYPEDEF_USERATTR
	typedef struct userattr_s {
		char   *name;
		char   *qualifier;
		char   *res1;
		char   *res2;
		kva_t  *attr;
	} userattr ;
#endif /* STRUCT_USERATTR */

#ifndef	SUBROUTINE_USERATTR
#define	SUBROUTINE_USERATTR
EXTERNC_begin

extern void setuserattr() noex ;
extern void enduserattr() noex ;
extern void free_userattr(userattr *) noex ;
extern userattr *getuserattr() noex ;
extern userattr *getusernam(const char *) noex ;
extern userattr *getuseruid(uid_t) noex ;

EXTERNC_end
#endif /* SUBROUTINE_USERATTR */

#endif /* (!defined(SYSHAS_USERATTR)) || (SYSHAS_USERATTR == 0) */
/* USERATTR end */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* PROJECT begin */
#if	defined(SYSHAS_PROJECT) && (SYSHAS_PROJECT > 0)
#else /* defined(SYSHAS_PROJECT) && (SYSHAS_PROJECT > 0) */

#ifndef	TYPEDEF_PROJID
#define	TYPEDEF_PROJID
typedef int	projid_t ;
#endif

#ifndef	SUBROUTINE_GETPROJID
#define	SUBROUTINE_GETPROJID
#ifdef	__cplusplus
extern "C" {
    extern projid_t getprojid() noex ;
}
#else
    extern projid_t getprojid() noex ;
#endif /* __cplusplus */
#endif /* SUBROUTINE_GETPROJID */

#endif /* defined(SYSHAS_PROJECT) && (SYSHAS_PROJECT > 0) */
/* PROJECT end */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* SYSV-MSG begin */

#ifndef	TYPEDEF_SYSVMSGTYPE
#define	TYPEDEF_SYSVMSGTYPE
typedef long		sysvmsgtype ;
#endif

#ifndef	MSQIDDS
#define	MSQIDDS		struct msqid_ds
#endif

/* SYSV-MSG end */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* MEMCNTL begin */
#if	(!defined(SYSHAS_MEMCNTL)) || (SYSHAS_MEMCNTL == 0)

#ifndef	SUBROUTINE_MEMCNTL
#define	SUBROUTINE_MEMCNTL
EXTERNC_begin

extern unixret_t memcntl(void *,size_t,int,void *,int,int) noex ;

EXTERNC_end
#endif /* SUBROUTINE_MEMCNTL */

#endif /* (!defined(SYSHAS_MEMCNTL)) || (SYSHAS_MEMCNTL == 0) */
/* MEMCNTL end */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* MEMPLOCK begin */
#if	(! defined(SYSHAS_MEMPLOCK)) || (SYSHAS_MEMPLOCK == 0)

enum syshasmemplock {
	syshasmemplock_locknon,
	syshasmemplock_lockall,
	syshasmemplock_locktxt,
	syshasmemplock_lockdat,
	syshasmemplock_verlast
} ;

#ifndef	UNLOCK
#define	UNLOCK		syshasmemplock_locknon
#endif

#ifndef	PROCLOCK
#define	PROCLOCK	syshasmemplock_lockall
#endif

#ifndef	TXTLOCK
#define	TXTLOCK		syshasmemplock_locktxt
#endif

#ifndef	DATLOCK
#define	DATLOCK		syshasmemplock_lockdat
#endif

#ifndef	SUBROUTINE_MEMPLOCK
#define	SUBROUTINE_MEMPLOCK
EXTERNC_begin

extern unixret_t plock(int) noex ;

EXTERNC_end
#endif /* SUBROUTINE_MEMPLOCK */

#endif /* (! defined(SYSHAS_MEMPLOCK)) || (SYSHAS_MEMPLOCK == 0) */
/* MEMPLOCK end */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* LOADAVGINT begin */
#if	(! defined(SYSHAS_LOADAVGINT)) || (SYSHAS_LOADAVGINT == 0)

#ifndef	SUBROUTINE_KLOADAVG
#define	SUBROUTINE_KLOADAVG
#ifdef	__cplusplus /* C++ only! */
namespace usys {
    extern unixret_t kloadavg(int *,int) noex ;
}
#endif /* __cplusplus (C++ only) */
#endif /* SUBROUTINE_KLOADAVG */

#endif /* (! defined(SYSHAS_LOADAVGINT)) || (SYSHAS_LOADAVGINT == 0) */
/* LOADAVGINT end */
/*----------------------------------------------------------------------------*/

/* all operating systems */
#ifndef	SUBROUTINE_STATFILE
#define	SUBROUTINE_STATFILE
EXTERNC_begin
extern int pipe2(int *,int) noex ;
static inline unixret_t statfile(cchar *fn,USTAT *sbp) noex {
    	return stat(fn,sbp) ;
}
static inline unixret_t statfilefs(cchar *fn,USTATFS *sbp) noex {
    	return statfs(fn,sbp) ;
}
static inline unixret_t statfilevfs(cchar *fn,USTATVFS *sbp) noex {
    	return statvfs(fn,sbp) ;
}
static inline unixret_t fstatfile(int fd,USTAT *sbp) noex {
    	return fstat(fd,sbp) ;
}
EXTERNC_end
#endif /* SUBROUTINE_STATFILE */

#ifndef	SUBROUTINE_DUPOVER
#define	SUBROUTINE_DUPOVER
EXTERNC_begin
static inline unixret_t dupover(int sfd,int dfd) noex {
    	return dup2(sfd,dfd) ;
}
EXTERNC_end
#endif /* SUBROUTINE_DUPOVER */

#ifndef	SUBROUTINE_PIPER
#define	SUBROUTINE_PIPER
EXTERNC_begin
static inline unixret_t piper(int *fds,int fl) noex {
    	return pipe2(fds,fl) ;
}
EXTERNC_end
#endif /* SUBROUTINE_PIPER */

#ifndef	DECLARATION_STRBRK
#define	DECLARATION_STRBRK
EXTERNC_begin
extern char *strbrk(cchar *,cchar *) noex ;
EXTERNC_end
#endif /* DECLARATION_STRBRK */


#endif /* USYSXXX_INCLUDE */


