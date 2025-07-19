/* utypedefs HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* virtual-system definitions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in MacOS Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

#ifndef	UTYPEDEFS_INCLUDE
#define	UTYPEDEFS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<sys/utsname.h>
#include	<sys/uio.h>
#include	<sys/time.h>		/* for |u_adjtime(3u)| */
#include	<sys/timeb.h>		/* for |uc_ftime(3uc)| */
#include	<sys/resource.h>
#include	<sys/stat.h>		/* |USTAT| */
#include	<sys/statvfs.h>
#include	<sys/socket.h>

#include	<arpa/inet.h>		/* <- |in_addr_t| */

#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<time.h>		/* for |u_utime(2)| */
#include	<utime.h>		/* for |u_utime(2)| */
#include	<pthread.h>
#include	<errno.h>
#include	<dirent.h>
#include	<ucontext.h>
#include	<netdb.h>
#include	<limits.h>
#include	<signal.h>
#include	<stddef.h>		/* |wchar_t| */
#include	<stdlib.h>
#include	<stdint.h>		/* |intptr_t| + |uintptr_t| */

#include	<clanguage.h>		/* <- necessary inclusion */


/* for |stat(2)| and its many friends */
#ifndef	STRUCT_USTAT
#define	STRUCT_USTAT
#define	USTAT		struct stat
#endif /* STRUCT_USTAT */

/* for |stat(2)| and its many friends */
#ifndef	STRUCT_USTATFS
#define	STRUCT_USTATFS
#define	USTATFS		struct statfs
#endif /* STRUCT_USTAT */

/* for |statvfs(2)| and its many friends */
#ifndef	STRUCT_USTATVFS
#define	STRUCT_USTATVFS
#define	USTATVFS	struct statvfs
#endif /* STRUCT_USTATVFS */

/* for |poll(2)| and its many friends */
#ifndef	STRUCT_UPOLLFD
#define	STRUCT_UPOLLFD
#define	UPOLLFD		struct pollfd
#endif /* STRUCT_USTAT */


/* PREDEFINED start */
/* determine if some unsigned-related typedefs have already been made */
/* the following unsigned typedefs are supposedly "System V" compatibility */
#if	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)
#if !defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE)

#ifndef	TYPEDEF_USHORT
#define	TYPEDEF_USHORT
/* already typedef'ed on Darwin */
#endif

#ifndef	TYPEDEF_UINT
#define	TYPEDEF_UINT
/* already typedef'ed on Darwin */
#endif

#endif /* !defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE) */
#endif /* defined(OSNAME_Darwin) && (OSNAME_Darwin > 0) */
/* PREDEFINED end */

/* PREDEFINED start */
/* determine if some unsigned-related typedefs have already been made */
/* the following unsigned typedefs are supposedly "System V" compatibility */
#if	defined(OSNAME_SunOS) && (OSNAME_SunOS > 0)
#if !defined(__XOPEN_OR_POSIX) || defined(__EXTENSIONS__)

#ifndef	TYPEDEF_USHORT
#define	TYPEDEF_USHORT
/* already typedef'ed on Solaris® */
#endif

#ifndef	TYPEDEF_UINT
#define	TYPEDEF_UINT
/* already typedef'ed on Solaris® */
#endif

#ifndef	TYPEDEF_ULONG
#define	TYPEDEF_ULONG
/* already typedef'ed on Solaris® */
#endif

#endif /* !defined(__XOPEN_OR_POSIX) || defined(__EXTENSIONS__) */
#endif /* defined(OSNAME_SunOS) && (OSNAME_SunOS > 0) */
/* PREDEFINED end */

/* types */

#ifndef	TYPEDEF_INTOFFT
#define	TYPEDEF_INTOFFT
typedef int			intoff_t ;
#endif /* TYPEDEF_INTOFFT */

#ifndef	TYPEDEF_NOTHROWT
#define	TYPEDEF_NOTHROWT
#ifdef	__cplusplus
#include			<new>		/* |nothrow(3c++)| */
typedef decltype(std::nothrow)	nothrow_t ;
#endif /* __cplusplus */
#endif /* TYPEDEF_NOTHROWT */

/* handle some really brain-damaged systems -- like MacOS-X Darwin®! */
#if	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)
#if	defined(OSNUM) && (OSNUM <= 7)
#ifndef	TYPEDEF_IDT
#define	TYPEDEF_IDT
typedef int			id_t ;
#endif /* TYPEDEF_IDT */
#endif
#endif

