<?php

class XMLInstruction
{
    private $order; // Poradi instukce
    private $op_code; // Kod instrukce
    private $element_attributes = array(); // Pole attributu insturkce
    private static $current_order = 1; // Staticka promena ktera urcuje poradi instrukce

    /**
     * XMLInstruction constructor.
     * Priradi poradi instrukce ze staticke promene, ktera se inkrementuje pri volani konstruktoru
     */
    function __construct()
    {
        $this->order = self::$current_order;
        self::$current_order++;
    }

    /***
     * Ziskani prvniho attributu z instrukce, metoda ziskany attribut maze z pole attributu
     * @return mixed|null Vraci prvni attribut v poli attributu a nebo vraci null pokud instrukce zadny attribut nema
     */
    function get_element_attribute()
    {
        if (count($this->element_attributes) != 0) {
            $element = array_shift($this->element_attributes);
            return $element;
        } else {
            return null;
        }
    }

    /**
     * Pridani noveho attributu do pole
     * @param $type Typ attributu
     * @param $value Hodnota attributu
     */
    function add_element_attribute($type, $value)
    {
        $attribute = new XMLAttribute(count($this->element_attributes) + 1);
        $attribute->set_type($type);
        $attribute->set_value($value);
        array_push($this->element_attributes, $attribute);
    }

    /**
     * Ziskava opcode instrukce
     * @return mixed op_code promena instrukce
     */
    function get_op_code()
    {
        return $this->op_code;
    }

    /**
     * Nastaveni opcode instukce
     * @param $op_code hodnota opcodu instrukce
     */
    function set_op_code($op_code)
    {
        $this->op_code = strtoupper($op_code);
    }

    /**
     * Ziskava poradi instrukce
     * @return int poradi instrukce
     */
    function get_order()
    {
        return $this->order;
    }
}

?>