<?php

class XMLAttribute
{
    private $type; // Typ attributu
    private $argument_order; // Poradi attributu
    private $value; // Hodnota attributu


    /**
     * XMLAttribute constructor.
     * @param $argument_order Poradi argumentu
     */
    function __construct($argument_order)
    {
        $this->argument_order = $argument_order;
    }

    /**
     * Ziskava hodnotu attributu
     * @return mixed Hodnota attributu
     */
    function get_value()
    {
        return $this->value;
    }

    /**
     * Nastaveni hodnoty attributu
     * @param $value Hodnota attributu
     */
    function set_value($value)
    {
        $this->value = $value;
    }

    /**
     * Ziskava poradi argumentu
     * @return mixed Poradi argumentu
     */
    function get_argument_order()
    {
        return $this->argument_order;
    }

    /**
     * Ziskava typ attributu
     * @return mixed Typ attributu
     */
    function get_type()
    {
        return $this->type;
    }

    /**
     * Nastaveni typu attributu
     * @param $type Typ attributu
     */
    function set_type($type)
    {
        $this->type = $type;
    }
}

?>