<?php
header ("Content-Type: application/xml");

include_once "funciones.php.inc";

$host="YOUR_IP_HERE";
$username="YOUR_USER_HERE";
$password="YOUR_PASSWORD_HERE";
$database="YOUR_DATABASE_NAME_HERE";

//$query = "SELECT p.*, m.mail FROM persona p LEFT JOIN mail m ON p.id = m.idPersona";
if ( isset($_REQUEST['sql']) ) { 
	$query = $_REQUEST['sql'];
} else {
	$query = "";
}

//Test the query to avoid some errors
if ( trim($query) == "" ) { finalizaConErrorXML("Peticion NO valida."); }
if ( false !== strpos($query, '"') ) { finalizaConErrorXML("No se permite el caracter comillas dobles en la query."); }
 
// Opens a connection to a MySQL server
$connection = new mysqli($host, $username, $password, $database);
if ($connection->connect_errno) { finalizaConError('Not connected : ' . $connection->connect_error); } 

//Call the function to get the results in XML
$dom = sqlToXmlObj($connection, "query", "resultado", $query);

//Send the results
echo $dom->saveXML();

mysqli_close($connection);
?>