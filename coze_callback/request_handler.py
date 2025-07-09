#!/usr/bin/env python3
"""
Request handler for the HTTP callback server.
Processes incoming HTTP requests, logs them, and saves them to files.
Includes signature verification for callback requests.
"""

import os
import json
import logging
import time
from http.server import BaseHTTPRequestHandler
from datetime import datetime
from urllib.parse import parse_qs
from signature_verifier import SignatureVerifier

class RequestHandler:
    """Handler for processing and storing HTTP requests."""
    
    def __init__(self, save_dir='data', secret_key=None):
        """
        Initialize the request handler.
        
        Args:
            save_dir (str): Directory to save request data (default: 'data')
            secret_key (str, optional): Secret key for signature verification
        """
        self.logger = logging.getLogger('coze_callback.handler')
        self.save_dir = save_dir
        self.signature_verifier = SignatureVerifier(secret_key)
        
        # Create save directory if it doesn't exist
        if not os.path.exists(self.save_dir):
            os.makedirs(self.save_dir)
            self.logger.info(f"Created directory: {self.save_dir}")
            
        # Log signature verification status
        if secret_key:
            self.logger.info("Signature verification enabled")
        else:
            self.logger.info("Signature verification disabled (no secret key)")
        
        # Define the handler class within this class
        outer_self = self
        
        class CallbackHTTPHandler(BaseHTTPRequestHandler):
            def _set_response(self, status_code=200, content_type='application/json'):
                self.send_response(status_code)
                self.send_header('Content-type', content_type)
                self.end_headers()
            
            def _read_content(self):
                content_length = int(self.headers.get('Content-Length', 0))
                return self.rfile.read(content_length) if content_length > 0 else b''
            
            def _save_request(self, method, path, headers, body, verification_result=None):
                # Generate timestamp for filename
                timestamp = datetime.now().strftime('%Y%m%d_%H%M%S_%f')
                filename = f"{outer_self.save_dir}/{timestamp}_{method}.json"
                
                # Format headers for saving
                headers_dict = dict(headers)
                
                # Prepare data to save
                data = {
                    'timestamp': datetime.now().isoformat(),
                    'method': method,
                    'path': path,
                    'headers': headers_dict,
                    'body': body.decode('utf-8', errors='replace') if isinstance(body, bytes) else body
                }
                
                # Add signature verification results if available
                if verification_result is not None:
                    data['signature_verification'] = verification_result
                
                # Save to file
                with open(filename, 'w', encoding='utf-8') as f:
                    json.dump(data, f, ensure_ascii=False, indent=2)
                
                outer_self.logger.info(f"Request saved to {filename}")
                
            def _log_request(self, method, path, body):
                outer_self.logger.info(f"Received {method} request to {path}")
                if body:
                    body_str = body.decode('utf-8', errors='replace') if isinstance(body, bytes) else str(body)
                    if len(body_str) > 1000:  # Truncate long bodies in logs
                        body_str = body_str[:1000] + "... [truncated]"
                    outer_self.logger.info(f"Request body: {body_str}")
            
            def do_GET(self):
                body = self._read_content()
                self._log_request('GET', self.path, body)
                self._save_request('GET', self.path, self.headers, body)
                
                # Send response
                self._set_response()
                self.wfile.write(json.dumps({"status": "ok", "message": "Request received"}).encode())
                
            def do_POST(self):
                body = self._read_content()
                self._log_request('POST', self.path, body)
                
                # Verify signature for callback requests
                try:
                    is_valid, message, event_data = outer_self.signature_verifier.verify_request(body)
                    verification_status = "✓ Signature Valid" if is_valid else "✗ Signature Invalid"
                    event_id = event_data.get('EventId', 'Unknown') if event_data else 'Unknown'
                    
                    outer_self.logger.info(f"Signature Verification: {verification_status} | EventId: {event_id} | Message: {message}")
                    
                    # Add verification result to saved data
                    verification_result = {
                        "signature_verified": is_valid,
                        "verification_message": message,
                        "event_id": event_id if event_data else None
                    }
                except Exception as e:
                    outer_self.logger.error(f"Signature verification error: {str(e)}")
                    verification_result = {
                        "signature_verified": False,
                        "verification_message": f"Error during verification: {str(e)}",
                        "event_id": None
                    }
                
                # Save request with verification info
                self._save_request('POST', self.path, self.headers, body, verification_result)
                
                # Send response
                self._set_response()
                self.wfile.write(json.dumps({"status": "ok", "message": "Request received"}).encode())
                
            def do_PUT(self):
                body = self._read_content()
                self._log_request('PUT', self.path, body)
                self._save_request('PUT', self.path, self.headers, body)
                
                # Send response
                self._set_response()
                self.wfile.write(json.dumps({"status": "ok", "message": "Request received"}).encode())
                
            def do_DELETE(self):
                body = self._read_content()
                self._log_request('DELETE', self.path, body)
                self._save_request('DELETE', self.path, self.headers, body)
                
                # Send response
                self._set_response()
                self.wfile.write(json.dumps({"status": "ok", "message": "Request received"}).encode())
            
            # Silence server logs to avoid double logging
            def log_message(self, format, *args):
                pass
                
        # Store the handler class as an attribute
        self.handler_class = CallbackHTTPHandler
