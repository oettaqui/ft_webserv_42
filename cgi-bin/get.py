#!/usr/bin/env python3
import cgi
import cgitb
import os
import html
from urllib.parse import parse_qs

# Enable CGI error reporting
cgitb.enable()

# Get query string from environment
query_string = os.environ.get('QUERY_STRING', '')

# Parse query parameters
params = parse_qs(query_string)

# Get parameters from URL
username = params.get('username', [''])[0].strip()
email = params.get('email', [''])[0].strip()
age = params.get('age', [''])[0].strip()

# Check if all parameters exist and are not empty
all_params_exist = username and email and age

# Print HTTP header
print("Content-Type: text/html\n")

# Determine container class
container_class = 'success-container' if all_params_exist else 'error-container'

# HTML content
html_content = f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>User Information</title>
    <style>
        * {{
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }}

        body {{
            font-family: 'Arial', sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
            padding: 20px;
        }}

        .container {{
            background: white;
            padding: 40px;
            border-radius: 15px;
            box-shadow: 0 20px 40px rgba(0, 0, 0, 0.1);
            max-width: 500px;
            width: 100%;
            text-align: center;
        }}

        .success-container {{
            border-left: 5px solid #28a745;
        }}

        .error-container {{
            border-left: 5px solid #dc3545;
        }}

        h1 {{
            color: #333;
            margin-bottom: 30px;
            font-size: 2.2em;
        }}

        .success-title {{
            color: #28a745;
        }}

        .error-title {{
            color: #dc3545;
        }}

        .user-info {{
            background: #f8f9fa;
            padding: 25px;
            border-radius: 10px;
            margin: 20px 0;
            text-align: left;
        }}

        .info-item {{
            display: flex;
            align-items: center;
            margin-bottom: 15px;
            padding: 10px;
            background: white;
            border-radius: 8px;
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.05);
        }}

        .info-item:last-child {{
            margin-bottom: 0;
        }}

        .info-label {{
            font-weight: bold;
            color: #495057;
            min-width: 80px;
            margin-right: 15px;
        }}

        .info-value {{
            color: #333;
            font-size: 1.1em;
        }}

        .icon {{
            width: 20px;
            height: 20px;
            margin-right: 10px;
            fill: #667eea;
        }}

        .error-message {{
            background: #f8d7da;
            color: #721c24;
            padding: 20px;
            border-radius: 10px;
            margin: 20px 0;
            border: 1px solid #f5c6cb;
        }}

        .form-link {{
            background: #667eea;
            color: white;
            padding: 12px 25px;
            text-decoration: none;
            border-radius: 25px;
            display: inline-block;
            margin-top: 20px;
            transition: background 0.3s ease;
        }}

        .form-link:hover {{
            background: #5a6fd8;
        }}

        .sample-url {{
            background: #e9ecef;
            padding: 15px;
            border-radius: 8px;
            margin-top: 15px;
            font-family: monospace;
            word-break: break-all;
            color: #495057;
        }}
    </style>
</head>
<body>
    <div class="container {container_class}">
"""

if all_params_exist:
    html_content += f"""
        <h1 class="success-title">
            <svg class="icon" viewBox="0 0 24 24">
                <path d="M9 12l2 2 4-4m6 2a9 9 0 11-18 0 9 9 0 0118 0z"/>
            </svg>
            User Information Found
        </h1>
        
        <div class="user-info">
            <div class="info-item">
                <svg class="icon" viewBox="0 0 24 24">
                    <path d="M16 7a4 4 0 11-8 0 4 4 0 018 0zM12 14a7 7 0 00-7 7h14a7 7 0 00-7-7z"/>
                </svg>
                <span class="info-label">Username:</span>
                <span class="info-value">{html.escape(username)}</span>
            </div>
            
            <div class="info-item">
                <svg class="icon" viewBox="0 0 24 24">
                    <path d="M3 8l7.89 4.26a2 2 0 002.22 0L21 8M5 19h14a2 2 0 002-2V7a2 2 0 00-2-2H5a2 2 0 00-2 2v10a2 2 0 002 2z"/>
                </svg>
                <span class="info-label">Email:</span>
                <span class="info-value">{html.escape(email)}</span>
            </div>
            
            <div class="info-item">
                <svg class="icon" viewBox="0 0 24 24">
                    <path d="M8 7V3a2 2 0 012-2h4a2 2 0 012 2v4m-6 9a6 6 0 1012 0v-3"/>
                </svg>
                <span class="info-label">Age:</span>
                <span class="info-value">{html.escape(age)} years old</span>
            </div>
        </div>
    """
else:
    username_status = '✅ Provided' if username else '❌ Missing'
    email_status = '✅ Provided' if email else '❌ Missing'
    age_status = '✅ Provided' if age else '❌ Missing'
    
    html_content += f"""
        <h1 class="error-title">
            <svg class="icon" viewBox="0 0 24 24">
                <path d="M12 9v2m0 4h.01m-6.938 4h13.856c1.54 0 2.502-1.667 1.732-2.5L13.732 4c-.77-.833-1.964-.833-2.732 0L3.732 16.5c-.77.833.192 2.5 1.732 2.5z"/>
            </svg>
            Missing Information
        </h1>
        
        <div class="error-message">
            <strong>Required parameters are missing!</strong><br>
            Please provide all of the following parameters in the URL:
            <ul style="text-align: left; margin-top: 10px;">
                <li><strong>username</strong> {username_status}</li>
                <li><strong>email</strong> {email_status}</li>
                <li><strong>age</strong> {age_status}</li>
            </ul>
        </div>
        
        <div class="sample-url">
            <strong>Example Query Data:</strong><br>
            ?username=john_doe&email=john@example.com&age=25
        </div>
    """

html_content += """
    </div>
</body>
</html>
"""

print(html_content)