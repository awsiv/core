<?php
require('fpdf.php');
#define('FPDF_FONTPATH','fonts/');
class PDF extends FPDF
{
    var $left=5;
    var $right=5;
    var $top=10;
    var $pagewidth=210;
    var $reportname;
    var $tabletitle;
    var $description;
    
    #****************************** 
    function PDF($orientation='p', $unit='mm', $format='A4')
    { 
    	$this->FPDF($orientation,$unit,$format);
	
	if($format=='A4' && $unit =='mm')
	{
	    $this->PDFSetMargins(5,10,5);
	    $this->pagewidth=210;
	}
    }
    #****************************** 
    function PDFSetMargins($left,$top,$right)
    {
	$this->left=$left;
	$this->right=$right;
	$this->top=$top;
	$this->SetMargins($this->left,$this->top,$this->right);
    }
    #****************************** 
    function PDFGetLeftMargin()
    {
	return $this->left;
    }
    #****************************** 
    function PDFGetRightMargin()
    {
	return $this->right;
    }
    #******************************
    function PDFSetReportName($name)
    {
	$this->reportname = $name;
    }

    #******************************
    function PDFGetPageWidth()
    {
	return $this->pagewidth;
    }
    #******************************
    function PDFSetTableTitle($name)
    {
	$this->tabletitle = $name;
    }
    #******************************
    function PDFSetDescription($name)
    {
	$this->description = $name;
    }
    #******************************
    #Page header
    function Header()
    {
	$this->SetFont('Arial','',8);
        #$this->Text(30,10,$timestamp);
	$logo_path = './logo_outside_new.jpg';
	$this->Image($logo_path,10,8,33);
	$this->Ln(20);
    }
    
    #******************************
    # Page footer
    
    function Footer()
    {
	# Position at 1.5 cm from bottom
	$this->SetY(-15);
	$this->SetFont('Arial','I',8);
	$this->Cell(0,10,'Page '.$this->PageNo().'/{nb}',0,0,'C');
	
	$this->SetY(-10);
	$this->SetFont('Arial','',6);
	$this->Cell(0,10,'Run: '.date("m.d.y H:i:s",time()).' - Powered by Cfengine - '.$this->reportname,0,0,'C');
    }
    
    #******************************
    # Title
    function ReportTitle()
    {
	$this->SetFont('Arial','',18);
	$this->SetTextColor(255,255,255);
	$this->SetFillColor(158,152,120);
	$width=$this->pagewidth-$this->left-$this->right;
	$this->Rect($this->left, 28, $width, 10, "F" );
	$this->Cell($width,6,$this->reportname,0,1,'L',true);
	$this->Ln(4);
	$this->SetTextColor(76,76,76);
    }
    
    #******************************
    # Description
    function ReportDescription()
    {
	$this->SetFont('Arial','',10);
	$width=$this->pagewidth-$this->left-$this->right;
	$this->MultiCell($width,5,$this->description);
	$this->Ln(4);
    }
    
    #******************************
    # Table Title
    function RptTableTitle($text, $y)
    {
	$this->SetFont('Arial','',14);
	$this->SetDrawColor(181,177,153);
	$this->SetFillColor(239,240,234);
	$width=$this->pagewidth-$this->left-$this->right;
	$this->Rect($this->left, $y, $width, 10, "DF" );
	$this->SetY($y+1);
	$this->SetX($this->left+1);
	$this->Cell($width-2, 8,"$text",0,0,'L',true);
	$this->Ln(4);
    }
    
    #******************************
    # Check for page break
    
    function CheckPageBreak($h) 
    { 
	#If the height h would cause an overflow, add a new page immediately 
	if($this->GetY()+$h>$this->PageBreakTrigger) 
	{ 
	    $this->AddPage($this->CurOrientation); 
	} 
    }
    
    #****************************** 
    # Parse data
    function ParseData($arr)
    {
	$lines=explode('<nova_nl>',$arr);
	$data=array();
	foreach($lines as $line)
	{
	    $line = trim($line);
	    if ($line!='')
	    {
	     $data[]=explode('<nc>',chop($line));
	    }
	}
	return $data;
    }
    
