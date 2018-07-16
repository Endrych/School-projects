<?php
class HTMLGenerator
{
    private $tests;
    private $correct_test = 0;
    private $all_test = 0;

    /**
     * HTMLGenerator constructor.
     * @param $tests Seznam testu
     */
    public function __construct($tests)
    {
        $this->tests = $tests;
    }

    /**
     * Generovani HTML stranky
     * @return string Vraci HTML stranku
     */
    public function generate_html()
    {
        $html = "";
        $html .= $this->generate_html_header();
        foreach ($this->tests as $test) {
            $html .= $this->generate_test_output($test);
        }
        $html .= $this->generate_html_footer();
        return $html;
    }

    /**
     * Vygenerovani Hlavicky HTML souboru
     * @return string HTML hlavicka
     */
    private function generate_html_header()
    {
        $header = "<!DOCTYPE html> <html lang=\"en\"> <head> <meta charset=\"UTF-8\"> <title>Test statistics</title> <style> table{ border: 1px solid black; margin: 10px 20px; } thead{ background-color:lightgray; } .status{ width: 180px; } #stats{font-size:23px;} .test-name{ min-width: 400px; padding-left: 20px; color:blue; text-decoration: underline; } h1,p{ margin-left: 20px; } .success{ background: lawngreen; } .failed{ background: red; } #detail{ margin-left: 20px; } #input{ min-height: 250px; width: 300px; } #parse_output{ min-height: 400px; width: 500px; } #detail{ display: none; } .status_text_pass{ color:green; font-size: 25px; } .status_text_failed{ color:red; font-size:25px; } </style> </head> <body> <h1>Test statistics</h1><p id=\"stats\">Success: <span id=\"correct\"></span>/<span id=\"all\"></span></p> <p>For test detail click on test name</p> <table> <thead> <tr> <th>Test Name</th> <th>Pass/Fail</th> </tr> </thead> <tbody> </tbody> </table> <div id=\"detail\"> <h2><span id=\"test_name\"></span> detail</h2> <h3 id=\"status_text\"></h3><h3>Input:</h3> <textarea disabled id=\"input\"></textarea> <h3>Parse output:</h3> <textarea disabled id=\"parse_output\"></textarea> <h3>Interpret input</h3><span id=\"interpret_input\"></span> <h3>Reference output</h3><span id=\"ref_output\"></span> <h3>Interpret output</h3><span id=\"interpret_output\"></span> <h3>Reference return code:</h3> <span id=\"ref_return_code\"></span> <h3>Parse return code:</h3> <span id=\"parse_return_code\"></span> <h3>Interpret return code:</h3> <span id=\"interpret_return_code\"></span>  </div> </body> <script> var tests = [";
        return $header;
    }

    /**
     * Generovani struktury pro JS ktera reprezentuje jeden test
     * @param $test Reprezentace testu
     * @return string Vygenerovana cast HTML souboru
     */
    private function generate_test_output($test)
    {
        $html = "{name:\"";
        $html .= $test->get_name();
        $html .= "\",success:";
        $res = $test->get_test_result();
        if ($res->get_test_correct() == true) {
            $html .= "true,input:\"";
            $this->correct_test += 1;
        } else {
            $html .= "false,input:\"";
        }
        $this->all_test += 1;
        $html .= $res->get_input() . "\",";
        $html .= "parse_output:\"";
        $html .= $res->get_parse_output() . "\",";
        $html .= "interpret_input:\"" . $res->get_interpret_input() . "\",";
        $html .= "ref_output:\"" . $res->get_reference_output() . "\",";
        $html .= "interpret_output:\"" . $res->get_interpret_output() . "\",";
        $html .= "ref_return_code:\"" . $res->get_reference_return_code() . "\",";
        $html .= "parse_return_code:\"" . $res->get_parse_return_code() . "\",";
        $html .= "interpret_return_code:\"" . $res->get_interpret_return_code() . "\"},";
        return $html;
    }

    /**
     * Generovani konce HTML stranky
     * @return string Vygenerovana cast v HTML
     */
    private function generate_html_footer()
    {
        $html = "]; function show_details(test) { var details = document.getElementById(\"detail\"); details.style.display = \"block\"; document.getElementById(\"test_name\").innerText = test.name; document.getElementById(\"input\").innerText = test.input; document.getElementById(\"parse_output\").innerText = test.parse_output; document.getElementById(\"interpret_input\").innerText = test.interpret_input; document.getElementById(\"ref_output\").innerText = test.ref_output; document.getElementById(\"interpret_output\").innerText = test.interpret_output; document.getElementById(\"ref_return_code\").innerText = test.ref_return_code; document.getElementById(\"parse_return_code\").innerText = test.parse_return_code; document.getElementById(\"interpret_return_code\").innerText = test.interpret_return_code; var status_text = document.getElementById(\"status_text\"); if(test.success == true){ status_text.innerText = \"PASSED\"; status_text.className = \"status_text_pass\"; } else { status_text.innerText = \"FAILED\"; status_text.className = \"status_text_failed\"; } } function create_row(test,row) { var table = document.getElementsByTagName('tbody')[0]; var row = table.insertRow(row); var cell = row.insertCell(0); cell.textContent = test.name; cell.addEventListener(\"click\",function(){show_details(test)}); cell.className = \"test-name\"; cell = row.insertCell(1); cell.className = \"status \"; if(test.success == true){ cell.className += \"success\"; } else { cell.className += \"failed\"; } } for(var i = 0; i < tests.length;i++){ create_row(tests[i],i) } document.getElementById(\"correct\").innerText = " . $this->correct_test . ";document.getElementById(\"all\").innerText = " . $this->all_test . "; </script> </html>";
        return $html;
    }
}
?>