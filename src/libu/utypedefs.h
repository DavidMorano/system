/* utypedefs HEADER */
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
#include	<sys/time.h>		/* for 'u_adjtime(3u)' */
#include	<sys/timeb.h>		/* for 'uc_ftime(3uc)' */
#include	<sys/resource.h>
#include	<sys/resource.h>
#include	<sys/stat.h>
#include	<sys/statvfs.h>
#include	<sys/socket.h>
#include	<sys/poll.h>

#if	defined(SYSHAS_ACL) && (SYSHAS_ACL > 0)
#include	<sys/acl.h>
#endif

#include	<arpa/inet.h>		/* <- |in_addr_t| */

#include	<unistd.h>
#include	<fcntl.h>
#include	<time.h>		/* for 'u_utime(2)' */
#include	<utime.h>		/* for 'u_utime(2)' */
#include	<pthread.h>
#include	<termios.h>
#include	<errno.h>
#include	<dirent.h>
#include	<ucontext.h>
#include	<netdb.h>
#include	<pwd.h>
#include	<grp.h>
#include	<limits.h>
#include	<signal.h>
#include	<stddef.h>
#include	<stdlib.h>

#if	defined(SYSHAS_XTI) && (SYSHAS_XTI > 0)
#include	<xti.h>
#endif

#if	defined(OSNAME_SunOS) && (OSNAME_SunOS > 0)
#include	<project.h>
#include	<user_attr.h>
#endif

#include	<usys.h>	/* <- auxilllary OS support */


/* for 'stat(2)' and its many friends */

#ifndef	STRUCT_USTAT
#define	STRUCT_USTAT
#if	defined(_LARGEFILE_SOURCE)
#define	ustat	stat
#else
#if	defined(_LARGEFILE64_SOURCE)
#define	ustat	stat64
#else
#define	ustat	stat
#endif
#endif
#endif

/* for 'statvfs(2)' and its many friends */

#ifndef	STRUCT_USTATVFS
#define	STRUCT_USTATVFS
#if	defined(_LARGEFILE_SOURCE)
#define	ustatvfs	statvfs
#else
#if	defined(_LARGEFILE64_SOURCE)
#define	ustatvfs	statvfs64
#else
#define	ustatvfs	statvfs
#endif
#endif
#endif

/* off_t */
#ifndef	TYPEDEF_OFFSET
#define	TYPEDEF_OFFSET
#if	defined(_FILE_OFFSET_BITS) && (_FILE_OFFSET_BITS == 64)
#if	defined(__LP64__) || defined(__LP64__)
typedef off_t		off_t ;
#else
typedef long long	off_t ;
#endif /* defined(__LP64__) || defined(__LP64__) */
#else
typedef off_t		off_t ;
#endif /* defined(_FILE_OFFSET_BITS) && (_FILE_OFFSET_BITS == 64) */
#endif /* TYPEDEF_OFFSET */

/* PREDEFINED start */
/* determine if some unsigned-related typedefs have already been made */
/* the following unsigned typedefs are supposedly "System V" compatibility */
#if !defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE)

#ifndef	TYPEDEFS_PREDEFINEDUNSIGNED
#define	TYPEDEFS_PREDEFINEDUNSIGNED

#ifndef	TYPEDEF_USHORT
#define	TYPEDEF_USHORT
#endif

#ifndef	TYPEDEF_UINT
#define	TYPEDEF_UINT
#endif

#endif /* TYPEDEFS_PREDEFINEDUNSIGNED */

#endif /* !defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE) */
/* PREDEFINED end */

/* constants */

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

#ifndef	TYPEDEF_CVOID
#define	TYPEDEF_CVOID
typedef const void		cvoid ;
#endif /* TYPEDEF_CVOID */

#ifndef	TYPEDEF_CSIZE
#define	TYPEDEF_CSIZE
typedef const size_t		csize ;
#endif /* TYPEDEF_CSIZE */

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

#ifndef	TYPEDEF_CVOIDP
#define	TYPEDEF_CVOIDP
typedef const void *		cvoidp ;
#endif

#ifndef	TYPEDEF_CNULLPTR
#define	TYPEDEF_CNULLPTR
typedef const nullptr_t		cnullptr ;
#endif

#ifndef	TYPEDEF_CCP
#define	TYPEDEF_CCP
typedef const char		*ccp ;
#endif

#ifndef	TYPEDEF_CHARP
#define	TYPEDEF_CHARP
typedef char			*charp ;
#endif

#ifndef	TYPEDEF_CCHARP
#define	TYPEDEF_CCHARP
typedef const char		*ccharp ;
#endif

#ifndef	TYPEDEF_CPCCHAR
#define	TYPEDEF_CPCCHAR
typedef const char *const	cpcchar ;
#endif

#ifndef	TYPEDEF_CPCCHARP
#define	TYPEDEF_CPCCHARP
typedef const char *const	*cpccharp ;
#endif

#ifndef	TYPEDEF_MAINV
#define	TYPEDEF_MAINV
typedef const char *const	*mainv ;
#endif

#ifndef	TYPEDEF_IN6ADDRT
#define	TYPEDEF_IN6ADDRT
typedef in_addr_t		in4_addr_t ;
#endif

/* handle some really brain-damaged systems -- like MacOS-X Darwin®! */
#if	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)
#if	defined(OSNUM) && (OSNUM <= 7)
typedef int			id_t ;
#endif
#endif

#if	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)
#if	defined(OSNUM) && (OSNUM <= 9)
typedef struct in6_addr		in6_addr_t ;
#endif
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
typedef int (*sort_vcmp)(cvoid *,cvoid *) noex ;
EXTERNC_end
#endif /* TYPEDEF_SORTVCMP */

#ifndef	TYPEDEF_FPERM
#define	TYPEDEF_FPERM
typedef mode_t	fsperm ;
#endif /* TYPEDEF_FPERM */

#ifndef	TYPEDEF_CFPERM
#define	TYPEDEF_CFPERM
typedef mode_t	const cfsperm ;
#endif /* TYPEDEF_CFPERM */


#endif /* UTYPEDEFS_INCLUDE */


