/* base64 INCLUDE */
/* lang=C20 */

/* perform BASE64 encoding and decoding */
/* version %I% last-modified %G% */


/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	BASE64_INCLUDE
#define	BASE64_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<localmisc.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern const uchar	base64_et[] ;
extern const uchar	base64_dt[] ;

extern int		base64_e(cchar *,int,char *) ;
extern int		base64_d(cchar *,int,char *) ;

#ifdef	__cplusplus
}
#endif


#endif /* BASE64_INCLUDE */


