#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QProxyStyle>
#include <QDateTime>
#include <QMainWindow>
#include <QLayout>
#include <QMessageBox>
#include <QSqlError>
#include <QTcpSocket>
#include "OMC8000Lib/OMC8000Types.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

/*
 * OMC8000 UDP IP simulator
 *
 * http://merret.cz/vzdalena-sprava-OMC8000-UDP
 *
 * adresa: OMC8000_IP:57999
 *
 * Čtení proměnné:
 *      1. Příkaz * vrací hodnotu proměnné jako string. Podporované typy: STRING, BYTE, INT, UINT, DINT, UDINT, REAL, BOOL
 *      2. Příkaz # vrací hodnotu proměnné jako HEXSTRING ve formátu „D;HHHH…“, kde D je délka proměnné, HHHH… je její HEXSTRING
 *      3. Pokud proměnná neexistuje, nebo dojde k jiné chybě, je na příkaz * vrácen string: „Error“, na příkaz # “0;No data”
 *
 *      0*Pmain.MyCnt1<CRC16><CRC16>
 *      0*@GV.MyCnt1<CRC16><CRC16>
 *
 * Zápis proměnné:
 *      1. Příkaz { zadá hodnotu proměnné jako string, pokud je to celé číslo. Podporované typy: BYTE, INT, UINT, DINT, UDINT, LINT, BOOL
 *      2. Příkaz [ zadá hodnotu proměnné jako hexstring, pokud je to celé číslo. Podporované typy: BYTE, INT, UINT, DINT, UDINT, LINT, BOOL
 *      3. Příkaz } zadá hodnotu proměnné jako string, pokud je to číslo s plovoucí desetinnou čárkou. Podporované typy: REAL
 *      4. Příkaz ] zadá hodnotu proměnné jako string. Podporované typy: STRING
 *      5. Odpovědí na příkaz {, }, případně příkaz [, ] je „!“ v případě úspěšného zapsání čísla. V jiném případě se zápis hodnoty nezdařil.
 *
 *      0{12,Pmain.MyCnt1<CRC16><CRC16>
 *      0{12,@GV.MyCnt1<CRC16><CRC16>
 *      0}Pmain.MyCnt1,1234.58<CRC16><CRC16>
 *      0}@GV.MyCnt1,1234.58<CRC16><CRC16>
 *      0]Pmain.MyCnt1,abcde <CRC16><CRC16>
 *      0]@GV.MyCnt1,abcde<CRC16><CRC16>
 *
 * Globalní proměnné:
 *      @GV.PLC_SYS_TICK_CNT
 *      @GV.PLC_TASK_DEFINED
 *      @GV.PLCMODE_ON
 *      @GV.PLCMODE_LOADING
 *      @GV.PLCMODE_STOP
 *      @GV.PLCMODE_RUN
 *      @GV.PLCMODE_HALT
 *      @GV.PLC_TICKS_PER_SEC
 *      @GV.PLC_MAX_ERRORS
 *      @GV.PLC_ERRORS
 *      @GV.PLC_TASK_AVAILABLE
 *      @GV.PLC_SYSTASK_AVAILABLE
 *      @GV.PLCDEBUG_FORCE
 *      @GV.PLCDEBUG_BPSET
 *      @GV.PLCDEBUG_POWERFLOW
 *
 *
 */
void MainWindow::responsePlc(QString cmd, QString* response)
{
    static int PC_ACT = 1;
    static int PC_PROGRAM = 15;
    static int COUNTER = 0;
    static int STEP = 0;
    static BYTE COLOR_1 = 0;
    static BYTE COLOR_2 = 0;
    static BYTE COLOR_3 = 0;
    static BYTE COLOR_4 = 0;

    if(cmd.length()>2)
    {
        if(cmd.at(1)=='*') //read
        {
            if(cmd.contains("GV.IN_Byte0"))
                response->append("3");
            else if(cmd.contains("GV.IN_Byte2"))
                response->append("64");
            else if(cmd.contains("GV.IN_Byte3"))
                response->append("3");
            else if(cmd.contains("GV.OUT_Byte0"))
                response->append("3");
            else if(cmd.contains("GV.OUT_Byte1"))
                response->append("3");
            else if(cmd.contains("GV.STEP"))
            {
                if(STEP==100)
                    STEP = 0;
                else
                    STEP++;

                if(STEP>7) STEP = 100;
                response->append(QString::number(STEP, 10));
            }
            else if(cmd.contains("GV.PC_PROGRAM"))
                response->append(QString::number(PC_PROGRAM, 10));
            else if(cmd.contains("GV.PC_ACT"))
                response->append(QString::number(PC_ACT, 10));
            else if(cmd.contains("GV.COUNTER"))
                response->append(QString::number(++COUNTER, 10));
            else if(cmd.contains("Pdisplay.COLOR"))
            {
                UINT32_MB val;

                COLOR_1 = 0;
                COLOR_2 = 0;
                COLOR_3 = 2;
                COLOR_4 = 0;

                val.byte.byte0 = COLOR_1;
                val.byte.byte1 = COLOR_2;
                val.byte.byte2 = COLOR_3;
                val.byte.byte3 = COLOR_4;
                response->append(QString::number(val.dword, 10));
            }
            else
                response->append("Error");
        }
        else if(cmd.at(1)=='#') //read hex
        {
            response->append("3;123");
        }
        else if(cmd.at(1)=='{')
        {
            if(cmd.contains("GV.PC_PROGRAM"))
            {
                QStringList split = cmd.split("{");
                QString snum = split.at(1);
                split = snum.split(",");
                snum = split.at(0);

                PC_PROGRAM = snum.toInt();
            }
            else if(cmd.contains("GV.PC_ACT"))
            {
                QStringList split = cmd.split("{");
                QString snum = split.at(1);
                split = snum.split(",");
                snum = split.at(0);

                PC_ACT = snum.toInt();
            }

            response->append("!"); //success
        }
        else if(cmd.at(1)=='}')
        {
            response->append("!"); //success
        }
        else if(cmd.at(1)=='[')
        {
            response->append("!"); //success
        }
        else if(cmd.at(1)==']')
        {
            response->append("!"); //success
        }
    }
}

void MainWindow::responsePrinter(QString cmd, QString* response)
{
    response->append("Error\r\n");
}

