/* hexdecoder HEADER */
/* charset=ISO8859-1 */
/* lang=C99 */

/* HEX decoder */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */

#ifndef	HEXDECODER_INCLUDE
#define	HEXDECODER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	HEXDECODER_MAGIC	0x13f3c202
#define	HEXDECODER		struct hexdecoder_head
#define	HEXDECODER_NSTAGE	2


struct hexdecoder_head {
	void		*outbuf ;	/* output-buffer */
	uint		magic ;
	int		rl ;		/* residue length ('0' or '1') */
	char		rb[HEXDECODER_NSTAGE + 1] ; /* staging buffer */
} ;

typedef HEXDECODER	hexdecoder ;

EXTERNC_begin

extern int hexdecoder_start(hexdecoder *) noex ;
extern int hexdecoder_load(hexdecoder *,cchar *,int) noex ;
extern int hexdecoder_read(hexdecoder *,char *,int) noex ;
extern int hexdecoder_finish(hexdecoder *) noex ;

EXTERNC_end


#endif /* hexdecoder_INCLUDE */


