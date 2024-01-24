/* b64decoder HEADER */
/* lang=C20 */

/* Base-64 (B64) decoder */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	B64DECODER_INCLUDE
#define	B64DECODER_INCLUDE


#include	<usysem.h>


#define	B64DECODER_MAGIC	0x13f3c204
#define	B64DECODER		struct b64decoder_head


struct b64decoder_head {
	uint		magic ;
	int		rl ;		/* stage length */
	void		*outbuf ;	/* output-buffer */
	char		rb[4+1] ;	/* stage buffer */
} ;

typedef struct b64decoder_head	b64decoder ;

EXTERNC_begin

extern int b64decoder_start(B64DECODER *) noex ;
extern int b64decoder_load(B64DECODER *,cchar *,int) noex ;
extern int b64decoder_read(B64DECODER *,char *,int) noex ;
extern int b64decoder_finish(B64DECODER *) noex ;

EXTERNC_end


#endif /* B64DECODER_INCLUDE */


