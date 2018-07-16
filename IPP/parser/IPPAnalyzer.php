<?php
include_once('XMLInstruction.php');

class IPPAnalyzer
{
    private $fs; // Vstupni filestream
    private $lines_of_code; // Pocet radku instukci
    private $lines_with_commentary; // Pocet radku s komentarema

    /**
     * Lexical constructor.
     * @param $file_name Nazev vstupniho souboru
     */
    function __construct($file_name)
    {
        $this->fs = fopen($file_name, 'r');
        if (!$this->fs) {
            fprintf(STDERR, "Cannot open inputstream\n");
            exit(11);
        }
        $this->lines_of_code = 0;
        $this->lines_with_commentary = 0;
    }

    /**
     * Kontrola hlavicky kodu ze vstupu
     * @return bool True pokud vstup obsahuje spravnou hlavicku, jinak false
     */
    function check_header()
    {
        do {
            if ($this->check_eof()) {
                return false;
            }
            $line = fgets($this->fs);
            $words = $this->split_to_words_without_commentens($line);
        } while (count($words) == 0);

        if (strtolower($words[0]) == ".ippcode18") {
            return true;
        }
        return false;
    }

    /**
     * Nacte, zkontroluje a reprezentuje jeden radek vstupu jako instrukci
     * Pokud je radek prazdny, je nacteny dalsi radek
     * @return null Pokud je radek prazdy a vstup dosahl EOFU
     * @return XMLInstruction Reprezentuje jeden radek jako instrukci
     */
    function read_line()
    {
        $line = trim(fgets($this->fs)); // nacte radek
        $words = $this->split_to_words_without_commentens($line); // Rozdeli na jednotliva slova a zbaveni komentaru
        if (count($words) == 0) { // Detekce prazdneho radku
            if($this->check_eof()){
                return null;
            }
            return $this->read_line();
        }
        $instruction = $this->detect_instruction($words); // Detekce instrukce
        if($instruction == null){
            exit(21);
        }
        $this->lines_of_code++;
        return $instruction;
    }

    /**
     * Kontrola jestli vstup nedosahl EOFU
     * @return bool True Pokud vstup dosahl EOF jinak false
     */
    public function check_eof()
    {
        return feof($this->fs);
    }

    /***
     * Metoda odstrani komentare z retezce a pote retezec reprezentuje jako pole slov
     * @param $line Retezec ke zpracovani
     * @return array[]|false|string[] Vraci pole slov
     */
    private function split_to_words_without_commentens($line)
    {
        // Detekce komentare
        if (preg_match('/#/', $line)) {
            $this->lines_with_commentary++;
        }
        // Odstraneni komentace
        $line = preg_replace('/#(.*)/', '', $line);
        // Rozdeleni do pole podle whitespacu
        $words = preg_split('/\s+/', $line, -1, PREG_SPLIT_NO_EMPTY);
        return $words;
    }

    /**
     * Detekce instrukce, ktera je na radku
     * @param $words Pole slov, ktere jsou na radku
     * @return null|XMLInstruction Reprezentace instukce ktera se nachzi na radku, null pokud je instrukce chybna
     */
    private function detect_instruction($words)
    {
        $ins = new XMLInstruction();
        $inst_word = strtolower($words[0]);
        $result = false;

        // Detecke instrukce podle poctu slov na radku
        switch (count($words)) {
            case 1:
                $result = $this->check_instruction_without_arguments($inst_word);
                break;
            case 2:
                $result = $this->check_instruction_with_one_argument($words, $inst_word, $ins);
                break;
            case 3:
                $result = $this->check_instruction_with_two_aguments($words, $inst_word, $ins);
                break;
            case 4:
                $result = $this->check_instruction_with_three_arguments($words, $inst_word, $ins);
                break;
        }
        if ($result == false) {
            return null;
        }
        $ins->set_op_code(strtoupper($inst_word));
        return $ins;
    }

