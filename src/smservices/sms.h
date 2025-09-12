/* sms HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* SMS operations */
/* version %I% last-modified %G% */


#ifndef	SMS_INCLUDE
#define	SMS_INCLUDE


#include	<envstandards.h>	/* must be ordered first to configure */
#include	<time.h>
#include	<usystem.h>
#include	<plainq.h>


#define	SMS		struct sms_head
/* other defines */
#define	SMS_SVCCODE	1		/* used when over UDPMUX */


struct sms_bufdesc {
	char	*buf ;
	int	buflen ;
} ;

struct sms_head {
	plainq		tq ;
	time_t		timeout ;
	uint		magic ;
	uint		svc ;		/* numeric service */
	int		fd ;		/* socket file descriptor */
	int		pid ;		/* daemon PID */
} ;

EXTERNC_begin

extern int sms_open(SMS *,char *,char *,char *,char *,int) noex ;
extern int sms_write(SMS *,char *,int) noex ;
extern int sms_close(SMS *) noex ;

EXTERNC_end


#endif /* SMS_INCLUDE */