    #******************************
    # Compute the number of lines
    
    function NbLines($w,$txt) 
    { 
	# Computes the number of lines a MultiCell of width w will take 
	$cw=&$this->CurrentFont['cw']; 
	if($w==0) 
	  $w=$this->w-$this->rMargin-$this->x; 
	$wmax=($w-2*$this->cMargin)*1000/$this->FontSize; 
	$s=str_replace("\r",'',$txt); 
	$nb=strlen($s); 
	if($nb>0 and $s[$nb-1]=="\n") 
	  $nb--; 
	$sep=-1; 
	$i=0; 
	$j=0; 
	$l=0; 
	$nl=1; 
	while($i<$nb) 
	{ 
	    $c=$s[$i]; 
	    if($c=="\n") 
	    { 
		$i++; 
		$sep=-1; 
		$j=$i; 
		$l=0; 
		$nl++; 
		continue; 
	    } 
	    if($c==' ') 
	      $sep=$i; 
	    $l+=$cw[$c]; 
	    if($l>$wmax) 
	    { 
		if($sep==-1) 
		{ 
		    if($i==$j) 
		      $i++; 
		} 
		else 
		  $i=$sep+1; 
		$sep=-1; 
		$j=$i; 
		$l=0; 
		$nl++; 
	    } 
	    else 
	      $i++; 
	} 
	return $nl; 
    } 
    

    #******************************     
    # draw table header
    function DrawTableHeader($header, $count,$w, $h)
    {
	$this->SetDrawColor(125,125,125);
	$this->SetFillColor(180,180,180);
	$this->SetXY($this->left, $this->GetY());
	for($i=0;$i<$count;$i++)
	{
	  if($i == 0)
	      $align = 'L';
	  $width = ($w[$i] * ($this->pagewidth - $this->left - $this->right))/100;
	  $this->Cell($width,$h,$header[$i],1,0,$align,1,true);
	}
	$this->Ln();
    }
    
    #****************************** 
    # section 
    function DrawTable($ar1, $cols, $col_len, $header, $header_font)
    { 
	# $this->CustomHeader(); 
	$this->DrawTableHeader($header, $cols, $col_len, $header_font);
        $font_size = 6;
	$this->SetFont('Arial', '', $font_size);
	$this->SetDrawColor(125,125,125);
	$align=1;
	
	for($i=0; $i<count($ar1); $i++) 
	{ 
	    $multi_col = array();
	    $nb = 0;
	    for($j=0; $j<$cols; $j++) 
	    {
		$f[$j] = $ar1[$i][$j];
		
		$width = ($col_len[$j] * ($this->pagewidth - $this->left - $this->right))/100;
		$tmp = $this->NbLines($width,$f[$j]);
		if($tmp > $nb)
		{
		    $nb = $tmp;
		}
		if($tmp > 1)
		{
		  $multi_col[$j] = true;
		}
		else 
		{
		  $multi_col[$j] = false;
		}
	    }
	    
	    # total y
	    $hx = $nb * $font_size;
	    
	    #  check for page break 
	    $startx = $this->left;
    	    $starty = $this->GetY();
	    $rowmaxy = $starty + $hx;
	    $newpage = false;
	    for($j=0; $j<$cols; $j++) 
	    {
		if(($j == 0) && ($this->GetY() + $hx > $this->PageBreakTrigger))
		{ 
		    $this->AddPage(); 
		    $this->Ln(5); 
		    $this->SetFont('Arial', '', $font_size);
		    $startx = $this->left;

		    $newpage = true;
		    $this->DrawTableHeader($header, $cols, $col_len, $header_font);
		    $starty = $this->GetY();
		}
		
		$this->SetXY($startx, $starty); 
		$width = ($col_len[$j] * ($this->pagewidth - $this->left - $this->right))/100;
		if($multi_col[$j] == true)
		{
		    $this->MultiCell($width,$font_size,$f[$j],1,$align,0);
		}
		else
		{		  	
		    $this->MultiCell($width,$hx,$f[$j],1,$align,0); 
		}

		$startx+= $width;
		$this->Ln(0);
	    }
	    if(!$newpage)
	    {
		$this->SetY($rowmaxy);
	    }	    
	}
    }

