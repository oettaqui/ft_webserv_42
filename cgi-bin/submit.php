<?php
header('Content-Type: text/html');
echo "<html><body>";
echo "<h1>Registration Received</h1>";

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    echo "<p>Username: " . htmlspecialchars($_POST['username']) . "</p>";
    echo "<p>Email: " . htmlspecialchars($_POST['email']) . "</p>";
    
    // Save to file for demonstration
    $data = "User: " . $_POST['username'] . ", Email: " . $_POST['email'] . "\n";
    file_put_contents('users.txt', $data, FILE_APPEND);
    
    echo "<p>Registration saved successfully!</p>";
}
echo "</body></html>";
?>