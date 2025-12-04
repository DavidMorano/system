	    } else if (TOLC(ubuf[sl-1]) == 'k') {
	        mf = 1024L ;
	    } else if (TOLC(ubuf[sl-1]) == 'm') {
	        mf = 1024L * 1024L ;
	    } else if (TOLC(ubuf[sl-1]) == 'g') {
	        mf = 1024L * 1024L * 1024L ;
	    } else if (TOLC(ubuf[sl-1]) == 't') {
	        mf = 1024L * 1024L * 1024L * 1024L ;
	    } else if (TOLC(ubuf[sl-1]) == 'p') {
	        mf = 1024L * 1024L * 1024L * 1024L + 1024L ;