    ###############################################
    # Draw table with multiple tables
    ###############################################
    function DrawTableSpecial($ar1, $cols, $col_len, $header, $header_font)
    { 
	$font_size = 6;
	$this->SetFont('Arial', '', $font_size);
	$this->SetDrawColor(125,125,125);
	for($i=0; $i<count($ar1); $i++) 
	{ 
	    $multi_col = array();
	    $nb = 0;
	    for($j=0; $j<$cols; $j++) 
	    {
		$f[$j] = $ar1[$i][$j];
		$width = ($col_len[$j] * ($this->pagewidth - $this->left - $this->right))/100;
		$tmp = $this->NbLines($width,$f[$j]);
		if($tmp > $nb)
		{
		    $nb = $tmp;
		}
		if($tmp > 1)
		{
		    $multi_col[$j] = true;
		}
		else 
		{
		    $multi_col[$j] = false;
		}
	    }
	    
	    # total y
	    $hx = $nb * $font_size;
	    
	    #  check for page break 
	    $startx = $this->left;
	    $starty = $this->GetY();
	    $rowmaxy = $starty + $hx;
	    $newpage = false;
	    for($j=0; $j<$cols; $j++) 
	    {
		if(($j == 0) && ($this->GetY() + $hx > $this->PageBreakTrigger))
		{ 
		    $this->AddPage(); 
		    $this->Ln(5); 
		    $this->SetFont('Arial', '', $font_size);
		    $startx = $this->left;
		    
		    $newpage = true;
#		    $this->DrawTableHeader($header, $cols, $col_len, $header_font);
		    $starty = $this->GetY();
		}
		
		$this->SetXY($startx, $starty); 
		$width = ($col_len[$j] * ($this->pagewidth - $this->left - $this->right))/100;
		
		$isheader = false;
		$nh_removed = '';

		if($j == 0){
		    $pos = strpos($f[$j],'<nh>');
		    if($pos !== false) {					
			$nh_removed = str_replace('<nh>', '', $f[$j]);
			$isheader = true;
		    }
		}
		
		if($isheader === true)
		{
		    $ar1[$i][$j] = $nh_removed;
		    $this->DrawTableHeader($ar1[$i], $cols, $col_len, $font_size);
		    $j = $cols + 1;
		}
		else
		{
		    if($multi_col[$j] == true)
		    {
			$this->MultiCell($width,$font_size,$f[$j],1,$align,0);
		    }
		    else
		    {		  	
			$this->MultiCell($width,$hx,$f[$j],1,$align,0); 
		    }
		}
		$startx+= $width;
		$this->Ln(0);
	    }
	    if(!$newpage)
	    {
		$this->SetY($rowmaxy);
	    }	    
	}
    }
}

## functions for reports
function calc_width_percent($total,$width)
{
    $ret = ($width/$total)*100;
    return $ret;
}

function rpt_bundle_profile($hostkey,$search,&$pdf,$class_regex)
{
    $cols=6;
    $col_len = array(24,23,23,10,10,10); #in percentage
    $header=array('Host','Bundle','Last verified','Hours Ago', 'Avg interval', 'Uncertainty');

    $ret = cfpr_report_bundlesseen_pdf($hostkey,$search,true,$class_regex);
    $data1 = $pdf->ParseData($ret);
    
    $pdf->ReportTitle();
    $pdf->ReportDescription();
    $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
        
    $pdf->SetFont('Arial','',9);
    $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
}

### business value report ###

function rpt_business_value($hostkey,$days,$months,$years,&$pdf)
{
    $cols=5;
    $col_len = array(24,19,19,19,19);
    $header=array('Host','Day','Kept','Repaired', 'Not kept');

    $ret = cfpr_report_value_pdf($hostkey,$days,$months,$years);
    $data1 = $pdf->ParseData($ret);
    
    $pdf->ReportTitle();
    $pdf->ReportDescription();
    $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    
    $pdf->SetFont('Arial','',9);
    $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
}

