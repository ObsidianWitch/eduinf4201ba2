# TP-DIST Notes #

## Fixed warnings from the base files ##

* gethostname() is not part of the C99 standard (<http://stackoverflow.com/questions/3355298/unistd-h-and-c99-on-linux>), set -D_GNU_SOURCE or -D_XOPEN_SOURCE=500, with a preference towards -D_XOPEN_SOURCE=500 for portability (but I used -D_GNU_SOURCE anyways because needed elsewhere)

* -D_XOPEN_SOURCE=500 meaning : X/Open 5, incorporating POSIX 1995 (<http://stackoverflow.com/questions/5378778/what-does-d-xopen-source-do-mean>)

* int size_sock; -> socklen_t size_sock; to correct warning : pointer targets in passing argument 3 of ‘accept’ differ in signedness [-Wpointer-sign]

* ‘struct hostent’ has no member named ‘h_addr’ -> -D_GNU_SOURCE
