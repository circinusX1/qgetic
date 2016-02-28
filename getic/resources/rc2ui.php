#!/usr/bin/php
<?php


class dialog
{
    public function __construct($id, $x, $y, $w, $h)
    {
        $this->_caption = strtolower($id);
        $this->_id = $id;
        $this->_x=$x;
        $this->_y=$y;
        $this->_w=$w;
        $this->_h=$h;
        $this->K=0;
    }
    var $_id;
    var $_x;
    var $_y;
    var $_w;
    var $_h;
    var $_caption;
    var $_pf;
    var $K;     

    function parse($pf)
    {
        if(strlen($this->_caption)==0)
        {
            echo "<!--Error no caption -->\n";
            return;
        }
        $uifname = strtolower($this->_id).".ui";
        $this->_pf = fopen($uifname,"wb");
        if($this->_pf!=null)
        {
            $this->start_ui();
            while(!feof($pf))
            {
                $line = fgets($pf);
                $line = str_replace("\"\"","\"X\"", $line);
                $line = str_replace("\n","", $line);
                $line = str_replace("\r","", $line);
                $line = str_replace("\t"," ", $line);                
                $line = str_replace(","," ", $line);
                $line = str_replace("&","&amp;", $line);
                $line = str_replace("\"","", $line);
                $line = preg_replace("/\s{2,}/"," ",$line);
                if($line[0]== ' ')
                    $line = substr($line,1);
                echo $line."\n";
                if(strstr($line,"END") || feof($pf))
                    break;

                //echo "<!--{$line}-->";
                if(strlen($line)==0 || !strchr($line, " "))
                    continue;
                
                $prts = explode(" ", $line);
                print_r($prts);
                $cords=0;
                for($cords=0; $cords < count($prts); $cords++)
                {
                    if(is_numeric($prts[$cords]))
                    {
                        break;
                    }
                }

                $x = intval($prts[$cords]*2.5);
                $y = intval($prts[$cords+1]*2.5);
                $w = intval($prts[$cords+2]*2.5);
                $h = intval($prts[$cords+3]*2.5);

                if(strstr($line,"EDITTEXT"))
                {
                    //fwrite($this->_pf, "    <!--         EDITTEXT                    -->\n");
                    fwrite($this->_pf, "    <widget class='QLineEdit' name='{$prts[1]}'>\n");
                     $this->rect($x, $y, $w, $h);
                    fwrite($this->_pf, "      </widget>\n");
                    
                }
                else if(strstr($line,"CONTROL"))
                {
                   //fwrite($this->_pf, "    <!--        CONTROL             -->\n");

                    if(strstr($line, "CHECKBOX"))
                    {
                        $this->w("    <widget class='QCheckBox' name='{$prts[2]}'>\n");
                         $this->rect($x, $y, $w, $h);
                        $this->w("     <property name='text'>\n");
                        $this->w("        <string>{$prts[1]}</string>\n");
                        $this->w("       </property>\n");
                        $this->w("      </widget>\n");

                    }
                    else if(strstr($line, "SysTree"))
                    {
                        $this->w("     <widget class='QTreeWidget' name='{$prts[2]}'>\n");
                        $this->rect($x, $y, $w, $h);
                        $this->w("     </widget>   \n");
                    }    
                    else if(strstr($line, "SysList"))
                    {
                        $this->w("     <widget class='QListWidget' name='{$prts[2]}'>\n");
                        $this->rect($x, $y, $w, $h);
                        $this->w("       </widget>\n");
                    }               
                    else if(strstr($line, "OWNERDRAW"))
                    {
                        $this->w("     <widget class='QPushButton' name='{$prts[2]}'>\n");
                        $this->rect($x, $y, $w, $h);
                        $this->w("        <property name='text'>\n");
                        $this->w("         <string>{$prts[1]}</string>\n");
                        $this->w("        </property>\n");
                        $this->w("       </widget>\n");
                    }
                    else if(strstr($line, "msctls_updown32"))
                    {
                        $this->w("      <widget class='QSpinBox' name='{$prts[2]}'>\n");
                        $this->rect($x, $y, $w, $h);
                        // $this->w("     <property name='text'>\n");
                        // $this->w("        <string>{$prts[1]}</string>\n");
                        // $this->w("     </property>\n");
                        $this->w("      </widget>\n");
                        
                    }
                    else if(strstr($line, "RADIOBUTTON"))
                    {
                        $this->w("    <widget class='QRadioButton' name='{$prts[2]}'>\n");
                        $this->rect($x, $y, $w, $h);
                         $this->w("     <property name='text'>\n");
                        $this->w("        <string>{$prts[1]}</string>\n");
                        $this->w("       </property>\n");
                        $this->w("      </widget>\n");
                    }
                }
                else if(strstr($line,"PUSHBUTTON"))
                {
                $name = $prts[2];
                if($name=="IDOK"||
                   $name=="IDNO"||
                   $name=="IDCANCEL"||
                   $name=="IDYES")
                        $name.=$this->K++;

                        $this->w("     <widget class='QPushButton' name='{$name}'>\n");
                        $this->rect($x, $y, $w, $h);
                        $this->w("        <property name='text'>\n");
                        $this->w("         <string>{$prts[1]}</string>\n");
                        $this->w("        </property>\n");
                        $this->w("       </widget>\n");
                }
                else if(strstr($line,"GROUPBOX"))
                {
                    $name = $prts[2];
                    if($name=="IDC_STATIC"||
                       $name=="IDC_YES"||
                       $name=="IDC_NO")
                        $name.=$this->K++;  
                    $name.=$this->K++;
                    $this->w("      <widget class='QGroupBox' name='{$name}'>\n");
                    $this->rect($x, $y, $w, $h);
                    $this->w("        <property name='title'>\n");
                    $this->w("         <string>{$prts[1]}</string>\n");
                    $this->w("        </property>\n");
                    $this->w("        <property name='flat'>\n");
                    $this->w("         <u_bool32>true</u_bool32>\n");
                    $this->w("        </property>\n");
                    $this->w("      </widget>\n");
                }
                else if(strstr($line,"LTEXT"))
                {
                    $name = $prts[2];
                    if($name=="IDC_STATIC")
                        $name.=$this->K++;  
                    $this->w("      <widget class='QLabel' name='{$name}'>\n");
                     $this->rect($x, $y, $w, $h);
                     $this->w("        <property name='text'>\n");
                    $this->w("          <string>{$prts[1]}</string>\n");
                    $this->w("         </property>\n");
                    $this->w("      </widget>\n");
                }
                else if(strstr($line,"COMBOBOX"))
                {
                    $this->w("      <widget class='QComboBox' name='{$prts[1]}'>\n");
                    $this->rect($x, $y, $w, $h);
                    $this->w("      </widget>\n");
                }
                else if(strstr($line,"DEFPUSHBUTTON"))
                {
                $name = $prts[2];
                if($name=="IDOK"||
                   $name=="IDNO"||
                   $name=="IDCANCEL"||
                   $name=="IDYES")
                        $name.=$this->K++;
                       $this->w("     <widget class='QPushButton' name='{$name}'>\n");
                       $this->rect($x, $y, $w, $h);
                        $this->w("       <property name='text'>\n");
                       $this->w("         <string>{$prts[1]}</string>\n");
                       $this->w("        </property>\n");
                       $this->w("     </widget>\n");
                }
                else
                {
                       $this->w("<!-- UNKNOWN_CONTROLE  {$line} -->\n");
                    
                }
            }
            $this->close_ui();
            fclose($this->_pf);
        }            
    }