#ifndef	TYPEDEF_IN4ADDRTT
#define	TYPEDEF_IN4ADDRTT
typedef in_addr_t		in4_addr_t ;		/* scalar type */
#endif

/* some OSes (which remain nameless but has initials "Linux") do not have */
#if	defined(SYSHAS_TYPEIN6ADDRT) && (SYSHAS_TYPEIN6ADDRT == 0)
#ifndef	TYPEDEF_IN6ADDRTT
#define	TYPEDEF_IN6ADDRTT
typedef struct in6_addr		in6_addr_t ;
#endif
#endif /* defined(SYSHAS_TYPEIN6ADDRT) && (SYSHAS_TYPEIN6ADDRT == 0) */

#ifndef	TYPEDEF_ERRNOT
#define	TYPEDEF_ERRNOT
typedef int			errno_t ;
#endif /* TYPEDEF_ERRNOT */

#ifndef	TYPEDEF_UNIXRETT
#define	TYPEDEF_UNIXRETT
typedef int			unixret_t ;
#endif /* TYPEDEF_UNIXRETT */

#ifndef	TYPEDEF_SYSRETT
#define	TYPEDEF_SYSRETT
typedef int			sysret_t ;
#endif /* TYPEDEF_SYSRETT */

#ifndef	TYPEDEF_CERRNOT
#define	TYPEDEF_CERRNOT
typedef const errno_t		cerrno_t ;
#endif /* TYPEDEF_CERRNOY */

#ifndef	TYPEDEF_CUNIXRETT
#define	TYPEDEF_CUNIXRETT
typedef const unixret_t		cunixret_t ;
#endif /* TYPEDEF_CUNIXRETT */

#ifndef	TYPEDEF_CSYSRETT
#define	TYPEDEF_CSYSRETT
typedef const sysret_t		csysret_t ;
#endif /* TYPEDEF_CSYSRETT */

/* this next type-def is related to the one afterwards */
#ifndef	TYPEDEF_SIGF
#define	TYPEDEF_SIGF
#if	(! defined(SYSHAS_TYPESIGF)) || (SYSHAS_TYPESIGF == 0)
EXTERNC_begin
typedef void (*sig_f)(int) noex ;
EXTERNC_end
#endif /* syshas */
#endif /* TYPEDEF_SIGF */

/****
The following is declared in the Apple-Darwin operating sytem.  So
in order to be portable everywhere else, we have to declared this
everywhere else also.  This is done so that the following type-def
is not accidentally declared elsewhere and used for a purpose other
than what Apple-Darwin is using it for.
****/
#ifndef	TYPEDEF_SIGT
#define	TYPEDEF_SIGT
#if	(! defined(SYSHAS_TYPESIGT)) || (SYSHAS_TYPESIGT == 0)
EXTERNC_begin
typedef void (*sig_t)(int) noex ;
EXTERNC_end
#endif /* syshas */
#endif /* TYPEDEF_SIGT */

#ifndef	TYPEDEF_VOIDF
#define	TYPEDEF_VOIDF
EXTERNC_begin
typedef void (*void_f)() noex ;
EXTERNC_end
#endif /* TYPEDEF_VOIDF */

#ifndef	TYPEDEF_SORTVCMP
#define	TYPEDEF_SORTVCMP
EXTERNC_begin
typedef int (*sort_vcmp)(const void *,const void *) noex ;
typedef int (*sortvcmp_f)(const void *,const void *) noex ;
EXTERNC_end
#endif /* TYPEDEF_SORTVCMP */

#ifndef	TYPEDEF_SCHAR
#define	TYPEDEF_SCHAR
typedef signed char		schar ;
#endif /* TYPEDEF_SCHAR */

#ifndef	TYPEDEF_UCHAR
#define	TYPEDEF_UCHAR
typedef unsigned char		uchar ;
#endif /* TYPEDEF_UCHAR */

#ifndef	TYPEDEF_UINT
#define	TYPEDEF_UINT
typedef unsigned int		uint ;
#endif /* TYPEDEF_UINT */

#ifndef	TYPEDEF_ULONG
#define	TYPEDEF_ULONG
typedef unsigned long		ulong ;
#endif /* TYPEDEF_ULONG */

#ifndef	TYPEDEF_LONGDOUBLE
#define	TYPEDEF_LONGDOUBLE
typedef long double		longdouble ;
#endif /* TYPEDEF_LONGDOUBLE */

