from __future__ import print_function
import json
from queue import Queue
import socket
import threading

# Python 3 compatibility
try:
    input = raw_input
except NameError:
    pass

class FirefoxRemoteControl(object):
    ''' Interact with a web browser running the Remote Control extension. '''
    def __init__(self, port):
        self.sock = socket.socket()
        self.sock.connect(('localhost', port))

    def execute(self, cmd):
        msg = cmd.replace('\n', ' ') + '\r\n'
        self.sock.send(msg.encode('utf8'))
        ret = []
        while True:
            chunk = self.sock.recv(4096)
            ret.append(chunk)
            if b'\n' in chunk:
                break
        res = json.loads(b''.join(ret).decode('utf8'))
        if 'error' in res:
            raise Exception(res['error'])
        elif not res:
            return None
        else:
            return res['result']

class FirefoxDebuggerControl(object):
    ''' Interact with a Firefox browser with remote debugging enabled.

    Requires the about:config options "devtools.debugger.remote-enabled" and "devtools.chrome.enabled" enabled,
    and the browser to be started with "--no-remote --start-debugger-server <port>"
    '''

    def __init__(self, port):
        self.sock = socket.socket()
        self.sock.connect(('localhost', port))

        info = self._recv_msg()
        self.actors = {}

        self.thread = threading.Thread(target=self._receive_thread)
        self.thread.daemon = True
        self.thread.start()

        tab_list = self._send_recv('root', 'listTabs')
        pages = tab_list['tabs']
        if len(pages) == 0:
            raise Exception("No pages to attach to!")
        elif len(pages) == 1:
            page = pages[0]
        else:
            print("Select a page to attach to:")
            for i, page in enumerate(pages):
                title = self._send_recv(page['actor'], 'getTarget')['frame']['title']
                title = title.encode('unicode_escape').decode('iso-8859-1')
                if len(title) > 100:
                    title = title[:100] + '...'
                print("%d) %s" % (i+1, title))
            while 1:
                try:
                    pageidx = int(input("Selection? "))
                    page = pages[pageidx-1]
                    break
                except Exception as e:
                    print("Invalid selection:", e)

        page = self._send_recv(page['actor'], 'getTarget')['frame']
        self.page = page
        self._send_recv(page['actor'], 'attach')

    def _actor_msgs(self, actor):
        while True:
            yield self.actors[actor].get()

    def _recv_msg(self):
        msgsz = b''
        for i in range(10):
            c = self.sock.recv(1)
            if not c:
                raise EOFError()
            if c == b':':
                break
            msgsz += c
        if not msgsz.isdigit():
            raise ValueError("invalid length field: %s" % msgsz)

        msgsz = int(msgsz)
        msg = bytearray()
        while len(msg) < msgsz:
            chunk = self.sock.recv(msgsz - len(msg))
            if not chunk:
                raise EOFError()
            msg += chunk
        return json.loads(bytes(msg))

    def _send_msg(self, actor, msgtype, obj=None):
        if actor not in self.actors:
            self.actors[actor] = Queue()
        if obj is None:
            obj = {}
        else:
            obj = obj.copy()
        obj['to'] = actor
        obj['type'] = msgtype
        msg = json.dumps(obj).encode()
        self.sock.send(str(len(msg)).encode() + b':')
        self.sock.send(msg)

    def _send_recv(self, actor, msgtype, obj=None):
        self._send_msg(actor, msgtype, obj)
        reply = next(self._actor_msgs(actor))
        if 'error' in reply:
            raise Exception(reply['error'], reply.get('message', ''))
        return reply

    def _receive_thread(self):
        ''' Continually read events and command results '''
        while 1:
            try:
                msg = self._recv_msg()
                self.actors[msg['from']].put(msg)
            except Exception as e:
                print("disconnect: %s" % e)
                break

    def execute(self, cmd):
        resp = self._send_recv(self.page['consoleActor'], 'evaluateJSAsync', {'text': cmd})
        resultID = resp['resultID']
        for result in self._actor_msgs(self.page['consoleActor']):
            if result['hasException']:
                raise Exception(result['exceptionMessage'])
            return result['result']
