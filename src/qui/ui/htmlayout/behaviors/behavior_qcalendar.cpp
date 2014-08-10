#include "behavior_aux.h"
#include "../htmlayout_canvas.hpp"

#include <math.h>
#include <time.h>


namespace htmlayout
{
    namespace dom
    {
        struct qcalendar : public behavior
        {
            struct _CalCell : public htmlayout::dom::ETable
            {
            public:
                _CalCell(HELEMENT h) : ETable(h)
                {

                }

                ECtrl _Day()
                {
                    return find_first(".day");
                }

                ECtrl _LunarDay()
                {
                    return find_first(".lunarday");
                }

                // �Զ��������Td
                ECtrl _FirstC()
                {
                    return find_first(".first");
                }

                ECtrl _SecondC()
                {
                    return find_first(".second");
                }

                BOOL SetDateTime(__in QTime & dtTime, __in BOOL bCurMonth)
                {
                    CHolidayLookup *pHoliday = CHolidayLookup::GetInstance();
                    if (bCurMonth)
                    {
                        // �ڱ�����
                        RemoveClass(L"other-month");
                    }
                    else
                    { 
                        // ���ڱ�����
                        AddClass(L"other-month");
                    }

                    // �Ƿ��������죿
                    if ((dtTime.GetDayOfWeek() == 1) || (dtTime.GetDayOfWeek() == 7))
                    {
                        AddClass(L"off");
                    }
                    else
                    {
                        RemoveClass(L"off");
                    }

                    // �Ƿ��ǽ��죿
                    if (dtTime.CompareDate(QTime::GetCurrentTime()) == 0)
                    {
                        AddClass(L"today");
                    }
                    else
                    {
                        RemoveClass(L"today");
                    }

                    QString sLunarDay = CLunarDateTime(dtTime).GetLunarDayName();
                    if(pHoliday->IsSolarHoliday(dtTime))
                    {
                        // �ǽڼ���, ���ýڼ��ձ���ͼƬ
                        AddClass(L"holiday");

                        sLunarDay = pHoliday->GetSolarHoliday(dtTime);
                    }

                    // day
                    _Day().SetText(L"%d", dtTime.GetDay());
                    // lunarday
                    _LunarDay().SetText( sLunarDay );

                    return TRUE;
                }
            };

            static const int MIN_YEAR = 1901;
            static const int MAX_YEAR = 2050;

            qcalendar() 
                : behavior(HANDLE_BEHAVIOR_EVENT|HANDLE_METHOD_CALL, "qcalendar")
            {

            }

            // �����һ��ȫ�������µģ��Ƿ����ص���һ�У�
            bool is_hide_othermonthweek(ECtrl& el)
            {
                return el.get_attribute("hide_othermonthweek") != NULL;
            }

            virtual void attached  (HELEMENT he ) 
            {
                // ����һ���� // 7 * 7
                // ��һ�� ��ͷ
                //                 <widget type="qcalendar">
                //                     <caption></caption>
                //                     <table .cal>
                //                         <tr><th>....</th></tr>
                //                         <tr>...</tr>
                //                     </table>
                //                 </widget>

                ECtrl el(he);
                // ������caption
                ECtrl cap = el.create("caption");
                el.append(cap);
                QTime tm;
                init_caption(cap, tm);

                // �����ɱ��� 7 * 9
                // �����к�������Ϊ�Զ������
                ETable cal = el.create("table");
                el.append(cal);
                cal.SetClass(L"body");
                init_body(cal, tm);
            } 

            // ��ʼ��caption
            void init_caption(__inout ECtrl& cap, __out QTime& tm)
            {
                // ��ǰʱ��
                tm = QTime::GetCurrentTime();

                // caption ��ʽ��
                // [����] <   2010 - 09   >
                // ����
                ECtrl bn_today = cap.create("div");
                cap.append(bn_today);
                bn_today.SetClass(L"qbtn to-today");

                // ǰ��
                ECtrl bn_l = cap.create("div");
                cap.append(bn_l);
                bn_l.SetClass(L"qbtn prev-date");

                // ��
                ECtrl cT = cap.create("widget");
                cT.set_attribute("type",L"select-dropdown");
                cap.append(cT);
                cT.SetClass(L"year");

                // ��ʼ����1901 ~ 2050
                ECombobox cmb = cT;
                QString s;
                for (int i = MIN_YEAR; i <= MAX_YEAR; i++) 
                {
                    s.Format(L"%d", i);
                    cmb.InsertItem(s);
                }
                s.Format(L"%d", tm.GetYear());
                cmb.SelectItem_Text(s);

                // ��
                cT = cap.create("select");
                cT.set_attribute("type",L"select-dropdown");
                cap.append(cT);
                cT.SetClass(L"month");

                // ��ʼ����1 - 12
                cmb = (HELEMENT)cT;
                for (int i = 1; i <= 12; i++) 
                {
                    s.Format(L"%02d", i);
                    cmb.InsertItem(s);
                }
                s.Format(L"%02d", tm.GetMonth());
                cmb.SelectItem_Text(s);

                // ��һ��
                ECtrl bn_r = cap.create("div");
                cap.append(bn_r);
                bn_r.SetClass(L"qbtn next-date");

                // ��ʾ������
                ECtrl bn_d = cap.create("div", L"��ʾ������");
                cap.append(bn_d);
                bn_d.SetClass(L"qbtn to-desktop");
                bn_d.SetID(L"btn_to_desktop");
            }

