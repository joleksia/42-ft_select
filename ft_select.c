/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_select.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joleksia <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 15:48:23 by joleksia          #+#    #+#             */
/*   Updated: 2026/05/20 10:35:09 by joleksia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <limits.h>
#include <stdlib.h>     /* getenv, malloc, free, exit */
#include <unistd.h>     /* isatty, ttyname, ttyslot, open, close, write, read */
#include <termios.h>    /* tcsetattr, tcgetattr */
#include <signal.h>     /* signal */
#include <sys/ioctl.h>  /* ioctl */
#
#include "./libft/libft.h"

/* getopt */

extern int ft_select_getopt(int, char **);

/* tui */

extern int ft_select_clear(void);

extern int ft_select_rawmode(int, struct termios *);

extern int ft_select_input(void);

/* select */

typedef struct s_select t_select;

struct s_select {
    /* main opts */
    int    s_ac;    /* s_ac - argument count */
    char **s_av;    /* s_av - argument vector */

    /* select */
    int   *s_dat;   /* s_dat - select / unselect data vector */
    size_t s_crxc;  /* s_crxc - cursor X position (current) */
    size_t s_crxm;  /* s_crxm - cursor X position (max) */
    size_t s_cryc;  /* s_cryc - cursor Y position (current) */
    size_t s_crym;  /* s_crym - cursor Y position (max) */
};

extern int ft_select_init(t_select *, int, char **);

extern int ft_select_deinit(t_select *);

/* main */

int main(int ac, char **av, char **envp) {
    (void) envp;

    /* get the name of the program... */
    char *pname = *av;
    (void) pname;

    /* process options... */
    ft_select_getopt(ac, av);

    /* create termios and enable raw mode... */
    struct termios termios = { 0 };
    ft_select_rawmode(1, &termios);

    /* create select objects... */
    t_select slct = { 0 };
    ft_select_init(&slct, --ac, ++av);

    /* update loop... */
    int exit = 0;
    while (!exit) {
        char c = ft_select_input();
        /* handle 'read' errors... */
        if (c == -1) {
            ft_select_rawmode(0, &termios);
            ft_select_deinit(&slct);
            return (errno);
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
    ft_select_deinit(&slct);
    
    return (0);
}


/* getopt */

extern int ft_select_getopt(int ac, char **av) {
    /* get program name... */
    char *pname = *av;

    /* check if there's enough argument passed... */
    if (ac < 2) {
        ft_putstr_fd(pname, 2);
        ft_putendl_fd(" [ ARGUMENTS ] ...", 2);
        exit(EXIT_FAILURE);
    }

    /* check if 'version' option... */
    if (!ft_strncmp(*av, "-v", 3) ||
        !ft_strncmp(*av, "--version", 10)
    ) {
        ft_putendl_fd("ft_select (42 Warsaw) 1.0.0.", 1);
        ft_putendl_fd("Project available under the GNU Lesser General Public License v.2.1.", 1);
        ft_putendl_fd("Source code available under <https://github.com/itsYakub/42-ft_select>.", 1);
        
        /* gently return... */
        exit(EXIT_SUCCESS);
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

        /* print options... */
        ft_putendl_fd("Available options:\n", 1);
        ft_putendl_fd("  -h, --help\t\tPrint this help message and gently exit.", 1);
        ft_putendl_fd("  -v, --version\t\tPrint the version of the program and gently exit.\n", 1);

        /* buf reports... */
        ft_putendl_fd("Report bugs to <https://github.com/itsYakub/42-ft_select/issues>.", 1);

        /* gently return... */
        exit(EXIT_SUCCESS);
    }
    return (1);
}


/* tui */

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


/* select */

extern int ft_select_init(t_select *slct, int ac, char **av) {
    if (!slct) { return (-1); }

    *slct = (t_select) { 0 };
    slct->s_ac  = ac;
    slct->s_av  = av;
    slct->s_dat = ft_calloc(ac, sizeof(int));
    if (!slct->s_dat) {
        return (-1);
    }

    /* for now... */
    slct->s_crxc = slct->s_cryc = 0;
    slct->s_crxm = 1;
    slct->s_crym = ac;
    return (0);
}


extern int ft_select_deinit(t_select *slct) {
    if (!slct) { return (-1); }

    /* print every selected element... */
    for (size_t i = 0; i < (size_t) slct->s_ac; i++) {
        if (slct->s_dat[i]) {
            ft_putendl_fd(slct->s_av[i], 1);
        }
    }

    /* unload resources... */
    free(slct->s_dat), slct->s_dat = 0;

    return (0);
}
