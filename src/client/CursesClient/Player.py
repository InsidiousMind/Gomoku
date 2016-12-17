import socket
import struct


class Player(object):
    def __init__(self, name, upid, wins, losses, ties, chat_v):
        self.chat = chat_v
        self.name = name
        self.upid = upid
        self.wins = wins
        self.losses = losses
        self.ties = ties
        self.win = None

    def print_player(self):
        print(str(self.name) + ' UPID: ' + str(self.upid) + ' has ' + str(self.wins) + ' wins ' + str(
            self.losses) + ' losses ' + ' and ' + str(self.ties) + ' ties.')

    def recv_player(self, sock):
        while True:
            check = sock.recv(1, socket.MSG_PEEK).decode("utf-8")
            if check == '\v':
                self.chat.recv_msg()
            else:
                player = sock.recv(40)
                player = struct.unpack('!IccccccccccccccccccccIIII', player)
                self.upid = int(player[0])
                self.name = ''
                for x in range(1, 21):
                    if player[x].decode("utf-8") != ' ':
                        self.name += player[x].decode("utf-8")
                self.wins = int(player[21])
                self.losses = int(player[22])
                self.ties = int(player[23])
                break

    def update_pwin(self):
        self.win.clear()
        self.win.addstr(0, 0, "Player: " + str(self.name))
        self.win.addstr(1, 0, "Unique PID: " + str(self.upid))
        self.win.addstr(2, 0, "Wins: " + str(self.wins))
        self.win.addstr(3, 0, "Losses: " + str(self.losses))
        self.win.addstr(4, 0, "Ties:" + str(self.ties))
        self.win.refresh()