### Classes report ###
function rpt_class_profile($hostkey,$search,&$pdf)
{
    $cols=5;
    $col_len = array(28,28,20,12,12);
    $header=array('Host','Class Context','Occurs with probability','Uncertainty', 'Last seen');
    
    $ret = cfpr_report_classes_pdf($hostkey,$search,true);
    $data1 = $pdf->ParseData($ret);
    
    # count the number of columns	
    #$cols = (count($data1,1)/count($data1,0))-1;
    
    $pdf->ReportTitle();
    $pdf->ReportDescription();
    $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    
    $pdf->SetFont('Arial','',9);
    $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
}

function rpt_promise_notkept($hostkey,$search,&$pdf)
{
    $col_len = array(20,20,40,20);
    $header=array('Host','Promise Handle','Report','Time');

    $ret = cfpr_report_notkept_pdf($hostkey,$search);
    $data1 = $pdf->ParseData($ret);
    
    $pdf->ReportTitle();
    $pdf->ReportDescription();
    $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    
    $pdf->SetFont('Arial','',9);
    $pdf->DrawTable($data1, 4, $col_len, $header, 8);
}

### Compliance by promise ##
function rpt_compliance_promises($hostkey,$search,$state,&$pdf)
{
    $cols=6;
    $col_len = array(21,24,14,14,12,15);
    $header=array('Host','Promise Handle','Last known state','Probability kept', 'Uncertainty', 'Last seen');
    
    $ret = cfpr_report_compliance_promises_pdf($hostkey,$search,$state,true);
    $data1 = $pdf->ParseData($ret);

    $pdf->ReportTitle();
    $pdf->ReportDescription();
    $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    
    $pdf->SetFont('Arial','',9);
    $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
}

### Compliance Summary ##
function rpt_compliance_summary($hostkey,&$pdf)
{
    $cols=6;
    $col_len = array(25,27,10,10,10,18);
    $header=array('Host','Policy','Kept','Repaired','Not kept', 'Last seen');
    
    $ret = cfpr_report_compliance_summary_pdf($hostkey,NULL,-1,-1,-1,-1,">");
    $data1 = $pdf->ParseData($ret);
    
    $pdf->ReportTitle();
    $pdf->ReportDescription();
    $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    
    $pdf->SetFont('Arial','',9);
    $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
}

### Classes report ###
function rpt_filechange_log($hostkey,$search,&$pdf)
{
    $cols=3;
    $col_len = array(33,34,33);
    $header=array('Host','File', 'Time of Change');
    
    $ret = cfpr_report_filechanges_pdf($hostkey,$search,true,-1,">");
    $data1 = $pdf->ParseData($ret);
    $pdf->ReportTitle();

    $pdf->ReportDescription();
    $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    $pdf->SetFont('Arial','',9);
    $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
}

### Classes report ###
function rpt_lastsaw_hosts($hostkey,$key,$name,$address,$ago,&$pdf)
{
    $cols=9;
    $col_len = array(14,8,10,14,12,7,7,7,21);
    $header=array('Host','Initiated', 'IP Address', 'Remote Host', 'Last Seen', 'Hours Ago', ' Avg Interval', 'Uncertainty', 'Remote Host Key');

    $ret =  cfpr_report_lastseen_pdf($hostkey,$key,$name,$address,$ago,true);
    $data1 = $pdf->ParseData($ret);
    $pdf->ReportTitle();
    $pdf->ReportDescription();
    $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    $pdf->SetFont('Arial','',6);
    $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
}

### Patches Available  ###
function rpt_patches_available($hostkey,$search,$version,$arch,&$pdf)
{
    $cols=4;
    $col_len = array(29,33,19,19);
    $header=array('Host','Name','Version','Architecture');
    
    $ret =  cfpr_report_patch_avail_pdf($hostkey,$search,$version,$arch,true);
    $data1 = $pdf->ParseData($ret);
    $pdf->ReportTitle();
    $pdf->ReportDescription();
    $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    $pdf->SetFont('Arial','',9);
    $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
}

