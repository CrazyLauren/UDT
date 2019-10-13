#ifndef SHARE_CONFIG_H
#define SHARE_CONFIG_H

#cmakedefine HAVE_LIBEXECINFO
#cmakedefine HAVE_BACKTRACE_SYMBOLS

#cmakedefine HAVE_DBGHELP

#cmakedefine HAVE_CXXABI_H
#cmakedefine HAVE_DEX
#cmakedefine HAVE_DLFCN
#cmakedefine HAVE_SIGNAL_H
#cmakedefine HAVE_SIGACTION
#cmakedefine HAVE_UNISTD_H
#cmakedefine HAVE_SYMLINK
#cmakedefine HAVE_STATEMENTS_WITH_INITIALIZER
#cmakedefine HAVE_INET_NTOP
#cmakedefine HAVE_IPHLPAPI_H
#cmakedefine HAVE_WINERROR_H
#cmakedefine HAVE_GETIFADDRS
#cmakedefine HAVE_WINSOCK_H
#cmakedefine HAVE_FUTEX_H
#cmakedefine HAVE_POSIX_SEMAPHORES
#cmakedefine SIZEOF_PTHREAD_MUTEX_T @SIZEOF_PTHREAD_MUTEX_T@
#cmakedefine SIZEOF_PTHREAD_COND_T @SIZEOF_PTHREAD_COND_T@
#cmakedefine SIZEOF_SEM_T @SIZEOF_SEM_T@




#endif /* SHARE_CONFIG_H */
