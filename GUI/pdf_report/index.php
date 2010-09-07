<?php
require('fpdf.php');
#define('FPDF_FONTPATH','fonts/');
class PDF extends FPDF
{
    var $left=5;
    var $right=5;
    var $top=10;
    var $pagewidth=210;
    
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
    
    function PDFSetMargins($left,$top,$right)
    {
	$this->left=$left;
	$this->right=$right;
	$this->top=$top;
	$this->SetMargins($this->left,$this->top,$this->right);
    }
    
    function PDFGetLeftMargin()
    {
	return $this->left;
    }
    
    function PDFGetRightMargin()
    {
	return $this->right;
    }
    
    function PDFGetPageWidth()
    {
	return $this->pagewidth;
    }
    #******************************
    #Page header
    function Header()
    {
	$this->SetFont('Arial','',8);
        $this->Text(30,10,$timestamp);
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
	$this->Cell(0,10,'Run: '.date("m.d.y H:i:s",time()).'  Powered by Cfengine',0,0,'C');
    }
    
    #******************************
    # Title
    function ReportTitle($title)
    {
	$this->SetFont('Arial','',18);
	$this->SetTextColor(255,255,255);
	$this->SetFillColor(158,152,120);
	$width=$this->pagewidth-$this->left-$this->right;
	$this->Rect($this->left, 28, $width, 10, "F" );
	$this->Cell($width,6,"$title",0,1,'L',true);
	$this->Ln(4);
	$this->SetTextColor(76,76,76);
    }
    
