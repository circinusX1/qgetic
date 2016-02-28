#include "progressdlg.h"


ProgressDlg GWaitDlg;

ProgressDlg::ProgressDlg():m_reffs(0)
{
    //GWaitDlg=this;
}

ProgressDlg::~ProgressDlg()
{
    if(m_reffs)
    {
        if(--m_reffs == 0 && _dlg)
        {
            _dlg->close();
            delete _dlg;
        }
    }
}

void	ProgressDlg::Show(int32_t action, const char* text, u_int32_t nDlgID)
{
  //  return;//
    if(action == -1 && _dlg)
    {
        _dlg->close();
        delete _dlg;
        _dlg=0;
        m_reffs = 0;
        return;
    }

    if(action == 1)
    {
        if(_dlg == 0)
        {
            _dlg = new QProgressDialog(QString(text), QString("Cancel"), 0, 100);
            _dlg->setModal(false);
            _dlg->show();
        }
        else
            action = 2;
        ++m_reffs;
    }
    if(0 == _dlg)
        return;

    if(action == 0)
    {
        if(m_reffs)
        {
            if(--m_reffs == 0 && _dlg)
            {
                _dlg->close();
                delete _dlg;
                _dlg=0;
            }
        }
    }

    if(action == 2 && _dlg) //only text comes
    {
        _dlg->setLabelText(QString(text));
        _dlg->repaint();
    }

}

/*
#define	START_SPLASH()		ProgressDlg::GWaitDlg.Show(1,"",DLG_SPLASH)
#define	START_WAIT(text_)	ProgressDlg::GWaitDlg.Show(1,text_)
#define	KEEP_WAIT(text_)	ProgressDlg::GWaitDlg.Show(2,text_)
#define	END_WAIT(text_)		ProgressDlg::GWaitDlg.Show(0,text_)
*/
