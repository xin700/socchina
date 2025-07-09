#!/usr/bin/env python3
"""
Signature verification module for callback requests.
Implements the SHA-256 signature algorithm described in the documentation.
"""

import hashlib
import json
import logging

class SignatureVerifier:
    """Verifies signatures for callback requests."""
    
    def __init__(self, secret_key=None):
        """
        Initialize the signature verifier.
        
        Args:
            secret_key (str): The secret key used for signature verification.
                              If None, signature verification will be skipped.
        """
        self.secret_key = secret_key
        self.logger = logging.getLogger('coze_callback.signature')
        
    def set_secret_key(self, secret_key):
        """Set or update the secret key."""
        self.secret_key = secret_key
        self.logger.info("Secret key updated")
        
    def verify(self, event_data):
        """
        Verify the signature of a callback event.
        
        Args:
            event_data (dict): The callback event data containing the required fields:
                - EventType
                - EventData
                - EventTime
                - EventId
                - AppId
                - Version
                - Nonce
                - Signature
                
        Returns:
            tuple: (bool, str) - (is_valid, message)
                - is_valid: True if signature is valid or verification is skipped
                - message: Description of the verification result
        """
        # Skip verification if no secret key is set
        if not self.secret_key:
            return True, "Signature verification skipped (no secret key)"
        
        # Check if all required fields are present
        required_fields = ['EventType', 'EventData', 'EventTime', 'EventId', 
                          'AppId', 'Version', 'Nonce', 'Signature']
        
        for field in required_fields:
            if field not in event_data:
                return False, f"Missing required field: {field}"
        
        # Extract the provided signature
        provided_signature = event_data['Signature']
        
        # Create data array for signature calculation
        data = [
            event_data['EventType'],
            event_data['EventData'],
            event_data['EventTime'],
            event_data['EventId'],
            event_data['AppId'],
            event_data['Version'],
            event_data['Nonce'],
            self.secret_key
        ]
        
        # Sort the data array alphabetically
        data.sort()
        
        # Join the sorted array into a string
        payload = ''.join(data)
        
        # Calculate SHA-256 hash
        hash_obj = hashlib.sha256(payload.encode('utf-8'))
        calculated_signature = hash_obj.hexdigest()
        
        # Verify signatures match
        is_valid = (calculated_signature == provided_signature)
        
        if is_valid:
            message = "Signature verification successful"
        else:
            message = f"Signature verification failed. Expected: {calculated_signature}, Got: {provided_signature}"
            
        return is_valid, message
        
    def verify_request(self, request_body):
        """
        Verify the signature from a request body string.
        
        Args:
            request_body (str or bytes): The request body as string or bytes
            
        Returns:
            tuple: (bool, str, dict) - (is_valid, message, event_data)
                - is_valid: True if signature is valid or verification is skipped
                - message: Description of the verification result
                - event_data: The parsed event data
        """
        # Skip verification if no secret key is set
        if not self.secret_key:
            try:
                # Try to parse the request body as JSON even if we're not verifying
                if isinstance(request_body, bytes):
                    request_body = request_body.decode('utf-8')
                event_data = json.loads(request_body)
                return True, "Signature verification skipped (no secret key)", event_data
            except json.JSONDecodeError:
                return False, "Invalid JSON format", None
        
        # Parse the request body as JSON
        try:
            if isinstance(request_body, bytes):
                request_body = request_body.decode('utf-8')
            event_data = json.loads(request_body)
        except json.JSONDecodeError:
            return False, "Invalid JSON format", None
            
        # Verify the signature
        is_valid, message = self.verify(event_data)
        return is_valid, message, event_data
