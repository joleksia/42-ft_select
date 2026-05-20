/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_select.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joleksia <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 15:48:23 by joleksia          #+#    #+#             */
/*   Updated: 2026/05/20 14:08:19 by joleksia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>     /* getenv, malloc, free, exit                         */
#include <unistd.h>     /* isatty, ttyname, ttyslot, open, close, write, read */
#include <termios.h>    /* tcsetattr, tcgetattr                               */
#include <signal.h>     /* signal                                             */
#include <sys/ioctl.h>  /* ioctl                                              */
#
#include "./libft/libft.h"
#
# /* text modes */
#define TEXT_NORMAL             "\033[0m"   /* no select, no cursor */
#define TEXT_UNDERLINE          "\033[4m"   /* no select, cursor    */
#define TEXT_INVERSE            "\033[7m"   /* select, no cursor    */
#define TEXT_UNDERLINE_INVERSE  "\033[4;7m" /* select, cursor       */
#
# /* inputs */
#define FT_KEY_ENTER        '\r'
#define FT_KEY_SPACE        ' '
#define FT_KEY_DELETE       127
#define FT_KEY_BACKSPACE    '\0'
#define FT_KEY_ARROWUP      1000
#define FT_KEY_ARROWDOWN    1001
#define FT_KEY_ARROWRIGHT   1002
#define FT_KEY_ARROWLEFT    1003

/* getopt */

extern int ft_select_getopt(int, char **);

/* tui */

extern int ft_select_tuiclear(void);

extern int ft_select_tuicrsrpos(size_t, size_t);

extern int ft_select_tuimode(int, struct termios *);

/* select */

typedef struct s_select t_select;

struct s_select {
    /* main opts */
    int    s_ac;    /* s_ac - argument count */
    char **s_av;    /* s_av - argument vector */

    /* select */
    int   *s_dat;   /* s_dat - select / unselect data vector */
    int    s_crxc;  /* s_crxc - cursor X position (current) */
    int    s_cryc;  /* s_cryc - cursor Y position (current) */
    size_t s_crxm;  /* s_crxm - cursor X position (max) */
    size_t s_crym;  /* s_crym - cursor Y position (max) */
};

extern int ft_select_init(t_select *, int, char **);

extern int ft_select_update(t_select *, int);

extern int ft_select_render(t_select *);

extern int ft_select_deinit(t_select *);

/* utils */

extern int ft_select_input(void);

extern int ft_select_cursor2index(t_select *);

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
    if (ft_select_tuimode(1, &termios) == -1) {
        return (errno);
    }

    /* create select objects... */
    t_select slct = { 0 };
    ft_select_init(&slct, --ac, ++av);

    /* single render before update-render loop... */
    ft_select_render(&slct);

    /* update-render loop... */
    int exit = 0;
    while (!exit) {
        int c = ft_select_input();
        /* handle 'read' errors... */
        if (c == -1) {
            ft_select_tuimode(0, &termios);
            ft_select_deinit(&slct);
            return (errno);
        }

        /* handle exit... */
        else if (c == 'q') { exit = 1; }

        /* handle update-render... */
        else {
            // printf("%d\n", c);
            ft_select_update(&slct, c);
            ft_select_render(&slct);
        }
    }

    /* disable raw mode... */
    if (ft_select_tuimode(0, &termios) == -1) {
        return (errno);
    }

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

extern int ft_select_tuiclear(void) {
    /* clear */
    ft_putstr_fd("\033[2J", 1);
    return (0);
}


extern int ft_select_tuicrsrpos(size_t x, size_t y) {
    /* [ 0,0 ] pos */
    if (!x && !y) {
        ft_putstr_fd("\033[H", 1);
    }

    return (0);
}


