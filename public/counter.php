<?php
$counterFile = 'counter.txt';

// Read current count
if (file_exists($counterFile)) {
    $count = (int)file_get_contents($counterFile);
} else {
    $count = 0;
}

// Increment count
$count++;

// Save new count
file_put_contents($counterFile, $count);

// Output response
header('Content-Type: text/html');
echo "<html><body>";
echo "<h1>Visitor Count: $count</h1>";
echo "<p>Query parameters: " . $_GET['param'] . "</p>";
echo "</body></html>";
?>