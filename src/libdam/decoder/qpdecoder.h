/* qpdecoder HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Quoted-Printable (QP) decoder */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */

#ifndef	QPDECODER_INCLUDE
#define	QPDECODER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	QPDECODER_MAGIC		0x13f3c205
#define	QPDECODER		struct qpdecoder_head
#define	QPDECODER_FL		struct qpdecoder_flags
#define	QPDECODER_NSTAGE	2


struct qpdecoder_flags {
	uint		esc:1 ;
	uint		space:1 ;
} ;

struct qpdecoder_head {
	void		*outbuf ;	/* output-buffer */
	QPDECODER_FL	fl ;
	uint		magic ;
	int		rl ;		/* stage length */
	char		rb[QPDECODER_NSTAGE + 1] ;	/* stage buffer */
} ;

typedef	QPDECODER	qpdecoder ;
typedef	QPDECODER_FL	qpdecoder_fl ;

EXTERNC_begin

extern int qpdecoder_start(qpdecoder *,int) noex ;
extern int qpdecoder_load(qpdecoder *,cchar *,int) noex ;
extern int qpdecoder_read(qpdecoder *,char *,int) noex ;
extern int qpdecoder_finish(qpdecoder *) noex ;

EXTERNC_end


#endif /* QPDECODER_INCLUDE */