    function set_caption($c)
    {
        $this->_caption=$c;
    }

    function start_ui()
    {
        fwrite($this->_pf, "<?xml version='1.0' encoding='UTF-8'?> \n");
        fwrite($this->_pf,  "<ui version='4.0'>\n");
        fwrite($this->_pf,  " <class>Dialog</class>\n");
        fwrite($this->_pf,  " <widget class='QDialog' name='Dialog'>\n");
        
        $this->rect($this->_x, $this->_y, $this->_w, $this->_h);

        fwrite($this->_pf,  "  <property name='{$this->_caption}'>\n");
        fwrite($this->_pf,  "   <string>Dialog</string>\n");
        fwrite($this->_pf,  "  </property>\n");
    }

    function close_ui()
    {
        $this->w("      </widget>\n");
        $this->w("   <resources/>\n");
        $this->w(" <connections/>\n");
        $this->w("</ui> \n");
    }
    

    function w($txt)
    {
        fwrite($this->_pf,  $txt);
    }

    function rect($x, $y, $w, $h)
    {
        $this->w("      <property name='geometry'>\n");
        $this->w("        <rect>\n");
        $this->w("         <x>{$x}</x>\n");
        $this->w("         <y>{$y}</y>\n");
        $this->w("         <width>{$w}</width>\n");
        $this->w("         <height>{$h}</height>\n");
        $this->w("        </rect>\n");
        $this->w("      </property>\n");
    }


};


