/* mkfnamesuf INCLUDE */
/* lang=C20 */

/* make a file name from parts (one base and some suffixes) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MKFNAMESUF_INCLUDE
#define	MKFNAMESUF_INCLUDE


#ifndef	TYPEDEF_CC
#define	TYPEDEF_CC
typedef const char	cc ;
#endif

#ifdef	__cplusplus
extern "C" {
#endif

extern int mkfnamesuf(char *,int,cc *,...) noex ;
extern int mkfnamesuf1(char *,cc *,cc *) noex ;
extern int mkfnamesuf2(char *,cc *,cc *,cc *) noex ;
extern int mkfnamesuf3(char *,cc *,cc *,cc *,cc *) noex ;
extern int mkfnamesuf4(char *,cc *,cc *,cc *,cc *,cc *) noex ;
extern int mkfnamesuf5(char *,cc *,cc *,cc *,cc *,cc *,cc *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* MKFNAMESUF_INCLUDE */


