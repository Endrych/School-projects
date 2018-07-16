<?php

class IPPXMLWriter
{
    private $xw; // Instance XMLWriteru

    /**
     * IPPXMLWriter constructor.
     */
    function __construct()
    {
        $this->xw = new XMLWriter();
        $this->prepare_writer();
    }

    /**
     * Zapis jedne instrukce do vystupniho XML
     * @param $instruction Vstupni instrukce ktera ma byt do XML zapsana
     */
    public function write_instruction($instruction)
    {
        $this->xw->startElement('instruction');
        $this->write_order($instruction);
        $this->write_opcode($instruction);
        $this->write_attributes($instruction);
        $this->xw->endElement();
    }

    /**
     * Metoda postupne zapisuje attributy do vystupniho XML
     * @param $instruction Vstupni instrukce ze ktere maji byt do XML zapsany jeji attributy
     */
    private function write_attributes($instruction)
    {
        while (($attribute = $instruction->get_element_attribute()) != null) {
            $this->write_attribute($attribute);
        }
    }

    /**
     * Zapis jednoho attributu do XML
     * @param $attribute Attribut, ktery ma byt zapsan
     */
    private function write_attribute($attribute)
    {
        $this->xw->startElement('arg' . $attribute->get_argument_order());
        $this->write_type($attribute);
        $this->xw->text($attribute->get_value());
        $this->xw->endElement();
    }

    /**
     * Metoda uzavira XML document a zapise ho do souboru
     * @param $filename Nazev souboru do ktereho se zapise XML
     */
    public function close_writer($filename)
    {
        $this->xw->endElement();
        $this->xw->endDocument();
        $output = fopen($filename, "w");
        if (!$output) {
            fprintf(STDERR, "Cannot open output file\n");
            exit(12);
        }
        fprintf($output, "%s", $this->xw->outputMemory());
    }

    /**
     * Zapise do XML poradi instruckce
     * @param $instruction Reprezentace instrukce, ktera se aktualne zapisuje do XML
     */
    private function write_order($instruction)
    {
        $this->xw->startAttribute('order');
        $this->xw->text($instruction->get_order());
        $this->xw->endAttribute();
    }

    /**
     * Zapise do XML OPcode instruckce
     * @param $instruction Reprezentace instrukce, ktera se aktualne zapisuje do XML
     */
    private function write_opcode($instruction)
    {
        $this->xw->startAttribute('opcode');
        $this->xw->text($instruction->get_op_code());
        $this->xw->endAttribute();
    }


    /**
     * Metoda zapise do XML typ attributu instrukce
     * @param $attribute Reprezentace Attributu, ktery se aktualne zapisuje do XML
     */
    private function write_type($attribute)
    {
        $this->xw->startAttribute('type');
        $this->xw->text($attribute->get_type());
        $this->xw->endAttribute();
    }

    /**
     * Metoda pripravy XML dokument, nastavi kodovani a root element dokumentu
     */
    private function prepare_writer()
    {
        $this->xw->openMemory();
        $this->xw->setIndent(true);
        $this->xw->setIndentString(' ');
        $this->xw->startDocument('1.0', "UTF-8");
        $this->xw->startElement('program');
        $this->xw->startAttribute('language');
        $this->xw->text('IPPcode18');
        $this->xw->endAttribute();
    }
}

?>