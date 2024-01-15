/* strdcpyxw HEADER */
/* lang=C20 */

/* string-copy variant */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRDCPYXW_INCLUDE
#define	STRDCPYXW_INCLUDE


EXTERNC_begin

extern char *strdcpy1w(char *,int,cc *,int) noex ;
extern char *strdcpy2w(char *,int,cc *,cc *,int) noex ;
extern char *strdcpy3w(char *,int,cc *,cc *,cc *,int) noex ;
extern char *strdcpy4w(char *,int,cc *,cc *,cc *,cc *,int) noex ;
extern char *strdcpy5w(char *,int,cc *,cc *,cc *,cc *,cc *,int) noex ;
extern char *strdcpy6w(char *,int,cc *,cc *,cc *,cc *,cc *,cc *,int) noex ;

EXTERNC_end


#endif /* STRDCPYXW_INCLUDE */


