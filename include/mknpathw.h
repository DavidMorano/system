/* mknpathw HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* make a file-path using multiple component c-strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MKNPATHW_INCLUDE
#define	MKNPATHW_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<usystem.h>
#include	<mknpathxw.h>

#ifdef	__cplusplus

inline int mknpathw(char *pp,int pl,cc *s1,int sl) noex {
	return mknpathxw(pp,pl,1,s1,sl) ;
}
inline int mknpathw(char *pp,int pl,cc *s1,cc *s2,int sl) noex {
	return mknpathxw(pp,pl,2,s1,s2,sl) ;
}
inline int mknpathw(char *pp,int pl,cc *s1,cc *s2,cc *s3,int sl) noex {
	return mknpathxw(pp,pl,3,s1,s2,s3,sl) ;
}
inline int mknpathw(char *pp,int pl,cc *s1,cc *s2,cc *s3,cc *s4,int sl) noex {
	return mknpathxw(pp,pl,4,s1,s2,s3,s4,sl) ;
}
inline int mknpathw(char *pp,int pl,cc *s1,cc *s2,cc *s3,cc *s4,
		cc *s5,int sl) noex {
	return mknpathxw(pp,pl,5,s1,s2,s3,s4,s5,sl) ;
}
inline int mknpathw(char *pp,int pl,cc *s1,cc *s2,cc *s3,cc *s4,
		cc *s5,cc *s6,int sl) noex {
	return mknpathxw(pp,pl,6,s1,s2,s3,s4,s5,s6,sl) ;
}

#endif /* __cplusplus */


#endif /* MKNPATHW_INCLUDE */