    /**
     * Detekce instrukci ktere nemaji zadny argument
     * @param $inst_word Retezec reprezentujici nazev instrukce
     * @return bool true pokud je instrukce spravne, jinak false
     */
    private function check_instruction_without_arguments($inst_word)
    {
        if (!($inst_word == "createframe" || $inst_word == "pushframe" || $inst_word == "popframe"
            || $inst_word == "return" || $inst_word == "break")) {
            return false;
        }
        return true;
    }

    /**
     * Detekce instrukci ktere maji jeden argument
     * @param $words Pole slov reprezentujici radek vstupu
     * @param $inst_word Retezez reprezentujici nazev instrukce
     * @param $ins Reprezentace aktualni instrukce
     * @return bool true pokud je instrukce spravne, jinak false
     */
    private function check_instruction_with_one_argument($words, $inst_word, $ins)
    {
        if ($inst_word == "defvar" || $inst_word == "pops") {
            if ($this->detect_attribute($words[1], "variable")) {
                $ins->add_element_attribute("var", $words[1]);
                return true;
            } else {
                return false;
            }

        } else if ($inst_word == "call" || $inst_word == "label" || $inst_word == "jump") {
            if ($this->detect_attribute($words[1], "label")) {
                $ins->add_element_attribute("label", $words[1]);
                return true;
            } else {
                fprintf(STDERR, "SYNTAX ERROR: Expected label");
                return false;
            }
        } else if ($inst_word == "pushs" || $inst_word == "write" || $inst_word == "dprint") {
            $this->detect_symbol($ins, $words[1]);
            return true;
        } else {
            fprintf(STDERR, "SYNTAX ERROR: Wrong instructor or number of argument");
            return false;
        }
    }

    /**
     * Detekce instrukci ktere maji dva argumenty
     * @param $words Pole slov reprezentujici radek vstupu
     * @param $inst_word Retezez reprezentujici nazev instrukce
     * @param $ins Reprezentace aktualni instrukce
     * @return bool true pokud je instrukce spravne, jinak false
     */
    private function check_instruction_with_two_aguments($words, $inst_word, $ins)
    {
        if ($inst_word == "move" || $inst_word == "int2char" || $inst_word == "strlen" || $inst_word == "type" || $inst_word == "not") {
            if ($this->detect_attribute($words[1], "variable")) {
                $ins->add_element_attribute("var", $words[1]);
                $this->detect_symbol($ins, $words[2]);
                return true;
            } else {
                fprintf(STDERR, "SYNTAX ERROR: Expected variable");
                return false;
            }
        } else if ($inst_word = "read") {
            if ($this->detect_attribute($words[1], "variable")) {
                $ins->add_element_attribute("var", $words[1]);
                if ($words[2] == "int" || $words[2] == "string" || $words[2] == "bool") {
                    $ins->add_element_attribute("type", $words[2]);
                    return true;
                } else {
                    fprintf(STDERR, "SYNTAX ERROR: Expected type");
                    return false;
                }
            } else {
                fprintf(STDERR, "SYNTAX ERROR: Expected variable");
                return false;
            }
        } else {
            return false;
        }
    }

    /**
     * Detekce instrukci ktere maji tri argumenty
     * @param $words Pole slov reprezentujici radek vstupu
     * @param $inst_word Retezez reprezentujici nazev instrukce
     * @param $ins Reprezentace aktualni instrukce
     * @return bool true pokud je instrukce spravne, jinak false
     */
    private function check_instruction_with_three_arguments($words, $inst_word, $ins)
    {
        if ($inst_word == "add" || $inst_word == "sub" || $inst_word == "mul" || $inst_word == "idiv"
            || $inst_word == "lt" || $inst_word == "gt" || $inst_word == "eq" || $inst_word == "and"
            || $inst_word == "or" || $inst_word == "stri2int" || $inst_word == "concat"
            || $inst_word == "getchar" || $inst_word == "setchar") {
            if ($this->detect_attribute($words[1], "variable")) {
                $ins->add_element_attribute("var", $words[1]);
                $this->detect_symbol($ins, $words[2]);
                $this->detect_symbol($ins, $words[3]);
                return true;
            } else {
                fprintf(STDERR, "SYNTAX ERROR: Expected variable");
                return false;
            }
        } else if ($inst_word == "jumpifeq" || $inst_word == "jumpifneq") {
            if ($this->detect_attribute($words[1], "label")) {
                $ins->add_element_attribute("label", $words[1]);
                $this->detect_symbol($ins, $words[2]);
                $this->detect_symbol($ins, $words[3]);
                return true;
            } else {
                fprintf(STDERR, "SYNTAX ERROR: Expected label");
                return false;
            }
        } else {
            return false;
        }
    }