/* contstant versions */

#ifndef	TYPEDEF_CBOOL
#define	TYPEDEF_CBOOL
typedef const bool		cbool ;
#endif

#ifndef	TYPEDEF_CCHAR
#define	TYPEDEF_CCHAR
typedef const char		cchar ;
#endif

#ifndef	TYPEDEF_CSCHAR
#define	TYPEDEF_CSCHAR
typedef const signed char	cschar ;
#endif /* TYPEDEF_SCHAR */

#ifndef	TYPEDEF_CSHORT
#define	TYPEDEF_CSHORT
typedef const short		cshort ;
#endif /* TYPEDEF_CSHORT */

#ifndef	TYPEDEF_CINT
#define	TYPEDEF_CINT
typedef const int		cint ;
#endif /* TYPEDEF_CINT */

#ifndef	TYPEDEF_CLONG
#define	TYPEDEF_CLONG
typedef const long		clong ;
#endif /* TYPEDEF_CULONG */

#ifndef	TYPEDEF_CFLOAT
#define	TYPEDEF_CFLOAT
typedef const float		cfloat ;
#endif /* TYPEDEF_CFLOAT */

#ifndef	TYPEDEF_CDOUBLE
#define	TYPEDEF_CDOUBLE
typedef const double		cdouble ;
#endif /* TYPEDEF_CDOUBLE */

#ifndef	TYPEDEF_CLONGDOUBLE
#define	TYPEDEF_CLONGDOUBLE
typedef const long double	clongdouble ;
#endif /* TYPEDEF_CLONGDOUBLE */

#ifndef	TYPEDEF_CVOID
#define	TYPEDEF_CVOID
typedef const void		cvoid ;
#endif /* TYPEDEF_CVOID */

#ifndef	TYPEDEF_CSIZE
#define	TYPEDEF_CSIZE
typedef const size_t		csize ;
#endif /* TYPEDEF_CSIZE */

#ifndef	TYPEDEF_COFF
#define	TYPEDEF_COFF
typedef const off_t		coff ;
#endif /* TYPEDEF_COFF */

#ifndef	TYPEDEF_CINTPTR
#define	TYPEDEF_CINTPTR
typedef const intptr_t		cintptr ;
#endif /* TYPEDEF_CINTPTR */

#ifndef	TYPEDEF_CUINTPTR
#define	TYPEDEF_CUINTPTR
typedef const uintptr_t		cuintptr ;
#endif /* TYPEDEF_CUINTPTR */

#ifndef	TYPEDEF_CNFDS
#define	TYPEDEF_CBFDS
typedef const nfds_t		cnfds ;
#endif

#ifndef	TYPEDEF_CSOCKLEN
#define	TYPEDEF_CSOCKLEN
typedef const socklen_t		csocklen ;
#endif /* TYPEDEF_CSOCKLEN */

#ifndef	TYPEDEF_CMODE
#define	TYPEDEF_CMODE
typedef const mode_t		cmode ;
#endif /* TYPEDEF_CMODE */

#ifndef	TYPEDEF_CUCHAR
#define	TYPEDEF_CUCHAR
typedef const unsigned char	cuchar ;
#endif /* TYPEDEF_UCHAR */

#ifndef	TYPEDEF_CUSHORT
#define	TYPEDEF_CUSHORT
typedef const unsigned short	cushort ;
#endif /* TYPEDEF_UCHAR */

#ifndef	TYPEDEF_CUINT
#define	TYPEDEF_CUINT
typedef const unsigned int	cuint ;
#endif /* TYPEDEF_CUINT */

#ifndef	TYPEDEF_CULONG
#define	TYPEDEF_CULONG
typedef const unsigned long	culong ;
#endif /* TYPEDEF_CULONG */

/* specials */

#ifndef	TYPEDEF_CC
#define	TYPEDEF_CC
typedef const char		cc ;
#endif

#ifndef	TYPEDEF_VOIDP
#define	TYPEDEF_VOIDP
typedef void *			voidp ;
#endif

#ifndef	TYPEDEF_VOIDPP
#define	TYPEDEF_VOIDPP
typedef void **			voidpp ;
#endif

#ifndef	TYPEDEF_CCP
#define	TYPEDEF_CCP
typedef const char *		ccp ;
#endif

#ifndef	TYPEDEF_CVOIDP
#define	TYPEDEF_CVOIDP
typedef const void *		cvoidp ;
#endif

