<?php
include_once('TestResult.php');

class TestRepresentation
{
    private $name;
    private $result; // Instance TestResultu

    /**
     * TestRepresentation constructor.
     */
    public function __construct()
    {
        $this->result = new TestResult();
    }

    /**
     * Funkce provede test
     * @param $parse_script Parser
     * @param $interpret_script Interpret
     */
    public function do_test($parse_script, $interpret_script)
    {
        $this->prepare_test();

        // Spusteni parseru
        exec("cat " . $this->name . ".src | php5.6 " . $parse_script . "  2>/dev/null   > " . $this->name . ".tmp", $res, $rc);

        // Kontrola Navratoveho kodu
        if ($rc == 0) {
            // Nacteni vystupu parseru
            $parse_out_file = fopen($this->name . ".tmp", "r");
            if (!$parse_out_file) {
                fprintf(STDERR, "Cannot open input file\n");
                exit(11);
            }
            $parse_out_size = filesize($this->name . ".tmp");
            if ($parse_out_size == 0) {
                $parse_out = "";
            } else {
                $parse_out = fread($parse_out_file, $parse_out_size);
            }

            // Nahrazeni specialnich znaku
            $parse_out = str_replace(PHP_EOL, "\\n", $parse_out);
            $parse_out = str_replace("\"", "\\\"", $parse_out);
            $this->result->set_parse_output($parse_out);

            // Spusteni interpretu
            system(" cat " . $this->name . ".in | python3.6 " . $interpret_script . " --source=" . $this->name . ".tmp  2>/dev/null  > " . $this->name . ".fi", $interpret_rc);
            $this->result->set_interpret_return_code($interpret_rc);

            //Nacteni vystupu interpretu
            $out_file = fopen($this->name . ".fi", "r");
            if (!$out_file) {
                fprintf(STDERR, "Cannot open input file\n");
                exit(11);
            }
            $out_size = filesize($this->name . ".fi");
            if ($out_size == 0) {
                $out = "";
            } else {
                $out = fread($out_file, $out_size);
            }

            // Nahrazeni specialnich znaku
            $out = str_replace(PHP_EOL, "\\n", $out);
            $this->result->set_interpret_output($out);

            // Zjisteni rozdilu vystupu
            $command = "diff " . $this->name . ".fi " . $this->name . ".out -Z";
            exec($command, $res, $diff_rc);
            if ($diff_rc == 0) {
                $this->result->set_test_correct(true);
            } else {
                $this->result->set_test_correct(false);
            }
        } else {
            if ($this->result->get_reference_return_code() == $rc) {
                $this->result->set_test_correct(true);
            } else {
                $this->result->set_test_correct(false);
            }
        }
        $this->result->set_parse_return_code($rc);
    }

    /**
     * Nastaveni nazvu
     * @param $value Nazev
     */
    public function set_name($value)
    {
        $this->name = $value;
    }

    /**
     * Ziskani nazvu
     * @return mixed Nazev
     */
    public function get_name()
    {
        return $this->name;
    }

    /**
     * Nastaveni vysledku
     * @param $result Vysledek
     */
    public function set_test_result($result)
    {
        $this->result = $result;
    }

    /**
     * Ziskani vysledku
     * @return mixed Vysledek
     */
    public function get_test_result()
    {
        return $this->result;
    }

    /**
     * Metoda pripravy test, potrebne soubory apod.
     */
    private function prepare_test()
    {
        // Nacteni navratoveho kodu
        $rc_file = fopen($this->name . ".rc", "r");
        if (!$rc_file) {
            fprintf(STDERR, "Cannot open input file\n");
            exit(11);
        }
        $rc_size = filesize($this->name . ".rc");
        if ($rc_size == 0) {
            $rc_ref = 0;
        } else {
            $rc_ref = intval(fread($rc_file, $rc_size));
        }
        $this->result->set_reference_return_code($rc_ref);

        // Nacteni vystupu
        $out_file = fopen($this->name . ".out", "r");
        if (!$out_file) {
            fprintf(STDERR, "Cannot open input file\n");
            exit(11);
        }
        $out_size = filesize($this->name . ".out");
        if ($out_size == 0) {
            $out_ref = "";
        } else {
            $out_ref = fread($out_file, $out_size);
        }
        $out_ref = str_replace(PHP_EOL, "\\n", $out_ref);
        $out_ref = str_replace("\r", "", $out_ref);
        $out_ref = str_replace("\"", "\\\"", $out_ref);
        $this->result->set_reference_output($out_ref);

        // Nacteni vstupu
        $in_file = fopen($this->name . ".in", "r");
        if (!$in_file) {
            fprintf(STDERR, "Cannot open input file\n");
            exit(11);
        }
        $in_size = filesize($this->name . ".in");
        if ($in_size == 0) {
            $in = "";
        } else {
            $in = fread($in_file, $in_size);
        }
        $this->result->set_interpret_input($in);

        // Nacteni zdrojoveho programu
        $src_file = fopen($this->name . ".src", "r");
        if (!$src_file) {
            fprintf(STDERR, "Cannot open input file\n");
            exit(11);
        }
        $src_size = filesize($this->name . ".src");
        if ($src_size == 0) {
            $src = "";
        } else {
            $src = fread($src_file, $src_size);
        }
        $src = str_replace(PHP_EOL, "\\n", $src);
        $src = str_replace("\r", "", $src);
        $src = str_replace("\"", "\\\"", $src);
        $this->result->set_input($src);
    }
}
?>