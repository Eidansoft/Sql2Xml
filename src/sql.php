<?php
/**
 * PHP funcions to generate responses in XML or JSON for a SQL query
 * @author Alejandro Lorente
 * @author eidansoft at gmail dot com
 * @license GPLv3.0
 * @sources https://github.com/Eidansoft/Sql2Xml
 */

include_once "./configuration.php.inc";
include_once "./funciones.php.inc";

if ($_REQUEST['format'] == "XML"){
	header ("Content-Type: application/xml");
} elseif ($_REQUEST['format'] == "JSON"){
	header ("Content-Type: application/json");
} else {
	header ("Content-Type: text/plain");
	endWithError("TXT", "Invalid format");
}

if ( ! isset($_REQUEST['sql']) ) { 
	endWithError($_REQUEST['format'], "Invalid request");
}

//Test the query to avoid some errors
if ( trim($_REQUEST['sql']) == "" ) { endWithError($_REQUEST['format'], "Empty query not valid"); }
if ( false !== strpos($_REQUEST['sql'], '"') ) { endWithError($_REQUEST['format'], "Double quotes character is not allowed at the query"); }
 
// Opens a connection to a MySQL server
$connection = new mysqli($host, $username, $password, $database);
if ($connection->connect_errno) { endWithError($_REQUEST['format'], 'Not connected to DB : ' . $connection->connect_error); } 

//Call the function to get the results in format requested by parameter
if ($_REQUEST['format'] == "XML"){
	$res = sqlToXml($connection, $_REQUEST['sql']);
} elseif ($_REQUEST['format'] == "JSON"){
	$res = sqlToJson($connection, $_REQUEST['sql']);
}

//Send the results
echo $res;

mysqli_close($connection);
?>