    #******************************
    # Description
    function ReportDescription($text)
    {
	$this->SetFont('Arial','',10);
	$width=$this->pagewidth-$this->left-$this->right;
	$this->MultiCell($width,5,"$text");
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
    # Load data
    function LoadData($file)
    {
	$lines=file($file);
	$data=array();
	foreach($lines as $line)
	  $data[]=explode(';',chop($line));
	return $data;
    }
    
    #****************************** 
    # Parse data
    function ParseData($arr)
    {
	$lines=explode('<nova_nl>',$arr);
	$data=array();
	foreach($lines as $line)
	{
	    if (trim($line)!='')
	    {
	     $data[]=explode(';',chop($line));
	    }
	}
	return $data;
    }
    
    #******************************
    # åcompute the number of lines
    
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
}

## functions for reports
function calc_width_percent($total,$width)
{
    $ret = ($width/$total)*100;
    return $ret;
}

function rpt_bundle_profile($hostkey,$search)
{
    $cols=6;
    $col_len = array(24,23,23,10,10,10); #in percentage
    $header=array('Host','Bundle','Last verified','Hours Ago', 'Avg interval', 'Uncertainty');
    $logo_path = 'logo_outside_new.jpg';
    
    $pdf=new PDF();
    $pdf->AliasNbPages();
    $pdf->SetFont('Arial','',14);
    $pdf->AddPage();
    
    $ret = cfpr_report_bundlesseen_pdf($hostkey,$search,true);
    
    $data1 = $pdf->ParseData($ret);
    
    # count the number of columns	
    #$cols = (count($data1,1)/count($data1,0))-1;
    
    $title = 'Bundle Profile';
    $pdf->ReportTitle($title);
    $description = 'This report shows the status of Bundles';
    $pdf->ReportDescription($description);
    
    $rptTableTitle = 'DATA REPORTED';
    $pdf->RptTableTitle($rptTableTitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    
    # TODO: calculate the length of individual columns
    
    $pdf->SetFont('Arial','',9);
    # DrawTableHeader(array, col_count, col_len, font_size)
    #	$pdf->DrawTableHeader($header, 4, $col_len, 8);
    # DrawTable(array, col_count, col_len, table header, font size)
    $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
    
    $pdf->Output("Nova_bundle_profile.pdf", "D");
}

### business value report ###

function rpt_business_value($hostkey,$days,$months,$years)
{
    $title = 'Business Value Report';
    $cols=5;
    $col_len = array(24,19,19,19,19);
    $header=array('Host','Day','Kept','Repaired', 'Not kept');
    $logo_path = 'logo_outside_new.jpg';
    
    $pdf=new PDF();
    $pdf->AliasNbPages();
    $pdf->SetFont('Arial','',14);
    $pdf->AddPage();

    # give host name TODO
    $ret = cfpr_report_value_pdf($hostkey,$days,$months,$years); #cfpr_report_bundle_profile_pdf(NULL,NULL,NULL);
    $data1 = $pdf->ParseData($ret);
    
    # count the number of columns	
    #$cols = (count($data1,1)/count($data1,0))-1;
    
    $pdf->ReportTitle($title);
    $description = 'This report shows the Business Value Report.';
    $pdf->ReportDescription($description);
    
    $rptTableTitle = 'DATA REPORTED';
    $pdf->RptTableTitle($rptTableTitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    
    # TODO: calculate the length of individual columns
    
    $pdf->SetFont('Arial','',9);
    # DrawTableHeader(array, col_count, col_len, font_size)
    #	$pdf->DrawTableHeader($header, 4, $col_len, 8);
    # DrawTable(array, col_count, col_len, table header, font size)
    $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
    
    $pdf->Output("Nova_business_value.pdf", "D");
}

### Classes report ###

function rpt_class_profile($hostkey,$search)
{
    $title = 'Class Profile';
    $cols=5;
    $col_len = array(28,28,20,12,12);
    $header=array('Host','Class Context','Occurs with probability','Uncertainty', 'Last seen');
    $logo_path = 'logo_outside_new.jpg';
    
    $pdf=new PDF();
    $pdf->AliasNbPages();
    $pdf->SetFont('Arial','',14);
    $pdf->AddPage();
    
    # give host name TODO
    $ret = cfpr_report_classes_pdf($hostkey,$search,true);
    $data1 = $pdf->ParseData($ret);
    
    # count the number of columns	
    #$cols = (count($data1,1)/count($data1,0))-1;
    
    $pdf->ReportTitle($title);
    $description = 'This report shows the Classes Profile.';
    $pdf->ReportDescription($description);
    
    $rptTableTitle = 'DATA REPORTED';
    $pdf->RptTableTitle($rptTableTitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    
    # TODO: calculate the length of individual columns
    
    $pdf->SetFont('Arial','',9);
    # DrawTableHeader(array, col_count, col_len, font_size)
    #	$pdf->DrawTableHeader($header, 4, $col_len, 8);
    # DrawTable(array, col_count, col_len, table header, font size)
    $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
    
    $pdf->Output("Nova_class_profile.pdf", "D");
}

## reports end
   # function run($title, $description)
    function rpt_promise_notkept($hostkey,$search)
    {
	$col_len = array(20,20,40,20);
	
	$pdf=new PDF();
	$pdf->AliasNbPages();
	$pdf->SetFont('Arial','',14);
	$pdf->AddPage();
        
	$logo_path = 'logo_outside_new.jpg';
		
	# cfpr function calls
	# 1. get header
	$header=array('Host','Promise Handle','Report','Time');

	$ret = cfpr_report_notkept_pdf($hostkey,$search);
	
	$data1 = $pdf->ParseData($ret);

	# count the number of columns	
	$cols = (count($data1,1)/count($data1,0))-1;
        
	$rptTableTitle = 'DATA REPORTED';
        $title = 'Promises Not Kept Log/Summary';
	$pdf->ReportTitle($title);
        $description = 'This report shows the list of promises Cfengine was not able to keep.';
	$pdf->ReportDescription($description);
	$pdf->RptTableTitle($rptTableTitle, $pdf->GetY() + 5);
	$pdf->Ln(8);
	
	# TODO: calculate the length of individual columns
	
	$pdf->SetFont('Arial','',9);
	# DrawTableHeader(array, col_count, col_len, font_size)
#	$pdf->DrawTableHeader($header, 4, $col_len, 8);
	# DrawTable(array, col_count, col_len)
	$pdf->DrawTable($data1, 4, $col_len, $header, 8);
	
	$pdf->Output("Nova_promises_not_kept.pdf", "D");
    
    }

### Compliance by promise ##

function rpt_compliance_promises($hostkey,$search,$state)
{
    $title = 'Compliance by promise';
    $cols=6;
    $col_len = array(24,24,14,14,12,12);
    $header=array('Host','Promise Handle','Last known state','Probability kept', 'Uncertainty', 'Last seen');
    $logo_path = 'logo_outside_new.jpg';
    
    $pdf=new PDF();
    $pdf->AliasNbPages();
    $pdf->SetFont('Arial','',14);
    $pdf->AddPage();
    
    # give host name TODO
    $ret = cfpr_report_compliance_promises_pdf($hostkey,$search,$state,true);
    $data1 = $pdf->ParseData($ret);
    
    # count the number of columns
    #$cols = (count($data1,1)/count($data1,0))-1;
    
    $pdf->ReportTitle($title);
    $description = 'This report shows the Compliance by promise.';
    $pdf->ReportDescription($description);
    
    $rptTableTitle = 'DATA REPORTED';
    $pdf->RptTableTitle($rptTableTitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    
    # TODO: calculate the length of individual columns
    
    $pdf->SetFont('Arial','',9);
    
    $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
    
    $pdf->Output("Nova_Compliance_by_promise.pdf", "D");
}

### Compliance Summary ##

function rpt_compliance_summary($hostkey)
{
    $title = 'Compliance Summary';
    $cols=6;
    $col_len = array(23,25,12,12,12,16);
    $header=array('Host','Policy','Kept','Repaired','Not kept', 'Last seen');
    $logo_path = 'logo_outside_new.jpg';
    
    $pdf=new PDF();
    $pdf->AliasNbPages();
    $pdf->SetFont('Arial','',14);
    $pdf->AddPage();
    
    $ret = cfpr_report_compliance_summary_pdf($hostkey,NULL,-1,-1,-1,-1,">");
    $data1 = $pdf->ParseData($ret);
    
    # count the number of columns
    #$cols = (count($data1,1)/count($data1,0))-1;
    
    $pdf->ReportTitle($title);
    $description = 'This report shows the Compliance Summary.';
    $pdf->ReportDescription($description);
    
    $rptTableTitle = 'DATA REPORTED';
    $pdf->RptTableTitle($rptTableTitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    
    # TODO: calculate the length of individual columns
    
    $pdf->SetFont('Arial','',9);
    
    $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
    
    $pdf->Output("Nova_Compliance_summary.pdf", "D");
}

### Classes report ###

function rpt_filechange_log($hostkey,$search)
{
    $title = 'File Change Log';
    $cols=3;
    $col_len = array(33,34,33);
    $header=array('Host','File', 'Time of Change');
    $logo_path = 'logo_outside_new.jpg';
    
    $pdf=new PDF();
    $pdf->AliasNbPages();
    $pdf->SetFont('Arial','',14);
    $pdf->AddPage();

    # give host name TODO
    $ret = cfpr_report_filechanges_pdf($hostkey,$search,true,-1,">");
    $data1 = $pdf->ParseData($ret);
    
    # count the number of columns
    #$cols = (count($data1,1)/count($data1,0))-1;
    
    $pdf->ReportTitle($title);
    $description = 'This report shows the Files changed log.';
    $pdf->ReportDescription($description);
    
    $rptTableTitle = 'DATA REPORTED';
    $pdf->RptTableTitle($rptTableTitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    
    # TODO: calculate the length of individual columns
    
    $pdf->SetFont('Arial','',9);
    
    $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
    
    $pdf->Output("Nova_File_change_log.pdf", "D");
}

### Classes report ###

function rpt_lastsaw_hosts($hostkey,$key,$name,$address,$ago)
{
    $title = 'Last saw hosts';
    $cols=9;
    $col_len = array(14,8,10,14,12,7,7,7,21);
    $header=array('Host','Initiated', 'IP Address', 'Remote Host', 'Last Seen', 'Hours Ago', ' Avg Interval', 'Uncertainty', 'Remote Host Key');
    $logo_path = 'logo_outside_new.jpg';
    
    $pdf=new PDF();
    $pdf->AliasNbPages();
    $pdf->SetFont('Arial','',10);
    $pdf->AddPage();

    # give host name TODO
    $ret =  cfpr_report_lastseen_pdf($hostkey,$key,$name,$address,$ago,true);
    $data1 = $pdf->ParseData($ret);
    
    # count the number of columns
    #$cols = (count($data1,1)/count($data1,0))-1;
    
    $pdf->ReportTitle($title);
    $description = 'This report shows the Last Seen data.';
    $pdf->ReportDescription($description);
    
    $rptTableTitle = 'DATA REPORTED';
    $pdf->RptTableTitle($rptTableTitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    
    # TODO: calculate the length of individual columns
    
    $pdf->SetFont('Arial','',6);
    $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
    $pdf->Output("Nova_last_saw_hosts.pdf", "D");
}

### Patches Available  ###

function rpt_patches_available($hostkey,$search,$version,$arch)
{
    $title = 'Patches Available';
    $cols=4;
    $col_len = array(29,33,19,19);
    $header=array('Host','Name','Version','Architecture');
    $logo_path = 'logo_outside_new.jpg';
    
    $pdf=new PDF();
    $pdf->AliasNbPages();
    $pdf->SetFont('Arial','',14);
    $pdf->AddPage();
    
    # give host name TODO
    $ret =  cfpr_report_patch_avail_pdf($hostkey,$search,$version,$arch,true);
    $data1 = $pdf->ParseData($ret);
    
    # count the number of columns
    #$cols = (count($data1,1)/count($data1,0))-1;
    
    $pdf->ReportTitle($title);
    $description = 'This report shows the List of patches available.';
    $pdf->ReportDescription($description);
    
    $rptTableTitle = 'DATA REPORTED';
    $pdf->RptTableTitle($rptTableTitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    
    # TODO: calculate the length of individual columns
    
    $pdf->SetFont('Arial','',9);
    $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
    $pdf->Output("Nova_patches_available.pdf", "D");
}

### Patch Status  ###

function rpt_patch_status($hostkey,$search,$version,$arch)
{
    $title = 'Patch Status';
    $cols=4;
    $col_len = array(29,33,19,19);
    $header=array('Host','Name','Version','Architecture');
    $logo_path = 'logo_outside_new.jpg';
    
    $pdf=new PDF();
    $pdf->AliasNbPages();
    $pdf->SetFont('Arial','',14);
    $pdf->AddPage();
    
    # give host name TODO
    $ret = cfpr_report_patch_in_pdf($hostkey,$search,$version,$arch,true);
    $data1 = $pdf->ParseData($ret);
    
    # count the number of columns
    #$cols = (count($data1,1)/count($data1,0))-1;
    
    $pdf->ReportTitle($title);
    $description = 'This report shows the Status of patches.';
    $pdf->ReportDescription($description);
    
    $rptTableTitle = 'DATA REPORTED';
    $pdf->RptTableTitle($rptTableTitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    
    # TODO: calculate the length of individual columns
    
    $pdf->SetFont('Arial','',9);
    $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
    $pdf->Output("Nova_patch_status.pdf", "D");
}

### Software Installed  ###

function rpt_software_installed($hostkey,$search,$version,$arch)
{
    $title = 'Software Installed';
    $cols=4;
    $col_len = array(21,33,19,19);
    $header=array('Host','Name','Version','Architecture');
    $logo_path = 'logo_outside_new.jpg';
    
    $pdf=new PDF();
    $pdf->AliasNbPages();
    $pdf->SetFont('Arial','',14);
    $pdf->AddPage();
    
    # give host name TODO
    $ret = cfpr_report_software_in_pdf($hostkey,$search,$version,$arch,true);
    $data1 = $pdf->ParseData($ret);
    
    # count the number of columns
    #$cols = (count($data1,1)/count($data1,0))-1;
    
    $pdf->ReportTitle($title);
    $description = 'This report shows the List of softwares installed.';
    $pdf->ReportDescription($description);
    
    $rptTableTitle = 'DATA REPORTED';
    $pdf->RptTableTitle($rptTableTitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    
    # TODO: calculate the length of individual columns
    
    $pdf->SetFont('Arial','',9);
    $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
    $pdf->Output("Nova_software_installed.pdf", "D");
}

### Performance Report  ###
function rpt_performance($hostkey,$search)
{
        $title = 'Performance';
        $cols=6;
        $col_len = array(19,38,7.5,7.5,10,18);
        $header=array('Host','Repair','Last Time','Avg Time','Uncertainty','Last Performed');
        $logo_path = 'logo_outside_new.jpg';
    
        $pdf=new PDF();
        $pdf->AliasNbPages();
        $pdf->SetFont('Arial','',14);
        $pdf->AddPage();
    
        # give host name TODO
	$ret = cfpr_report_performance_pdf($hostkey,$search,true);
        $data1 = $pdf->ParseData($ret);
    
        # count the number of columns
	    #$cols = (count($data1,1)/count($data1,0))-1;
	    
        $pdf->ReportTitle($title);
        $description = 'This report shows the List of softwares installed.';
        $pdf->ReportDescription($description);
    
        $rptTableTitle = 'DATA REPORTED';
        $pdf->RptTableTitle($rptTableTitle, $pdf->GetY() + 5);
        $pdf->Ln(8);
    
        # TODO: calculate the length of individual columns
	
        $pdf->SetFont('Arial','',9);
        $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
        $pdf->Output("Nova_performance_report.pdf", "D");
}

function rpt_repaired_log($hostkey,$search)
{
    $title = 'Promises repaired log';
    $cols=4;
    $col_len = array(19,19,43,19);
    $header=array('Host','Promise Handle','Report','Time');
    $logo_path = 'logo_outside_new.jpg';
    
    $pdf=new PDF();
    $pdf->AliasNbPages();
    $pdf->SetFont('Arial','',14);
    $pdf->AddPage();
    
    # give host name TODO
    $ret = cfpr_report_repaired_pdf($hostkey,$search);
    $data1 = $pdf->ParseData($ret);
    
    # count the number of columns
    #$cols = (count($data1,1)/count($data1,0))-1;
    
    $pdf->ReportTitle($title);
    $description = 'This report shows the List Promises repaired.';
    $pdf->ReportDescription($description);
    
    $rptTableTitle = 'DATA REPORTED';
    $pdf->RptTableTitle($rptTableTitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    
    # TODO: calculate the length of individual columns
    
    $pdf->SetFont('Arial','',9);
    $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
    $pdf->Output("Nova_promises_repaired_log.pdf", "D");
    
    }

function rpt_repaired_summary($hostkey,$search)
{
    $title = 'Promises repaired summary';
    $cols=4;
    $col_len = array(19,19,43,19);
    $header=array('Host','Promise Handle','Report','Occurrences');
    $logo_path = 'logo_outside_new.jpg';
    
    $pdf=new PDF();
    $pdf->AliasNbPages();
    $pdf->SetFont('Arial','',14);
    $pdf->AddPage();
    
    # give host name TODO
    $ret =  cfpr_summarize_repaired_pdf($hostkey,$search);

    $data1 = $pdf->ParseData($ret);
    
    # count the number of columns
    #$cols = (count($data1,1)/count($data1,0))-1;
    
    $pdf->ReportTitle($title);
    $description = 'This report shows the summary of promises repaired.';
    $pdf->ReportDescription($description);
    
    $rptTableTitle = 'DATA REPORTED';
    $pdf->RptTableTitle($rptTableTitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    
    # TODO: calculate the length of individual columns
    
    $pdf->SetFont('Arial','',9);
    $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
    $pdf->Output("Nova_promises_repaired_summary.pdf", "D");
    
    }

function rpt_notkept_summary($hostkey,$search)
{
    $title = 'Promises not kept summary';
    $cols=4;
    $col_len = array(19,19,43,19);
    $header=array('Host','Promise Handle','Report','Occurrences');
    $logo_path = 'logo_outside_new.jpg';
    
    $pdf=new PDF();
    $pdf->AliasNbPages();
    $pdf->SetFont('Arial','',14);
    $pdf->AddPage();
    
    # give host name TODO
    $ret =  cfpr_summarize_notkept_pdf($hostkey,$search);

    $data1 = $pdf->ParseData($ret);
    
    # count the number of columns
    #$cols = (count($data1,1)/count($data1,0))-1;
    
    $pdf->ReportTitle($title);
    $description = 'This report shows the summary of promises not kept.';
    $pdf->ReportDescription($description);
    
    $rptTableTitle = 'DATA REPORTED';
    $pdf->RptTableTitle($rptTableTitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    
    # TODO: calculate the length of individual columns
    
    $pdf->SetFont('Arial','',9);
    $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
    $pdf->Output("Nova_promises_notkept_summary.pdf", "D");
    
    }

function rpt_variables($hostkey,$search,$scope,$lval,$rval,$type)
{
    $title = 'Variables';
    $cols=4;
    $col_len = array(19,19,19,43);
    $header=array('Host','Type','Name','Value');
    $logo_path = 'logo_outside_new.jpg';
    
    $pdf=new PDF();
    $pdf->AliasNbPages();
    $pdf->SetFont('Arial','',14);
    $pdf->AddPage();
    if($hostkey == NULL)
    {
	$ret =  cfpr_report_vars_pdf(NULL,$scope,$lval,$rval,$type,true);
    }
    else
    {
	$ret =  cfpr_report_vars_pdf($hostkey,NULL,$search,NULL,NULL,true);
    }
    $data1 = $pdf->ParseData($ret);
    
    # count the number of columns
    #$cols = (count($data1,1)/count($data1,0))-1;
    
    $pdf->ReportTitle($title);
    $description = 'This report shows the Variable status.';
    $pdf->ReportDescription($description);
    
    $rptTableTitle = 'DATA REPORTED';
    $pdf->RptTableTitle($rptTableTitle, $pdf->GetY() + 5);
    $pdf->Ln(8);
    
    # TODO: calculate the length of individual columns
    
    $pdf->SetFont('Arial','',9);
    $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
    $pdf->Output("Nova_variables.pdf", "D");
}


# main control

$report_type = $_GET['type'];

switch($report_type)
{
 case  "Bundle profile":
    rpt_bundle_profile($_GET['hostkey'],$_GET['search']);
    break;
 
 case  "Business value report":
    rpt_business_value($_GET['hostkey'],$_GET['days'],$_GET['months'],$_GET['years']);
    break;
 
 case "Class profile":
    rpt_class_profile($_GET['hostkey'],$_GET['search']);
    break;
 
 case "Compliance by promise":
    rpt_compliance_promises($_GET['hostkey'],$_GET['search'],$_GET['state']);
    break;
    
 case "Compliance summary":
    rpt_compliance_summary($_GET['hostkey']);
    break;
    
 case "File change log":
    rpt_filechange_log($_GET['hostkey'],$_GET['search']);
    break;
    
 case "Last saw hosts":
    rpt_lastsaw_hosts($_GET['hostkey'],$_GET['key'],$_GET['search'],$_GET['address'],$_GET['ago']);
    break;
    
 case "Patches available":
    rpt_patches_available($_GET['hostkey'],$_GET['search'],$_GET['version'],$_GET['arch']);
    break;
 
 case "Patch status":
    rpt_patch_status($_GET['hostkey'],$_GET['search'],$_GET['version'],$_GET['arch']);
    break;
 
 case "Performance":
        rpt_performance($_GET['hostkey'],$_GET['search']);
        break;
    
 case "Promises repaired summary":
 case "Promises repaired log":
    rpt_repaired_log($_GET['hostkey'],$_GET['search']);
    break;
 case "Promises not kept summary":
 case "Promises not kept log":
    rpt_promise_notkept($_GET['hostkey'],$_GET['search']);
    break;
 
 case "Setuid/gid root programs":
    print "Not implemented!!";
    break;
 
 case "Software installed":
    rpt_software_installed($_GET['hostkey'],$_GET['search'],$_GET['version'],$_GET['arch']);
    break;
 
 case "Variables":
    rpt_variables($_GET['hostkey'],$_GET['search'],$_GET['scope'],$_GET['lval'],$_GET['rval'],$_GET['var_type']);
    break;
    
}

#rpt_bundle_profile();
#rpt_value();
#rpt_class_profile();
#rpt_compliance_promises();
#rpt_filechanges();
#rpt_lastseen();
#rpt_patches_available();
# rpt_patch_status();
# rpt_software_installed();
# rpt_performance();

# rpt_repaired_log(); # this doesn't work : need better formatted data
# rpt_not_kept();
# rpt_repaired_summary();
# rpt_notkept_summary();
?>