# !/usr/bin/env python

import curses
import sys
import threading
import logging
from curses.textpad import Textbox
import socket
from socket import htonl
from socket import ntohl
import struct
import ctypes


class Chat(threading.Thread):
    def __init__(self, win):
        super().__init__()
        self.win = win

    def update(self):
        # Get a chat message.
        # Add the chat message to win.
        self.win.refresh()


# define screen variables
class Screen(object):
    def __init__(self, height, width, one_begin_x, one_begin_y, two_begin_x, two_begin_y, thr_begin_x, thr_begin_y):
        # Window 1 takes commands for the game.
        self.win1 = curses.newwin(height, width, one_begin_y, one_begin_x)
        # Window 2 carries the chat.
        self.win2 = curses.newwin(((3 * height) // 4), width, two_begin_y, two_begin_x)
        # Window 3 displays the current game board.
        self.win3 = curses.newwin(66, 66, thr_begin_y, thr_begin_x)
        # Window 4 displays the message that the player is currently typing out.
        self.win4 = curses.newwin((height // 4), width,
                                  ((two_begin_y) + ((3 * height) // 4)), two_begin_x)
        self.box1 = Textbox(self.win1)
        self.box2 = Textbox(self.win4)
        self.chat = Chat(self.win2)

    def print_title(self, stdscr):
        stdscr.addstr(0, 0, "GGGGGGGGGG OOOOOOOOOO MMM MMM MMM OOOOOOOOOO KK      KK UU     UU")
        stdscr.addstr(1, 0, "GG         OO      OO MMM MMM MMM OO      OO KK     KK  UU     UU")
        stdscr.addstr(2, 0, "GG         OO      OO MM M   M MM OO      OO KK    KK   UU     UU")
        stdscr.addstr(3, 0, "GG         OO      OO MM M   M MM OO      OO KK   KK    UU     UU")
        stdscr.addstr(4, 0, "GG         OO      OO MM M   M MM OO      OO KK KK      UU     UU")
        stdscr.addstr(5, 0, "GG         OO      OO MM M   M MM OO      OO KKK        UU     UU")
        stdscr.addstr(6, 0, "GG   GGGG  OO      OO MM M   M MM OO      OO KK KK      UU     UU")
        stdscr.addstr(7, 0, "GG   GGGG  OO      OO MM M   M MM OO      OO KK  KK     UU     UU")
        stdscr.addstr(8, 0, "GG     GG  OO      OO MM M   M MM OO      OO KK   KK    UU     UU")
        stdscr.addstr(9, 0, "GG     GG  OO      OO MM M   M MM OO      OO KK    KK   UU     UU")
        stdscr.addstr(10, 0, "GG     GG  OO      OO MM M   M MM OO      OO KK     KK   UU   UU")
        stdscr.addstr(11, 0, "GGGGGGGGG  OOOOOOOOOO MM M   M MM OOOOOOOOOO KK      KK   UUUUU")
        stdscr.addstr(14, 70, "Chat")
        stdscr.addstr(14, 1, "Game Window")
        stdscr.addstr(14, 120, "The Board")


class GIPS(object):
    def __init__(self, sock):
        logging.debug("GIPS.sock is being defined.")
        self.sock = sock
        logging.debug(self.sock)
        self.pid = 0
        self.isWin = 0
        self.move_a = -1
        self.move_a = -1
        self.isEarlyExit = 0

    '''
    def unpack(self):
        self.is_win = 0
        try:
            t = struct.unpack('ccccc', self.gips)
            self.pid = int.from_bytes(t[0], byteorder='big')
            self.is_win = int.from_bytes(t[1], byteorder='big')
            self.move_a = int.from_bytes(t[2], byteorder='big')
            self.move_a = int.from_bytes(t[3], byteorder='big')
            self.isEarlyExit = int.from_bytes(t[4], byteorder='big')
            if self.move_a == 255:
                self.move_a = -1
            if self.move_b == 255:
                self.move_b = -1
        except:
            logging.critical("socket.recv call DID NOT BLOCK")
            logging.exception("Exception text")
    '''

    def pack(self, pid, isWin, move_a, move_b, isEarlyExit):
        logging.debug("Packing: " + str(pid) + " " + str(isWin) +
                      " " + str(move_a) + " " + str(move_b))
        self.isWin = isWin
        self.pid = pid
        self.move_a = move_a
        self.move_b = move_b
        self.isEarlyExit = isEarlyExit

    def send(self):
        self.sock.send(struct.pack('ccccc', int(self.pid).to_bytes(1, sys.byteorder), \
                                   int(self.isWin).to_bytes(1, sys.byteorder), \
                                   int(self.move_a).to_bytes(1, sys.byteorder), \
                                   int(self.move_b).to_bytes(1, sys.byteorder), \
                                   int(self.isEarlyExit).to_bytes(1, sys.byteorder)))

    def recv(self):
        self.pid = ord(self.sock.recv(1))
        self.isWin = ord(self.sock.recv(1))
        self.move_a = ord(self.sock.recv(1))
        self.move_b = ord(self.sock.recv(1))
        self.isEarlyExit = ord(self.sock.recv(1))
        if self.move_a == 255:
            self.move_a = -1
        if self.move_b == 255:
            self.move_b = -1
        logging.debug("Received: " + str(self))


def main():
    logging.basicConfig(filename='log.txt', level=logging.DEBUG,
                        format='[%(asctime)-15s] %(message)s LINE: %(lineno)d')
    host = "localhost"
    port = 32200
    logging.info("Trying to connect on " + str(host) + ":" + str(port))
    print("Welcome to GOMOKU")
    print("USERNAME")
    username = input("> ")
    print("PLAYER ID NUMBER")
    upid = input("> ")
    # Login with our unique pid.
    # Talk to the server and see what we can get.
    # Get a chat_socket
    # Get your player number from the server.
    stdscr = initialize()  # Starts the Curses application.
    try:
        logging.warning("Trying to connect to the server.")
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        logging.warning("Socket created, connecting... " + str(sock))
        sock.connect((host, port))
        logging.warning("Socket issue check: " + str(sock))
    except Exception:
        logging.critical("Server could not be reached!")
        down(stdscr)
        print("Couldn't connect to the server. Check your internet connection and try again.")
        sys.exit(0)
    upid = login(sock, upid, username)
    pid = ord(sock.recv(1))
    logging.debug("Received PID: " + str(pid))
    logging.debug("Pointer to sock: " + str(sock))
    board = init_board()
    # height/width/one_begin_x/one_begin_y/etc
    # GOOD UP TO HERE (With send/recv)
    screen = Screen(40, 40, 1, 15, 70, 15, 121, 15)
    screen.print_title(stdscr)

    logging.debug("Game starting.")
    print("game starting")
    try:
        gips = GIPS(sock)
        logging.debug("The GIPS is defined.")
        gips = gameLoop(board, pid, username, screen, stdscr, sock, gips)
        down(stdscr)
        sys.exit(0)
    except Exception:
        logging.exception("Exception caught")
        down(stdscr)  # Breaks the application down and ends it.
        sys.exit(0)
    except KeyboardInterrupt:
        logging.exception("SIGINT received")
        down(stdscr)
        sys.exit(0)


def gameLoop(board, pid, username, screen, stdscr, sock, gips):
    game_running = True
    while gips.isWin == 0 and gips.isEarlyExit == 0 and game_running:
        logging.debug("Starting the loop.")
        gips.recv()
        logging.debug("Received a GIPS packet.")
        # Check if someone won.
        display_board(board, screen.win3)
        logging.debug("Current value of winner: " + str(gips.isWin))
        if gips.move_a == -1 and gips.move_b == -1:
            pass
        else:  # update board
            board = update_board(gips, board)
            stdscr.refresh()  # This line begins the interface logic.
            display_board(board, screen.win3)
            stdscr.refresh()  # This begins the user interaction
            c = stdscr.getch()
            game_running = checkKeys(c, screen, stdscr, gips, board, pid, sock, username)
    return gips


def checkKeys(c, screen, stdscr, gips, board, pid, sock, username):
    if c == ord('q'):
        return False
    if c == ord('m'):
        # Get the next move and send it.
        screen.box1.edit()
        stuff = screen.box1.gather()
        # Split the move into two components.
        move = (str(stuff)).split(' ')
        move.remove('\n')  # kill the newline
        for m in move:
            m = int(m)
            # Check move validity.
        # If the move is not valid:
        if not move_is_valid(move):
            # Send 'invalid move' to chat.
            send_to_chat(sock, "server: Invalid move, "
                         + str(username) + "!")
        else:
            # Otherwise:
            # Encode a GIPS
            gips.pack(pid, gips.isWin, move[0], move[1], 0)
            # Send the GIPS
            gips.send()
            board = update_board(gips, board)
            display_board(board, screen.win3)
    if c == ord('c'):
        screen.box2.edit()
        stuff = screen.box2.gather()
        message = str(username) + ": " + str(stuff)
        # Send message to the server as a bytestring.
        send_to_chat(sock, message)
        stdscr.refresh()  # Redraws the screen.


def login(sock, upid, username):
    upid = int(upid)
    sock.send(struct.pack('!I', upid))
    send_string(sock, username)
    upid = sock.recv(4)
    upid = struct.unpack('@I', upid)
    upid = ntohl(upid[0])
    logging.debug(str(upid))
    return upid


def send_string(sock, string):
    string = bytes(string, 'utf-8')
    sent = sock.send(string)
    if sent != (len(string)):
        logging.warning("Bytes sent DID NOT MATCH")


def send_to_chat(sock, message):
    logging.debug(str(message) + " to chat")


def update_board(gips, board):
    logging.debug("Updating to the next board.")
    logging.debug("move_x: " + str(gips.move_a))
    logging.debug("move_y: " + str(gips.move_b))
    if gips.move_a == -1 or gips.move_b == -1:
        logging.debug("Returning board unchanged.")
        return board
    if gips.pid == 1:
        board[gips.move_a][gips.move_b] = 'B'
    elif gips.pid == 2:
        board[gips.move_a][gips.move_b] = 'W'
    logging.debug("Returning updated board.")
    return board


def move_is_valid(move):
    logging.debug("Move: " + str(move))
    if 8 > int(move[0]) > 0:
        if 8 > int(move[1]) > 0:
            return True
        else:
            return False
    else:
        return False


def display_board(board, win):
    x = 1
    y = 1
    logging.debug(board)
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
    return stdscr


def down(stdscr):
    curses.nocbreak()
    stdscr.keypad(False)
    curses.echo()
    curses.endwin()
    import doctest
    doctest.testmod()


if __name__ == "__main__":
    main()
