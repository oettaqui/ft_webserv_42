<?php
// Basic while(true) loop example
$counter = 0;

while (true) {
    echo "Loop iteration: " . $counter . PHP_EOL;
    $counter++;
    
    // Add a small delay to prevent overwhelming output
    sleep(1);
    
    // Example break condition to prevent infinite execution
    if ($counter >= 10) {
        echo "Breaking out of infinite loop after 10 iterations" . PHP_EOL;
        break;
    }
}

echo "Loop ended." . PHP_EOL;
?>