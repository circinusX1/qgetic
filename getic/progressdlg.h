#ifndef PROGRESSDLG_H
#define PROGRESSDLG_H

#include <baselib.h>
#include <QProgressDialog>

class ProgressDlg
{
public:
    ProgressDlg();
    ~ProgressDlg();
    void	Show(int32_t action, const char* text, u_int32_t nDlgID = 0);
private:

    QProgressDialog *_dlg;
    int32_t             m_reffs;

public:

};

extern ProgressDlg GWaitDlg;

#define	START_SPLASH()     GWaitDlg.Sh4ow(1,"",DLG_SPLASH)
#define	START_WAIT(text_)  	GWaitDlg.Show(1,text_)
#define	KEEP_WAIT(text_)   GWaitDlg.Show(2,text_)
#define	END_WAIT(text_)    	GWaitDlg.Show(0,text_)

#endif // PROGRESSDLG_H
