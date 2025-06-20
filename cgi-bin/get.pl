#!/usr/bin/perl

print "Content-Type: text/html\n\n";
print <<HTML;
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Hey Page</title>
    <style>
        body {
            font-family: 'Arial', sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            margin: 0;
            padding: 0;
            height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
        }
        .container {
            text-align: center;
            background: rgba(255, 255, 255, 0.1);
            backdrop-filter: blur(10px);
            border-radius: 20px;
            padding: 60px 80px;
            box-shadow: 0 8px 32px rgba(31, 38, 135, 0.37);
            border: 1px solid rgba(255, 255, 255, 0.18);
        }
        h1 {
            color: #ffffff;
            font-size: 4rem;
            margin: 0;
            text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.3);
            animation: pulse 2s ease-in-out infinite alternate;
        }
        .subtitle {
            color: #f0f0f0;
            font-size: 1.2rem;
            margin-top: 20px;
            opacity: 0.8;
        }
        @keyframes pulse {
            from { transform: scale(1); }
            to { transform: scale(1.05); }
        }
        .emoji {
            font-size: 2rem;
            margin: 20px 0;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="emoji">👋</div>
        <h1>Hey!</h1>
        <p class="subtitle">Welcome to this awesome page</p>
    </div>
</body>
</html>
HTML