extern int ft_select_tuimode(int enable, struct termios *tptr) {
    /* null check... */
    if (!tptr) { return (0); }
    
    if (enable) {
        /* get the original termios... */
        if (tcgetattr(0, tptr) == 1) { return (-1); }
        
        /* copy termios... */
        struct termios raw = *tptr;
        raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
        raw.c_oflag &= ~(OPOST);
        raw.c_cflag |= (CS8);
        raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 1;
        if (tcsetattr(0, TCSAFLUSH, &raw) == -1) { return (-1); }
    }
    else if (!enable) {
        /* clear the screen... */
        ft_select_tuiclear();
        ft_select_tuicrsrpos(0, 0);
        
        /* set back the original termios... */
        if (tcsetattr(0, TCSAFLUSH, tptr) == -1) { return (-1); }
    }

    return (0);
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

    slct->s_crxc = slct->s_cryc = 0;
    /* for now hard-coded */
    slct->s_crxm = 1;
    slct->s_crym = ac;
    return (0);
}


extern int ft_select_update(t_select *slct, int input) {
    if (!slct) { return (-1); }
   
    /* handle cursor... */
    if (input >= FT_KEY_ARROWUP && input <= FT_KEY_ARROWLEFT) {
        switch (input) {
            case (FT_KEY_ARROWUP):    { slct->s_cryc--; } break;
            case (FT_KEY_ARROWDOWN):  { slct->s_cryc++; } break;
            case (FT_KEY_ARROWRIGHT): { slct->s_crxc--; } break;
            case (FT_KEY_ARROWLEFT):  { slct->s_crxc++; } break;
        }
        slct->s_cryc %= slct->s_crym;
        slct->s_crxc %= slct->s_crxm;

        return (0);
    }
    else {
        /* get cursor index... */ 
        int slct_cridx = ft_select_cursor2index(slct);

        switch (input) {
            /* select / deselect... */
            case (FT_KEY_ENTER):
            case (FT_KEY_SPACE): {
                slct->s_dat[slct_cridx] = !slct->s_dat[slct_cridx];
            } break;
            
            /* deletion... */
            case (FT_KEY_DELETE):
            case (FT_KEY_BACKSPACE): {
                for (int i = slct_cridx; i < slct->s_ac - 1; i++) {
                    slct->s_av[i]  = slct->s_av[i + 1];
                    slct->s_dat[i] = slct->s_dat[i + 1];
                }
                slct->s_ac--;
            } break;

            default: { } break;
        }
    }
    
    return (0);
}


extern int ft_select_render(t_select *slct) {
    if (!slct) { return (-1); }

    /* get cursor index... */ 
    int slct_cridx = ft_select_cursor2index(slct);

    /* clear the screen... */
    ft_select_tuiclear();
    ft_select_tuicrsrpos(0, 0);

    /* render all the entries... */
    for (int i = 0; i < slct->s_ac; i++) {
        /* get print mode... */
        char *mode = TEXT_NORMAL;
        if (slct->s_dat[i]) {
            mode = (i == slct_cridx) ? TEXT_UNDERLINE_INVERSE : TEXT_INVERSE;
        }
        else {
            if (i == slct_cridx) {
                mode = TEXT_UNDERLINE;
            }
        }

        /* apply style*/
        ft_putstr_fd(mode, 1);

        /* print entry */
        ft_putstr_fd(slct->s_av[i], 1);
        
        /* reset style */
        ft_putendl_fd(TEXT_NORMAL, 1);
    }
    
    ft_select_tuicrsrpos(0, 0);
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


/* utils */

extern int ft_select_input(void) {
    /* input "buffer"... */
    char c = 0;

    /* number of bytes read... */
    ssize_t nread = 0;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno == EAGAIN) {
            return (-1);
        }
    }

    /* process escape sequences... */
    if (c == '\x1b') {
        char seq[3] = { 0 };

        if (read(STDIN_FILENO, &seq[0], 1) != 1) { return ('\x1b'); }
        if (read(STDIN_FILENO, &seq[1], 1) != 1) { return ('\x1b'); }
        if (seq[0] == '[') {
            /* handle arrows... */
            switch (seq[1]) {
                case ('A'): { return (FT_KEY_ARROWUP); }
                case ('B'): { return (FT_KEY_ARROWDOWN); }
                case ('C'): { return (FT_KEY_ARROWRIGHT); }
                case ('D'): { return (FT_KEY_ARROWLEFT); }
            }
        }

        else {

        }

        return ('\x1b');
    }

    return (c);
}


extern int ft_select_cursor2index(t_select *slct) {
    if (!slct) { return (-1); }

    return (slct->s_cryc * slct->s_crxm + slct->s_crxc);
}
