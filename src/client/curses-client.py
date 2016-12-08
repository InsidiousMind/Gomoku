#!/usr/bin/env python

import curses
import logging
import random
import socket
import struct
import sys
import threading
import time
from curses.textpad import Textbox
from socket import ntohl


class Player(object):
    def __init__(self, name, upid, wins, losses, ties):
        self.name = name
        self.upid = upid
        self.wins = wins
        self.losses = losses
        self.ties = ties

    def print_player(self):
        print('{0} UPID: {1} has {2} wins {3} losses  and {4} ties.'.format(str(self.name), str(self.upid),
                                                                            str(self.wins), str(
                self.losses), str(self.ties)))


class Chat(threading.Thread):
    def __init__(self, win, sock):
        super().__init__()
        self.sock = sock
        self.win = win
        self.row = 0
        self.col = 0

    def update(self):
        # Get a chat message.
        msg = self.sock.recv(1024)
        # This next bit just implements some kind of half-working auto-scrolling
        if self.row >= 40:
            self.win.clear()
            self.row = 0
        self.win.addstr(self.row, self.col, msg)
        self.row += 1
        # Add the chat message to win.
        self.win.refresh()

    def run(self):
        while True:
            time.sleep(1)
            self.update()


# define screen variables

class Screen(object):
    def __init__(self, height, width, one_begin_x,
                 one_begin_y, two_begin_x, two_begin_y,
                 thr_begin_x, thr_begin_y, player):
        # init for screen in main
        # screen = Screen(40, 40, 1, 15, 70, 15, 121, 15, player)

        self.stdscr = self.initialize()  # Starts the Curses application.
        # Window 1 takes commands for the game.
        self.win1 = curses.newwin(height, width, one_begin_y, one_begin_x)
        # self.win1_sub = self.win1.derwin(1, 1)
        # self.win1.box()

        # Window 2 carries the chat.
        self.win2 = curses.newwin(((3 * height) // 4), width, two_begin_y, two_begin_x)
        # self.win2_sub = self.win2.derwin(2, 1)
        # self.win2.box()

        # Window 3 displays the current game board.
        self.win3 = curses.newwin(66, 66, thr_begin_y, thr_begin_x)
        # self.win3_sub = self.win2.derwin(2, 1)
        # self.win3.box()

        # Window 4 displays the message that the player is currently typing out.
        self.win4 = curses.newwin((height // 4), width,
                                  (two_begin_y + ((3 * height) // 4)), two_begin_x)
        # self.win4_sub = self.win4.derwin(2, 1)
        # self.win4.box()

        # takes messages for the game
        self.game = Textbox(self.win1)
        # takes messages for current gameboard
        self.board_mesg = Textbox(self.win4)
        # commands for game
        self.chat = Chat(self.win2, self.stdscr)
        self.player = player

    @staticmethod
    def initialize():
        stdscr = curses.initscr()
        stdscr.clear()
        curses.noecho()
        curses.cbreak()
        stdscr.keypad(True)
        return stdscr

    def halt(self):  # Just to cut down on a few lines in main()
        down(self.stdscr)

    def print_title(self):
        self.stdscr.addstr(0, 0, "GGGGGGGGGG OOOOOOOOOO MMM MMM MMM OOOOOOOOOO KK      KK UU     UU", curses.A_BLINK)
        self.stdscr.addstr(1, 0, "GG         OO      OO MMM MMM MMM OO      OO KK     KK  UU     UU", curses.A_BLINK)
        self.stdscr.addstr(2, 0, "GG         OO      OO MM M   M MM OO      OO KK    KK   UU     UU", curses.A_BLINK)
        self.stdscr.addstr(3, 0, "GG         OO      OO MM M   M MM OO      OO KK   KK    UU     UU", curses.A_BLINK)
        self.stdscr.addstr(4, 0, "GG         OO      OO MM M   M MM OO      OO KK KK      UU     UU", curses.A_BLINK)
        self.stdscr.addstr(5, 0, "GG         OO      OO MM M   M MM OO      OO KKK        UU     UU", curses.A_BLINK)
        self.stdscr.addstr(6, 0, "GG   GGGG  OO      OO MM M   M MM OO      OO KK KK      UU     UU", curses.A_BLINK)
        self.stdscr.addstr(7, 0, "GG   GGGG  OO      OO MM M   M MM OO      OO KK  KK     UU     UU", curses.A_BLINK)
        self.stdscr.addstr(8, 0, "GG     GG  OO      OO MM M   M MM OO      OO KK   KK    UU     UU", curses.A_BLINK)
        self.stdscr.addstr(9, 0, "GG     GG  OO      OO MM M   M MM OO      OO KK    KK   UU     UU", curses.A_BLINK)
        self.stdscr.addstr(10, 0, "GG     GG  OO      OO MM M   M MM OO      OO KK     KK   UU   UU", curses.A_BLINK)
        self.stdscr.addstr(11, 0, "GGGGGGGGG  OOOOOOOOOO MM M   M MM OOOOOOOOOO KK      KK   UUUUU", curses.A_BLINK)
        self.stdscr.addstr(14, 70, "Chat")
        self.stdscr.addstr(14, 1, "Game Window")
        self.stdscr.addstr(14, 120, "The Board")
        self.stdscr.addstr(0, 70, "Player: " + str(self.player.name))
        self.stdscr.addstr(1, 70, "Unique PID: " + str(self.player.upid))
        self.stdscr.addstr(2, 70, "Wins: " + str(self.player.wins))
        self.stdscr.addstr(3, 70, "Losses: " + str(self.player.losses))
        self.stdscr.addstr(4, 70, "Ties:" + str(self.player.ties))


class GIPS(object):
    def __init__(self, sock):
        logging.debug("GIPS.sock is being defined.")
        self.sock = sock
        logging.debug(self.sock)
        self.pid = 0
        self.isWin = 0
        self.move_a = -1
        self.move_b = -1
        self.isEarlyExit = 0

    def pack(self, pid, is_win, move_a, move_b, is_early_exit):
        logging.debug("Packing: " + str(pid) + " " + str(is_win) +
                      " " + str(move_a) + " " + str(move_b))
        self.isWin = is_win
        self.pid = pid
        self.move_a = move_a
        self.move_b = move_b
        self.isEarlyExit = is_early_exit

    def send(self):
        logging.debug("Sending.")
        self.sock.send(struct.pack('ccccc', int(self.pid).to_bytes(1, sys.byteorder),
                                   int(self.isWin).to_bytes(1, sys.byteorder),
                                   int(self.move_a).to_bytes(1, sys.byteorder),
                                   int(self.move_b).to_bytes(1, sys.byteorder),
                                   int(self.isEarlyExit).to_bytes(1, sys.byteorder)))

    def recv(self):
        self.sock.setblocking(True)
        self.pid = ord(self.sock.recv(1))
        self.isWin = ord(self.sock.recv(1))
        self.move_a = ord(self.sock.recv(1))
        self.move_b = ord(self.sock.recv(1))
        self.isEarlyExit = ord(self.sock.recv(1))
        ''''
        data = bytearray()
        while not data:
            data = self.recv_timeout()
            time.sleep(0.1)
            if data:
                data = list(map(int, data))
            while data and data[0] == 0:
                data.remove(data[0])
        self.pid = data[0]
        self.isWin = data[1]
        self.move_a = data[2]
        self.move_b = data[3]
        self.isEarlyExit = data[4]
        '''
        if self.move_a == 255:
            self.move_a = -1
        if self.move_b == 255:
            self.move_b = -1
        logging.debug("Received: " + str(self))

    # timeout after receiving for a little bit
    def recv_timeout(self, timeout=2):
        self.sock.setblocking(False)
        total_data = bytearray()
        # data = bytes()
        begin = time.time()
        while 1:
            # if you got some data, then break after wait sec
            if total_data and time.time() - begin > timeout:
                break
            # if you got no data at all, wait a little longer
            elif time.time() - begin > timeout * 2:
                break
            # noinspection PyBroadException
            try:
                data = self.sock.recv(8)
                if data:
                    total_data.extend(data)
                    begin = time.time()
                else:
                    time.sleep(0.1)
            except:
                pass
        return total_data


# noinspection PyBroadException
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

    player = Player(username, upid, 0, 0, 0)

    # height/width/one_begin_x/one_begin_y/etc
    # GOOD UP TO HERE (With send/recv)
    screen = Screen(40, 40, 1, 15, 70, 15, 121, 15, player)
    screen.print_title()
    logging.debug("Game starting.")
    print("game starting")
    gips = GIPS
    try:
        logging.debug("The GIPS is defined.")
        keep_playing = True
        while keep_playing:
            sock = establish_connection(host, port)
            gips = GIPS(sock)
            login(sock, upid, username)
            pid = ord(sock.recv(1))
            board = init_board()
            gips = game_loop(board, pid, username, screen, sock, gips)
            if gips.isEarlyExit == 1 and gips.isWin == 0:
                screen.halt()
                print("Thanks for playing!!!")
                gips.sock.shutdown(socket.SHUT_RDWR)
                gips.sock.close()
                sys.exit(0)
            else:
                screen.halt()
                print("Thanks for playing!!!")
                # end sequence
                end_game(gips, screen, pid)

    except Exception:
        logging.exception("Exception caught")
        gips.sock.shutdown(socket.SHUT_RDWR)
        gips.sock.close()
        screen.halt()
        sys.exit(0)
    except KeyboardInterrupt:
        logging.exception("SIGINT received")
        gips.sock.shutdown(socket.SHUT_RDWR)
        gips.sock.close()
        screen.halt()
        sys.exit(0)


# noinspection PyBroadException
def establish_connection(host, port):
    try:
        logging.warning("Trying to connect to the server.")
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        logging.warning("Socket created, connecting... " + str(sock))
        sock.connect((host, port))
        logging.warning("Socket issue check: " + str(sock))
        return sock
    except Exception:
        logging.critical("Server could not be reached!")
        print("Couldn't connect to the server. Check your internet connection and try again.")
        sys.exit(0)


def end_game(gips, screen, pid):
    if gips.isWin == pid:
        print("You Win! :-}")
    else:
        print("You Lost! :-{")
    player = gips.sock.recv(40)
    player = struct.unpack('@icccccccccccccccccccciiii', player)
    screen.player.upid = ntohl(player[0])
    screen.player.name = ''
    for x in range(1, 21):
        screen.player.name += player[x].decode("utf-8")
    print(screen.player.name)
    screen.player.wins = ntohl(player[21])
    screen.player.losses = ntohl(player[22])
    screen.player.ties = ntohl(player[23])
    if pid != gips.isWin:
        gips.sock.send(bytes(b'x01'))
    print("Your new stats are:")
    screen.player.print_player()
    gips.sock.shutdown(socket.SHUT_RDWR)
    gips.sock.close()
    sys.exit(0)


def game_loop(board, pid, username, screen, sock, gips):
    game_running = True
    while gips.isWin == 0 and gips.isEarlyExit == 0 and game_running:
        logging.debug("Starting the loop.")
        gips.recv()

        if gips.isWin != 0 or gips.isEarlyExit != 0:
            return gips
        elif gips.move_a == -1 and gips.move_b == -1:
            pass
        else:  # update board, get move
            board = update_board(gips, board)

        screen.stdscr.refresh()  # This line begins the interface logic.
        display_board(board, screen.win3)
        screen.stdscr.refresh()  # This begins the user interaction
        c = screen.stdscr.getch()
        game_running = check_keys(c, screen, gips, board, pid, sock, username)
        is_win = gips.sock.recv(4)
        is_win = struct.unpack('!i', is_win)
        is_win = ntohl(is_win[0])
        gips.isWin = is_win
    return gips


def check_keys(c, screen, gips, board, pid, sock, username):
    logging.debug("checkKeys")
    if c == ord('q'):
        logging.debug("Key: q")
        return False
    if c == ord('m'):
        done = False
        while not done:
            screen.win1.clear()
            logging.debug("Key: m")
            # Get the next move and send it.
            screen.game.edit()
            stuff = screen.game.gather()
            # Split the move into two components.
            if len(stuff) == 0:
                done = False
                continue
            move = (str(stuff)).split(' ')
            
            move.remove('\n')  # kill the newline=
            # for m in move:
            #    m = int(m)
            # If the move is not valid:
            # make moves ints
            move = list(map(int, move))
            if not move_is_valid(move):
                send_to_chat(sock, "server: Invalid move, " + str(username) + "!")
                done = False
                continue
            done = True
        screen.win1.clear()
        # subtract 1 from moves
        move[0] -= 1
        move[1] -= 1
        # Otherwise:
        # Encode a GIPS
        gips.pack(pid, gips.isWin, move[0], move[1], 0)
        # Send the GIPS
        gips.send()
        board = update_board(gips, board)
        display_board(board, screen.win3)
        # moves = []
        return True
    if c == ord('c'):
        screen.board_mesg.edit()
        stuff = screen.board_mesg.gather()
        message = "\v" + str(username) + ": " + str(stuff)
        # Send message to the server as a bytestring.
        send_to_chat(sock, message)
        screen.stdscr.refresh()  # Redraws the screen.
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
    # because of the way curses displays the board
    # board[b][a] so not inverted
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


# noinspection PyUnusedLocal
def init_board():
    """
    >>> board = init_board()
    >>> board[0][0]
    'o'
    """
    return [['o' for x in range(8)] for y in range(8)]


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
