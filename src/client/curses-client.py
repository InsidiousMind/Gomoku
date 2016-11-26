#!/usr/bin/env python

import curses
import sys
import threading
import logging
from curses.textpad import Textbox
import socket

class Chat (threading.Thread):
    def __init__(self, win):
        self.win = win

    def update():
        # Get a chat message.
        # Add the chat message to win.
        self.win.refresh()


class GIPS (object):
    def __init__(self, gips):
        self.is_win = 0


def main():
    host = "127.0.0.1"
    port1 = 32200
    port2 = 32201
    print("WELCOME TO GOMOKU")
    print("USERNAME")
    username = input("> ")
    print("PLAYER ID NUMBER")
    unq_pid = input("> ")
    # Login with our unique pid.
    # Talk to the server and see what we can get.
    # Get a chat_socket
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        chat_socket = s.connect((host, port1))
        game_socket = s.connect((host, port2))
    # Get your player number from the server.
    pid = 0
    stdscr = initialize()  # Starts the Curses application.
    board = init_board()
    game_running = True
    one_begin_x = 1
    one_begin_y = 15
    two_begin_x = 70
    two_begin_y = 15
    thr_begin_x = 121
    thr_begin_y = 15
    height = 40
    width = 40
    # Window 1 takes commands for the game.
    win1 = curses.newwin(height, width, one_begin_y, one_begin_x)
    # Window 2 carries the chat.
    win2 = curses.newwin(((3 * height) // 4), width, two_begin_y, two_begin_x)
    # Window 3 displays the current game board.
    win3 = curses.newwin(66, 66, thr_begin_y, thr_begin_x)
    # Window 4 displays the message that the player is currently typing out.
    win4 = curses.newwin((height // 4), width,
                         ((two_begin_y) + ((3 * height) // 4)), two_begin_x)
    print_title(stdscr)
    box1 = Textbox(win1)
    box2 = Textbox(win4)
    chat = Chat(win2)
    print("Game starting.")
    try:
        while game_running:
            # Receive a GIPS
            gips = recv_gips(game_socket)
            print("Received a GIPS packet.")
            # Decode the gips.
            pack = decode_gips(gips)
            # Check if someone won.
            if pack.is_win is 0:
                print("Game continuing.")
                pass
            elif pack.is_win is pid:
                game_running = False
                print("You win!")
            else:
                game_running = False
                print("You lose.")
            # Else update the board.
            board = update_board(pack, board)
            stdscr.refresh()  # This line begins the interface logic. 
            display_board(board, win3)
            stdscr.refresh()  # This begins the user interaction
            c = stdscr.getch()
            if c == ord('q'):
                game_running = False  # Exit the while loop
            if c == ord('m'):
                # Get the next move and send it.
                box1.edit()
                stuff = box1.gather()
                # Split the move into two components.
                move = (str(stuff)).split(' ')
                # Check move validity.
                # If the move is not valid:
                if not move_is_valid(move):
                    # Send 'invalid move' to chat.
                    send_to_chat(chat_socket, "server: Invalid move, "
                                 + str(username) + "!")
                else:
                    # Otherwise:
                    # Encode a GIPS
                    gips = encode_gips(username, pid, move)
                    # Send the GIPS
                    send_gips(game_socket, gips)
            if c == ord('c'):
                box2.edit()
                stuff = box2.gather()
                message = str(username) + ": " + str(stuff)
                # Send message to the server as a bytestring.
                send_to_chat(chat_socket, message)
            stdscr.refresh() # Redraws the screen.
    except Exception as e:
        logging.exception("Exception caught")
        down(stdscr)
    down(stdscr)  # Breaks the application down and ends it.


def encode_gips(username, pid, move):
    print("Encoding GIPS object to byte struct")
    pass


def decode_gips(gips):
    print("Decoding packet")
    return GIPS(gips)


def send_gips(sock, gips):
    print("Sending a gips")
    pass


def recv_gips(sock):
    print("Waiting for a GIPS")
    pass


def send_to_chat(sock, message):
    print(str(messge) + " to chat")
    pass


def update_board(gips, board):
    print("Updating to the next board.")
    return board


def move_is_valid(move):
    if int(move[0]) < 8 and int(move[0]) > 0:
        if int(move[1]) < 8 and int(move[1]) > 0:
            return True
        else:
            return False
    else:
        return False


def display_board(board, win):
    x = 1
    y = 1
    for a in board:
        for b in a:
            win.addch(y, x, ord(b))
            y += 2
        y = 1
        x += 4
    win.refresh()


def init_board():
    '''
    >>> board = init_board()
    >>> board[0][0]
    'o'
    '''
    return [['o' for x in range(8)] for y in range(8)]


def initialize():
    stdscr = curses.initscr()
    stdscr.clear()
    curses.noecho()
    curses.cbreak()
    stdscr.keypad(True)
    sys.stdout = open("log.txt", "a")
    sys.stderr = open("log.txt", "a")
    print("")
    print("****************************************************************")
    print("New client started")
    print("****************************************************************")
    return stdscr


def down(stdscr):
    curses.nocbreak()
    stdscr.keypad(False)
    curses.echo()
    curses.endwin()
    import doctest
    doctest.testmod()
    print("****************************************************************")
    print("Client exiting.")
    print("****************************************************************")
    print("")
    sys.stdout.close()
    sys.stdout.close()
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