    /**
     * Metoda detekuje jestli retezec reprezentuje symbol a popripadne jej ulozi do instukce
     * @param $instruction Instrukce do ktere se ma symbol vlozit
     * @param $word Zkoumany retezec
     */
    private function detect_symbol($instruction, $word)
    {
        if ($this->detect_attribute($word, "bool")) {
            if (substr($word, 0, 5) == "bool@") {
                $word = substr($word, 5);
            }
            $instruction->add_element_attribute("bool", $word);
        } else if ($this->detect_attribute($word, "int")) {
            if (substr($word, 0, 4) == "int@") {
                $word = substr($word, 4);
            }
            $instruction->add_element_attribute("int", $word);
        } else if ($this->detect_attribute($word, "string")) {
            if (substr($word, 0, 7) == "string@") {
                $word = substr($word, 7);
            }
            $instruction->add_element_attribute("string", $word);
        } else if ($this->detect_attribute($word, "variable")) {
            $instruction->add_element_attribute("var", $word);
        } else {
            fprintf(STDERR, "SYNTAX ERROR: Expected symbol");
            exit(21);
        }
    }

    /**
     * Detecke attributu instrukce
     * @param $word Zkoumany retezec
     * @param $type Zkoumany typ
     * @return bool True pokud retezec souhlasi s typem jinak false
     */
    private function detect_attribute($word, $type)
    {
        if ($type == "label") {
            if (preg_match("/^([a-z]|[A-Z]|[-]|[_]|[&]|[%]|[*]|[$])([a-z]|[A-Z]|[0-9]|[-]|[_]|[&]|[%]|[*]|[$])*$/", $word)) {
                return true;
            }
            return false;
        } else if ($type == "variable") {
            if (preg_match("/^(GF|LF|TF)([@])([a-z]|[A-Z]|[-]|[_]|[&]|[%]|[*]|[$])([a-z]|[A-Z]|[0-9]|[-]|[_]|[&]|[%]|[*]|[$])*$/", $word)) {
                return true;
            }
            return false;
        } else if ($type == "bool") {
            if (preg_match("/^(bool)([@])(true|false)$/", $word)) {
                return true;
            }
            return false;
        } else if ($type == "int") {
            if (preg_match("/^int[@]/", $word)) {
                return true;
            }
            return false;
        } else if ($type == "string") {
            if (preg_match("/^string[@]/", $word)) {
                $state = 0;
                $count = 0;
                // Kontrola escape sekvenci
                for ($i = 0, $len = strlen($word); $i < $len; $i++) {
                    switch ($state) {
                        case 0:
                            if ($word[$i] == '\\') {
                                $state = 1;
                            }
                            break;
                        case 1:
                            if ($word[$i] >= '0' && $word[$i] <= '9') {
                                $count++;
                                if ($count == 3) {
                                    $count = 0;
                                    $state = 0;
                                }
                            } else {
                                fprintf(STDERR, "Cannot be char \\ without 3 numbers\n");
                                exit(21);
                            }
                            break;
                    }
                }
                return true;
            }
            return false;
        }
        return false;
    }

    /***
     * Metoda vraci pocet komentaru
     * @return int Pocet komentaru
     */
    public function get_number_of_commentary()
    {
        return $this->lines_with_commentary;
    }

    /***
     * Metoda vraci pocet radku s instrukcemi
     * @return int Pocet radku s instrukcemi
     */
    public function get_lines_of_code()
    {
        return $this->lines_of_code;
    }
}
?>