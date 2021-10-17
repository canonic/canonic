from http.server import HTTPServer, SimpleHTTPRequestHandler
from socketserver import ThreadingMixIn

import json
import pathlib
import os
import ssl
import typing
import urllib.request

if typing.TYPE_CHECKING:
    from http.client import HTTPResponse
    from socketserver import BaseServer
    from os import PathLike

BUILD_DIR = pathlib.Path(__file__).absolute().parent
DEV_HTTPS_KEY_FILE = BUILD_DIR / 'dev_server.key'
DEV_HTTPS_CERT_FILE = BUILD_DIR / 'dev_server.cert'


def resolve_actual_address(name: str):
    """
    Given a domain name resolve the address it points to using Google's DNS.
    """
    response = urllib.request.urlopen('https://dns.google/resolve?name={}'.format(name))
    data = json.loads(response.read())
    answer = data['Answer']
    return answer[0]['data']


# Since this server is designed to be used on a system which has modified it's /etc/hosts file to point www.canonic.com
# to 127.0.0.1 we need to find the actual IP of www.canonic.com using an external DNS to get the real value.
PASS_THROUGH_TARGET_ADDRESS = 'http://' + resolve_actual_address('www.canonic.com')
print("Pass-through target URL:", PASS_THROUGH_TARGET_ADDRESS)


class SimpleHTTPPassThroughRequestHandler(SimpleHTTPRequestHandler):
    """
    This class serves files from the directory directory and below, or the current directory if directory is not
    provided, directly mapping the directory structure to HTTP requests.

    If a matching file is not found under directory then the server will pass the request through to the passthrough
    target address and proxy the response. This makes the server a useful way to override endpoints on the target server
    with local files.
    """

    def __init__(self,
                 request: bytes,
                 client_address: typing.Tuple[str, int],
                 server: 'BaseServer',
                 directory: typing.Union[str, 'PathLike', None] = None):

        SimpleHTTPRequestHandler.__init__(self, request, client_address, server, directory=directory)

    def do_GET(self) -> None:

        if self.path == '/':
            path = pathlib.Path(self.translate_path('/index.html'))
        else:
            path = pathlib.Path(self.translate_path(self.path))

        if path.exists() and path.is_file():
            print("USING LOCAL: ", path.as_posix(), 'for:', self.path)
            SimpleHTTPRequestHandler.do_GET(self)
        else:
            print("PASS-THROUGH: ", self.path)
            response: 'HTTPResponse' = urllib.request.urlopen(PASS_THROUGH_TARGET_ADDRESS + self.path)

            self.send_response(response.status)
            for header, value in response.headers.items():
                self.send_header(header, value)
            self.end_headers()

            try:
                self.wfile.write(response.read())
            except BrokenPipeError:
                pass


class ThreadedHTTPServer(ThreadingMixIn, HTTPServer):
    """Handle requests in a separate thread."""


def main() -> None:
    httpd = ThreadedHTTPServer(('127.0.0.1', 443), SimpleHTTPPassThroughRequestHandler)
    context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    context.load_cert_chain(DEV_HTTPS_CERT_FILE, DEV_HTTPS_KEY_FILE)
    httpd.socket = context.wrap_socket(httpd.socket, server_side=True)
    httpd.serve_forever()


if __name__ == '__main__':
    import sys

    # Switch to the directory passed as an argument
    if len(sys.argv) == 2:
        os.chdir(sys.argv[1])

    sys.exit(main())
