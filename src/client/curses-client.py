#!/usr/bin/env python

import curses
import sys
import threading
import logging
from curses.textpad import Textbox
import socket, struct, sys, time
from socket import htonl
from socket import ntohl
import struct
import ctypes
import time
import traceback
import random


class Chat(threading.Thread):
    def __init__(self, win, sock):
        super().__init__()
        self.win = win
        self.row = 0
        self.col = 0

    def update(self):
        # Get a chat message.
        msg = self.sock.recv(1024)
        win.addstr(self.row, self.col, msg)
        self.row += 1
        # Add the chat message to win.
        self.win.refresh()

    def run(self):
        while True:
            time.sleep(1)
            self.update()


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
        self.game = Textbox(self.win1)
        self.board_mesg = Textbox(self.win4)
        # self.chat = Chat(self.win2)

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
        self.ack = bytearray()
        logging.debug(self.sock)
        self.pid = 0
        self.isWin = 0
        self.move_a = -1
        self.move_b = -1
        self.isEarlyExit = 0

    def pack(self, pid, isWin, move_a, move_b, isEarlyExit):
        logging.debug("Packing: " + str(pid) + " " + str(isWin) +
                      " " + str(move_a) + " " + str(move_b))
        self.isWin = isWin
        self.pid = pid
        self.move_a = move_a
        self.move_b = move_b
        self.isEarlyExit = isEarlyExit

    def send(self):
        logging.debug("Sending.")
        self.sock.send(struct.pack('ccccc', int(self.pid).to_bytes(1, sys.byteorder), \
                                   int(self.isWin).to_bytes(1, sys.byteorder), \
                                   int(self.move_a).to_bytes(1, sys.byteorder), \
                                   int(self.move_b).to_bytes(1, sys.byteorder), \
                                   int(self.isEarlyExit).to_bytes(1, sys.byteorder)))

    def recv(self):
        self.sock.setblocking(True)
        data = bytearray()
        rand = self.sock.recv(50)
        while(not data):
            data = self.recv_timeout()
        data = list(map(int, data))
        for a in data:
            if a != 0:
                break
            else:
                data.remove(a)
        self.pid = data[0]
        self.isWin = data[1]
        self.move_a = data[2]
        self.move_b = data[3]
        self.isEarlyExit = data[4]
        if self.move_a == 255:
            self.move_a = -1
        if self.move_b == 255:
            self.move_b = -1
        logging.debug("Received: " + str(self))

    ## timeout after receiving for a little bit
    def recv_timeout(self, timeout=2):
        self.sock.setblocking(False)
        total_data = bytearray()
        data = bytes()
        begin = time.time()
        while 1:
            # if you got some data, then break after wait sec
            if total_data and time.time() - begin > timeout:
                break
            # if you got no data at all, wait a little longer
            elif time.time() - begin > timeout * 2:
                break
            try:
                data = self.sock.recv(8192)
                if data:
                    total_data.extend(data)
                    begin = time.time()
                else:
                    time.sleep(0.1)
            except:
                pass
        return total_data


def main():
    file_id = str(random.randrange(1000))
    logging.basicConfig(filename='log' + file_id + '.txt', level=logging.DEBUG,
            format='[%(asctime)-15s] %(message)s PID: %(process)d LINE: %(lineno)d')
    logging.debug("Client initializing!")
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
        halt(stdscr)
    except Exception:
        logging.exception("Exception caught")
        halt(stdscr)
    except KeyboardInterrupt:
        logging.exception("SIGINT received")
        halt(stdscr)


def halt(stdscr):  # Just to cut down on a few lines in main()
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
            logging.debug("Placeholder move.")
            pass
        else:  # update board
            logging.debug("Updating the board after the recv call.")
            board = update_board(gips, board)
        stdscr.refresh()  # This line begins the interface logic.
        display_board(board, screen.win3)
        stdscr.refresh()  # This begins the user interaction
        c = stdscr.getch()
        game_running = checkKeys(c, screen, stdscr, gips, board, pid, sock, username)
    return gips


def checkKeys(c, screen, stdscr, gips, board, pid, sock, username):
    logging.debug("checkKeys")
    if c == ord('q'):
        logging.debug("Key: q")
        return False
    if c == ord('m'):
        logging.debug("Key: m")
        # Get the next move and send it.
        screen.game.edit()
        stuff = screen.game.gather()
        # Split the move into two components.
        move = (str(stuff)).split(' ')
        move.remove('\n')  # kill the newline=
        for m in move:
            m = int(m)
        # If the move is not valid:
        #make moves ints
        move = list(map(int, move))
        while not move_is_valid(move):
            send_to_chat(sock, "server: Invalid move, " + str(username) + "!")
        #subtract 1 from moves
        move[len(move)-1] -= 1
        move[(len(move)-2)] -= 1
        # Otherwise:
        # Encode a GIPS
        gips.pack(pid, gips.isWin, move[len(move)-2], move[len(move)-1], 0)
        # Send the GIPS
        gips.send()
        board = update_board(gips, board)
        display_board(board, screen.win3)
        return True
    if c == ord('c'):
        screen.board_mesg.edit()
        stuff = screen.board_mesg.gather()
        message = str(username) + ": " + str(stuff)
        # Send message to the server as a bytestring.
        send_to_chat(sock, message)
        stdscr.refresh()  # Redraws the screen.
        return True


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
    sock.send(bytes(message, 'utf-8'))


def update_board(gips, board):
    logging.debug("Updating to the next board.")
    logging.debug("move_x: " + str(gips.move_a))
    logging.debug("move_y: " + str(gips.move_b))
    #because of the way curses displays the board
    #board[b][a] so not inverted
    if gips.pid == 1:
        board[int(gips.move_b)][int(gips.move_a)] = 'B'
    elif gips.pid == 2:
        board[int(gips.move_b)][int(gips.move_a)] = 'W'
    logging.debug("Returning updated board.")
    return board


def move_is_valid(move):
    logging.debug("Move: " + str(move))
    if 9 > int(move[0]) > 0:
        if 9 > int(move[1]) > 0:
            logging.debug("Valid")
            return True
        else:
            logging.debug("Invalid")
            return False
    else:
        logging.debug("Invalid.")
        return False


def display_board(board, win):
    x = 1
    y = 1
    logging.debug("board")
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
    logging.debug("Breaking down the application.")
    curses.nocbreak()
    stdscr.keypad(False)
    curses.echo()
    curses.endwin()
    import doctest
    doctest.testmod()


if __name__ == "__main__":
    main()
