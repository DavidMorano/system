/* outbuf */
/* lang=C20 */

/* output buffer management (really?) */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	OUTBUF_INCLUDE
#define	OUTBUF_INCLUDE


#define	OUTBUF		struct outbuf_head


struct outbuf_head {
	char		*obuf ;
	int		olen ;
	int		f_alloc ;
} ;

typedef OUTBUF		outbuf ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int	outbuf_start(outbuf *,char *,int) noex ;
extern int	outbuf_get(outbuf *,char **) noex ;
extern int	outbuf_finish(outbuf *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* OUTBUF_INCLUDE */


