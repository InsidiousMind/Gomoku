import logging
import socket
import struct
import sys
from socket import ntohl


class GIPS(object):
    """
    Gomoku Inter-Process Shuttle
    """
    def __init__(self, sock, chat_v):
        logging.debug("GIPS.sock is being defined.")
        self.sock = sock
        logging.debug(self.sock)
        # Player ID - whether we're player one or player two.
        self.pid = 0
        self.is_win = 0
        # Cartesian-type coordinates for which point is being toggled.
        self.move_a = -1
        self.move_b = -1
        # Are we quitting the game early?
        self.isEarlyExit = 0
        # The chat object involved in the game.
        self.chat = chat_v
        # not a real part of gips struct
        # it's in gips here for ease of use
        self.upid = 0

    def pack(self, pid, is_win, move_a, move_b, is_early_exit):
        logging.debug("Packing: " + str(pid) + " " + str(is_win) +
                      " " + str(move_a) + " " + str(move_b))
        self.is_win = is_win
        self.pid = pid
        self.move_a = move_a
        self.move_b = move_b
        self.isEarlyExit = is_early_exit

    def send(self):
        logging.debug("Sending.")
        self.sock.send(struct.pack('ccccc', int(self.pid).to_bytes(1, sys.byteorder),
                                   int(self.is_win).to_bytes(1, sys.byteorder),
                                   int(self.move_a).to_bytes(1, sys.byteorder),
                                   int(self.move_b).to_bytes(1, sys.byteorder),
                                   int(self.isEarlyExit).to_bytes(1, sys.byteorder)))

    def recv(self):
        while True:
            check = self.sock.recv(1, socket.MSG_PEEK).decode("utf-8")
            if check == '\v':
                self.chat.recv_msg()
            else:
                self.sock.setblocking(True)
                self.pid = ord(self.sock.recv(1))
                self.is_win = ord(self.sock.recv(1))
                self.move_a = ord(self.sock.recv(1))
                self.move_b = ord(self.sock.recv(1))
                self.isEarlyExit = ord(self.sock.recv(1))

                if self.move_a == 255:
                    self.move_a = -1
                if self.move_b == 255:
                    self.move_b = -1
                logging.debug("Received: " + str(self))
                break

    def recv_iswin(self):
        while True:
            check = self.sock.recv(1, socket.MSG_PEEK).decode("utf-8")
            if check == '\v':
                self.chat.recv_msg()
            else:
                is_win = self.sock.recv(4)
                is_win = struct.unpack('!i', is_win)
                is_win = ntohl(is_win[0])
                return is_win

    def recv_pid(self):
        while True:
            check = self.sock.recv(1, socket.MSG_PEEK).decode("utf-8")
            if check == '\v':
                self.chat.recv_msg()
            else:
                return ord(self.sock.recv(1))

    # Upid is not apart of the GIPs being sent to and from the server
    def set_upid(self, upid):
        self.upid = upid

