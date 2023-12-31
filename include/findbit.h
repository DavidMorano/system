/* findbit HEADER */
/* lang=C20 */

/* find bit patterns (whatever!) */
/* version %I% last-modified %G% */


/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	FINDBIT_INCLUDE
#define FINDBIT_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<localmisc.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int ffbsi(uint) noex ;
extern int ffbsl(ulong) noex ;
extern int ffbsll(ulonglong) noex ;

extern int ffbci(uint) noex ;
extern int ffbcl(ulong) noex ;
extern int ffbcll(ulonglong) noex ;

extern int flbsi(uint) noex ;
extern int flbsl(ulong) ;
extern int flbsll(ulonglong) ;

extern int flbci(uint) noex ;
extern int flbcl(ulong) noex ;
extern int flbcll(ulonglong) ;

extern int fbscounti(uint) noex ;
extern int fbscountl(ulong) noex ;
extern int fbscountll(ulonglong) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* FINDBIT_INCLUDE */


