// z_ed2Doc.cpp : implementation of the Document class
//

#include "stdafx.h"
#include "document.h"
#include "geticapp.h"
#include "view2d.h"
#include "view3d.h"
#include <algorithm>
#include <bsptree.h>

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
// make csg
void Document::DoHSR()
{
    //SaveTempFile();

    _scene.ClearBrushes();
    if(theApp->isKeyDown(Qt::Key_Control)) // MERGES BRUSHES
    {


        Brush* pResultSceneBrsh = new Brush();

        pResultSceneBrsh->_brushflags=BRSH_SOLID;

        PBrushes* pBrushes =  _scene.GetPrimitives();
        if( 0 == pBrushes->size())
            return ;

        FOREACH(PBrushes, (*pBrushes), ppBrsh)
        {
            FOREACH(vvector<Poly>,(*ppBrsh)->_polys, pPoly)
            {
                pResultSceneBrsh->_polys << *pPoly;
            }
        }

        pResultSceneBrsh->Recalc();

        if(pResultSceneBrsh->_polys.size())
            _scene.AddBrush(pResultSceneBrsh);
        else
            delete pResultSceneBrsh;
    }
    else
    {
        u_int32_t dw = ge_gettick();
        _compiler.PerformHSR(_scene.GetPrimitives());
        dw = ge_gettick()-dw;
        SBT(0,MKSTR("%d polygons in %d ms", _compiler._polysStats, dw));
    }
    /*
    #pragma message("see thsi save infile later ????")
    if(_scene.GetBrushes()->size())
    {
        QString cs = GetTitle();
        cs+=".cg";
        SaveInFile(cs,true);
        END_WAIT("");
    }
    */
}


void Document::SaveFinalBSP()
{
    /*
    if(!_compiler._finalBSpFileName.isEmpty())
    {
        if(_compiler._exBspPlugin.length())
        {
            assert(0); //MFC to QT
            //_compiler.ExportBSP(_compiler._finalBSpFileName);
        }
        else
        if(_compiler.Save((const char*)_compiler._finalBSpFileName.toUtf8()))
        {
            if(_compiler._bExportFile)
            {
                _compil
er.Export((const char*)_compiler._finalBSpFileName.toUtf8());
            }
            LOG(0,"File: '%s' saved Succesfully",(const char*)_compiler._finalBSpFileName.toUtf8());
        }
        else
        {
            LOG(0,"Error:  saving '%s' ",(const char*)_compiler._finalBSpFileName.toUtf8());
        }

    }
    else
    {
        LOG(0,"Error:  saving '%s' ",(const char*)_compiler._finalBSpFileName.toUtf8());
    }
    */
}



