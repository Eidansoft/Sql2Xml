<?php
/**
 * PHP funcions to generate responses in XML or JSON for a SQL query
 * @author Alejandro Lorente
 * @author eidansoft at gmail dot com
 * @license GPLv3.0
 * @sources https://github.com/Eidansoft/Sql2Xml
 */
// Limit max num results to respond for a request
$maxResults = 50;

/**
 * Print to the user an error message at the specified format
 * @param string - $type - The format, between XML or JSON. For other values it'll print a generic message
 * @param string - $msg - The message with error info
 * @param int - $cod - Optional. A number to identify the error by the application of the client. 
 */
function endWithError($type, $msg, $cod = 1){

	if ($type == "XML"){
		$dom = new DOMDocument("1.0");

		$node = $dom->createElement("error");
		$parnode = $dom->appendChild($node);

		$nodeMsg = $dom->createElement("msg", $msg);
		$parnode->appendChild($nodeMsg);

		$nodeCod = $dom->createElement("cod", $cod);
		$parnode->appendChild($nodeCod);

		$res = $dom->saveXML();
	} elseif ($type == "JSON"){
		$row = array();
		$row["error"]["msg"] = $msg;
		$row["error"]["cod"] = $cod;
		$res = json_encode($row);
	} else {
		$res = $msg . ". Error code " . $cod . ".";
	}

	echo $res;
	die();
}

/**
 * Return a DOM object with the contents of execute the SQL at a XML format
 * @param mysqli - $connection - mysql Connection
 * @param string - $rootElementName - root element name
 * @param string - $childElementName - each child element name
 * @param string - $sql - SQL sentence
 * @param boolean - $withNullValues - If true, for empty values, the DOM will contain the "null" string
 */
function sqlToXmlObj($connection, $rootElementName, $childElementName, $sql, $withNullValues=true){
	global $maxResults;
	// Select all the rows in the query
	$result = $connection->query($sql);
	if (!$result) { endWithError("XML", 'Invalid query (' . $sql . '): ' . $connection->connect_error); }
	$numRegistros = $result->num_rows;
	
	$dom = new DOMDocument("1.0");
	$node = $dom->createElement($rootElementName);
	$parnode = $dom->appendChild($node);
	$parnode->setAttribute("sql", $sql);
	$parnode->setAttribute("total", $numRegistros);
	//$parnode->setAttribute("limit", $maxResults);

	$counter = 0;
	while( ($counter < $maxResults) && $registro = $result->fetch_row()){
		$counter++;
		// ADD TO XML DOCUMENT NODE
		$node = $dom->createElement($childElementName);
		$newnode = $parnode->appendChild($node);
		
		for ($i=0; $i < $connection->field_count; $i++)
        {
			$fieldMetadata = $result->fetch_field_direct($i);
			$fieldName = $fieldMetadata->name;
			 // We set empty columns with NULL
			 if( $withNullValues && empty( $registro[$i] ) ) {
				 $fieldValue = "null";
			 } else {
				 $fieldValue = $registro[$i];
			 }

			 try{
				// The child will take the name of the table column
				$field = $dom->createElement($fieldName, $fieldValue);
				$newfield = $newnode->appendChild($field);
			} catch (DOMException $e) {
				endWithError("XML", 'It seems you have not valid characters at columns names');
			}
        }
	}
	$parnode->setAttribute("totalReturned", $counter);

	$result->close();
	return $dom;
}

/**
 * Return a XML string with the contents of execute the SQL
 * @param mysqli - $connection - mysql Connection
 * @param string - $sql - SQL sentence
 * @param boolean - $withNullValues - If true, for empty values, the XML correspondant values will contain the "null" string
 */
function sqlToXml($connection, $sql, $withNullValues=true){
	$dom = sqlToXmlObj($connection, "query", "resultado", $sql, $withNullValues);
	return $dom->saveXML();
}

/**
 * Return a JSON string with the contents of execute the SQL
 * @param mysqli - $connection - mysql Connection
 * @param string - $sql - SQL sentence
 * @param boolean - $withNullValues - If true, for empty values, the JSON correspondant values will contain the "null" string
 */
function sqlToJson($connection, $sql, $withNullValues=true){
	global $maxResults;
	// Select all the rows in the query
	$result = $connection->query($sql);
	if (!$result) { endWithError("JSON", 'Invalid query (' . $sql . '): ' . $connection->connect_error); }
	$numRegistros = $result->num_rows;

	$rows = array();
	while( (count($rows) < $maxResults) && $registro = $result->fetch_row()){
		$row = array();
		for ($i=0; $i < $connection->field_count; $i++)
        {
			$fieldMetadata = $result->fetch_field_direct($i);
			$fieldName = $fieldMetadata->name;
			// We set empty columns with NULL
			if( $withNullValues && empty( $registro[$i] ) ) {
				$fieldValue = "null";
			} else {
				$fieldValue = $registro[$i];
			}

			$row[$fieldName] = $fieldValue;
        }
		$rows[] = $row;
	}
	
	$result->close();
	return json_encode($rows);
}

/** DEPRECATED Old function to create the XML manually. Now use "sqlToXmlObj" instead.
 * @param mysql_resource - $queryResult - mysql query result
 * @param string - $rootElementName - root element name
 * @param string - $childElementName - each child element name

function sqlToXml($queryResult, $rootElementName, $childElementName)
{ 
    $xmlData = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\n"; 
    $xmlData .= "<" . $rootElementName . ">";
 
    while($record = mysql_fetch_object($queryResult))
    { 
        // Create the first child element
        $xmlData .= "<" . $childElementName . ">";
 
        for ($i = 0; $i < mysql_num_fields($queryResult); $i++)
        { 
            $fieldName = mysql_field_name($queryResult, $i); 
 
            // The child will take the name of the table column
            $xmlData .= "<" . $fieldName . ">";
 
            // We set empty columns with NULL, or you could set it to '0' or a blank.
            if(!empty($record->$fieldName))
                $xmlData .= $record->$fieldName; 
            else
                $xmlData .= "null"; 
 
            $xmlData .= "</" . $fieldName . ">"; 
        } 
        $xmlData .= "</" . $childElementName . ">"; 
    } 
    $xmlData .= "</" . $rootElementName . ">"; 
 
    return $xmlData; 
} */

function debug($msg){
	echo $msg;
	die();
}
?>
