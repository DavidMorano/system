static int tty_wait(uterm *op,int timeout) noex {
	POLLFD		fds[2] ;
	time_t		daytime = getustime ;
	time_t		lasttime ;
	int		rs ;
	int		i ;
	int		len = 0 ;
	int		nfds ;
	int		polltime ;
	int		f_starting, f_looping ;
	char		cbuf[TTY_READCHARS+1] ;

/* initialize the POLL structure */

	nfds = 0 ;
	fds[nfds].fd = op->fd ;
	fds[nfds].events = POLLIN ;
	nfds += 1 ;
	fds[nfds].fd = -1 ;
	fds[nfds].events = POLLIN ;

	if (timeout < 0)
	    timeout = INT_MAX ;

	polltime = MIN(3000,(timeout * 1000)) ;

	op->timeout = timeout ;
	lasttime = daytime ;

#if	CF_FIRSTREAD

/* try it out the first time */

	rs = u_read(op->fd,cbuf,TTY_READCHARS) ;
	len = rs ;
	f_starting = true ;
	goto enter ;

#endif /* CF_FIRSTREAD */

/* loop it */
loop:
	f_starting = false ;

/* zero out the buffer so that we can tell if Solaris screws us later */

	for (int i = 0 ; i < TTY_READCHARS ; i += 1) {
	    cbuf[i] = '\0' ;
	}

/* wait for read attention */

	rs = u_poll(fds,1,polltime) ;

	if (rs == SR_INTR) {
	    goto loop ;
	}

/* do the system-call read */

	if ((rs >= 0) && (fds[0].revents & POLLIN)) {
	    rs = u_read(op->fd,cbuf,TTY_READCHARS) ;
	    len = rs ;
	}  /* end if */

enter:
	if (rs < 0)
	    goto ret0 ;

	op->loopcount += 1 ;
	if (len < 0)
	    goto ret0 ;

	if (len == 0) {
	    daytime = time(nullptr) ;
	    if (op->timeout >= 0) {
	        op->timeout -= (daytime - lasttime) ;
	        if (op->timeout <= 0)
	            return SR_OK ;

	    }

	    if ((! f_starting) && 
	        ((daytime - op->ti_start) >= TO_HANGUP)) {

	        f_looping = (op->loopcount > MAXLOOPS) ;
	        op->ti_start = daytime ;
	        op->loopcount = 0 ;
	        if (f_looping) {

	            rs = SR_NOANODE ;
	            goto ret0 ;
	        }

	    } /* end if (looping check) */

	} else {
	    int		i ; /* used-afterwards */
/* skip over leading zero characters fabricated by Solaris SVR4 */
	    for (i = 0 ; (i < len) && (cbuf[i] == '\0') ; i += 1) {
	    } /* end for */
/* call the Receive-Interrupt-Service-Routine with what we do have */
	    if ((len - i) > 0) {
	        rs = tty_risr(op,(cbuf + i),(len - i)) ;
	   }
	} /* end if */

/* should we go around again? */

	lasttime = daytime ;
	if ((rs >= 0) &&
	    (len <= 0) && ((op->timeout < 0) || (op->timeout > 0)))
	    goto loop ;

/* we-re out of here */
ret0:
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (tty_wait) */

