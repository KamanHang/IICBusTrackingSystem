<?php
header('Content-Type: application/json');

// Connect to the database
$conn = mysqli_connect("localhost", "root", "", "busmanagement");

// Check the connection
if (mysqli_connect_errno()) {
    echo "Failed to connect to MySQL: " . mysqli_connect_error();
    exit();
}

// Prepare the SQL query
$sqlQuery = "SELECT id, lat, lng FROM tbl_gps";
$result = mysqli_query($conn, $sqlQuery);

// Create an empty array to store the data
$data = array();

// Fetch the data and add it to the array
while ($row = mysqli_fetch_assoc($result)) {
    $data[] = $row;
}

// Close the database connection
mysqli_close($conn);

// Return the data as JSON
echo json_encode($data);
?>
