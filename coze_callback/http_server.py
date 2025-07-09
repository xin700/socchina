#!/usr/bin/env python3
"""
HTTP Server implementation for listening to callbacks on a specified port.
"""

import logging
import socket
from http.server import HTTPServer, ThreadingHTTPServer
from request_handler import RequestHandler

class HttpCallbackServer:
    """A HTTP server that continuously listens for requests on a specified port."""
    
    def __init__(self, port=11452, host='0.0.0.0', use_threading=True, request_handler=None):
        """
        Initialize the HTTP server.
        
        Args:
            port (int): Port to listen on (default: 11452)
            host (str): Host to bind to (default: '0.0.0.0', all interfaces)
            use_threading (bool): Whether to use a threaded server (default: True)
            request_handler (RequestHandler): Request handler instance
        """
        self.host = host
        self.port = port
        self.logger = logging.getLogger('coze_callback.server')
        self.server_class = ThreadingHTTPServer if use_threading else HTTPServer
        
        # Use the provided handler or create a new one
        self.handler = request_handler or RequestHandler()
        
    def start(self):
        """Start the HTTP server and listen indefinitely."""
        server_address = (self.host, self.port)
        
        try:
            httpd = self.server_class(server_address, self.handler.handler_class)
            self.logger.info(f"Server running at http://{self.host}:{self.port}/")
            self.logger.info("Press Ctrl+C to stop the server")
            httpd.serve_forever()
            
        except socket.error as e:
            self.logger.error(f"Socket error: {e}")
            self.logger.error(f"Make sure port {self.port} is available")
            raise
            
        except Exception as e:
            self.logger.error(f"Server error: {e}")
            raise
