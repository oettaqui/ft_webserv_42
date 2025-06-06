<?php
// Check request method
$requestMethod = $_SERVER['REQUEST_METHOD'];

// If GET method is used, show error message
if ($requestMethod === 'GET') {
    $showGetError = true;
    $showResults = false;
    $allParamsExist = false;
} else {
    // Handle POST method
    $showGetError = false;
    $formSubmitted = $requestMethod === 'POST';
    
    // Get parameters from POST
    $username = isset($_POST['username']) ? trim($_POST['username']) : '';
    $email = isset($_POST['email']) ? trim($_POST['email']) : '';
    $age = isset($_POST['age']) ? trim($_POST['age']) : '';
    
    // Check if all parameters exist and are not empty
    $allParamsExist = $formSubmitted && !empty($username) && !empty($email) && !empty($age);
    $showResults = $formSubmitted;
}
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>User Information - POST Method</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        body {
            font-family: 'Arial', sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
            padding: 20px;
        }

        .container {
            background: white;
            padding: 40px;
            border-radius: 15px;
            box-shadow: 0 20px 40px rgba(0, 0, 0, 0.1);
            max-width: 500px;
            width: 100%;
            text-align: center;
        }

        .success-container {
            border-left: 5px solid #28a745;
        }

        .error-container {
            border-left: 5px solid #dc3545;
        }

        .form-container {
            border-left: 5px solid #667eea;
        }

        h1 {
            color: #333;
            margin-bottom: 30px;
            font-size: 2.2em;
        }

        .success-title {
            color: #28a745;
        }

        .error-title {
            color: #dc3545;
        }

        .form-title {
            color: #667eea;
        }

        /* Form Styles */
        .form-group {
            margin-bottom: 20px;
            text-align: left;
        }

        .form-label {
            display: block;
            margin-bottom: 8px;
            font-weight: bold;
            color: #495057;
        }

        .form-input {
            width: 100%;
            padding: 12px 15px;
            border: 2px solid #e9ecef;
            border-radius: 8px;
            font-size: 16px;
            transition: border-color 0.3s ease;
        }

        .form-input:focus {
            outline: none;
            border-color: #667eea;
            box-shadow: 0 0 0 3px rgba(102, 126, 234, 0.1);
        }

        .submit-btn {
            background: #667eea;
            color: white;
            padding: 15px 30px;
            border: none;
            border-radius: 25px;
            font-size: 16px;
            cursor: pointer;
            transition: background 0.3s ease;
            width: 100%;
        }

        .submit-btn:hover {
            background: #5a6fd8;
        }

        /* Results Styles */
        .user-info {
            background: #f8f9fa;
            padding: 25px;
            border-radius: 10px;
            margin: 20px 0;
            text-align: left;
        }

        .info-item {
            display: flex;
            align-items: center;
            margin-bottom: 15px;
            padding: 10px;
            background: white;
            border-radius: 8px;
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.05);
        }

        .info-item:last-child {
            margin-bottom: 0;
        }

        .info-label {
            font-weight: bold;
            color: #495057;
            min-width: 80px;
            margin-right: 15px;
        }

        .info-value {
            color: #333;
            font-size: 1.1em;
        }

        .icon {
            width: 20px;
            height: 20px;
            margin-right: 10px;
            fill: #667eea;
        }

        .error-message {
            background: #f8d7da;
            color: #721c24;
            padding: 20px;
            border-radius: 10px;
            margin: 20px 0;
            border: 1px solid #f5c6cb;
        }

        .back-btn {
            background: #6c757d;
            color: white;
            padding: 12px 25px;
            text-decoration: none;
            border-radius: 25px;
            display: inline-block;
            margin-top: 20px;
            transition: background 0.3s ease;
        }

        .back-btn:hover {
            background: #5a6268;
        }

        .form-description {
            background: #e7f3ff;
            padding: 15px;
            border-radius: 8px;
            margin-bottom: 25px;
            color: #0c5460;
            border-left: 4px solid #667eea;
        }
    </style>