            // ��ʼ��������
            bool init_body(__inout ETable& cal, __in QTime& tm)
            {
                // ����һ�����Ĵ�����Ԫ�أ���Ҫ�������е�html��һ��������
                QString shtm = L"<tr>";
                // ��һ�б�ͷ Ϊ��
                //                 for (int i = 0; i < 9; ++i) 
                //                 {
                //                     shtm += L"<th .weekday />";
                //                 }
                shtm += L"<th .weekday />";
                shtm += L"<th .weekday>��</th>";
                shtm += L"<th .weekday>һ</th>";
                shtm += L"<th .weekday>��</th>";
                shtm += L"<th .weekday>��</th>";
                shtm += L"<th .weekday>��</th>";
                shtm += L"<th .weekday>��</th>";
                shtm += L"<th .weekday>��</th>";
                shtm += L"<th .weekday />";
                shtm += L"</tr>";

                // ������6��Ϊ���壬���˵�һ�к����һ�У�
                // �����ÿ��<td>�ڶ���һ��<table>
                QString s;
                for (int i = 0; i < 6; ++i) 
                {
                    shtm += L"<tr>";
                    for (int j = 0; j < 9; ++j) 
                    {
                        if ((j == 0) || (j == 8))
                        {
                            shtm += L"<td .nweek/>";
                        }
                        else
                        {
                            shtm += L"<td>"
                                L"<table .cell>"
                                L"  <tr>"
                                L"      <td .first />"  // �Զ���cell-1
                                L"      <td .day />"    // ����day
                                L"      <td .second />" // �Զ���cell-1
                                L"  </tr>"
                                L"  <tr><td .lunarday colspan=3 /></tr>" // ũ������
                                L"</table>"
                                L"</td>";
                        }
                    }
                    shtm += L"</tr>";
                }

                // ����֮
                cal.SetHtml(shtm);

                // �ı�ʱ��
                return change_date(ECtrl(cal.parent()), tm.GetYear(), tm.GetMonth());
            }

            // helper
            inline ECombobox _CmbYear(ECtrl& el)
            {
                return el.find_first("caption>.year");
            }

            inline ECombobox _CmbMonth(ECtrl& el)
            {
                return el.find_first("caption>.month");
            }

            // ���ڸı� el Ϊ widget wrapper
            void on_date_changed(ECtrl& el)
            {
                int y = StrToInt(_CmbYear(el).GetCurItemText());
                int m = StrToInt(_CmbMonth(el).GetCurItemText());

                change_date(el, y, m );
            }

            // ��ǰ���ť�ı��£����ڸı� el Ϊ widget wrapper
            void change_month(__in ECtrl& el, __in BOOL bGotoPrev)
            {
                ECombobox cmb_y = _CmbYear(el);
                ECombobox cmb_m = _CmbMonth(el);

                int nYear = StrToInt(cmb_y.GetCurItemText());
                int nMonth = StrToInt(cmb_m.GetCurItemText());

                if (bGotoPrev) // ����ǰ
                {
                    nMonth -=1;
                    if(nMonth<1)
                    {
                        nMonth=12;
                        nYear -=1;
                    }
                }
                else // �����
                {
                    nMonth +=1;
                    if(nMonth>12)
                    {
                        nMonth=1;
                        nYear +=1;
                    }
                }

                // ���½���
                if ( (nYear <= MAX_YEAR) && (nYear >= MIN_YEAR))
                {
                    QString s;
                    s.Format(L"%d", nYear);
                    cmb_y.SelectItem_Text(s);

                    s.Format(L"%02d", nMonth);
                    cmb_m.SelectItem_Text(s);

                    // ���ڸı���
                    on_date_changed(el);
                }
            }

