/* usys_xxx HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* miscelllaneous (XXX) operating system support */
/* version %I% last-modified %G% */

#define	CF_STRTOX	0		/* enable compilation */

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
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX for |statfs(2)| */
#include	<sys/stat.h>		/* POSIX |stat| */
#include	<sys/statvfs.h>		/* POSIX */
#include	<sys/wait.h>		/* POSIX <- type |idtype_t| is there */
#include	<sys/time.h>		/* POSIX <- |TIMESPEC| is there */
#include	<sys/mount.h>		/* POSIX for |statfs(2)| */
#include	<unistd.h>		/* POSIX |dup2(2)| + |pipe2(2)| */
#include	<signal.h>		/* POSIX <- |SIGEVENT| */
#include	<pthread.h>		/* POSIX */
#include	<string.h>		/* CSTD |strpbrk(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


#ifndef	CF_STRTOX
#define	CF_STRTOX	0		/* enable compilation */
#endif


/*----------------------------------------------------------------------------*/
/* SECDB begin */
#if	(!defined(SYSHAS_SECDB)) || (SYSHAS_SECDB == 0)

#include	<secdb.h>		/* <- money shot (at least partly) */

#endif /* (!defined(SYSHAS_SECDB)) || (SYSHAS_SECDB == 0) */
/* SECDB end */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* USERATTR begin */
#if	defined(SYSHAS_USERATTR) && (SYSHAS_USERATTR > 0)


/* these systems should already somehow include 'secdb.h" from LIBSECDB */

/* but it cannot hurt to do it again! */
#include		<secdb.h>


#else /* defined(SYSHAS_USERATTR) && (SYSHAS_USERATTR > 0) */

/* this is placed locally on purpose (to get something moving) */
#include		<secdb.h>

#ifndef	STRUCT_USERATTR
#define	STRUCT_USERATTR	
#define	TYPEDEF_USERATTR
typedef struct userattr_s {
	char   *name ;
	char   *qualifier ;
	char   *res1 ;
	char   *res2 ;
	kva_t  *attr ;
} userattr ;
#endif /* STRUCT_USERATTR */

#ifndef	DECLARATION_USERATTR
#define	DECLARATION_USERATTR
EXTERNC_begin

extern void setuserattr() noex ;
extern void enduserattr() noex ;
extern void free_userattr(userattr *) noex ;
extern userattr *getuserattr() noex ;
extern userattr *getusernam(const char *) noex ;
extern userattr *getuseruid(uid_t) noex ;

EXTERNC_end
#endif /* DECLARATION_USERATTR */

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

#ifndef	DECLARATION_GETPROJID
#define	DECLARATION_GETPROJID
#ifdef	__cplusplus
extern "C" {
    extern projid_t getprojid() noex ;
}
#else
    extern projid_t getprojid() noex ;
#endif /* __cplusplus */
#endif /* DECLARATION_GETPROJID */

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

#ifndef	DECLARATION_MEMCNTL
#define	DECLARATION_MEMCNTL
EXTERNC_begin

extern unixret_t memcntl(void *,size_t,int,void *,int,int) noex ;

EXTERNC_end
#endif /* DECLARATION_MEMCNTL */

#endif /* (!defined(SYSHAS_MEMCNTL)) || (SYSHAS_MEMCNTL == 0) */
/* MEMCNTL end */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* MEMINHERIT begin */
#if	(!defined(SYSHAS_MEMINHERIT)) || (SYSHAS_MEMINHERIT == 0)

#ifndef	DECLARATION_MEMINHERIT
#define	DECLARATION_MEMINHERIT
EXTERNC_begin

extern unixret_t minherit(void *,size_t,int) noex ;

EXTERNC_end
#endif /* DECLARATION_MEMINHERIT */

