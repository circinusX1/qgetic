#!/usr/bin/php
<?php


class dialog
{
    public function __construct($id, $x, $y, $w, $h)
    {
        $this->_id = $id;
        $this->_x=$x;
        $this->_y=$y;
        $this->_w=$w;
        $this->_h=$h;
    }
    var $_id;
    var $_x;
    var $_y;
    var $_w;
    var $_h;
    var $_caption;
    var $_fn;

    function parse($pf)
    {
        if(strlen($this->_capt)==0)
        {
            echo "<!--Error no caption -->";
            return;
        }
        $uifname = strtolower($this->_id).".ui";
        $this->fn = fopen($uifname,"wb");
        if($this->fn)
        {
            start_ui();
            while(!feof($pf))
            {
                $line = fgets($pf);
                $line = str_replace(","," ", $line);
                $line = str_replace("\"","", $line);

                if(strstr($line,"END") || feof($pf))
                    break;
                echo "<!--{$line}-->";
                $prts = explode(" ", $line);
                $cords=0;
                for($cords=0;$cords<count($prts);$cords++)
                {
                    if(is_numeric($prts[$coords]))
                    {
                        break;
                    }
                }
                if(strstr($line,"EDITTEXT"))
                {
                    fwrite($this->$_fn, "    <!--         EDITTEXT                    -->\n");
                    fwrite($this->$_fn, "    <widget class='QLineEdit' name='{$prts[1]}'>\n");
                    fwrite($this->$_fn, "       <property name='geometry'>\n");
                    $this->rect($prts[$cords], $prts[$cords+1], $prts[$cords+2], $prts[$cords+3]);
                    fwrite($this->$_fn, "       </property>\n");
                    fwrite($this->$_fn, "      </widget>\n");
                    
                }
                else if(strstr($line,"CONTROL"))
                {
                    fwrite($this->$_fn, "    <!--        CONTROL             -->\n");

                    if(strstr($line, "CHECKBOX"))
                    {
                        $this->w("    <widget class='QCheckBox' name='{$prts[2]}'>\n");
                        $this->w("       <property name='geometry'>\n");
                        $this->rect($prts[$cords], $prts[$cords+1], $prts[$cords+2], $prts[$cords+3]);\n");
                        $this->w("       </property>\n");
                        $this->w("     <property name='text'>\n");
                        $this->w("        <string>{$prts[1]}</string>\n");
                        $this->w("       </property>\n");
                        $this->w("      </widget>\n");

                    }
                    else if(strstr($line, "SysTree"))
                    {
                        $this->w("     <widget class='QTreeWidget' name='{$prts[2]}'>\n");
                        $this->w("       <property name='geometry'>\n");
                        $this->rect($prts[$cords], $prts[$cords+1], $prts[$cords+2], $prts[$cords+3]);\n");
                        $this->w("       </property>\n");
                        $this->w("     </widget>   \n");
                    }    
                    else if(strstr($line, "SysList"))
                    {
                        $this->w("     <widget class='QListWidget' name='{$prts[2]}'>\n");
                        $this->w("        <property name='geometry'>\n");
                        $this->rect($prts[$cords], $prts[$cords+1], $prts[$cords+2], $prts[$cords+3]);\n");
                        $this->w("        </property>\n");
                        $this->w("       </widget>\n");
                    }               
                    else if(strstr($line, "OWNERDRAW"))
                    {
                        $this->w("     <widget class='QPushButton' name='{$prts[2]}'>\n");
                        $this->w("       <property name='geometry'>\n");
                        $this->rect($prts[$cords], $prts[$cords+1], $prts[$cords+2], $prts[$cords+3]);\n");
                        $this->w("        </property>\n");
                        $this->w("        <property name='text'>\n");
                        $this->w("         <string>{$prts[1]}</string>\n");
                        $this->w("        </property>\n");
                        $this->w("       </widget>\n");
                    }
                    else if(strstr($line, "msctls_updown32"))
                    {
                        $this->w("      <widget class='QSpinBox' name='{$prts[2]}'>\n");
                        $this->rect($prts[$cords], $prts[$cords+1], $prts[$cords+2], $prts[$cords+3]);
                        // $this->w("     <property name='text'>\n");
                        // $this->w("        <string>{$prts[1]}</string>\n");
                        // $this->w("       </property>\n");
                        $this->w("      </widget>\n");
                        
                    }
                    else if(strstr($line, "RADIOBUTTON"))
                    {
                        $this->w("    <widget class='QRadioButton' name='{$prts[2]}'>\n");
                        $this->w("       <property name='geometry'>\n");
                        $this->rect($prts[$cords], $prts[$cords+1], $prts[$cords+2], $prts[$cords+3]);\n");
                        $this->w("       </property>\n");
                        $this->w("     <property name='text'>\n");
                        $this->w("        <string>{$prts[1]}</string>\n");
                        $this->w("       </property>\n");
                        $this->w("      </widget>\n");
                    }
                }
                else if(strstr($line,"PUSHBUTTON"))
                {
                        $this->w("     <widget class='QPushButton' name='{$prts[2]}'>\n");
                        $this->rect($prts[$cords], $prts[$cords+1], $prts[$cords+2], $prts[$cords+3]);\n");
                        $this->w("        </property>\n");
                        $this->w("        <property name='text'>\n");
                        $this->w("         <string>{$prts[1]}</string>\n");
                        $this->w("        </property>\n");
                        $this->w("       </widget>\n");
                }
                else if(strstr($line,"GROUPBOX"))
                {
                    $this->w("      <widget class='QGroupBox' name='{$prts[2]}'>\n");
                    $this->w("       <property name='geometry'>\n");
                    $this->rect($prts[$cords], $prts[$cords+1], $prts[$cords+2], $prts[$cords+3]);\n");
                    $this->w("        </property>\n");
                    $this->w("        <property name='title'>\n");
                    $this->w("         <string>{$prts[1]}</string>\n");
                    $this->w("        </property>\n");
                    $this->w("        <property name='flat'>\n");
                    $this->w("         <bool>true</bool>\n");
                    $this->w("        </property>\n");
                    $this->w("       </widget>\n");
                }
                else if(strstr($line,"LTEXT"))
                {
                    $this->w("      <widget class='QLabel' name='{$prts[2]}'>\n");
                    $this->w("         <property name='geometry'>\n");
                    $this->rect($prts[$cords], $prts[$cords+1], $prts[$cords+2], $prts[$cords+3]);\n");\n");
                    $this->w("         </property>\n");
                    $this->w("         <property name='text'>\n");
                    $this->w("          <string>{$prts[1]}</string>\n");
                    $this->w("         </property>\n");
                    $this->w("        </widget>\n");
                }
                else if(strstr($line,"COMBOBOX"))
                {
                    $this->w("      <widget class='QComboBox' name='{$prts[1]}'>\n");
                    $this->w("         <property name='geometry'>\n");
                    $this->rect($prts[$cords], $prts[$cords+1], $prts[$cords+2], $prts[$cords+3]);\n");\n");
                    $this->w("         </property>\n");
                    $this->w("        </widget>\n");
                }
                else if(strstr($line,"DEFPUSHBUTTON"))
                {
                       $this->w("     <widget class='QPushButton' name='{$prts[2]}'>\n");
                       $this->rect($prts[$cords], $prts[$cords+1], $prts[$cords+2], $prts[$cords+3]);\n");
                       $this->w("        </property>\n");
                       $this->w("        <property name='text'>\n");
                       $this->w("         <string>{$prts[1]}</string>\n");
                       $this->w("        </property>\n");
                       $this->w("       </widget>\n");
                }
                else
                {
                       $this->w("<!-- UNKNOWN_CONTROLE  {$line} -->\n");
                    
                }
            }
            
