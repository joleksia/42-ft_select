/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_select.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joleksia <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 15:48:23 by joleksia          #+#    #+#             */
/*   Updated: 2026/10/23 13:11:48 by joleksia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>
#
#include "./libft/libft.h"

/* getopt */

extern int ft_select_getopt(int, char **);

/* input */

typedef enum ft_select_input_e ft_select_input_t;

enum ft_select_input_e {
    
    FT_SELECT_ARROW_UP    = ('\x1b' << 16) | ('[' << 8) | ('A' << 0),
    FT_SELECT_ARROW_DOWN  = ('\x1b' << 16) | ('[' << 8) | ('B' << 0),
    FT_SELECT_ARROW_RIGHT = ('\x1b' << 16) | ('[' << 8) | ('C' << 0),
    FT_SELECT_ARROW_LEFT  = ('\x1b' << 16) | ('[' << 8) | ('D' << 0),
    
    FT_SELECT_F1 = ('\x1b' << 24) | ('[' << 16) | ('O' << 8) | ('P' << 0),
    FT_SELECT_F2 = ('\x1b' << 24) | ('[' << 16) | ('O' << 8) | ('Q' << 0),
    FT_SELECT_F3 = ('\x1b' << 24) | ('[' << 16) | ('O' << 8) | ('R' << 0),
    FT_SELECT_F4 = ('\x1b' << 24) | ('[' << 16) | ('O' << 8) | ('S' << 0),

    FT_SELECT_F5 = ('\x1b' << 24) | ('[' << 16) | ('1' << 8) | ('5' << 0),
    FT_SELECT_F6 = ('\x1b' << 24) | ('[' << 16) | ('1' << 8) | ('7' << 0),
    FT_SELECT_F7 = ('\x1b' << 24) | ('[' << 16) | ('1' << 8) | ('8' << 0),
    FT_SELECT_F8 = ('\x1b' << 24) | ('[' << 16) | ('1' << 8) | ('9' << 0),
    FT_SELECT_F9 = ('\x1b' << 24) | ('[' << 16) | ('2' << 8) | ('0' << 0),
    
    FT_SELECT_HOME     = ('\x1b' << 24) | ('[' << 16) | ('1' << 8) | ('~' << 0),
    FT_SELECT_INSERT   = ('\x1b' << 24) | ('[' << 16) | ('2' << 8) | ('~' << 0),
    FT_SELECT_DELETE   = ('\x1b' << 24) | ('[' << 16) | ('3' << 8) | ('~' << 0),
    FT_SELECT_END      = ('\x1b' << 24) | ('[' << 16) | ('4' << 8) | ('~' << 0),
    FT_SELECT_PAGEUP   = ('\x1b' << 24) | ('[' << 16) | ('5' << 8) | ('~' << 0),
    FT_SELECT_PAGEDOWN = ('\x1b' << 24) | ('[' << 16) | ('6' << 8) | ('~' << 0),

};


extern int ft_select_input(void);

/* exec */

extern int ft_select_exec(const char *, const size_t);

/* main */

int main(int ac, char **av) {
    /* process options... */
    ft_select_getopt(ac, av);

    /* terminal configuration */
    struct
    termios t_default = { 0 };

    struct
    termios t_rawmode = { 0 };

    tcgetattr(STDIN_FILENO, &t_default);
    tcgetattr(STDIN_FILENO, &t_rawmode);
    t_rawmode.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    t_rawmode.c_oflag &= ~(OPOST);
    t_rawmode.c_cflag |=  (CS8);
    t_rawmode.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    t_rawmode.c_cc[VMIN]  = 0;
    t_rawmode.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &t_rawmode);

    /* execute commands */
    ft_select_exec("\x1b[?1049h", 8); /* alt. buffer */
    ft_select_exec("\x1b[2J",     4); /* clear buffer */
    ft_select_exec("\x1b[H",      4); /* move cursor to home position */

    /* update loop */
    int exit = 0;
    int stat = 0;
    while (!exit) {
        /* update */
        /* ... */

        /* render */
        ft_select_exec("\x1b[2J", 4); /* clear buffer */
        ft_select_exec("\x1b[H",  4); /* move cursor to home position */

        /* input */
        int c = ft_select_input();
        switch (c) {
            /* case: 'error' */
            case (-1): {
                stat = errno;
            };

            /* case: 'exit' */
            case ('\x1b'): {
                exit = true;
            } break;

            /* case: 'input' */
            default: {
                /* ... */
            } break;
        }
    }

    /* leave alternative buffer mode */    
    write(STDOUT_FILENO, "\x1b[?1049l", 8);

    /* terminal configuration */
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &t_default);
   
    /* exit */
    return (stat);
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

    return (0);
}

/* input */

extern int ft_select_input(void) {
    /* read the 'input' from 'STDIN_FILENO' */
    char input = 0; ssize_t nread = 0;
    while ((nread = read(STDIN_FILENO, &input, 1)) != 1) {
        if (nread == -1 && errno == EAGAIN) {
            return (-1);
        }
    }

    /* process 'input' */
    if (input == '\x1b') {
        /* read the following excape 'sequance' */
        char sequance[8] = { 0 };
        if (read(STDIN_FILENO, &sequance[0], 1) != 1) { return ('\x1b'); }
        if (read(STDIN_FILENO, &sequance[1], 1) != 1) { return ('\x1b'); }
        if (sequance[0] == '[') {
            if (sequance[1] > '0' && sequance[1] < '9') {
                if (read(STDIN_FILENO, &sequance[2], 1) != 1) { return ('\x1b'); }
                if (sequance[2] == '~') {
                    switch (sequance[1]) { 

                        /* HOME: "\x1b[1~" */
                        case ('1'): { return (FT_SELECT_HOME); }

                        /* INSERT: "\x1b[2~" */
                        case ('2'): { return (FT_SELECT_INSERT); }

                        /* DELETE: "\x1b[3~" */
                        case ('3'): { return (FT_SELECT_DELETE); }

                        /* END: "\x1b[4~" */
                        case ('4'): { return (FT_SELECT_END); }

                        /* PAGEUP: "\x1b[5~" */
                        case ('5'): { return (FT_SELECT_PAGEUP); }

                        /* PAGEDOWN: "\x1b[6~" */
                        case ('6'): { return (FT_SELECT_PAGEDOWN); }

                    }
                }
            }

            else {
                switch (sequance[1]) {

                    /* UP: "\x1b[A" */
                    case ('A'): { return (FT_SELECT_ARROW_UP); }

                    /* DOWN: "\x1b[B" */
                    case ('B'): { return (FT_SELECT_ARROW_DOWN); }

                    /* RIGHT: "\x1b[C" */
                    case ('C'): { return (FT_SELECT_ARROW_RIGHT); }

                    /* LEFT: "\x1b[D" */
                    case ('D'): { return (FT_SELECT_ARROW_LEFT); }

                }
            }
        }

        else if (sequance[0] == 'O') {
            switch (sequance[1]) { }
        }
       
        /* return 'escape' */
        return ('\x1b');
    }

    /* return 'input' */
    return (input);
}

/* exec */

extern int ft_select_exec(const char *cmd, const size_t siz) {
    /* null-check */
    if (!cmd) { return (-1); }
    if (!siz) { return (-1); }

    /* run 'cmd' */
    ssize_t nwrite = 0;
    if ((nwrite = write(STDOUT_FILENO, cmd, siz)) != (ssize_t) siz) {
        return (errno);
    }

    /* success */
    return (0);
}