#endif /* (!defined(SYSHAS_MEMINHERIT)) || (SYSHAS_MEMINHERIT == 0) */
/* MEMINHERIT end */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* MEMPLOCK begin */
#if	(! defined(SYSHAS_MEMPLOCK)) || (SYSHAS_MEMPLOCK == 0)

enum syshasmemplock {
	syshasmemplock_locknon,
	syshasmemplock_lockall,
	syshasmemplock_locktxt,
	syshasmemplock_lockdat,
	syshasmemplock_overlast
} ; /* end enum (syshasmemplock) */

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

#ifndef	DECLARATION_MEMPLOCK
#define	DECLARATION_MEMPLOCK
EXTERNC_begin

extern unixret_t plock(int) noex ;

EXTERNC_end
#endif /* DECLARATION_MEMPLOCK */

#endif /* (! defined(SYSHAS_MEMPLOCK)) || (SYSHAS_MEMPLOCK == 0) */
/* MEMPLOCK end */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* LOADAVGINT begin */
#if	(! defined(SYSHAS_LOADAVGINT)) || (SYSHAS_LOADAVGINT == 0)

#ifndef	DECLARATION_KLOADAVG
#define	DECLARATION_KLOADAVG
#ifdef	__cplusplus /* C++ only! */
namespace usys {
    extern sysret_t kloadavg(int *,int) noex ;
}
#endif /* __cplusplus (C++ only) */
#endif /* DECLARATION_KLOADAVG */

#endif /* (! defined(SYSHAS_LOADAVGINT)) || (SYSHAS_LOADAVGINT == 0) */
/* LOADAVGINT end */
/*----------------------------------------------------------------------------*/

/* all operating systems */
#ifndef	SUBROUTINE_STATFILE
#define	SUBROUTINE_STATFILE
EXTERNC_begin
local inline unixret_t statfile(cchar *fn,USTAT *sbp) noex {
    	return stat(fn,sbp) ;
}
local inline unixret_t statfilefs(cchar *fn,USTATFS *sbp) noex {
    	return statfs(fn,sbp) ;
}
local inline unixret_t statfilevfs(cchar *fn,USTATVFS *sbp) noex {
    	return statvfs(fn,sbp) ;
}
local inline unixret_t fstatfile(int fd,USTAT *sbp) noex {
    	return fstat(fd,sbp) ;
}
local inline unixret_t fstatfilevfs(int fd,USTATVFS *sbp) noex {
    	return fstatvfs(fd,sbp) ;
}
EXTERNC_end
#endif /* SUBROUTINE_STATFILE */

#ifndef	SUBROUTINE_DUPOVER
#define	SUBROUTINE_DUPOVER
EXTERNC_begin
local inline unixret_t dupover(int sfd,int dfd) noex {
    	return dup2(sfd,dfd) ;
}
EXTERNC_end
#endif /* SUBROUTINE_DUPOVER */

#ifndef	SUBRUOTINE_STRBRK
#define	SUBROUTINE_STRBRK
EXTERNC_begin
local inline char *strbrk(cchar *sp,cchar *sc) noex {
    	return strpbrk(sp,sc) ;
}
EXTERNC_end
#endif /* SUBROUTINE_STRBRK */

/* this ( |strochr(3c)| ) is the companion subroutine of |strrchr(3c)| */
#ifndef	SUBROUTINE_STROCHR
#define	SUBROUTINE_STROCHR
EXTERNC_begin
local inline char *strochr(cchar *sp,int sch) noex {
    	return strchr(sp,sch) ;
}
EXTERNC_end
#endif /* SUBROUTINE_STROCHR */

#if	CF_STRTOX
#ifndef	DECLARATION_STRTOX
#define	DECLARATION_STRTOX
EXTERNC_begin
extern sint	strtoi(cchar *,char **,int) noex ;
extern uint	strtoui(cchar *,char **,int) noex ;
EXTERNC_end
#endif /* DECLARATION_STRTOX */
#endif /* CF_STRTOX */


#endif /* USYSXXX_INCLUDE */


