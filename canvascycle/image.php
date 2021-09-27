<?php

$file = 'images/'.preg_replace('/\.\./', '', $_GET['file']).'.LBM.json';
$last_modified_time = filemtime($file);

if (!$last_modified_time) die("File does not exist: $file\n");

if (isset($_SERVER['HTTP_IF_MODIFIED_SINCE']) && (strtotime($_SERVER['HTTP_IF_MODIFIED_SINCE']) == $last_modified_time)) { 
    header("HTTP/1.1 304 Not Modified"); 
    exit(0);
}

$ttl = 31536000;
header( 'Content-Type: text/javascript' );
header( "Cache-Control: max-age=$ttl" );
header( 'Last-Modified: ' . gmdate(DATE_RFC822, $last_modified_time) );
header( 'Expires: ' . gmdate(DATE_RFC822, time() + $ttl) );
header( "Vary: Accept-Encoding" );
header( "Accept-Ranges: none" );

$content = $_GET['callback'] . '(' . file_get_contents($file) . ");\n";

if (isset($_SERVER['HTTP_ACCEPT_ENCODING']) && preg_match("/gzip/i", $_SERVER['HTTP_ACCEPT_ENCODING'])) {
	$content = gzencode( $content, 9 );
	header( "Content-Encoding: gzip" );
}

header( "Content-Length: " . strlen($content) );
print $content;
exit();

?>