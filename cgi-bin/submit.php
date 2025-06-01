<?php


if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $username = htmlspecialchars($_POST['username']);
    $email = htmlspecialchars($_POST['email']);
    $age = intval($_POST['age']);
    $isAdult = $age >= 18;
    
    if ($isAdult) {
        echo '<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Welcome Adult User</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: "Segoe UI", Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            color: white;
        }
        
        .container {
            background: rgba(255, 255, 255, 0.1);
            backdrop-filter: blur(10px);
            border-radius: 20px;
            padding: 40px;
            box-shadow: 0 20px 40px rgba(0, 0, 0, 0.3);
            text-align: center;
            max-width: 500px;
            width: 90%;
            border: 1px solid rgba(255, 255, 255, 0.2);
        }
        
        .icon {
            font-size: 60px;
            margin-bottom: 20px;
            animation: pulse 2s infinite;
        }
        
        @keyframes pulse {
            0% { transform: scale(1); }
            50% { transform: scale(1.1); }
            100% { transform: scale(1); }
        }
        
        h1 {
            font-size: 2.5em;
            margin-bottom: 30px;
            text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.3);
        }
        
        .user-info {
            background: rgba(255, 255, 255, 0.1);
            border-radius: 15px;
            padding: 25px;
            margin: 20px 0;
            border: 1px solid rgba(255, 255, 255, 0.2);
        }
        
        .info-item {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin: 15px 0;
            padding: 12px;
            background: rgba(255, 255, 255, 0.05);
            border-radius: 10px;
        }
        
        .label {
            font-weight: bold;
            color: #e0e6ff;
        }
        
        .value {
            color: #ffffff;
            font-size: 1.1em;
        }
        
        .status-badge {
            background: linear-gradient(45deg, #28a745, #20c997);
            padding: 10px 20px;
            border-radius: 25px;
            font-weight: bold;
            display: inline-block;
            margin-top: 20px;
            box-shadow: 0 4px 15px rgba(40, 167, 69, 0.4);
        }
        
        .welcome-message {
            font-size: 1.2em;
            margin-top: 20px;
            line-height: 1.6;
            color: #e0e6ff;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="icon">👔</div>
        <h1>Professional Registration</h1>
        
        <div class="user-info">
            <div class="info-item">
                <span class="label">👤 Username:</span>
                <span class="value">' . $username . '</span>
            </div>
            <div class="info-item">
                <span class="label">📧 Email:</span>
                <span class="value">' . $email . '</span>
            </div>
            <div class="info-item">
                <span class="label">🎂 Age:</span>
                <span class="value">' . $age . ' years old</span>
            </div>
        </div>
        
        <div class="status-badge">✅ Adult Account Verified</div>
        
        <div class="welcome-message">
            Welcome to our professional platform! As an adult user, you have access to all features and services. Your registration has been processed successfully.
        </div>
    </div>
</body>
</html>';
    
    } else {
        // Minor design - Colorful and playful
        echo '<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Welcome Young User</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: "Comic Sans MS", cursive, sans-serif;
            background: linear-gradient(45deg, #ff9a9e 0%, #fad0c4 25%, #a8edea 50%, #fed6e3 75%, #d299c2 100%);
            background-size: 400% 400%;
            animation: gradientShift 4s ease infinite;
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            color: #333;
        }
        
        @keyframes gradientShift {
            0% { background-position: 0% 50%; }
            50% { background-position: 100% 50%; }
            100% { background-position: 0% 50%; }
        }
        
        .container {
            background: rgba(255, 255, 255, 0.9);
            border-radius: 25px;
            padding: 40px;
            box-shadow: 0 20px 40px rgba(0, 0, 0, 0.2);
            text-align: center;
            max-width: 500px;
            width: 90%;
            border: 3px solid #ff6b6b;
            position: relative;
            overflow: hidden;
        }
        
        .container::before {
            content: "";
            position: absolute;
            top: -50%;
            left: -50%;
            width: 200%;
            height: 200%;
            background: repeating-linear-gradient(
                45deg,
                transparent,
                transparent 10px,
                rgba(255, 107, 107, 0.1) 10px,
                rgba(255, 107, 107, 0.1) 20px
            );
            animation: movePattern 10s linear infinite;
            z-index: -1;
        }
        
        @keyframes movePattern {
            0% { transform: translate(-50%, -50%) rotate(0deg); }
            100% { transform: translate(-50%, -50%) rotate(360deg); }
        }
        
        .icon {
            font-size: 60px;
            margin-bottom: 20px;
            animation: bounce 1.5s ease-in-out infinite;
        }
        
        @keyframes bounce {
            0%, 20%, 50%, 80%, 100% { transform: translateY(0); }
            40% { transform: translateY(-20px); }
            60% { transform: translateY(-10px); }
        }
        
        h1 {
            font-size: 2.5em;
            margin-bottom: 30px;
            color: #ff6b6b;
            text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.1);
            animation: colorChange 3s ease-in-out infinite;
        }
        
        @keyframes colorChange {
            0% { color: #ff6b6b; }
            33% { color: #4ecdc4; }
            66% { color: #45b7d1; }
            100% { color: #ff6b6b; }
        }
        
        .user-info {
            background: linear-gradient(45deg, #ffeaa7, #fab1a0);
            border-radius: 20px;
            padding: 25px;
            margin: 20px 0;
            border: 2px solid #e17055;
        }
        
        .info-item {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin: 15px 0;
            padding: 15px;
            background: rgba(255, 255, 255, 0.7);
            border-radius: 15px;
            border: 2px dashed #ff7675;
        }
        
        .label {
            font-weight: bold;
            color: #2d3436;
            font-size: 1.1em;
        }
        
        .value {
            color: #e84393;
            font-size: 1.2em;
            font-weight: bold;
        }
        
        .status-badge {
            background: linear-gradient(45deg, #fd79a8, #fdcb6e);
            padding: 15px 25px;
            border-radius: 30px;
            font-weight: bold;
            display: inline-block;
            margin-top: 20px;
            color: white;
            text-shadow: 1px 1px 2px rgba(0, 0, 0, 0.3);
            box-shadow: 0 6px 20px rgba(253, 121, 168, 0.4);
            animation: wiggle 2s ease-in-out infinite;
        }
        
        @keyframes wiggle {
            0%, 100% { transform: rotate(0deg); }
            25% { transform: rotate(1deg); }
            75% { transform: rotate(-1deg); }
        }
        
        .welcome-message {
            font-size: 1.3em;
            margin-top: 25px;
            line-height: 1.6;
            color: #2d3436;
            background: linear-gradient(45deg, #a29bfe, #6c5ce7);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            background-clip: text;
            font-weight: bold;
        }
        
        .stars {
            position: absolute;
            top: 10px;
            right: 10px;
            font-size: 20px;
            animation: twinkle 1.5s ease-in-out infinite;
        }
        
        @keyframes twinkle {
            0%, 100% { opacity: 1; transform: scale(1); }
            50% { opacity: 0.5; transform: scale(1.2); }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="stars">✨⭐✨</div>
        <div class="icon">🎉</div>
        <h1>Young Explorer Registration</h1>
        
        <div class="user-info">
            <div class="info-item">
                <span class="label">👦 Username:</span>
                <span class="value">' . $username . '</span>
            </div>
            <div class="info-item">
                <span class="label">📮 Email:</span>
                <span class="value">' . $email . '</span>
            </div>
            <div class="info-item">
                <span class="label">🎂 Age:</span>
                <span class="value">' . $age . ' years young!</span>
            </div>
        </div>
        
        <div class="status-badge">🌟 Youth Account Created!</div>
        
        <div class="welcome-message">
            Hey there, young explorer! Welcome to our fun platform! Since you\'re under 18, you\'ll have access to age-appropriate content and features designed just for you. Have fun and stay safe! 🚀
        </div>
    </div>
</body>
</html>';
    }
    
} else {
    echo '<!DOCTYPE html>
<html>
<head>
    <title>Error</title>
    <style>
        body { 
            font-family: Arial, sans-serif; 
            background: #f0f0f0; 
            display: flex; 
            justify-content: center; 
            align-items: center; 
            height: 100vh; 
            margin: 0;
        }
        .error { 
            background: white; 
            padding: 30px; 
            border-radius: 10px; 
            box-shadow: 0 4px 6px rgba(0,0,0,0.1);
            text-align: center;
        }
    </style>
</head>
<body>
    <div class="error">
        <h2>❌ No Data Received</h2>
        <p>Please submit the form using POST method.</p>
    </div>
</body>
</html>';
}
?>