### Patch Status  ###
function rpt_patch_status($hostkey,$search,$version,$arch,&$pdf)
{
    $cols=4;
    $col_len = array(29,33,19,19);
    $header=array('Host','Name','Version','Architecture');

    $ret = cfpr_report_patch_in_pdf($hostkey,$search,$version,$arch,true);
    $data1 = $pdf->ParseData($ret);
    $pdf->ReportTitle();
    $pdf->ReportDescription();
    $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    $pdf->SetFont('Arial','',9);
    $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
}

### Software Installed  ###

function rpt_software_installed($hostkey,$search,$version,$arch,&$pdf)
{
    $cols=4;
    $col_len = array(21,33,19,19);
    $header=array('Host','Name','Version','Architecture');
    
    $ret = cfpr_report_software_in_pdf($hostkey,$search,$version,$arch,true);
    $data1 = $pdf->ParseData($ret);
    $pdf->ReportTitle();
    $pdf->ReportDescription();
    $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    $pdf->SetFont('Arial','',9);
    $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
}

### Performance Report  ###
function rpt_performance($hostkey,$search,&$pdf)
{
        $cols=6;
        $col_len = array(19,38,7.5,7.5,10,18);
        $header=array('Host','Repair','Last Time','Avg Time','Uncertainty','Last Performed');

	$ret = cfpr_report_performance_pdf($hostkey,$search,true);
        $data1 = $pdf->ParseData($ret);
        $pdf->ReportTitle();
        $pdf->ReportDescription();
        $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
        $pdf->Ln(8);
        $pdf->SetFont('Arial','',9);
        $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
}

function rpt_repaired_log($hostkey,$search,&$pdf)
{
    $cols=4;
    $col_len = array(19,19,43,19);
    $header=array('Host','Promise Handle','Report','Time');

    $ret = cfpr_report_repaired_pdf($hostkey,$search);
    $data1 = $pdf->ParseData($ret);
    $pdf->ReportTitle();
    $pdf->ReportDescription();
    $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    $pdf->SetFont('Arial','',9);
    $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
}

function rpt_repaired_summary($hostkey,$search,&$pdf)
{
    $cols=4;
    $col_len = array(19,19,43,19);
    $header=array('Host','Promise Handle','Report','Occurrences');

    $ret =  cfpr_summarize_repaired_pdf($hostkey,$search);
    $data1 = $pdf->ParseData($ret);
    
    $pdf->ReportTitle();
    $pdf->ReportDescription();
    $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
    $pdf->Ln(8);

    $pdf->SetFont('Arial','',9);
    $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
}

function rpt_notkept_summary($hostkey,$search,&$pdf)
{
    $cols=4;
    $col_len = array(19,19,43,19);
    $header=array('Host','Promise Handle','Report','Occurrences');

    $ret =  cfpr_summarize_notkept_pdf($hostkey,$search);
    $data1 = $pdf->ParseData($ret);
    $pdf->ReportTitle();
    $pdf->ReportDescription();
    $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    $pdf->SetFont('Arial','',9);
    $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
}

function rpt_variables($hostkey,$search,$scope,$lval,$rval,$type,&$pdf)
{
    $cols=4;
    $col_len = array(19,15,19,47);
    $header=array('Host','Type','Name','Value');

    if($hostkey == NULL)
    {
	$ret = cfpr_report_vars_pdf(NULL,$scope,$lval,$rval,$type,true);
    }
    else
    {   
	$ret = cfpr_report_vars_pdf($hostkey,NULL,$search,NULL,NULL,true);
    }
    $data1 = $pdf->ParseData($ret);
    $pdf->ReportTitle();
    $pdf->ReportDescription();
    $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    
    $pdf->SetFont('Arial','',9);
    $pdf->DrawTableSpecial($data1, $cols, $col_len, $header, 8);
}

