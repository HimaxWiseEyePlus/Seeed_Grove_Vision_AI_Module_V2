#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <errno.h>
//#include <reent.h>
#include <unistd.h>
//#include <sys/wait.h>

#undef errno
extern int errno;

#define USE_IO_STDOUT_STDERR   1
#define USE_IO_STDIN           0
//#define STDIN_ECHO           1
//#define STDOUT_STDERR_CR_LF  1

#ifndef STDIN_ECHO
#define STDIN_ECHO             0  /* STDIN: echo to STDOUT */
#endif
#ifndef STDOUT_STDERR_CR_LF
#define STDOUT_STDERR_CR_LF    0  /* STDOUT and STDERR: add CR for LF */
#endif

#if (USE_IO_STDOUT_STDERR != 0)
extern int stdout_putchar(int ch);
extern int stderr_putchar(int ch);
#endif

#if (USE_IO_STDIN != 0)
extern int stdin_getchar(void);
#endif

_READ_WRITE_RETURN_TYPE _write(int fd, const char *buf, size_t nbyte) //_READ_WRITE_RETURN_TYPE _write (int __fd, const void *__buf, size_t __nbyte);
{
#if (USE_IO_STDOUT_STDERR != 0)
    if (fd == STDOUT_FILENO)
    { 
        for (int idx = 0; idx < nbyte; idx++) {
#if (STDOUT_STDERR_CR_LF != 0)
            if (buf[idx] == '\n')
                stdout_putchar('\r');
#endif
            stdout_putchar(buf[idx]);
        }

        return nbyte;
    }
    else if (fd == STDERR_FILENO)
    { 
        for (int idx = 0; idx < nbyte; idx++) {
#if (STDOUT_STDERR_CR_LF != 0)
            if (buf[idx] == '\n')
                stderr_putchar('\r');
#endif
            stderr_putchar(buf[idx]);
        }

        return nbyte;
    }
    else
#endif
    {
        errno = EBADF;
        return (-1);
    }
}

_READ_WRITE_RETURN_TYPE _read(int fd, char *buf, size_t nbyte) //_READ_WRITE_RETURN_TYPE _read (int __fd, void *__buf, size_t __nbyte);
{
#if (USE_IO_STDIN != 0)
    if (STDIN_FILENO == fd) {
        int idx = 0;

        for (; idx < nbyte; idx++) {
            buf[idx] = stdin_getchar();
#if (STDIN_ECHO != 0)
            stdout_putchar(buf[idx]);
#endif
            if ('\n' == buf[idx]) // find LF \'n'
                break;
        }

        return idx;
    }
    else {
        errno = EBADF;
        return (-1);
    }
#else
    return 0; /* EOF */
#endif
}

int _open(const char *name, int flags, ...) //int _open (const char *, int, ...);
{
    (void)name;
    (void)flags;
    errno = ENOSYS;
    return (-1); /* Always fails */
}

int _close(int fildes) //int _close (int __fildes);
{
    (void)fildes;
    errno = EBADF; 
    return (-1); /* Always fails */
}

_off_t _lseek(int fildes, _off_t offset, int whence) //_off_t _lseek (int __fildes, _off_t __offset, int __whence);
{
#if (USE_IO_STDOUT_STDERR != 0)
    if ((STDOUT_FILENO == fildes) || (STDERR_FILENO == fildes))
    {
        return (1);
    }
    else
#endif
    {
        errno = EBADF;
        return (-1);
    }
}

int _fstat(int fd, struct stat *sbuf) //int _fstat (int __fd, struct stat *__sbuf );
{
#if (USE_IO_STDOUT_STDERR != 0)
    if ((STDOUT_FILENO == fd) || (STDERR_FILENO == fd))
    {
        sbuf->st_mode = S_IFCHR;
        return (1);
    }
    else
#endif
    {
        errno = EBADF;
        return (-1);
    }
}

int _isatty(int fildes) //int _isatty (int __fildes);
{
#if (USE_IO_STDOUT_STDERR != 0)
    if ((STDOUT_FILENO == fildes) || (STDERR_FILENO == fildes))
    {
        return (1);
    }
    else
#endif
    {
        errno = EBADF;
        return (-1);
    }
}

int _kill(pid_t pid, int sig) //int _kill (pid_t, int);
{
    errno = EINVAL;
    return (-1); /* Always fails */
}

pid_t _getpid(void) //pid_t _getpid (void);
{
    return (1); /* Success */
}

#define log_str(...)                                \
    do {                                                \
        const char *pchSrc = __VA_ARGS__;               \
        uint_fast16_t hwSize = sizeof(__VA_ARGS__);     \
        do {                                            \
            stdout_putchar(*pchSrc++);                  \
        } while(--hwSize);                              \
    } while(0)

_ATTRIBUTE((noreturn))
void _exit(int status)//void _exit (int __status) _ATTRIBUTE ((__noreturn__));
{
    (void)status;
    log_str("\n");
    log_str("_[TEST COMPLETE]_________________________________________________\n");
    log_str("\n\n");
  //  *((volatile unsigned *) (SERIAL_BASE_ADDRESS-0x10000)) = 0xa;
    while(1);
}

extern unsigned char __HeapBase[];  /* Symbol defined by linker map */
extern unsigned char __HeapLimit[]; /* Symbol defined by linker map */

void *_sbrk(ptrdiff_t incr) { //void *_sbrk (ptrdiff_t __incr);
    static void *heap = NULL;
    void *prev_heap;
    static void *HeapEndPtr = (void *)__HeapLimit;
    caddr_t Status;

    if (heap == NULL) {
        heap = (void *)__HeapBase;
    }

    prev_heap = heap;

    if (((heap + incr) <= HeapEndPtr) && (prev_heap != NULL)) {
        heap += incr;
        Status = (caddr_t) prev_heap;
    } else {
        errno = ENOMEM;
        Status = (caddr_t) -1;
    }

    return (void *)Status;
}
