/* usd HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


#ifndef	USD_INCLUDE
#define	USD_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<logfile.h>

#include	"sysdialer.h"


#define	USD_MAGIC	31415926
#define	USD		struct usd_head
#define	USD_FL		struct usd_flags


struct usd_flags {
	uint		log:1 ;
} ;

struct usd_head {
	LOGFILE		lh ;
	USD_FL		fl ;
	uint		magic ;
	int		tlen ;
	int		fd ;
} ;

EXTERNC_begin

extern int usd_open(USD *,SYSDIALER_ARGS *,
		cchar *,cchar *,cchar **) noex ;
extern int usd_reade(USD *,char *,int,int,int) noex ;
extern int usd_recve(USD *,char *,int,int,int,int) noex ;
extern int usd_recvfrome(USD *,char *,int,int,void *,int *,int,int) noex ;
extern int usd_recvmsge(USD *,MSGHDR *,int,int,int) noex ;
extern int usd_write(USD *,cchar *,int) noex ;
extern int usd_send(USD *,cchar *,int,int) noex ;
extern int usd_sendto(USD *,cchar *,int,int,void *,int) noex ;
extern int usd_sendmsg(USD *,MSGHDR *,int) noex ;
extern int usd_shutdown(USD *,int) noex ;
extern int usd_close(USD *) noex ;

EXTERNC_end


#endif /* USD_INCLUDE */


