#!/usr/bin/env python3
"""
HTTP Server listening on port 11452 that logs and saves incoming requests.
This is the main entry point for the server.
Supports signature verification for secure callbacks.
"""

import logging
import argparse
import os
from http_server import HttpCallbackServer
from request_handler import RequestHandler
from signature_verifier import SignatureVerifier

def parse_arguments():
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(description='Callback server with signature verification')
    parser.add_argument('-p', '--port', type=int, default=11452,
                        help='Port to listen on (default: 11452)')
    parser.add_argument('-s', '--secret-key', type=str, default=None,
                        help='Secret key for signature verification')
    parser.add_argument('-k', '--secret-key-file', type=str, default=None,
                        help='File containing the secret key')
    parser.add_argument('-d', '--data-dir', type=str, default='data',
                        help='Directory to save request data (default: data)')
    parser.add_argument('-l', '--log-level', type=str, default='INFO',
                        choices=['DEBUG', 'INFO', 'WARNING', 'ERROR', 'CRITICAL'],
                        help='Logging level (default: INFO)')
    return parser.parse_args()

def get_secret_key(args):
    """Get the secret key from arguments or file."""
    if args.secret_key:
        return args.secret_key
    elif args.secret_key_file and os.path.exists(args.secret_key_file):
        try:
            with open(args.secret_key_file, 'r') as f:
                return f.read().strip()
        except Exception as e:
            print(f"Error reading secret key file: {e}")
    return None

def main():
    # Parse command line arguments
    args = parse_arguments()
    
    # Configure logging
    log_level = getattr(logging, args.log_level.upper())
    logging.basicConfig(
        level=log_level,
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
        handlers=[
            logging.StreamHandler(),
            logging.FileHandler('server.log')
        ]
    )
    
    logger = logging.getLogger('coze_callback')
    logger.info(f"Starting HTTP server on port {args.port}...")
    
    # Get secret key for signature verification
    secret_key = get_secret_key(args)
    if secret_key:
        logger.info("Signature verification enabled with provided secret key")
    else:
        logger.info("Signature verification disabled (no secret key provided)")
    
    # Create and start server
    try:
        handler = RequestHandler(save_dir=args.data_dir, secret_key=secret_key)
        server = HttpCallbackServer(port=args.port, request_handler=handler)
        server.start()
    except KeyboardInterrupt:
        logger.info("Server shutting down...")
    except Exception as e:
        logger.error(f"Server error: {e}")
    
if __name__ == "__main__":
    main()
