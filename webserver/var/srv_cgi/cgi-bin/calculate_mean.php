<?php

function parse_arguments($query_string) {
    $params = explode('&', $query_string);
    $parsed_args = [];
    foreach ($params as $param) {
        if (is_numeric($param)) {
            $parsed_args[] = intval($param);
        }
    }
    return $parsed_args;
}

if ($argc < 2) {
    echo "Error: one argument is needed.\n";
    exit(1);
}

$query_string = $argv[1];
$parsed_args = parse_arguments($query_string);

if (!empty($parsed_args)) {
    $mean = array_sum($parsed_args) / count($parsed_args);
    echo "The mean of the integer values is: ", intval($mean), "\n";
} else {
    echo "There are no integer values to calculate the mean.\n";
}
?>
