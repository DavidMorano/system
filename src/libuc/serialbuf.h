/* serialbuf HEADER */
/* lang=C20 */

/* object that serializes various types into a given buffer */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SERIALBUF_INCLUDE
#define	SERIALBUF_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<limits.h>
#include	<usystem.h>


#define	SERIALBUF	struct serialbuf_head


struct serialbuf_head {
	char		*bp ;	/* current buffer pointer (changes) */
	int		len ;	/* supplied buffer length (doesn't change) */
	int		i ;	/* current buffer index (changes) */
} ;

typedef	SERIALBUF	serialbuf ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int serialbuf_start(serialbuf *,char *,int) noex ;
extern int serialbuf_robj(serialbuf *,void *,int) noex ;
extern int serialbuf_rc(serialbuf *,char *) noex ;
extern int serialbuf_rs(serialbuf *,short *) noex ;
extern int serialbuf_ri(serialbuf *,int *) noex ;
extern int serialbuf_ria(serialbuf *,int *,int) noex ;
extern int serialbuf_rl(serialbuf *,long *) noex ;
extern int serialbuf_rla(serialbuf *,long *,int) noex ;
extern int serialbuf_rll(serialbuf *,longlong *) noex ;
extern int serialbuf_rstrw(serialbuf *,char *,int) noex ;
extern int serialbuf_rstrn(serialbuf *,char *,int) noex ;
extern int serialbuf_rbuf(serialbuf *,char *,int) noex ;
extern int serialbuf_ruc(serialbuf *,uchar *) noex ;
extern int serialbuf_rus(serialbuf *,ushort *) noex ;
extern int serialbuf_rui(serialbuf *,uint *) noex ;
extern int serialbuf_ruia(serialbuf *,uint *,int) noex ;
extern int serialbuf_ru(serialbuf *,ulong *) noex ;
extern int serialbuf_rula(serialbuf *,ulong *,int) noex ;
extern int serialbuf_rull(serialbuf *,ulonglong *) noex ;
extern int serialbuf_rustrw(serialbuf *,uchar *,int) noex ;
extern int serialbuf_rustrn(serialbuf *,uchar *,int) noex ;
extern int serialbuf_rubuf(serialbuf *,uchar *,int) noex ;

extern int serialbuf_wobj(serialbuf *,cvoid *,int) noex ;

extern int serialbuf_wc(serialbuf *,int) noex ;
extern int serialbuf_ws(serialbuf *,int) noex ;
extern int serialbuf_wi(serialbuf *,int) noex ;
extern int serialbuf_wia(serialbuf *,int *,int) noex ;
extern int serialbuf_wl(serialbuf *,long) noex ;
extern int serialbuf_wla(serialbuf *,long *,int) noex ;
extern int serialbuf_wll(serialbuf *,longlong) noex ;
extern int serialbuf_wstrw(serialbuf *,cchar *,int) noex ;
extern int serialbuf_wstrn(serialbuf *,cchar *,int) noex ;
extern int serialbuf_wbuf(serialbuf *,cchar *,int) noex ;

extern int serialbuf_wuc(serialbuf *,uint) noex ;
extern int serialbuf_wus(serialbuf *,uint) noex ;
extern int serialbuf_wui(serialbuf *,uint) noex ;
extern int serialbuf_wuia(serialbuf *,uint *,int) noex ;
extern int serialbuf_wu(serialbuf *,ulong) noex ;
extern int serialbuf_wula(serialbuf *,ulong *,int) noex ;
extern int serialbuf_wull(serialbuf *,ulonglong) noex ;
extern int serialbuf_wustrw(serialbuf *,const uchar *,int) noex ;
extern int serialbuf_wustrn(serialbuf *,const uchar *,int) noex ;
extern int serialbuf_wubuf(serialbuf *,const uchar *,int) noex ;

extern int serialbuf_adv(serialbuf *,int) noex ;

extern int serialbuf_getlen(serialbuf *) noex ;
extern int serialbuf_finish(serialbuf *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* SERIALBUF_INCLUDE */