            fclose($this->_fn);
        }            
    }

    function set_caption($c)
    {
        $this->_caption=$c;
    }

    function start_ui()
    {
        fwrite($this->_fn, "\<?xml version='1.0' encoding='UTF-8'?\> \n");
        fwrite($this->_fn,  "<ui version='4.0'>\n");
        fwrite($this->_fn,  " <class>Dialog</class>\n");
        fwrite($this->_fn,  " <widget class='QDialog' name='Dialog'>\n");
        fwrite($this->_fn,  "  <property name='geometry'>\n");
        $this->rect($this->x, $this->y, $this->w, $this->h)
        fwrite($this->_fn,  "  </property>\n");
        fwrite($this->_fn,  "  <property name='{$this->_caption}'>\n");
        fwrite($this->_fn,  "   <string>Dialog</string>\n");
        fwrite($this->_fn,  "  </property>\n");
    }

    function w($txt)
    {
        fwrite($this->_fn,  $txt);
    }

    function rect($x $y $w $h)
    {
        $this->w("        <rect>\n");
        $this->w("         <x>{$x}</x>\n");
        $this->w("         <y>{$y}</y>\n");
        $this->w("         <width>{$w}</width>\n");
        $this->w("         <height>{$h}</height>\n");
        $this->w("        </rect>\n");
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
            if(strstr($line,"CAPTION"))
            {
                $prts = explode(" ", $line); 
                $capt = str_replace("\"","",$prts[1]);               
                $d->set_caption($capt);
            }
        }while(!feof($pf) && 
               !strstr($line,"BEGIN"));
        $d->parse($pf);
    }     

}


function process($line, &$pf)
{
    if(strstr($line, "DIALOGEX"))
    {
        return proc_dialog($line, $pf);
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