            // �ı����ڵ�ָ��������£�nYearӦ���� [MIN_YEAR, MAX_YEAR]��Χ��
            bool change_date(ECtrl&el , int nYear,int nMonth) 
            {
                if (   (nYear > MAX_YEAR) || (nYear < MIN_YEAR)
                    || (nMonth < 1) || (nMonth > 12))
                {
                    ASSERT(FALSE);
                    return false;
                }

                // �����ꡢ�������б���ʾ
                QString s;
                s.Format(L"%d",nYear);
                _CmbYear(el).SelectItem_Text(s);
                s.Format(L"%02d", nMonth);
                _CmbMonth(el).SelectItem_Text(s);

                // ��������
                ETable body = el.find_first(L":root>table.body");

                int nApartDays;
                int nWeek = WeekDay(nYear,nMonth,1);
                int nMonthday = MonthDays(nYear,nMonth);
                int nFirstDay = (nWeek == 0) ? 7 : nWeek;

                ECtrl rowWeek, tdWeek;
                QTime dtFirstDay(nYear, nMonth, 1, 0 , 0 , 0);
                QTime dtDay;
                bool bHideOtherWeek = is_hide_othermonthweek(el);

                // ȥ���Ǹ���ǰ��check
                ECtrl cCur = el.find_first("table:checked");
                if (cCur.is_valid())
                {
                    cCur.SetCheck(FALSE);
                }

                // ������Ҫ���õ�day-cellֻ�� 6 * 7 ��
                // 6 - ��6�У�һ�����������Կ�Խ6����
                // 7 - һ��7��
                for (int i = 1, day = 0; i <= 6; ++i)  // �� 6 ��
                {   
                    // �����е�DOM
                    rowWeek = body.child(i);
                    // ��һ����ʾ ��xx �ܡ�
                    tdWeek = rowWeek.child(0);

                    for (int j = 1; j <= 7; ++j)  // һ��һ�ܣ��� 7 ��
                    {
                        nApartDays = day++ - nFirstDay;
                        dtDay = dtFirstDay + QTimeSpan(nApartDays, 0, 0, 0);

                        // row + 1, col + 1, ��Ϊ��һ��Ϊ�ܣ���һ��Ϊ�Զ�����
                        // _CalCell dayCell = _DayCell(body, j / 7 + 1, j % 7 + 1);
                        _CalCell dayCell = _DayCell(body, i, j);
                        // �Ƿ��ڱ�����
                        //BOOL bCurMonth = ((nApartDays >= 0) && (nApartDays < nMonthday));
                        BOOL bCurMonth = ((nApartDays >= 0) && (nApartDays < nMonthday));
                        if (1 == j)
                        {
                            // ��ʾ��
                            tdWeek.SetHtml(dtDay.Format(L"%W") + L"<br/>��");
                            if ((6 == i) && bHideOtherWeek)
                            {
                                // ������һ�еĵ�һ�춼�ڱ���·��ڡ���һ�п��Բ���ʾ��
                                rowWeek.ShowCtrl(bCurMonth ? SHOW_MODE_SHOW : SHOW_MODE_COLLAPSE);
                            }
                        }
                        else if(bHideOtherWeek && (7 == j) && (1 == i))
                        {
                            // �����һ�е����һ�춼�ڱ�����ڣ���һ�п��Բ���ʾ��
                            rowWeek.ShowCtrl(bCurMonth ? SHOW_MODE_SHOW : SHOW_MODE_COLLAPSE);
                        }

                        // ������ʾһ����
                        dayCell.SetDateTime(dtDay, bCurMonth);
                    }
                }
                return true;
            }

            // �ҵ�iRow�� iCol��Ӧ��table>tr>td
            // iRow Ӧ���� [0, 6] ��Χ��
            // icol [0, 8]
            ECtrl _CalendarCell(__in ETable& body, __in int iRow, __in int iCol) 
            {
                // ��һ��Ϊ�ܣ�������0
                if ((iRow < 0) || (iCol < 0) || (iCol > 8))
                {
                    ASSERT(FALSE);
                    return NULL;
                }

                ECtrl tr = body.find_first(":root>tr:nth-child(%d)", iRow + 1);
                if (tr.is_valid())
                {
                    return tr.child(iCol);
                }
                return NULL;
            }

            // �ҵ�iRow�� iCol��Ӧ��day-cell
            // table.body>tr>td>table.cell
            _CalCell _DayCell(__in ETable& body, __in int iRow, __in int iCol) 
            {
                return _CalendarCell(body, iRow, iCol).find_first(":root>table.cell");
            }

            virtual BOOL handle_event (HELEMENT he, BEHAVIOR_EVENT_PARAMS& params ) 
            {
#ifdef _DEBUG
                TraceBehaviorEventToDC(params);
#endif

                return behavior::handle_event(he, params);
            }

            void on_bnclick(ECtrl& el, ECtrl btn)
            {
                QString sCls = btn.GetClass();

                // �ϸ���
                if (sCls.Find(L"prev-date") != -1)
                    change_month(el, TRUE);
                // �¸���
                else if (sCls.Find(L"next-date") != -1)
                    change_month(el, FALSE);
                else if (sCls.Find(L"to-today") != -1)
                {
                    // ת������
                    QTime t = QTime::GetCurrentTime();
                    change_date(el, t.GetYear(), t.GetMonth());
                }
                else if (sCls.Find(L"to-desktop") != -1)
                {
                    // ��������
                    // ��Ҫ֪ͨ���ϼ�
                }
            }

            virtual BOOL on_event(HELEMENT he, HELEMENT target, BEHAVIOR_EVENTS type, UINT_PTR reason )
            {
                ECtrl el(he);
                switch (type)
                {
                case BUTTON_CLICK:
                    {
                        on_bnclick(el, target);
                        break;
                    }

                case SELECT_SELECTION_CHANGED:
                    {
                        on_date_changed(el);

                        break;
                    }
                }
                return false;
            }
        };

        qcalendar qcalendar_instance;
    }
}