function proc_dialog($line, &$pf)
{
    $line= str_replace(",","", $line);
    $parts = explode(" ",$line);
    //print_r($parts);
    if(count($parts)>=6)
    {
        $d = new dialog($parts[0], 
                           intval($parts[2]),
                           intval($parts[3]),
                           intval($parts[4]),
                           intval($parts[5]));
        do{

            $line = fgets($pf);
            if(strstr($line,"CAPTION") && strstr($line,"\""))
            {
                $line = str_replace("\"","", $line);
                $line = str_replace("\n","", $line);
                $line = str_replace("\r","", $line);
                $line = substr($line, 8);
                $d->set_caption($line);
                
            }
        }while(!feof($pf) && 
               !strstr($line,"BEGIN"));

        $d->parse($pf);
    }     

}

class menu
{
    public function __construct($id, $text)
    {
        $this->_id = $id;
        $this->_text = $text;
        $this->_pf = fopen($text,"wb");

        fwrite($this->_pf,"<widget class='QMenuBar' name='menuBar'>\n");
        fwrite($this->_pf,"   <property name='geometry'>\n");
        fwrite($this->_pf,"    <rect>\n");
        fwrite($this->_pf,"     <x>0</x>\n");
        fwrite($this->_pf,"     <y>0</y>\n");
        fwrite($this->_pf,"     <width>984</width>\n");
        fwrite($this->_pf,"     <height>25</height>\n");
        fwrite($this->_pf,"    </rect>\n");
        fwrite($this->_pf,"   </property>\n");
        $this->_depth=0;
    }

    var $_id;
    var $_text;
    var $_pf;
    var $_depth;

    function parse(&$pf)
    {
        $popname="";

        while(!feof($pf))
        {
            $line = fgets($pf);
            $line = str_replace("\n","", $line);
            $line = str_replace("\r","", $line);
            $line = str_replace("\t"," ", $line);
            $line = str_replace(","," ", $line);
            $line = str_replace("&","&amp;", $line);
            $line = preg_replace("/\s{2,}/"," ",$line);
            $parts = explode ("\"",$line);

            echo "=========[".$line."]\n";
            print_r($parts);

            if(strstr($line,"MENUITEM"))
            {
                fwrite($this->_pf,"    <widget class='QMenu' name='{$parts[2]}'>\n");
                fwrite($this->_pf,"     <property name='title'>\n");
                fwrite($this->_pf,"      <string>{$parts[1]}</string>\n");
                fwrite($this->_pf,"     </property>\n");
                fwrite($this->_pf,"    </widget>\n");

            }
            else if(strstr($line,"POPUP"))
            {
                $popname=$parts[1];
                continue;
            }

            else if(strstr($line,"BEGIN"))
            {
                if(strlen($popname)==0)
                    $popname="NO_NAME";
                $idpop = str_replace("&amp;","",$popname);
                $idpop = substr($idpop,0,5);
                $idpop = strtolower($idpop);

                fwrite($this->_pf,"    <widget class='QMenu' name='menu_{$idpop}'>\n");
                fwrite($this->_pf,"         <property name='title'>\n");
                fwrite($this->_pf,"          <string>{$popname}</string>\n");
                fwrite($this->_pf,"         </property>\n");
                $popname="";
                $this->_depth++;
            }
            else if(strstr($line,"END"))
            {
                fwrite($this->_pf,"    </widget>\n");
                $this->_depth--;
            }
        }
        fclose($this->_pf);
    }
}

function proc_menu($line, &$pf)
{

    $line = str_replace("\n","", $line);
    $line = str_replace("\r","", $line);
    $line = str_replace("\t"," ", $line);
    $line = str_replace("&","&amp;", $line);
    $line = preg_replace("/\s{2,}/"," ",$line);



    $parts = explode(" ", $line);
    $m = new menu($parts[0],"MAIN_MENU");
    $m->parse($pf);



}



function process($line, &$pf)
{

    if(strstr($line, "DIALOGEX"))
    {
        return proc_dialog($line, $pf);
    }


    if(strstr($line, "IDR_MAINFRAME") && strstr($line, "MENU"))
    {

        return proc_menu($line, $pf);
    }
}


if(!isset($argv[1]))
{
    echo "\n<!--specify the rc file-->";
    exit();
}
    
$rcfile = fopen($argv[1],"rb");
if($rcfile)
{
    system("rm *.ui");

    while(!feof($rcfile))
    {
        $line = fgets($rcfile);
        if(feof($rcfile))
            break;
        process($line, $rcfile);
    }


    fclose($rcfile);
}    





?>
