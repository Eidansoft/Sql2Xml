<?php
/**
 * @param msg - $msg - The message with error info
 */
function finalizaConErrorXML($msg, $cod = 0){
	$dom = new DOMDocument("1.0");
	$node = $dom->createElement("error");
	$parnode = $dom->appendChild($node);
	$nodeMsg = $dom->createElement("msg", $msg);
	$parnode->appendChild($nodeMsg);
	if ( $cod != 0 ){
		$nodeCod = $dom->createElement("cod", $cod);
		$parnode->appendChild($nodeCod);
	}
	echo $dom->saveXML();
	die();
}

/**
 * @param mysql_resource - $queryResult - mysql query result
 * @param string - $rootElementName - root element name
 * @param string - $childElementName - each child element name
 */
function sqlToXmlObj($connection, $rootElementName, $childElementName, $sql, $conNulos=true){

	//Limit max num results in XML
	$maxResults = 100;

	// Select all the rows in the query
	$result = $connection->query($sql);
	if (!$result) { finalizaConErrorXML('Invalid query: ' . $connection->connect_error); }
	$numRegistros = $result->num_rows;
	
	$dom = new DOMDocument("1.0");
	$node = $dom->createElement($rootElementName);
	$parnode = $dom->appendChild($node);
	$parnode->setAttribute("sql", $sql);
	$parnode->setAttribute("numTotal", $numRegistros);
	$parnode->setAttribute("maxRes", $maxResults);

	$contador = 0;
	while( ($contador < $maxResults) && $registro = $result->fetch_row()){
		$contador++;
		// ADD TO XML DOCUMENT NODE  
		$node = $dom->createElement($childElementName);
		$newnode = $parnode->appendChild($node);
		
		for ($i=0; $i < $connection->field_count; $i++)
        {
			$fieldMetadata = $result->fetch_field_direct($i);
			$fieldName = $fieldMetadata->name;
			 /* We set empty columns with NULL */
			 if( $conNulos && empty( $registro[$i] ) ) {
				 $fieldValue = "null";
			 } else {
				 $fieldValue = $registro[$i];
			 }

			 try{
				/* The child will take the name of the table column */
				$field = $dom->createElement($fieldName, $fieldValue);
				$newfield = $newnode->appendChild($field);
			} catch (DOMException $e) {
				finalizaConErrorXML('Parece que hay caracteres invalidos en los nombres de las columnas');
			}
        }
	}
	$result->close();
	return $dom;
}

/**
 * @param mysql_resource - $queryResult - mysql query result
 * @param string - $rootElementName - root element name
 * @param string - $childElementName - each child element name
 */
function sqlToXml($queryResult, $rootElementName, $childElementName)
{ 
    $xmlData = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\n"; 
    $xmlData .= "<" . $rootElementName . ">";
 
    while($record = mysql_fetch_object($queryResult))
    { 
        /* Create the first child element */
        $xmlData .= "<" . $childElementName . ">";
 
        for ($i = 0; $i < mysql_num_fields($queryResult); $i++)
        { 
            $fieldName = mysql_field_name($queryResult, $i); 
 
            /* The child will take the name of the table column */
            $xmlData .= "<" . $fieldName . ">";
 
            /* We set empty columns with NULL, or you could set 
                it to '0' or a blank. */
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
}
?>