<?php
require('fpdf.php');
#define('FPDF_FONTPATH','/pdf_report/fonts/');
class PDF extends FPDF
{
    #****************************** 
    function PDF($orientation='p', $unit='mm', $format='A4') 
    { 
    	$this->FPDF($orientation,$unit,$format); 
    }
    
    #******************************
    #Page header
    function Header()
    {
	$this->SetFont('Arial','',8);
        $this->Text(30,10,$timestamp);
	$logo_path = 'logo_outside_new.jpg';
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
	$this->Cell(0,10,'Run: '.date("m.d.y H:i:s",time()).'  Cfengine Inc.',0,0,'C');
    }
    
    #******************************
    # Title
    function ReportTitle($title)
    {
	$this->SetFont('Arial','',18);
	$this->SetTextColor(255,255,255);
	$this->SetFillColor(158,152,120);
	
	$this->Rect(0, 28, 500, 10, "F" );
	$this->Cell(0,6,"$title",0,1,'L',true);
	$this->Ln(4);
	$this->SetTextColor(76,76,76);
    }
    
    #******************************
    # Description
    function ReportDescription($text)
    {
	$this->SetFont('Arial','',10);
	$this->MultiCell(0,5,"$text");
	$this->Ln(4);
    }
    
    #******************************
    # Table Title
    function RptTableTitle($text, $y)
    {
	$this->SetFont('Arial','',14);
	$this->SetDrawColor(181,177,153);
	$this->SetFillColor(239,240,234);
	
	$this->Rect(0, $y, 500, 10, "DF" );
	$this->SetY($y+1);
	$this->Cell(0, 8,"$text",0,0,'L',true);
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
    # compute the number of lines
    
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
	$this->SetXY(0, $this->GetY());
	for($i=0;$i<$count;$i++)
	{
	  if($i == 0)
	      $align = 'L';
	  $this->Cell($w[$i],$h,$header[$i],1,0,$align,1,true);
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
		$tmp = $this->NbLines($col_len[$j],$f[$j]);
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
	    $startx = 0;    
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
		    $startx = 0;

		    $newpage = true;
		    $this->DrawTableHeader($header, $cols, $col_len, $header_font);
		    $starty = $this->GetY();
		}
		
		
		
		$this->SetXY($startx, $starty); 
		
		if($multi_col[$j] == true)
		{
		    $this->MultiCell($col_len[$j],$font_size,$f[$j],1,$align,0);
		}
		else
		{		  	
		    $this->MultiCell($col_len[$j],$hx,$f[$j],1,$align,0); 
		}
		
		$startx+= $col_len[$j];
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

function rpt_bundle_profile()
{
    $cols=6;
    $col_len = array(50,50,50,20,20,20);
    
    $pdf=new PDF();
	$pdf->AliasNbPages();
	$pdf->SetFont('Arial','',14);
	$pdf->AddPage();
	
	$logo_path = 'logo_outside_new.jpg';

  	# header
	$header=array('Host','Bundle','Last verified','Hours Ago', 'Avg interval', 'Uncertainty');

	# give host name TODO
        $ret = cfpr_report_bundlesseen_pdf(NULL,NULL,NULL); #cfpr_report_bundle_profile_pdf(NULL,NULL,NULL);

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

function rpt_value()
{
    $title = 'Business Value Report';
    $cols=5;
    $col_len = array(50,40,40,40,40);
    $header=array('Host','Day','Kept','Repaired', 'Not kept');
    $logo_path = 'logo_outside_new.jpg';
    
    $pdf=new PDF();
    $pdf->AliasNbPages();
    $pdf->SetFont('Arial','',14);
    $pdf->AddPage();

    # give host name TODO
    $ret = cfpr_report_value_pdf(NULL,NULL,NULL,NULL); #cfpr_report_bundle_profile_pdf(NULL,NULL,NULL);
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

function rpt_class_profile()
{
    $title = 'Class Profile';
    $cols=5;
    $col_len = array(60,60,40,25,25);
    $header=array('Host','Class Context','Occurs with probability','Uncertainty', 'Last seen');
    $logo_path = 'logo_outside_new.jpg';
    
    $pdf=new PDF();
    $pdf->AliasNbPages();
    $pdf->SetFont('Arial','',14);
    $pdf->AddPage();
    
    # give host name TODO
    $ret = cfpr_report_classes_pdf(NULL,NULL,NULL);
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
    function rpt_not_kept()
    {
	$pdf=new PDF();
	$pdf->AliasNbPages();
	$pdf->SetFont('Arial','',14);
	$pdf->AddPage();
        
	$logo_path = 'logo_outside_new.jpg';
		
	# cfpr function calls
	# 1. get header
	$header=array('Host','Promise Handle','Report','Time');

	$ret = cfpr_report_notkept_pdf(NULL,NULL);
	
	$data1 = $pdf->ParseData($ret);

	# count the number of columns	
	$cols = (count($data1,1)/count($data1,0))-1;
        
	$rptTableTitle = 'DATA REPORTED';
        $title = 'Promises Not Kept Log';
	$pdf->ReportTitle($title);
        $description = 'This report shows the list of promises Cfengine was not able to keep.';
	$pdf->ReportDescription($description);
	$pdf->RptTableTitle($rptTableTitle, $pdf->GetY() + 5);
	$pdf->Ln(8);
	
	# TODO: calculate the length of individual columns
	$col_len = array(40,40,90,40);
	$pdf->SetFont('Arial','',9);
	# DrawTableHeader(array, col_count, col_len, font_size)
#	$pdf->DrawTableHeader($header, 4, $col_len, 8);
	# DrawTable(array, col_count, col_len)
	$pdf->DrawTable($data1, 4, $col_len, $header, 8);
	
	$pdf->Output("Nova_promises_not_kept.pdf", "D");
    
    }

### Classes report ###

function rpt_compliance_promises()
{
    $title = 'Compliance by promise';
    $cols=6;
    $col_len = array(50,50,30,30,25,25);
    $header=array('Host','Promise Handle','Last known state','Probability kept', 'Uncertainty', 'Last seen');
    $logo_path = 'logo_outside_new.jpg';
    
    $pdf=new PDF();
    $pdf->AliasNbPages();
    $pdf->SetFont('Arial','',14);
    $pdf->AddPage();
    
    # give host name TODO
    $ret = cfpr_report_classes_pdf(NULL,NULL,NULL);
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

### Classes report ###

function rpt_filechanges()
{
    $title = 'File Change Log';
    $cols=3;
    $col_len = array(70,70,70);
    $header=array('Host','File', 'Time of Change');
    $logo_path = 'logo_outside_new.jpg';
    
    $pdf=new PDF();
    $pdf->AliasNbPages();
    $pdf->SetFont('Arial','',14);
    $pdf->AddPage();

    # give host name TODO
    $ret = cfpr_report_filechanges_pdf(NULL,NULL,NULL,NULL,NULL);
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

function rpt_lastseen()
{
    $title = 'Last saw hosts';
    $cols=9;
    $col_len = array(30,15,20,30,25,15,15,15,45);
    $header=array('Host','Initiated', 'IP Address', 'Remote Host', 'Last Seen', 'Hours Ago', ' Avg Interval', 'Uncertainty', 'Remote Host Key');
    $logo_path = 'logo_outside_new.jpg';
    
    $pdf=new PDF();
    $pdf->AliasNbPages();
    $pdf->SetFont('Arial','',10);
    $pdf->AddPage();

    # give host name TODO
    $ret =  cfpr_report_lastseen_pdf(NULL,NULL,NULL,NULL,NULL,NULL);
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


#rpt_bundle_profile();
#rpt_value();
#rpt_class_profile();
#rpt_compliance_promises();
#rpt_filechanges();
rpt_lastseen();
?>