function rpt_filediffs($hostkey,$search,$diff,$cal,&$pdf)
{
    $cols=4;
    $col_len = array(19,15,19,47);
    $header=array('Host','File','Change detected at','Change');

    $ret = cfpr_report_filediffs_pdf($hostkey,$search,$diff,true,$cal,">");

    $data1 = $pdf->ParseData($ret);
    $pdf->ReportTitle();
    $pdf->ReportDescription();
    $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    
    $pdf->SetFont('Arial','',9);
    $pdf->DrawTableSpecial($data1, $cols, $col_len, $header, 8);
}

function rpt_setuid($hostkey,$search,&$pdf)
{
    $cols=2;
    $col_len = array(30,70);
    #        $header=array('Host','Type','Name','Value');
    
    $ret = cfpr_report_setuid_pdf($hostkey,$search,true);
    
    $data1 = $pdf->ParseData($ret);
    $pdf->ReportTitle();
    $pdf->ReportDescription();
    $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    
    $pdf->SetFont('Arial','',9);
    $pdf->DrawTableSpecial($data1, $cols, $col_len, $header, 8);
}

#
# Send email
#
function EmailPDF($pdf, $pdf_filename)
{
    $to = "bishwa.shrestha@gmail.com";
    $from = "bishwa.shrestha@cfengine.com";
    $subject = "Nova Report";
    $message = "<p>Please see the attachment.</p>";

    // a random hash will be necessary to send mixed content
    $separator = md5(time());

    // carriage return type (we use a PHP end of line constant)
    $eol = PHP_EOL;
    
    // attachment name
    $filename = $pdf_filename;

    // encode data (puts attachment in proper format)
    $pdfdoc = $pdf->Output("", "S");
    $attachment = chunk_split(base64_encode($pdfdoc));
    
    // main header (multipart mandatory)
    $headers = "From: ".$from.$eol;
    $headers .= "MIME-Version: 1.0".$eol;
    $headers .= "Content-Type: multipart/mixed; boundary=\"".$separator."\"".$eol.$eol;
    $headers .= "Content-Transfer-Encoding: 7bit".$eol;
    $headers .= "This is a MIME encoded message.".$eol.$eol;
    
    // message
    $headers .= "--".$separator.$eol;
    $headers .= "Content-Type: text/html; charset=\"iso-8859-1\"".$eol;
    $headers .= "Content-Transfer-Encoding: 8bit".$eol.$eol;
    $headers .= $message.$eol.$eol;
    
    // attachment
    $headers .= "--".$separator.$eol;
    $headers .= "Content-Type: application/octet-stream; name=\"".$filename."\"".$eol;
    $headers .= "Content-Transfer-Encoding: base64".$eol;
    $headers .= "Content-Disposition: attachment".$eol.$eol;
    $headers .= $attachment.$eol.$eol;
    $headers .= "--".$separator."--";
    
    // send message
    mail($to, $subject, "", $headers);
    
}

# main control
$report_type = $_GET['type'];
$pdf_filename = preg_replace('/ /', '_', $report_type).'.pdf';
$pdf=new PDF();
$pdf->PDFSetReportName($report_type);
$pdf->PDFSetTableTitle('DATA REPORTED');
$pdf->AliasNbPages();
$pdf->SetFont('Arial','',14);
$pdf->AddPage();

