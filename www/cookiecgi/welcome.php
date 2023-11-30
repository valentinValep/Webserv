<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Welcome!</title>
	<link rel="stylesheet" type="text/css" href="/style/style.css">
	<link rel="icon" href="/favicon.gif" type="image/gif"/>
</head>
	<body>
		<?php
			$query = getenv('QUERY_STRING');
			parse_str($query, $output);
			echo "Hi, " . $output['fname'] . " " . $output['lname'];
			echo "<br>";
			echo "Welcome!";
		?>
    </body>
</html>