</head>
<body>
    <div class="container <?php 
        if ($showGetError) echo 'error-container';
        elseif (!$showResults) echo 'form-container';
        elseif ($allParamsExist) echo 'success-container';
        else echo 'error-container';
    ?>">
        
        <?php if ($showGetError): ?>
            <!-- Show GET Method Error -->
            <h1 class="error-title">
                <svg class="icon" viewBox="0 0 24 24">
                    <path d="M18.364 18.364A9 9 0 005.636 5.636m12.728 12.728L5.636 5.636m12.728 12.728L18.364 5.636M5.636 18.364l12.728-12.728"/>
                </svg>
                Method Not Supported
            </h1>
            
            <div class="error-message">
                <strong>GET method is not supported!</strong><br>
                This page only accepts POST requests. Please use a form or send a POST request to access this functionality.
            </div>
            
        <?php elseif (!$showResults): ?>
            <!-- Show Form -->
            <h1 class="form-title">
                <svg class="icon" viewBox="0 0 24 24">
                    <path d="M16 4h2a2 2 0 012 2v14a2 2 0 01-2 2H6a2 2 0 01-2-2V6a2 2 0 012-2h2m8 0V2a2 2 0 00-2-2h-4a2 2 0 00-2 2v2m8 0V4a2 2 0 00-2-2h-4a2 2 0 00-2 2v0"/>
                </svg>
                User Information Form
            </h1>
            
            <div class="form-description">
                Please fill in all the required fields below to submit your information.
            </div>
            
            <form method="POST" action="<?php echo $_SERVER['PHP_SELF']; ?>">
                <div class="form-group">
                    <label class="form-label" for="username">
                        <svg class="icon" style="display: inline;" viewBox="0 0 24 24">
                            <path d="M16 7a4 4 0 11-8 0 4 4 0 018 0zM12 14a7 7 0 00-7 7h14a7 7 0 00-7-7z"/>
                        </svg>
                        Username *
                    </label>
                    <input type="text" id="username" name="username" class="form-input" placeholder="Enter your username" required>
                </div>
                
                <div class="form-group">
                    <label class="form-label" for="email">
                        <svg class="icon" style="display: inline;" viewBox="0 0 24 24">
                            <path d="M3 8l7.89 4.26a2 2 0 002.22 0L21 8M5 19h14a2 2 0 002-2V7a2 2 0 00-2-2H5a2 2 0 00-2 2v10a2 2 0 002 2z"/>
                        </svg>
                        Email Address *
                    </label>
                    <input type="email" id="email" name="email" class="form-input" placeholder="Enter your email address" required>
                </div>
                
                <div class="form-group">
                    <label class="form-label" for="age">
                        <svg class="icon" style="display: inline;" viewBox="0 0 24 24">
                            <path d="M8 7V3a2 2 0 012-2h4a2 2 0 012 2v4m-6 9a6 6 0 1012 0v-3"/>
                        </svg>
                        Age *
                    </label>
                    <input type="number" id="age" name="age" class="form-input" placeholder="Enter your age" min="1" max="120" required>
                </div>
                
                <button type="submit" class="submit-btn">
                    Submit Information
                </button>
            </form>
            
        <?php elseif ($allParamsExist): ?>
            <!-- Show Success Results -->
            <h1 class="success-title">
                <svg class="icon" viewBox="0 0 24 24">
                    <path d="M9 12l2 2 4-4m6 2a9 9 0 11-18 0 9 9 0 0118 0z"/>
                </svg>
                Information Submitted Successfully
            </h1>
            
            <div class="user-info">
                <div class="info-item">
                    <svg class="icon" viewBox="0 0 24 24">
                        <path d="M16 7a4 4 0 11-8 0 4 4 0 018 0zM12 14a7 7 0 00-7 7h14a7 7 0 00-7-7z"/>
                    </svg>
                    <span class="info-label">Username:</span>
                    <span class="info-value"><?php echo htmlspecialchars($username); ?></span>
                </div>
                
                <div class="info-item">
                    <svg class="icon" viewBox="0 0 24 24">
                        <path d="M3 8l7.89 4.26a2 2 0 002.22 0L21 8M5 19h14a2 2 0 002-2V7a2 2 0 00-2-2H5a2 2 0 00-2 2v10a2 2 0 002 2z"/>
                    </svg>
                    <span class="info-label">Email:</span>
                    <span class="info-value"><?php echo htmlspecialchars($email); ?></span>
                </div>
                
                <div class="info-item">
                    <svg class="icon" viewBox="0 0 24 24">
                        <path d="M8 7V3a2 2 0 012-2h4a2 2 0 012 2v4m-6 9a6 6 0 1012 0v-3"/>
                    </svg>
                    <span class="info-label">Age:</span>
                    <span class="info-value"><?php echo htmlspecialchars($age); ?> years old</span>
                </div>
            </div>
            
           
            
        <?php else: ?>
            <!-- Show Error Results -->
            <h1 class="error-title">
                <svg class="icon" viewBox="0 0 24 24">
                    <path d="M12 9v2m0 4h.01m-6.938 4h13.856c1.54 0 2.502-1.667 1.732-2.5L13.732 4c-.77-.833-1.964-.833-2.732 0L3.732 16.5c-.77.833.192 2.5 1.732 2.5z"/>
                </svg>
                Incomplete Information
            </h1>
            
            <div class="error-message">
                <strong>Some required fields are missing or empty!</strong><br>
                Please check the following fields:
                <ul style="text-align: left; margin-top: 10px;">
                    <li><strong>Username</strong> <?php echo empty($username) ? '❌ Missing or empty' : '✅ Provided'; ?></li>
                    <li><strong>Email</strong> <?php echo empty($email) ? '❌ Missing or empty' : '✅ Provided'; ?></li>
                    <li><strong>Age</strong> <?php echo empty($age) ? '❌ Missing or empty' : '✅ Provided'; ?></li>
                </ul>
            </div>
            
            <a href="<?php echo $_SERVER['PHP_SELF']; ?>" class="back-btn">
                Try Again
            </a>
        <?php endif; ?>
    </div>
</body>
</html>