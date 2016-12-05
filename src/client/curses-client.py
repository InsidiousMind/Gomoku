# !/usr/bin/env python

import curses
import sys
import threading
import logging
from curses.textpad import Textbox
import socket
import struct
import ctypes
import time


class Chat(threading.Thread):
    def __init__(self, win):
        super().__init__()
        self.win = win

    def update(self):
        # Get a chat message.
        # Add the chat message to win.
        self.win.refresh()

    def run(self):
        time.sleep(1)


class GIPS(object):
    def __init__(self, sock):
        logging.debug("GIPS.sock is being defined.")
        self.sock = sock
        logging.debug(self.sock)
        self.pid = 0
        self.is_win = 0
        self.move_x = -1
        self.move_y = -1

    def unpack(self):
        self.is_win = 0
        try:
            t = struct.unpack('cccc', self.gips)
            self.pid = int.from_bytes(t[0], byteorder='big')
            self.is_win = int.from_bytes(t[1], byteorder='big')
            self.move_x = int.from_bytes(t[2], byteorder='big')
            self.move_y = int.from_bytes(t[3], byteorder='big')
            if self.move_x == 255:
                self.move_x = -1
            if self.move_y == 255:
                self.move_y = -1
        except:
            logging.critical("socket.recv call DID NOT BLOCK")
            logging.exception("Exception text")

    def pack(self, pid, is_win, move_x, move_y):
        logging.debug("Packing: " + str(pid) + " " + str(is_win) +
                      " " + str(move_x) + " " + str(move_y))
        self.is_win = is_win
        self.pid = pid
        self.move_x = move_x
        self.move_y = move_y
        try:
            self.gips = struct.pack('cccc',
                                    ctypes.c_char(int.from_bytes(pid,
                                                                 byteorder="big")),
                                    ctypes.c_char(is_win),
                                    ctypes.c_char(move_x),
                                    ctypes.c_char(move_y))
        except:
            logging.debug("Pack did not work even remotely.")

    def send(self):
        self.sock.send(self.gips)

    def recv(self):
        self.gips = self.sock.recv(5)
        logging.debug("Received: " + str(self.gips))


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
    pid = input("> ")
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
    upid = login(sock, pid, username)
    pid = sock.recv(1)
    logging.debug("Received PID: " + str(pid))
    logging.debug("Pointer to sock: " + str(sock))
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
    # logging.debug("Game starting.")
    print("game starting")
    try:
        while game_running:
            # Receive a GIPS
            logging.debug("Starting the loop.")
            gips = GIPS(sock)
            logging.debug("The GIPS is defined.")
            gips.recv()
            logging.debug("Received a GIPS packet.")
            # Decode the gips.
            gips.unpack()
            # Check if someone won.
            display_board(board, win3)
            logging.debug("Current value of winner: " + str(gips.is_win))
            if gips.is_win == 0:
                logging.debug("Game continuing.")
                pass
            elif gips.is_win == pid:
                game_running = False
                print("You win!")
                logging.warning("This client won.")
                break
            elif gips.is_win != 0 and gips.is_win != pid:
                game_running = False
                print("You lose.")
                logging.warning("This client lost.")
                break
            elif gips.move_x == -1 and gips.move_y == -1:
                logging.warning("This client received an invalid move.")
            else:
                pass  # Keep doing your thing buddy you're doing great
            # Else update the board.
            board = update_board(gips, board)
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
                move.remove('\n')
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
                    gips.pack(pid, gips.is_win, move[0], move[1])
                    # Send the GIPS
                    gips.send()
                    board = update_board(gips, board)
                    display_board(board, win3)
            if c == ord('c'):
                box2.edit()
                stuff = box2.gather()
                message = str(username) + ": " + str(stuff)
                # Send message to the server as a bytestring.
                send_to_chat(sock, message)
                stdscr.refresh()  # Redraws the screen.
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


def login(sock, upid, username):
    sock.send(struct.pack("I", int(upid)))
    send_string(sock, username)
    upid = sock.recv(4)
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
    logging.debug("move_x: " + str(gips.move_x))
    logging.debug("move_y: " + str(gips.move_y))
    if gips.move_x == -1 or gips.move_y == -1:
        logging.debug("Returning board unchanged.")
        return board
    if gips.pid == 1:
        board[gips.move_x][gips.move_y] = 'B'
    elif gips.pid == 2:
        board[gips.move_x][gips.move_y] = 'W'
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


def print_title(stdscr):
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


if __name__ == "__main__":
    main()
