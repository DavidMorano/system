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
#include	<clanguage.h>
#include	<usysbase.h>


#define	B64DECODER		struct b64decoder_head
#define	B64DECODER_MAGIC	0x13f3c204
#define	B64DECODER_NSTAGE	4


struct b64decoder_head {
	void		*outbuf ;	/* output-buffer */
	uint		magval ;
	int		rl ;		/* stage length */
	char		rb[B64DECODER_NSTAGE + 1] ;	/* stage buffer */
} ; /* end struct */

#ifdef	__cplusplus
enum b64decodermems {
    	b64decodermem_start,
	b64decodermem_finish,
	b64decodermem_count ,
	b64decodermem_overlast
} ; /* end enum (b64decodermems) */
struct b64decoder ;
struct b64decoder_co {
	b64decoder	*op = nullptr ;
	int		w = -1 ;
	void operator () (b64decoder *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (b64decoder_co) */
struct b64decoder : b64decoder_head {
	b64decoder_co	start ;
	b64decoder_co	finish ;
	b64decoder_co	count ;
	b64decoder() noex {
	    start	(this,b64decodermem_start) ;
	    finish	(this,b64decodermem_finish) ;
	    count	(this,b64decodermem_count) ;
	    outbuf = nullptr ;
	} ; /* end ctor */
	b64decoder(const b64decoder &) = delete ;
	b64decoder &operator = (const b64decoder &) = delete ;
	int load	(cchar *,int) noex ;
	int read	(char *,int) noex ;
	void dtor() noex ;
	operator int () noex ;
	destruct b64decoder() {
	    if (outbuf) dtor() ;
	} ;
} ; /* end struct (b64decoder) */
#else	/* __cplusplus */
typedef B64DECODER	b64decoder ;
#endif /* __cplusplus */

EXTERNC_begin

extern int b64decoder_start	(b64decoder *) noex ;
extern int b64decoder_load	(b64decoder *,cchar *,int) noex ;
extern int b64decoder_read	(b64decoder *,char *,int) noex ;
extern int b64decoder_count	(b64decoder *) noex ;
extern int b64decoder_finish	(b64decoder *) noex ;

EXTERNC_end


#endif /* B64DECODER_INCLUDE */


