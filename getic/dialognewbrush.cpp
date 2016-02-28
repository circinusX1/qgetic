#include "dialognewbrush.h"
#include "ui_dialognewbrush.h"
#include "document.h"

DialogNewBrush::DialogNewBrush(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNewBrush),_hideZone(false)
{
    ui->setupUi(this);

}

void DialogNewBrush::showEvent(QShowEvent *pev)
{

    _brshSett._isSolid    = 1;
    _brshSett._cutAll     = 0;
    _brshSett._skyDom     = 0;
    _brshSett._splitter   = 0;
    _brshSett._zone       = 0;
    _brshSett._detached   = 0;
    _brshSett._zone       = 0;
    _brshSett._detail    = 0;
    _brshSett._reversed  = 0;

    DOC()->_cachedSett._detached = 0;
    DOC()->_cachedSett._zone = 0;

    if(_brshSett._boxdim.x<0)
        _brshSett._boxdim.x=1000;
    if(_brshSett._boxdim.y<0)
        _brshSett._boxdim.y=1000;
    if(_brshSett._boxdim.z<0)
        _brshSett._boxdim.z=1000;


    if(_brshSett._thikness)
    {
        ui->IDC_SPIN1->setValue(_brshSett._thikness/GMeters);
        ui->CK_SHELL->setChecked(true);
    }

    ui->EF_WIDTH->setText(QString::number(_brshSett._boxdim.x/GMeters));
    ui->EF_HEIGHT->setText(QString::number(_brshSett._boxdim.y/GMeters));
    ui->EF_DEPTH->setText(QString::number(_brshSett._boxdim.z/GMeters));
    QDialog::showEvent(pev);
}



DialogNewBrush::~DialogNewBrush()
{
    delete ui;
}

void DialogNewBrush::on_IDOK8_clicked()
{
    _brshSett._isSolid    = ui->RB_SOLID->isChecked();
    _brshSett._cutAll     = ui->RB_CUTALL->isChecked();
    _brshSett._skyDom     = ui->CK_SKYDOM->isChecked();
    _brshSett._splitter   = ui->CK_SPLITTER->isChecked();
    _brshSett._zone       = ui->CK_ZONE->isChecked();
    _brshSett._detached   = ui->CK_DETACHED->isChecked();


    if(ui->CK_SHELL->isChecked())
    {
        _brshSett._thikness = ui->IDC_SPIN1->value();
    }
    else
    {
        _brshSett._thikness=0;
    }

    _brshSett._detail    = ui->CK_SETDETAIL->isChecked();
    _brshSett._reversed  = ui->CK_REVERSE->isChecked();
    _brshSett._vstrips   = ui->IDC_SPIN2->value();
    _brshSett._radstrips = ui->IDC_SPIN3->value();

    if(ui->RB_LINEAR->isChecked())
    {
        _brshSett._tm = TM_LIN;
    }
    else if(ui->RB_CYLINDRIC->isChecked())
    {
        _brshSett._tm = TM_CYL;
    }
    else
        _brshSett._tm = TM_SPH;

    v3d_t prevVal = _brshSett._boxdim;

    _brshSett._boxdim.x = ParseREAL(*ui->EF_WIDTH);
    if(0 >= _brshSett._boxdim.x)
        _brshSett._boxdim.x = prevVal.x;

    _brshSett._boxdim.y = ParseREAL(*ui->EF_HEIGHT);
    if(0 >= _brshSett._boxdim.y)
        _brshSett._boxdim.y = prevVal.y;

    _brshSett._boxdim.z = ParseREAL(*ui->EF_DEPTH);
    if(0 >= _brshSett._boxdim.z)
        _brshSett._boxdim.z = prevVal.z;

    _brshSett._boxdim *= GMeters;

    if(!ui->CK_ZONE->isChecked())
        _brshSett._thikness=0;
    _brshSett._thikness *= GMeters;
    this->setResult(QDialog::Accepted);
    QDialog::accept();
    //this->close();
}


void DialogNewBrush::on_CK_SETDETAIL_clicked(bool checked)
{
    if(checked)
    {
        ui->RB_SOLID->setChecked(true);
    }
    else
    {
        ui->CK_DETACHED->setCheckState(Qt::Unchecked);
    }
    ui->CK_DETACHED->setEnabled(checked);
    ui->RB_CUT->setEnabled(!checked);
    ui->RB_CUTALL->setEnabled(!checked);
    ui->CK_SKYDOM->setEnabled(!checked);

}

void DialogNewBrush::on_CK_SHELL_clicked(bool checked)
{
    ui->IDC_SPIN1->setEnabled(checked);
    ui->IDC_SPIN2->setEnabled(checked);
}

void DialogNewBrush::on_IDCANCEL9_clicked()
{
    this->setResult(QDialog::Rejected);
    QDialog::reject();
    //this->close();
}


void DialogNewBrush::on_CK_SPLITTER_clicked(bool checked)
{
    ui->CK_ZONE->setCheckState(Qt::Unchecked);
    ui->CK_ZONE->setEnabled(!checked);
    ui->RB_CUT->setEnabled(!checked);
    ui->RB_CUTALL->setEnabled(!checked);
    ui->CK_SETDETAIL->setEnabled(!checked);

}

void DialogNewBrush::on_CK_ZONE_clicked(bool checked)
{

    ui->CK_SPLITTER->setCheckState(Qt::Unchecked);
    ui->CK_SPLITTER->setEnabled(!checked);
    ui->RB_CUT->setEnabled(!checked);
    ui->RB_CUTALL->setEnabled(!checked);
    ui->CK_SETDETAIL->setEnabled(!checked);
}

void DialogNewBrush::on_CK_SKYDOM_clicked(bool checked)
{
    if(checked)
    {
        ui->RB_SOLID->setChecked(true);
        ui->CK_SETDETAIL->setCheckState(Qt::Unchecked);
        on_CK_SETDETAIL_clicked(true) ;
    }
    ui->RB_CUT->setEnabled(!checked);
    ui->RB_CUTALL->setEnabled(!checked);
    ui->CK_SETDETAIL->setEnabled(!checked);
}