#ifndef	TYPEDEF_CPCVOID
#define	TYPEDEF_CPCVOID
typedef const void *const	cpcvoid ;
#endif

#ifndef	TYPEDEF_CVOIDPP
#define	TYPEDEF_CVOIDPP
typedef const void **		cvoidpp ;
#endif

#ifndef	TYPEDEF_CNULLPTR
#define	TYPEDEF_CNULLPTR
typedef const nullptr_t		cnullptr ;
#endif

#ifndef	TYPEDEF_CNULLOPT
#define	TYPEDEF_CNULLOPT
#ifdef	__cplusplus
#include	<optional>
typedef const std::nullopt_t	cnullopt ;
#endif /* __cplusplus */
#endif

#ifndef	TYPEDEF_CNOTHROW
#define	TYPEDEF_CNOTHROW
#ifdef	__cplusplus
#include			<new>		/* |nothrow(3c++)| */
typedef const nothrow_t		cnothrow ;
#endif /* __cplusplus */
#endif

#ifndef	TYPEDEF_WCHARP
#define	TYPEDEF_WCHARP
typedef wchar_t *		wcharp ;
#endif

#ifndef	TYPEDEF_CHARP
#define	TYPEDEF_CHARP
typedef char *			charp ;
#endif

#ifndef	TYPEDEF_SHORTP
#define	TYPEDEF_SHORTP
typedef short *			shortp ;
#endif

#ifndef	TYPEDEF_INTP
#define	TYPEDEF_INTP
typedef int *			intp ;
#endif

#ifndef	TYPEDEF_LONGP
#define	TYPEDEF_LONGP
typedef long *			longp ;
#endif

#ifndef	TYPEDEF_UCHARP
#define	TYPEDEF_UCHARP
typedef unsigned char *		ucharp ;
#endif

#ifndef	TYPEDEF_USHORTP
#define	TYPEDEF_USHORTP
typedef unsigned short *	ushortp ;
#endif

#ifndef	TYPEDEF_UINTP
#define	TYPEDEF_UINTP
typedef unsigned int *		uintp ;
#endif

#ifndef	TYPEDEF_ULONGP
#define	TYPEDEF_ULONGP
typedef unsigned long *		ulongp ;
#endif

#ifndef	TYPEDEF_CHARPP
#define	TYPEDEF_CHARPP
typedef char **			charpp ;
#endif

/* usigned (pointer-to-pointer-to) */
#ifndef	TYPEDEF_UCHARPP
#define	TYPEDEF_UCHARPP
typedef unsigned char **	ucharpp ;
#endif

#ifndef	TYPEDEF_USHORTPP
#define	TYPEDEF_USHORTPP
typedef unsigned short **	ushortpp ;
#endif

#ifndef	TYPEDEF_UINTPP
#define	TYPEDEF_UINTPP
typedef unsigned int **		uintpp ;
#endif

#ifndef	TYPEDEF_ULONGPP
#define	TYPEDEF_ULONGPP
typedef unsigned long **	ulongpp ;
#endif

/* constant (pointer-to) */
#ifndef	TYPEDEF_CCHARP
#define	TYPEDEF_CCHARP
typedef const char *		ccharp ;
#endif

#ifndef	TYPEDEF_CSHORTP
#define	TYPEDEF_CSHORTP
typedef const short *		cshortp ;
#endif

#ifndef	TYPEDEF_CINTP
#define	TYPEDEF_CINTP
typedef const int *		cintp ;
#endif

#ifndef	TYPEDEF_CLONGP
#define	TYPEDEF_CLONGP
typedef const long *		clongp ;
#endif

/* constant (pointer-to-unsigned) */
#ifndef	TYPEDEF_CUCHARP
#define	TYPEDEF_CUCHARP
typedef const unsigned char *	cucharp ;
#endif

#ifndef	TYPEDEF_CUSHORTP
#define	TYPEDEF_CUSHORTP
typedef const unsigned short *	cushortp ;
#endif

#ifndef	TYPEDEF_CUINTP
#define	TYPEDEF_CUINTP
typedef const unsigned int *	cuintp ;
#endif

#ifndef	TYPEDEF_CULONGP
#define	TYPEDEF_CULONGP
typedef const unsigned long *	culongp ;
#endif

