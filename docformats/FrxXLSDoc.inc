<?php
class FrxXLSDoc extends FrxDocument { 
  
  
  public function render($r, $format, $options = array()) { 
    $body = $r->html; 
    $output = '<?xml version="1.0" encoding="UTF-8"?>' . "\n";
    $output .= '<?mso-application progid="Excel.Sheet"?>' . "\n";
    $output .= '<Workbook xmlns="urn:schemas-microsoft-com:office:spreadsheet"' . "\n";
    $output .= '  xmlns:o="urn:schemas-microsoft-com:office:office"' . "\n";
    $output .= '  xmlns:x="urn:schemas-microsoft-com:office:excel"' . "\n";
    $output .= '  xmlns:ss="urn:schemas-microsoft-com:office:spreadsheet"' . "\n";
    $output .= '  xmlns:html="http://www.w3.org/TR/REC-html40">' . "\n";
    $output .= '<Styles>' . "\n";
    $output .= '  <Style ss:ID="Default" ss:Name="Normal">' . "\n";
    $output .= '    <Alignment ss:Vertical="Bottom"/>' . "\n";
    $output .= '    <Borders/>' . "\n";
    $output .= '    <Font/>' . "\n";
    $output .= '    <Interior/>' . "\n";
    $output .= '    <NumberFormat/>' . "\n";
    $output .= '    <Protection/>' . "\n";
    $output .= '  </Style>' . "\n";
    $output .= '</Styles>' . "\n";

    $doc = new DOMDocument();
    $doc->strictErrorChecking = FALSE;
    $doc->loadHTML($body);
    $xml = simplexml_import_dom($doc);

    $tables = $xml->xpath('//table');
    $count = 1;
    if ($tables) foreach ($tables as $table) {
      $output .= '<Worksheet ss:Name="sheet' . ' ' . $count . '">' . "\n";
      $count ++;
      $output .= '  <Table>' . "\n";

      $rows = $table->xpath('descendant::tr');
      if ($rows) foreach ($rows as $row) {
        $output .= '    <Row>' . "\n";
        foreach ($row as $column) {
          $value = $column->asXML();
          $value = strip_tags($value);
          $tval = trim($value);
          // Find if it contains invalid number characters
          $non_numeric_chars = trim($value, ' +-.,0123456789');
          // Determine if it contains +- in the interior
          // Zero is ok here bu
          $inner_symbols = FALSE;
          if (strpos($tval, '+') || strpos($tval, '-') || strpos($tval, ' ')) $inner_symbols =  TRUE;
          if (substr_count($tval, '.') > 1) $inner_symbols = TRUE;
          if (empty($non_numeric_chars) && trim($value)!=='' && !$inner_symbols) {
            $output .= '      <Cell><Data ss:Type="Number">' . $tval . '</Data></Cell>' . "\n";
          }
          else {
            $output .= '      <Cell><Data ss:Type="String">' . $value . '</Data></Cell>' . "\n";
          }
        }
        $output .= '    </Row>' . "\n";
      }
      $output .= '  </Table>' . "\n";
      $output .= '  <WorksheetOptions xmlns="urn:schemas-microsoft-com:office:excel">' . "\n";
      $output .= '    <Selected/>' . "\n";
      $output .= '    <ProtectObjects>False</ProtectObjects>' . "\n";
      $output .= '    <ProtectScenarios>False</ProtectScenarios>' . "\n";
      $output .= '  </WorksheetOptions>' . "\n";
      $output .= '</Worksheet>' . "\n";
    }
    $output .= '</Workbook>';
    header('Content-Type: application/msexcel');
    header('Cache-Control:');
    header('Pragma:');
    header("Cache-Control: must-revalidate");
    header("Content-Disposition: attachment; filename=report.xls");
    print $output; 
  }
}