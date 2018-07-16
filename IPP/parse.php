<?php
include_once('parser/XMLAttribute.php');
include_once('parser/XMLInstruction.php');
include_once('parser/IPPXMLWriter.php');
include_once('parser/IPPAnalyzer.php');

$shortopts = "hs::lc";
$longopts = array("help", "stats::", "loc", "comments");
$bonus_file = "";
$lines_of_codes = false;
$comments = false;
$loc_first = false;

// Ziskani parametru
$options = getopt($shortopts, $longopts);
if (count($options) + 1 != $argc) {
    echo("You enter wrong arguments use -h or --help for help\n");
    exit(10);
}
// Zpracovani jednotlivych parametru
if (!empty($options)) {
    foreach (array_keys($options) as $option) {
        switch ($option) {
            case "help":
            case "h":
                if (count($options) + 1 != 2) {
                    fprintf(STDERR, 'Can\'t use help with other parametr\n');
                    exit(10);
                }
                echo("usage: parser.php [-h] [--stats=STATS] [-l] [-c]\noptional arguments:\n\t-h, --help\t\t\tshow this help message and exit\n"
                    . "\t--stats=FILE, -s=FILE\t\tfile where statistics will be save\n\t-l, --loc\t\t\tprint to statistics number of rows with instructions"
                    . "\n\t-c, --comments\t\t\tprint to statistics number of rows with commentary\n");
                exit(0);
                break;
            case "stats":
                if ($bonus_file != "") {
                    fprintf(STDERR, "Can't use --stats and -s at same time\n");
                    exit(10);
                }
                $bonus_file = $options["stats"];
                break;
            case "s":
                if ($bonus_file != "") {
                    fprintf(STDERR, "Can't use --stats and -s at same time\n");
                    exit(10);
                }
                $bonus_file = $options["s"];
                break;
            case "loc":
            case "l":
                if ($comments == false) {
                    $loc_first = true;
                }
                $lines_of_codes = true;
                break;
            case "comments":
            case "c":
                $comments = true;
                break;
        }
    }
}

// Kontrola jestli nebyli zadany parametry loc nebo comments bez parametru stats
if (($comments == true || $lines_of_codes == true) && $bonus_file == "") {
    fprintf(STDERR, "Can't use --loc or --comments or -l or -c without --stats=FILE or -s=FILE\n");
    exit(10);
}

$xw = new IPPXMLWriter();
$lexical = new IPPAnalyzer("php://stdin");

// Kontrola jestli vstup obsahuje hlavicku
if(!$lexical->check_header()){
    fprintf(STDERR, "SYNTAX ERROR: Missing code header\n");
    exit(21);
}

// Nacitani a zpracovani jednotlivych radku
while(!$lexical->check_eof()){
    $instruction = $lexical->read_line();
    if($instruction != null){
        $xw->write_instruction($instruction);
    }
}

// Ukonceni zapisovani XML
$xw->close_writer("php://stdout");

// Zpracovani statistik, zápis do souboru
if ($bonus_file != "") {
    $file = fopen($bonus_file, "w");
    if (!$file) {
        fprintf(STDERR, "Cannot open output file\n");
        exit(12);
    }
    if ($loc_first == true) {
        fprintf($file, $lexical->get_lines_of_code() . "\n");
        if ($comments == true) {
            fprintf($file, $lexical->get_number_of_commentary());
        }
    } else {
        if ($comments == true) {
            fprintf($file, $lexical->get_number_of_commentary());
            if ($lines_of_codes == true) {
                fprintf($file, "\n" . $lexical->get_lines_of_code());
            }
        }
    }
    fclose($file);
}
exit(0);
?>