/* special types for characters */
#ifndef	TYPEDEF_CCHARPP
#define	TYPEDEF_CCHARPP
typedef const char **		ccharpp ;
#endif

#ifndef	TYPEDEF_CSHORTPP
#define	TYPEDEF_CSHORTPP
typedef const short **		cshortpp ;
#endif

#ifndef	TYPEDEF_CINTPP
#define	TYPEDEF_CINTPP
typedef const int **		cintpp ;
#endif

#ifndef	TYPEDEF_CLONGPP
#define	TYPEDEF_CLONGPP
typedef const long **		clongpp ;
#endif

#ifndef	TYPEDEF_CPCCHAR
#define	TYPEDEF_CPCCHAR
typedef const char *const	cpcchar ;
#endif

#ifndef	TYPEDEF_CPCCHARP	/* same as |mainv| */
#define	TYPEDEF_CPCCHARP	/* same as |mainv| */
typedef const char *const *	cpccharp ;
#endif

#ifndef	TYPEDEF_CUCHARPP
#define	TYPEDEF_CUCHARPP
typedef const unsigned char **	cucharpp ;
#endif

#ifndef	TYPEDEF_CUSHORTPP
#define	TYPEDEF_CUSHORTPP
typedef const unsigned short **	cushortpp ;
#endif

#ifndef	TYPEDEF_CUINTPP
#define	TYPEDEF_CUINTPP
typedef const unsigned int **	cuintpp ;
#endif

#ifndef	TYPEDEF_CULONGPP
#define	TYPEDEF_CULONGPP
typedef const unsigned long **	culongpp ;
#endif

#ifndef	TYPEDEF_CWCHAR
#define	TYPEDEF_CWCHAR
typedef const wchar_t		cwchar ;
#endif

#ifndef	TYPEDEF_MAINV
#define	TYPEDEF_MAINV
typedef const char *const *	mainv ;
#endif

#ifndef	TYPEDEF_USTAT
#define	TYPEDEF_USTAT
typedef USTAT			ustat ;
#endif
#ifndef	TYPEDEF_USTATFS
#define	TYPEDEF_USTATFS
typedef USTATFS			ustatfs ;
#endif
#ifndef	TYPEDEF_USTATVFS
#define	TYPEDEF_USTATVFS
typedef USTATVFS		ustatvfs ;
#endif

#ifndef	TYPEDEF_UPOLLFD
#define	TYPEDEF_UPOLLFD
typedef UPOLLFD			upollfd ;
#endif

#ifndef	TYPEDEF_CUSTAT
#define	TYPEDEF_CUSTAT
typedef const USTAT		custat ;
#endif
#ifndef	TYPEDEF_CUSTATFS
#define	TYPEDEF_CUSTATFS
typedef const USTATFS		custatfs ;
#endif
#ifndef	TYPEDEF_CUSTATVFS
#define	TYPEDEF_CUSTATVFS
typedef const USTATVFS		custatvfs ;
#endif

#ifndef	TYPEDEF_CDEV
#define	TYPEDEF_CDEV
typedef const dev_t		cdev ;
#endif
#ifndef	TYPEDEF_CINO
#define	TYPEDEF_CINO
typedef const ino_t		cino ;
#endif

#ifndef	TYPEDEF_VOIDF
#define	TYPEDEF_VOIDF
EXTERNC_begin
typedef void (*void_f)() noex ;
EXTERNC_end
#endif /* TYPEDEF_VOIDF */

#ifndef	TYPEDEF_SORTVCMP
#define	TYPEDEF_SORTVCMP
EXTERNC_begin
typedef int (*sort_vcmp)(const void *,const void *) noex ;
typedef int (*sortvcmp_f)(const void *,const void *) noex ;
EXTERNC_end
#endif /* TYPEDEF_SORTVCMP */

#ifndef	TYPEDEF_FPERM
#define	TYPEDEF_FPERM
typedef mode_t			fsperm ;
#endif /* TYPEDEF_FPERM */

#ifndef	TYPEDEF_CFPERM
#define	TYPEDEF_CFPERM
typedef const mode_t		cfsperm ;
#endif /* TYPEDEF_CFPERM */

#ifndef	TYPEDEF_USTIME
#define	TYPEDEF_USTIME
typedef time_t			ustime ;
#endif

#ifndef	TYPEDEF_CUSTIME
#define	TYPEDEF_CUSTIME
typedef const time_t		custime ;
#endif


#endif /* UTYPEDEFS_INCLUDE */