switch($report_type)
{
 case  "Bundle profile":
    $desc=cfpr_report_description('bundle profile');
    $pdf->PDFSetDescription($desc);
    rpt_bundle_profile($_GET['hostkey'],$_GET['search'],$pdf,$_GET['class_regex']);
    break;
 
 case  "Business value report":
    $desc=cfpr_report_description('value report');
    $pdf->PDFSetDescription($desc);
    rpt_business_value($_GET['hostkey'],$_GET['days'],$_GET['months'],$_GET['years'],$pdf);
    break;
 
 case "Class profile":
    $desc=cfpr_report_description('classes report');
    $pdf->PDFSetDescription($desc);
    rpt_class_profile($_GET['hostkey'],$_GET['search'],$pdf);
    break;
 
 case "Compliance by promise":
    $desc=cfpr_report_description('promise report');
    $pdf->PDFSetDescription($desc);
    rpt_compliance_promises($_GET['hostkey'],$_GET['search'],$_GET['state'],$pdf);
    break;
    
 case "Compliance summary":
    $desc=cfpr_report_description('compliance report');
    $pdf->PDFSetDescription($desc);
    rpt_compliance_summary($_GET['hostkey'],$pdf);
    break;
    
 case "File change log":
    $desc=cfpr_report_description('file_changes report');
    $pdf->PDFSetDescription($desc);
    rpt_filechange_log($_GET['hostkey'],$_GET['search'],$pdf);
    break;
    
 case "Last saw hosts":
    $desc=cfpr_report_description('lastseen report');
    $pdf->PDFSetDescription($desc);
    rpt_lastsaw_hosts($_GET['hostkey'],$_GET['key'],$_GET['search'],$_GET['address'],$_GET['ago'],$pdf);
    break;
    
 case "Patches available":
    $desc=cfpr_report_description('patches available report');
    $pdf->PDFSetDescription($desc);
    rpt_patches_available($_GET['hostkey'],$_GET['search'],$_GET['version'],$_GET['arch'],$pdf);
    break;
 
 case "Patch status":
    $desc=cfpr_report_description('patches installed report');
    $pdf->PDFSetDescription($desc);
    rpt_patch_status($_GET['hostkey'],$_GET['search'],$_GET['version'],$_GET['arch'],$pdf);
    break;
 
 case "Performance":
    $desc=cfpr_report_description('performance report');
    $pdf->PDFSetDescription($desc);
    rpt_performance($_GET['hostkey'],$_GET['search'],$pdf);
    break;
    
 case "Promises repaired summary":
    $desc=cfpr_report_description('promises repaired summary');
    $pdf->PDFSetDescription($desc);
    rpt_repaired_log($_GET['hostkey'],$_GET['search'],$pdf);
    break;
    
 case "Promises repaired log":
    $desc=cfpr_report_description('promises repaired report');
    $pdf->PDFSetDescription($desc);
    rpt_repaired_log($_GET['hostkey'],$_GET['search'],$pdf);
    break;
    
 case "Promises not kept summary":
    $desc=cfpr_report_description('promises repaired report');
    $pdf->PDFSetDescription($desc);
    rpt_promise_notkept($_GET['hostkey'],$_GET['search'],$pdf);
    break;
    
 case "Promises not kept log":
    $desc=cfpr_report_description('promises not kept report');
    $pdf->PDFSetDescription($desc);
    rpt_promise_notkept($_GET['hostkey'],$_GET['search'],$pdf);
    break;
 
 case "Setuid/gid root programs":
    $desc=cfpr_report_description('setuid report');
    $pdf->PDFSetDescription($desc);
    rpt_setuid($_GET['hostkey'],$_GET['search'],$pdf);    
    break;
 
 case "Software installed":
    $desc=cfpr_report_description('software installed report');
    $pdf->PDFSetDescription($desc);
    rpt_software_installed($_GET['hostkey'],$_GET['search'],$_GET['version'],$_GET['arch'],$pdf);
    break;
 
 case "Variables":
    $desc=cfpr_report_description('variables report');
    $pdf->PDFSetDescription($desc);    
    rpt_variables($_GET['hostkey'],$_GET['search'],$_GET['scope'],$_GET['lval'],$_GET['rval'],$_GET['var_type'],$pdf);
    break;

 case "File change diffs":
    $desc=cfpr_report_description('file_diffs report');
    $pdf->PDFSetDescription($desc);    
    rpt_filediffs($_GET['hostkey'],$_GET['search'],$_GET['diff'],$_GET['cal'],$pdf);
    break;
    
}

$pdf_action = $_GET['pdf_action'];  
if($pdf_action == 'email')
{
  EmailPDF($pdf,$pdf_filename);
}
else
{
   $pdf->Output($pdf_filename, "D");
}
?>
