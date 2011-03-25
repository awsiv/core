<?php
require('fpdf.php');
class cf_pdf extends FPDF
{
    var $left=5;
    var $right=5;
    var $top=10;
    var $pagewidth=210;
    var $reportname;
    var $tabletitle;
    var $description;
    var $max_col_height=2;

    #******************************
    function cf_pdf($orientation='p', $unit='mm', $format='A4')
    {
        parent::fpdf($orientation,$unit,$format);

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
	$logo_path = './images/'.'logo_outside_new.jpg';
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
	  if($i == 0) $align = 'L';
	  $width = ($w[$i] * ($this->pagewidth - $this->left - $this->right))/100;
	  $this->Cell($width,$h,$header[$i],1,0,$align,1,false);
	}
	$this->Ln();
    }

    #******************************
    # section
    function DrawTable($ar1, $cols, $col_len, $header, $header_font)
    {
	# $this->CustomHeader();
	$this->SetFont('Arial', '', $header_font);
	$this->DrawTableHeader($header, $cols, $col_len, $header_font);
        $font_size = 6;
	$this->SetDrawColor(125,125,125);
	$align=1;
 	$max_col_height = 2;
	$tmp_font = $font_size;
	$tmp_multi = -1;
	for($i=0; $i<count($ar1); $i++)
	{
	    $tmp_font = $font_size;

	    $multi_col = array();
	    $nb = 0;
	    for($j=0; $j<$cols; $j++)
	    {
		$f[$j] = $ar1[$i][$j];
		$this->SetFont('Arial', '', $tmp_font);
		$width = ($col_len[$j] * ($this->pagewidth - $this->left - $this->right))/100;
		$tmp = $this->NbLines($width,$f[$j]);

		if($tmp > $nb)
		{
		    $nb = $tmp;
		}
		if($tmp > $max_col_height)
		{
		  $j--;
		  $tmp_font--;
		  $nb = $max_col_height;
		}
		else if($tmp > 1)
		{
		  $multi_col[$j] = true;
		  $tmp_multi = $j;
		}
		else
		{
		  $multi_col[$j] = false;
		}
	    }

	    # total y
	    $hx = $nb * $tmp_font;

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
      		    $this->SetFont('Arial', '', $tmp_font);
		    if($j != $tmp_multi && $tmp_multi != -1)
		    	  $this->MultiCell($width,$hx,$f[$j],1,$align,0);
		    else if($j == $tmp_multi)
		    	 $this->MultiCell($width,$tmp_font,$f[$j],1,$align,0);
		}
		else
		{
		    $this->SetFont('Arial', '', $font_size);
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
?>
