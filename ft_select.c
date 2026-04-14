#include <errno.h>
#include <stdlib.h>     /* getenv, malloc, free, exit */
#include <unistd.h>     /* isatty, ttyname, ttyslot, open, close, write, read */
#include <termios.h>    /* tcsetattr, tcgetattr */
#include <signal.h>     /* signal */
#include <sys/ioctl.h>  /* ioctl */
#
#include "./libft/libft.h"

extern int ft_select_clear(void);

extern int ft_select_rawmode(int, struct termios *);

extern int ft_select_input(void);

int main(int ac, char **av, char **envp) {
    (void) envp;

    /* get the name of the program... */
    char *pname = *av;

    /* check if there's enough argument passed... */
    if (ac < 2) {
        ft_putstr_fd(pname, 2);
        ft_putendl_fd(" [ ARGUMENTS ] ...", 2);
        return (1);   
    }

    /* move to the argument list... */
    av++;
    ac--;

    /* check if 'version' option... */
    if (!ft_strncmp(*av, "-v", 3) ||
        !ft_strncmp(*av, "--version", 10)
    ) {
        ft_putendl_fd("ft_select (42 Warsaw) 1.0.0.", 1);
        ft_putendl_fd("Project available under the GNU Lesser General Public License v.2.1.", 1);
        ft_putendl_fd("Source code available under <https://github.com/itsYakub/42-ft_select>.", 1);
        
        /* gently return... */
        return (0);
    }

    /* check if 'help' option... */
    else if (!ft_strncmp(*av, "-h", 3) ||
             !ft_strncmp(*av, "--help", 7)
    ) {
        /* print usage... */
        ft_putendl_fd("Usage:\n", 1);
        ft_putstr_fd("  ", 1);
        ft_putstr_fd(pname, 1);
        ft_putendl_fd(" [ LIST OF ARGUMENTS ] ...\n", 1);

        /* print description... */
        ft_putendl_fd("From the official subject.pdf:\n", 1);
        ft_putendl_fd("  This project is meant to make you code a small program using termcaps", 1);
        ft_putendl_fd("  which will allow you to pick from a list of choices and return it to your shell.\n", 1);

        ft_putendl_fd("ft_select is a program that takes a vector of strings and return a selected key to the standard output.", 1);
        ft_putendl_fd("Key is selected using arrow keys to move the cursor in four directions and by pressing the return key.", 1);
        ft_putendl_fd("ft_select can accept an arbitrary type of key: regular string, file names etc. Use the program responsibly.", 1);
        ft_putendl_fd("If no key is given, the program will exit. If every key is removed from the list, the program will exit as well.", 1);
        ft_putendl_fd("If any error occurs during the program execution, intended behaviour is printing the error message and exiting the program with an exit code.\n", 1);

        /* print options... */
        ft_putendl_fd("Available options:\n", 1);
        ft_putendl_fd("  -h, --help\t\tPrint this help message and gently exit.", 1);
        ft_putendl_fd("  -v, --version\t\tPrint the version of the program and gently exit.\n", 1);

        /* buf reports... */
        ft_putendl_fd("Report bugs to <https://github.com/itsYakub/42-ft_select/issues>.", 1);

        /* gently return... */
        return (0);
    }

    /* create termios and enable raw mode... */
    struct termios termios = { 0 };
    ft_select_rawmode(1, &termios);

    /* update loop... */
    int exit = 0;
    while (!exit) {
        char c = ft_select_input();
        /* handle 'read' errors... */
        if (c == -1) {
            ft_select_rawmode(0, &termios);
            return (1);
        }

        /* process input... */
        else {
            /* for now handle only exitting... */
            if (c == 'q') {
                exit = 1;
            }

            else {
                char str[2] = { c, 0 };
                ft_putendl_fd(str, 1);
            }
        }
    }

    /* disable raw mode... */
    ft_select_rawmode(0, &termios);
    return (0);
}


extern int ft_select_clear(void) {
    return (write(1, "\033[2J", 5));
}


extern int ft_select_rawmode(int enable, struct termios *tptr) {
    /* null check... */
    if (!tptr) { return (0); }
    
    if (enable) {
        /* get the original termios... */
        if (tcgetattr(0, tptr) == 1) { return (-1); }
        
        /* copy termios... */
        struct termios raw = *tptr;
        raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
        raw.c_oflag &= ~(OPOST);
        raw.c_cflag &= ~(CS8);
        raw.c_lflag &= ~(ECHO | ICANON | IEXTEN);
        if (tcsetattr(0, TCSAFLUSH, &raw) == -1) { return (-1); }
    }
    else if (!enable) {
        /* set back the original termios... */
        if (tcsetattr(0, TCSAFLUSH, tptr) == -1) { return (-1); }
    }

    return (0);
}


extern int ft_select_input(void) {
    /* input "buffer"... */
    char c = 0;

    /* number of bytes read... */
    ssize_t nread = 0;
    while ((nread = read(0, &c, 1)) != 1) {
        if (nread == -1 && errno == EAGAIN) {
            return (nread);
        }
    }

    return (c);
}
