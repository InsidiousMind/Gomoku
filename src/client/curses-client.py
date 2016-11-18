#!/usr/bin/env python

import curses
import sys


def main():
    stdscr = initialize()  # Starts the Curses application.
    game_running = True
    one_begin_x = 1
    one_begin_y = 12
    two_begin_x = 70
    two_begin_y = 12
    height = 5
    width = 40
    win1 = curses.newwin(height, width, one_begin_y, one_begin_x)
    win2 = curses.newwin(height, width, two_begin_y, two_begin_x)
    print_title(stdscr)
    stdscr.move(13,2)
    while game_running:
        c = stdscr.getch()
        if c == ord('q'):
            game_running = False  # Exit the while loop
        stdscr.refresh() # Redraws the screen.
    down(stdscr)  # Breaks the application down and ends it.


def initialize():
    stdscr = curses.initscr()
    stdscr.clear()
    curses.noecho()
    curses.cbreak()
    stdscr.keypad(True)
    return stdscr


def down(stdscr):
    curses.nocbreak()
    stdscr.keypad(False)
    curses.echo()
    curses.endwin()
    sys.exit(0)


def print_title(stdscr):
    stdscr.addstr(0,0,"GGGGGGGGGG OOOOOOOOOO MMM MMM MMM OOOOOOOOOO KK      KK UU     UU")
    stdscr.addstr(1,0,"GG         OO      OO MMM MMM MMM OO      OO KK     KK  UU     UU")
    stdscr.addstr(2,0,"GG         OO      OO MM M   M MM OO      OO KK    KK   UU     UU")
    stdscr.addstr(3,0,"GG         OO      OO MM M   M MM OO      OO KK   KK    UU     UU")
    stdscr.addstr(4,0,"GG         OO      OO MM M   M MM OO      OO KK KK      UU     UU")
    stdscr.addstr(5,0,"GG         OO      OO MM M   M MM OO      OO KKK        UU     UU")
    stdscr.addstr(6,0,"GG   GGGG  OO      OO MM M   M MM OO      OO KK KK      UU     UU")
    stdscr.addstr(7,0,"GG   GGGG  OO      OO MM M   M MM OO      OO KK  KK     UU     UU")
    stdscr.addstr(8,0,"GG     GG  OO      OO MM M   M MM OO      OO KK   KK    UU     UU")
    stdscr.addstr(9,0,"GG     GG  OO      OO MM M   M MM OO      OO KK    KK   UU     UU")
    stdscr.addstr(10,0,"GG     GG  OO      OO MM M   M MM OO      OO KK     KK   UU   UU")
    stdscr.addstr(11,0,"GGGGGGGGG  OOOOOOOOOO MM M   M MM OOOOOOOOOO KK      KK   UUUUU")

if __name__ == "__main__":
    main()
