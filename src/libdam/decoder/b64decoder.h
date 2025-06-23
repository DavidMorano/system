/* b64decoder HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Base-64 (B64) decoder */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	B64DECODER_INCLUDE
#define	B64DECODER_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<usystem.h>


#define	B64DECODER_MAGIC	0x13f3c204
#define	B64DECODER		struct b64decoder_head
#define	B64DECODER_NSTAGE	4


struct b64decoder_head {
	void		*outbuf ;	/* output-buffer */
	uint		magic ;
	int		rl ;		/* stage length */
	char		rb[B64DECODER_NSTAGE + 1] ;	/* stage buffer */
} ;

typedef B64DECODER	b64decoder ;

EXTERNC_begin

extern int b64decoder_start(b64decoder *) noex ;
extern int b64decoder_load(b64decoder *,cchar *,int) noex ;
extern int b64decoder_read(b64decoder *,char *,int) noex ;
extern int b64decoder_finish(b64decoder *) noex ;

EXTERNC_end


#endif /* B64DECODER_INCLUDE */


