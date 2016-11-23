#!/usr/bin/env python

import curses
import sys
from curses.textpad import Textbox, rectangle


def main():
    stdscr = initialize()  # Starts the Curses application.
    game_running = True
    one_begin_x = 1
    one_begin_y = 15
    two_begin_x = 70
    two_begin_y = 15
    thr_begin_x = 120
    thr_begin_y = 15
    height = 5
    width = 40
    win1 = curses.newwin(height, width, one_begin_y, one_begin_x)
    win2 = curses.newwin(height, width, two_begin_y, two_begin_x)
    win3 = curses.newwin(height, width, thr_begin_y, thr_begin_x)
    print_title(stdscr)
    box1 = Textbox(win1)
    box2 = Textbox(win2)
    box3 = Textbox(win3)
    while game_running:
        stdscr.refresh()  # This line begins the interface logic.
        stdscr.refresh()  # This begins the user interaction
        c = stdscr.getch()
        if c == ord('q'):
            game_running = False  # Exit the while loop
        if c == ord('m'):
            box1.edit()
            stuff = box1.gather()
        if c == ord('c'):
            box2.edit()
            stuff = box2.gather()
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
    stdscr.addstr(14,70,"Chat")
    stdscr.addstr(14,1,"Game Window")
    stdscr.addstr(14,120,"The Board")


if __name__ == "__main__":
    main()
