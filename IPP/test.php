<?php
include('test/Tests.php');
include('test/HTMLGenerator.php');

$shortopts = "hd::rp::i::";
$longopts = array("help", "directory::", "recursive", "parse-script::", "int-script::");
$options = getopt($shortopts, $longopts);

// Kontrola parametru
if (count($options) + 1 != $argc) {
    echo("You enter wrong arguments use -h or --help for help\n");
    exit(10);
}

$parse_script = "parse.php";
$interpret_script = "interpret.py";
$directory_path = ".";
$recursive = false;
$parse_set = false;
$intepret_set = false;
$directory_set = false;

// Zpracovani parametru
if (!empty($options)) {
    foreach (array_keys($options) as $option) {
        switch ($option) {
            case "h":
            case "help":
                echo("usage: test.php [-h] [-d=PATH] [-r] [-p=FILE] [-i=FILE]\noptional arguments:\n\t-h, --help\t\t\tshow this help message and exit\n"
                    . "\t--directory=PATH, -d=PATH\tpath to directory with tests\n\t-r, --recursive\t\t\ttests will be find in subdirs"
                    . "\n\t-p=FILE, --parse-script=FILE\tparser file\n"
                    . "\t-i=FILE, --int-script=FILE\tinterpret file\n");
                exit(0);
                break;
            case "r":
            case "recursive":
                $recursive = true;
                break;
            case "d":
                if ($directory_set == true) {
                    fprintf(STDERR, "Can't use --directory and -d at same time\n");
                    exit(10);
                }
                $directory_path = $options["d"];
                $directory_set = true;
                break;
            case "directory":
                if ($directory_set == true) {
                    fprintf(STDERR, "Can't use --directory and -d at same time\n");
                    exit(10);
                }
                $directory_path = $options["directory"];
                $directory_set = true;
                break;
            case "i":
                if ($intepret_set == true) {
                    fprintf(STDERR, "Can't use --int-script and -i at same time\n");
                    exit(10);
                }
                $intepret_set = true;
                $interpret_script = $options["i"];
                break;
            case "int-script":
                if ($intepret_set == true) {
                    fprintf(STDERR, "Can't use --int-script and -i at same time\n");
                    exit(10);
                }
                $intepret_set = true;
                $interpret_script = $options["int-script"];
                break;
            case "p":
                if ($parse_set == true) {
                    fprintf(STDERR, "Can't use --parse-script and -p at same time\n");
                    exit(10);
                }
                $parse_script = $options["p"];
                $parse_set = true;
                break;
            case "parse-script":
                if ($parse_set == true) {
                    fprintf(STDERR, "Can't use --parse-script and -p at same time\n");
                    exit(10);
                }
                $parse_script = $options["parse-script"];
                $parse_set = true;
                break;
        }
    }
}

$tests = new Tests($parse_script, $interpret_script, $directory_path, $recursive);
$tests->find_tests();
$tests->do_tests();

$generator = new HTMLGenerator($tests->get_tests());
$output = $generator->generate_html();
echo $output;

?>