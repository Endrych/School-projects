<?php

class TestResult
{
    private $input;
    private $reference_output;
    private $interpret_output;
    private $reference_return_code;
    private $parse_output;
    private $interpret_return_code;
    private $parse_return_code;
    private $test_correct;
    private $interpret_input;

    /**
     * Nastavi navratovou hodnotu parseru
     * @param $value Navratova hodnota z parseru
     */
    public function set_parse_return_code($value)
    {
        $this->parse_return_code = $value;
    }

    /**
     * Vrati navratovou hodnotu parseru
     * @return mixed Navratova hodnota z parseru
     */
    public function get_parse_return_code()
    {
        return $this->parse_return_code;
    }

    /**
     * Nastavi vystup parseru
     * @param $value Vystup parseru
     */
    public function set_parse_output($value)
    {
        $this->parse_output = $value;
    }

    /**
     * Ziska vystup parseru
     * @return mixed Vystup parseru
     */
    public function get_parse_output()
    {
        return $this->parse_output;
    }

    /**
     * Zjisti jestli test prosel
     * @return mixed Spravnost testu
     */
    public function get_test_correct()
    {
        return $this->test_correct;
    }

    /**
     * Nastavi vysledek testu
     * @param $value Vysledek testu
     */
    public function set_test_correct($value)
    {
        $this->test_correct = $value;
    }

    /**
     * Ziska referenci vystup
     * @return mixed Referenci vystup
     */
    public function get_reference_output()
    {
        return $this->reference_output;
    }

    /**
     * Ziska vstup
     * @return mixed Vstup
     */
    public function get_input()
    {
        return $this->input;
    }

    /**
     * Nastavi vystup
     * @param array $input Vystup
     */
    public function set_input($input)
    {
        $this->input = $input;
    }

    /**
     * Nastavi referencni vystup
     * @param mixed referencni vystup
     */
    public function set_reference_output($reference_output)
    {
        $this->reference_output = $reference_output;
    }

    /**
     * Ziska vystup interpretu
     * @return mixed Vystup interpretu
     */
    public function get_interpret_output()
    {
        return $this->interpret_output;
    }

    /**
     * Nastavi vystup interpretu
     * @param mixed $interpret_output Vystup interpretu
     */
    public function set_interpret_output($interpret_output)
    {
        $this->interpret_output = $interpret_output;
    }

    /**
     * Ziska referenci navratovy kod
     * @return mixed referenci navratovy kod
     */
    public function get_reference_return_code()
    {
        return $this->reference_return_code;
    }

    /**
     * Nastavi referenci navratovy kod
     * @param mixed referenci navratovy kod
     */
    public function set_reference_return_code($reference_return_code)
    {
        $this->reference_return_code = $reference_return_code;
    }

    /**
     * Ziska navratovy kod interpretu
     * @return mixed Navratovy kod interpretu
     */
    public function get_interpret_return_code()
    {
        return $this->interpret_return_code;
    }

    /**
     * Ziska vstup interpretu
     * @return mixed Vstup interpretu
     */
    public function get_interpret_input()
    {
        return $this->interpret_input;
    }

    /**
     * Nastavi vstup interpretu
     * @param $value Vstup interpretu
     */
    public function set_interpret_input($value)
    {
        $this->interpret_input = $value;
    }

    /**
     * Nastavi navratovy kod interpretu
     * @param mixed $interpret_return_code navratovy kod interpretu
     */
    public function set_interpret_return_code($interpret_return_code)
    {
        $this->interpret_return_code = $interpret_return_code;
    }
}

?>

