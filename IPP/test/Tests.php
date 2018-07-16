<?php
include_once('TestRepresentation.php');
class Tests
{
    private $parse_script;
    private $interpret_script;
    private $directory_path;
    private $recursive;
    private $tests = array();

    /**
     * Tests constructor.
     * @param $parse_script Cesta k parseru
     * @param $interpret_script Cesta k interpretu
     * @param $directory_path Slozka s testy
     * @param $recursive Rekurzivni prohledavani (bool)
     */
    public function __construct($parse_script, $interpret_script, $directory_path, $recursive)
    {
        $this->parse_script = $parse_script;
        $this->interpret_script = $interpret_script;
        $this->directory_path = $directory_path;
        $this->recursive = $recursive;
    }

    /**
     * Funkce hleda soubory s testy, popr. vytvori nove potrebne soubory
     */
    public function find_tests()
    {
        $command = "find " . $this->directory_path;
        if (!$this->recursive) {
            $command .= " -maxdepth 1";
        }
        $src_files = preg_split('/\s+/', shell_exec($command . " | grep .src$"), -1, PREG_SPLIT_NO_EMPTY);
        $out_files = preg_split('/\s+/', shell_exec($command . " | grep .out$"), -1, PREG_SPLIT_NO_EMPTY);
        $rc_files = preg_split('/\s+/', shell_exec($command . " | grep .rc$"), -1, PREG_SPLIT_NO_EMPTY);
        $in_files = preg_split('/\s+/', shell_exec($command . " | grep .in$"), -1, PREG_SPLIT_NO_EMPTY);

        foreach ($src_files as $src) {
            $test = new TestRepresentation();
            $base_name = preg_replace("/.src$/", "", $src, 1);
            if (!in_array($base_name . ".out", $out_files)) {
                touch($base_name . ".out");
            }
            if (!in_array($base_name . ".rc", $rc_files)) {
                touch($base_name . ".rc");
                $rc_file = fopen($base_name . ".rc", "w");
                fprintf($rc_file, "0");
            }
            if (!in_array($base_name . ".in", $in_files)) {
                touch($base_name . ".in");
            }
            $test->set_name($base_name);
            $this->add_test($test);
        }
    }

    /**
     * Funkce provede vsechny testy
     */
    public function do_tests()
    {
        foreach ($this->tests as $test) {
            $test->do_test($this->parse_script, $this->interpret_script);
        }
    }

    /**
     * Funkce prida do pole novy test
     * @param $test Novy test
     */
    public function add_test($test)
    {
        array_push($this->tests, $test);
    }

    /**
     * Ziska cestu k parseru
     * @return mixed cesta k parseru
     */
    public function get_parse_script()
    {
        return $this->parse_script;
    }

    /**
     * Ziska cestu k interpretu
     * @return mixed Cesta k interpretu
     */
    public function get_interpret_script()
    {
        return $this->interpret_script;
    }

    /**
     * Ziska cestu k testum
     * @return mixed Cesta k testum
     */
    public function get_directory_path()
    {
        return $this->directory_path;
    }

    /**
     * Zjisti jestli se ma vyhlevat rekurzivne
     * @return mixed True pokud ano jinak false
     */
    public function get_recursive()
    {
        return $this->recursive;
    }

    /**
     * Ziska vsechny testy
     * @return array Pole kde jsou ulozeny testy
     */
    public function get_tests()
    {
        return $this->tests;
    }
}

?>