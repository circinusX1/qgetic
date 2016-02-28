#ifndef __UNDO_H__
#define __UNDO_H__

#include "basecont.h"
#include "baseentity.h"

class Document;
class Brush;
class SceItem;
class CUndo
{
public:
    typedef enum _CLS_TYPE{
                    CLS_BRUSH = 0,
                    CLS_ITEM,
                 }CLS_TYPE;
    struct Record
    {
        BaseEntity       *p_Obj;
        BaseEntity       *p_Obj0; //inital 0
        CLS_TYPE        e_type;
        u_int32_t           e_action;  // action taken to taht changed the p_ObjOriginal
    };
    CUndo():_cursor(0){_fromHere=0;};
    virtual ~CUndo(){Clear();};
    void  Clear();
    void  Add(Brush* pB, u_int32_t action);
    void  Add(SceItem* pB, u_int32_t action);
    void  Undo(Document* pDoc, u_int32_t action);
    u_int32_t  Size(){return _records.size();}

private:
    void  _UpdatePtrs(BaseEntity* pB, BaseEntity* pBnew);
    void  _DelPtrs(BaseEntity* pB);
    void  _Resize();
    void  _ClearRecord(Record& record);
    void  _UndoItem(Document* pDoc, const Record& rec);
    void  _UndoBrush(Document* pDoc, const Record& rec);

    int32_t                 _cursor;
    vvector<Record>     _records;
    u_bool32                _fromHere;
};

#endif // __UNDO_H__
