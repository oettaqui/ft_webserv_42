<?php
// logout.php
session_start();

// Destroy session
session_destroy();

// Delete cookie
setcookie('user_login', '', time() - 3600, '/');

// Redirect to login page
echo '<script>window.location.href="http://localhost:8888/cgi-bin/session_cookie/login.php";</script>';
exit();
?>