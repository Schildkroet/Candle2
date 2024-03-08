/*
 * Candle2
 * Copyright (C) 2015-2016 Hayrullin Denis Ravilevich
 * Copyright (C) 2018-2019 Patrick F.

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include <QDebug>
#include <QTextBlock>
#include <QScrollBar>
#include <QThread>

#include "frmmain.h"
#include "ui_frmmain.h"

#include "interface/SerialInterface.h"
#include "GrIP/GrIP.h"


void frmMain::ProcessGRBL1_1()
{
    while(SerialIf_CanReadLine())
    {
        QString data = SerialIf_ReadLine().trimmed();

        qDebug() << "<" << data << ">";

        // Filter prereset responses
        if(m_reseting)
        {
            qDebug() << "reseting filter:" << data;
            if (!DataIsReset(data))
                continue;
            else
            {
                m_reseting = false;
                m_timerStateQuery.setInterval(m_settings->queryStateTime());
            }
        }

        if(data.length() == 0)
        {
            continue;
        }

        // Status response
        if(data[0] == '<')
        {
            int status = -1;

            m_statusReceived = true;

            // Update machine coordinates
            static QRegExp mpx;
            if(!m_settings->UseRotaryAxis())
            {
                mpx.setPattern("MPos:([^,]*),([^,]*),([^,^>^|]*)");
            }
            else
            {
                mpx.setPattern("MPos:([^,]*),([^,]*),([^,]*),([^,]*),([^,^>^|]*)");
            }

            if(mpx.indexIn(data) != -1)
            {
                ui->txtMPosX->setText(mpx.cap(1));
                ui->txtMPosY->setText(mpx.cap(2));
                ui->txtMPosZ->setText(mpx.cap(3));
            }
            if(m_settings->UseRotaryAxis())
            {
                // Set A & B
                //qDebug() << "A: " << mpx.cap(4);
                ui->txtMPosA->setText(mpx.cap(4));
                ui->txtMPosB->setText(mpx.cap(5));
            }

            // Status
            static QRegExp stx("<([^,^>^|]*)");
            if (stx.indexIn(data) != -1)
            {
                status = m_status.indexOf(stx.cap(1));

                // Undetermined status
                if (status == -1) status = 0;

                // Update status
                if(status != m_lastGrblStatus)
                {
                    ui->txtStatus->setText(m_statusCaptions[status]);
                    ui->txtStatus->setStyleSheet(QString("background-color: %1; color: %2;").arg(m_statusBackColors[status]).arg(m_statusForeColors[status]));
                }

                // Update controls
                ui->cmdRestoreOrigin->setEnabled(status == IDLE);
                ui->cmdSafePosition->setEnabled(status == IDLE);
                ui->cmdZeroX->setEnabled(status == IDLE);
                ui->cmdZeroY->setEnabled(status == IDLE);
                ui->cmdZeroZ->setEnabled(status == IDLE);
                ui->chkTestMode->setEnabled(status != RUN && !m_processingFile);
                ui->chkTestMode->setChecked(status == CHECK);
                ui->cmdFilePause->setChecked(status == HOLD0 || status == HOLD1 || status == QUEUE);
                ui->cmdSpindle->setEnabled(!m_processingFile || status == HOLD0);
                ui->cmdTouch->setEnabled(status == IDLE);
                ui->cmdHome->setEnabled(status == IDLE || status == ALARM);
#ifdef WINDOWS
                if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7)
                {
                    if (m_taskBarProgress) m_taskBarProgress->setPaused(status == HOLD0 || status == HOLD1 || status == QUEUE);
                }
#endif

                // Update "elapsed time" timer
                if(m_processingFile)
                {
                    QTime time(0, 0, 0);
                    int elapsed = m_startTime.elapsed();
                    ui->glwVisualizer->setSpendTime(time.addMSecs(elapsed));
                }

                // Test for job complete
                if(m_processingFile && m_transferCompleted && ((status == IDLE && m_lastGrblStatus == RUN) || status == CHECK))
                {
                    qDebug() << "job completed:" << m_fileCommandIndex << m_currentModel->rowCount() - 1;

                    // Shadow last segment
                    GcodeViewParse *parser = m_currentDrawer->viewParser();
                    QList<LineSegment*> list = parser->getLineSegmentList();
                    if(m_lastDrawnLineIndex < list.count())
                    {
                        list[m_lastDrawnLineIndex]->setDrawn(true);
                        m_currentDrawer->update(QList<int>() << m_lastDrawnLineIndex);
                    }

                    // Update state
                    m_processingFile = false;
                    m_fileProcessedCommandIndex = 0;
                    m_lastDrawnLineIndex = 0;
                    m_storedParserStatus.clear();

                    updateControlsState();

                    qApp->beep();

                    m_timerStateQuery.stop();
                    m_timerSpindleUpdate.stop();

                    QMessageBox::information(this, qApp->applicationDisplayName(), tr("Job done.\nTime elapsed: %1").arg(ui->glwVisualizer->spendTime().toString("hh:mm:ss")));

                    m_timerStateQuery.setInterval(m_settings->queryStateTime());
                    m_timerSpindleUpdate.start();
                    m_timerStateQuery.start();
                }

                // Store status
                if(status != m_lastGrblStatus)
                {
                    m_lastGrblStatus = status;
                }

                // Abort
                static double x = sNan;
                static double y = sNan;
                static double z = sNan;
                static double a = sNan;
                static double b = sNan;

                if(m_aborting)
                {
                    switch(status)
                    {
                    case IDLE: // Idle
                        if (!m_processingFile && m_resetCompleted)
                        {
                            m_aborting = false;
                            //restoreOffsets();
                            restoreParserState();
                            return;
                        }
                        break;

                    case HOLD0: // Hold
                    case HOLD1:
                    case QUEUE:
                        if (!m_reseting && compareCoordinates(x, y, z))
                        {
                            x = sNan;
                            y = sNan;
                            z = sNan;
                            a = sNan;
                            b = sNan;
                            GrblReset();
                        }
                        else
                        {
                            x = ui->txtMPosX->text().toDouble();
                            y = ui->txtMPosY->text().toDouble();
                            z = ui->txtMPosZ->text().toDouble();
                            a = ui->txtMPosA->text().toDouble();
                            b = ui->txtMPosB->text().toDouble();
                        }
                        break;
                    }
                }
            }

            // Store work offset
            static QVector3D workOffset;
            static double workOffsetAB[2] = {0.0};
            static QRegExp wpx;

            if(!m_settings->UseRotaryAxis())
            {
                wpx.setPattern("WCO:([^,]*),([^,]*),([^,^>^|]*)");
            }
            else
            {
                wpx.setPattern("WCO:([^,]*),([^,]*),([^,]*),([^,]*),([^,^>^|]*)");
            }

            if(wpx.indexIn(data) != -1)
            {
                workOffset = QVector3D(wpx.cap(1).toDouble(), wpx.cap(2).toDouble(), wpx.cap(3).toDouble());

                // Store offsets for rotary axis
                if(m_settings->UseRotaryAxis())
                {
                    workOffsetAB[0] = wpx.cap(4).toDouble();
                    workOffsetAB[1] = wpx.cap(5).toDouble();
                }
            }

            // Update work coordinates
            int prec = m_settings->units() == 0 ? 3 : 4;
            ui->txtWPosX->display(QString::number(ui->txtMPosX->text().toDouble() - workOffset.x(), 'f', prec));
            ui->txtWPosY->display(QString::number(ui->txtMPosY->text().toDouble() - workOffset.y(), 'f', prec));
            ui->txtWPosZ->display(QString::number(ui->txtMPosZ->text().toDouble() - workOffset.z(), 'f', prec));

            if(m_settings->UseRotaryAxis())
            {
                ui->txtWPosA->display(QString::number(ui->txtMPosA->text().toDouble() - workOffsetAB[0], 'f', prec));
                ui->txtWPosB->display(QString::number(ui->txtMPosB->text().toDouble() - workOffsetAB[1], 'f', prec));
            }

            // Update tool position
            QVector3D toolPosition;
            if(!(status == CHECK && m_fileProcessedCommandIndex < m_currentModel->rowCount() - 1))
            {
                toolPosition = QVector3D(toMetric(ui->txtWPosX->value()), toMetric(ui->txtWPosY->value()), toMetric(ui->txtWPosZ->value()));

                m_toolDrawer.setToolPosition(m_codeDrawer->getIgnoreZ() ? QVector3D(toolPosition.x(), toolPosition.y(), 0) : toolPosition);
            }

            // toolpath shadowing
            if(m_processingFile && status != CHECK)
            {
                GcodeViewParse *parser = m_currentDrawer->viewParser();

                bool toolOntoolpath = false;

                QList<int> drawnLines;
                QList<LineSegment*> list = parser->getLineSegmentList();

                for(int i = m_lastDrawnLineIndex; i < list.count() && list.at(i)->getLineNumber() <= (m_currentModel->data(m_currentModel->index(m_fileProcessedCommandIndex, 4)).toInt() + 1); i++)
                {
                    if(list.at(i)->contains(toolPosition))
                    {
                        toolOntoolpath = true;
                        m_lastDrawnLineIndex = i;
                        break;
                    }
                    drawnLines << i;
                }

                if(toolOntoolpath)
                {
                    foreach (int i, drawnLines)
                    {
                        list.at(i)->setDrawn(true);
                    }

                    if(!drawnLines.isEmpty())
                    {
                        m_currentDrawer->update(drawnLines);
                    }
                }
                else if(m_lastDrawnLineIndex < list.count())
                {
                    qDebug() << "tool missed:" << list.at(m_lastDrawnLineIndex)->getLineNumber()
                             << m_currentModel->data(m_currentModel->index(m_fileProcessedCommandIndex, 4)).toInt()
                             << m_fileProcessedCommandIndex;
                }
            }

            // Get overridings
            static QRegExp ov("Ov:([^,]*),([^,]*),([^,^>^|]*)");
            if(ov.indexIn(data) != -1)
            {
                UpdateOverride(ui->slbFeedOverride, ov.cap(1).toInt(), 0x91);
                UpdateOverride(ui->slbSpindleOverride, ov.cap(3).toInt(), 0x9a);

                int rapid = ov.cap(2).toInt();
                ui->slbRapidOverride->setCurrentValue(rapid);

                int target = ui->slbRapidOverride->isChecked() ? ui->slbRapidOverride->value() : 100;

                if(rapid != target) switch(target)
                {
                case 25:
                    // Rapid override: 25%
                    if(m_Protocol == PROT_GRBL1_1)
                    {
                        SerialIf_Write(QByteArray(1, char(0x97)));
                    }
                    else if(m_Protocol == PROT_GRIP)
                    {
                        //QByteArray data(1, char(0x97));
                        uint8_t c = 0x97;
                        Pdu_t p = {&c, 1};
                        GrIP_Transmit(MSG_REALTIME_CMD, 0, &p);
                    }
                    break;

                case 50:
                    // Rapid override: 50%
                    if(m_Protocol == PROT_GRBL1_1)
                    {
                        SerialIf_Write(QByteArray(1, char(0x96)));
                    }
                    else if(m_Protocol == PROT_GRIP)
                    {
                        //QByteArray data(1, char(0x96));
                        uint8_t c = 0x96;
                        Pdu_t p = {&c, 1};
                        GrIP_Transmit(MSG_REALTIME_CMD, 0, &p);
                    }
                    break;

                case 100:
                    // Rapid override: 100%
                    if(m_Protocol == PROT_GRBL1_1)
                    {
                        SerialIf_Write(QByteArray(1, char(0x95)));
                    }
                    else if(m_Protocol == PROT_GRIP)
                    {
                        QByteArray data(1, char(0x95));
                        uint8_t c = 0x95;
                        Pdu_t p = {&c, 1};
                        GrIP_Transmit(MSG_REALTIME_CMD, 0, &p);
                    }
                    break;
                }

                // Update pins state
                QString pinState;
                static QRegExp pn("Pn:([^|^>]*)");
                if(pn.indexIn(data) != -1)
                {
                    pinState.append(QString(tr("PS: %1")).arg(pn.cap(1)));
                }

                // Process spindle state
                static QRegExp as("A:([^,^>^|]+)");
                if(as.indexIn(data) != -1)
                {
                    QString state = as.cap(1);
                    m_spindleCW = state.contains("S");

                    if(state.contains("S") || state.contains("C"))
                    {
                        m_timerToolAnimation.start(25, this);
                        ui->cmdSpindle->setChecked(true);
                    }
                    else
                    {
                        m_timerToolAnimation.stop();
                        ui->cmdSpindle->setChecked(false);
                    }

                    if(!pinState.isEmpty())
                    {
                        pinState.append(" / ");
                    }

                    pinState.append(QString(tr("AS: %1")).arg(as.cap(1)));
                }
                else
                {
                    m_timerToolAnimation.stop();
                    ui->cmdSpindle->setChecked(false);
                }

                ui->glwVisualizer->setPinState(pinState);
            }

            // Get feed/spindle values
            static QRegExp fs("FS:([^,]*),([^,^|^>]*)");
            if(fs.indexIn(data) != -1)
            {
                ui->glwVisualizer->setSpeedState((QString(tr("F/S: %1 / %2")).arg(fs.cap(1)).arg(fs.cap(2))));
            }

        }
        else if(data.length() > 0)
        {
            // Processed commands
            //if(m_CommandAttributesList.length() > 0 && !DataIsFloating(data) && !(m_CommandAttributesList[0].command != "[CTRL+X]" && DataIsReset(data)))
            auto &val = mCommandsSent.front();

            if(mCommandsSent.size() > 0 && !DataIsFloating(data) && !(val.command != "[CTRL+X]" && DataIsReset(data)))
            {
                static QString response; // Full response string

                if((mCommandsSent.front().command != "[CTRL+X]" && DataIsEnd(data)) || (mCommandsSent.front().command == "[CTRL+X]" && DataIsReset(data)))
                {
                    response.append(data);

                    // Take command from buffer
                    CommandQueue2 ca = mCommandsSent.front();
                    mCommandsSent.pop_front();
                    QTextBlock tb = ui->txtConsole->document()->findBlockByNumber(ca.consoleIndex);
                    QTextCursor tc(tb);

                    if(m_settings->UseM6() && (ca.command.contains("M6") || ca.command.contains("M06")) && response.contains("ok") && ca.command[0] != ';' && ca.command[0] != '(')
                    {
                        qDebug() << "Waiting for tool change...";

                        int ret = QMessageBox::information(this, qApp->applicationDisplayName(), tr("Confirm tool change"), QMessageBox::Ok | QMessageBox::Abort);

                        const char res[4] = "$T\r";

                        if(ret == QMessageBox::Ok)
                        {
                            if(m_Protocol == PROT_GRBL1_1)
                            {
                                SerialIf_Write(res, strlen(res));
                                //SerialIf_Write("\r", 1);
                            }
                            else if(m_Protocol == PROT_GRIP)
                            {
                                uint8_t ttt[3] = {'$', 'T', '\r'};
                                Pdu_t p = {ttt, 3};
                                GrIP_Transmit(MSG_REALTIME_CMD, 0, &p);
                            }
                            ca.command = "$T";
                            mCommandsSent.push_front(ca);
                        }

                        m_jogVector.setZ(0.0);
                        m_toolChangeActive = false;
                    }

                    // Restore absolute/relative coordinate system after jog
                    if(ca.command.toUpper() == "$G" && ca.tableIndex == -2)
                    {
                        if (ui->chkKeyboardControl->isChecked()) m_absoluteCoordinates = response.contains("G90");
                        else if (response.contains("G90")) sendCommand("G90", -1, m_settings->showUICommands());
                    }

                    // Jog
                    if(ca.command.toUpper().contains("$J=") && ca.tableIndex == -2)
                    {
                        jogStep();
                    }

                    if(response.contains(QString("[GC")))
                    {
                        // Update status in visualizer window
                        ui->glwVisualizer->setParserStatus(response.left(response.indexOf("; ")));

                        // Spindle speed
                        QRegExp rx(".*S([\\d\\.]+)");
                        if(rx.indexIn(response) != -1)
                        {
                            double speed = toMetric(rx.cap(1).toDouble()); //RPM in imperial?
                            ui->slbSpindle->setCurrentValue(speed);
                        }
                    }

                    if(ca.command.toUpper().contains(QString("S")))
                    {
                            m_updateParserStatus = true;
                    }

                    // Process parser status
                    if(ca.command.toUpper() == "$G" && ca.tableIndex == -3)
                    {
                        // Update status in visualizer window
                        ui->glwVisualizer->setParserStatus(response.left(response.indexOf("; ")));

                        // Store parser status
                        if (m_processingFile)
                            storeParserState();

                        // Spindle speed
                        QRegExp rx(".*S([\\d\\.]+)");
                        if(rx.indexIn(response) != -1)
                        {
                            double speed = toMetric(rx.cap(1).toDouble()); //RPM in imperial?
                            ui->slbSpindle->setCurrentValue(speed);
                        }
                        //qDebug() << "Update Parser true";
                        //m_updateParserStatus = true;
                    }

                    // Store origin
                    if(ca.command == "$#" && ca.tableIndex == -2)
                    {
                        qDebug() << "Received offsets:" << response;
                        QRegExp rx(".*G92:([^,]*),([^,]*),([^\\]]*)");

                        if(rx.indexIn(response) != -1)
                        {
                            if(m_settingZeroX)
                            {
                                m_settingZeroX = false;
                                m_storedX = toMetric(rx.cap(1).toDouble());
                            }
                            else if(m_settingZeroXY)
                            {
                                m_settingZeroXY = false;
                                m_storedY = toMetric(rx.cap(2).toDouble());
                            }
                            else if(m_settingZeroZ)
                            {
                                m_settingZeroZ = false;
                                m_storedZ = toMetric(rx.cap(3).toDouble());
                            }

                            ui->cmdRestoreOrigin->setToolTip(QString(tr("Restore origin: %1, %2, %3\n")).arg(m_storedX).arg(m_storedY).arg(m_storedZ));
                        }
                    }

                    // Homing response
                    if ((ca.command.toUpper() == "$H" || ca.command.toUpper() == "$T") && m_homing)
                        m_homing = false;

                    // Reset complete
                    if(ca.command == "[CTRL+X]")
                    {
                        m_resetCompleted = true;
                        m_updateParserStatus = true;
                        qDebug() << "Reset complete";
                    }

                    // Clear command buffer on "M2" & "M30" command (old firmwares)
                    if((ca.command.contains("M2") || ca.command.contains("M30")) && response.contains("ok") && !response.contains("[Pgm End]"))
                    {
                        m_CommandAttributesList.clear();
                        m_CommandQueueList.clear();
                        mCommandsWait.clear();
                        mCommandsSent.clear();
                    }

                    // Process probing on heightmap mode only from table commands
                    if(ca.command.contains("G38.2") && m_heightMapMode && ca.tableIndex > -1)
                    {
                        // Get probe Z coordinate
                        // "[PRB:0.000,0.000,0.000:0];ok"
                        QRegExp rx(".*PRB:([^,]*),([^,]*),([^]^:]*)");
                        double z = qQNaN();
                        if(rx.indexIn(response) != -1)
                        {
                            qDebug() << "probing coordinates:" << rx.cap(1) << rx.cap(2) << rx.cap(3);
                            z = toMetric(rx.cap(3).toDouble());
                        }

                        static double firstZ;

                        if(m_probeIndex == -1)
                        {
                            firstZ = z;
                            z = 0;
                        }
                        else
                        {
                            // Calculate delta Z
                            z -= firstZ;

                            // Calculate table indexes
                            int row = trunc(m_probeIndex / m_heightMapModel.columnCount());
                            int column = m_probeIndex - row * m_heightMapModel.columnCount();
                            if (row % 2) column = m_heightMapModel.columnCount() - 1 - column;

                            // Store Z in table
                            m_heightMapModel.setData(m_heightMapModel.index(row, column), z, Qt::UserRole);
                            ui->tblHeightMap->update(m_heightMapModel.index(m_heightMapModel.rowCount() - 1 - row, column));
                            updateHeightMapInterpolationDrawer();
                        }

                        m_probeIndex++;
                    }

                    // Change state query time on check mode on
                    if(ca.command.contains(QRegExp("$[cC]")))
                    {
                        m_timerStateQuery.setInterval(response.contains("Enable") ? 1000 : m_settings->queryStateTime());
                    }

                    // Add response to console
                    if(tb.isValid() && tb.text() == ca.command)
                    {
                        bool scrolledDown = ui->txtConsole->verticalScrollBar()->value() == ui->txtConsole->verticalScrollBar()->maximum();

                        // Update text block numbers
                        int blocksAdded = response.count("; ");

                        if(blocksAdded > 0)
                        {
                            for (int i = 0; i < mCommandsSent.size(); i++)
                            {
                                auto it = mCommandsSent.get_at(i);

                                //if (m_CommandAttributesList[i].consoleIndex != -1) m_CommandAttributesList[i].consoleIndex += blocksAdded;
                                if((*it).consoleIndex != -1)
                                {
                                    (*it).consoleIndex += blocksAdded;
                                }
                            }
                        }

                        tc.beginEditBlock();
                        tc.movePosition(QTextCursor::EndOfBlock);

                        tc.insertText(" < " + QString(response).replace("; ", "\r\n"));
                        tc.endEditBlock();

                        if (scrolledDown)
                            ui->txtConsole->verticalScrollBar()->setValue(ui->txtConsole->verticalScrollBar()->maximum());
                    }

                    // Check queue
                    /*if(m_CommandQueueList.length() > 0)
                    {
                        CommandQueue cq = m_CommandQueueList.takeFirst();

                        while((BufferLength2() + cq.command.length() + 1) <= GRBL_BUFFERLENGTH)
                        {
                            sendCommand(cq.command, cq.tableIndex, cq.showInConsole);

                            if(m_CommandQueueList.isEmpty())
                                break;
                            else
                                cq = m_CommandQueueList.takeFirst();
                        }
                    }*/

                    // Add response to table, send next program commands
                    if(m_processingFile)
                    {
                        // Only if command from table
                        if(ca.tableIndex > -1)
                        {
                            m_currentModel->setData(m_currentModel->index(ca.tableIndex, 2), GCodeItem::Processed);
                            m_currentModel->setData(m_currentModel->index(ca.tableIndex, 3), response);

                            m_fileProcessedCommandIndex = ca.tableIndex;

                            if(ui->chkAutoScroll->isChecked() && ca.tableIndex != -1)
                            {
                                ui->tblProgram->scrollTo(m_currentModel->index(ca.tableIndex + 1, 0));      // TODO: Update by timer
                                ui->tblProgram->setCurrentIndex(m_currentModel->index(ca.tableIndex, 1));
                            }
                        }

#ifdef WINDOWS
                        // Update taskbar progress
                        if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7)
                        {
                            if (m_taskBarProgress) m_taskBarProgress->setValue(m_fileProcessedCommandIndex);
                        }
#endif
                        // Process error messages
                        static bool holding = false;
                        static QString errors;

                        if(ca.tableIndex > -1 && response.toUpper().contains("ERROR") && !m_settings->ignoreErrors())
                        {
                            errors.append(QString::number(ca.tableIndex + 1) + ": " + ca.command + " < " + response + "\n");

                            m_senderErrorBox->setText(tr("Error message(s) received:\n") + errors);

                            if(!holding)
                            {
                                holding = true;         // Hold transmit while messagebox is visible
                                response.clear();

                                // Feed hold: !
                                if(m_Protocol == PROT_GRBL1_1)
                                {
                                    SerialIf_Write("!");
                                }
                                else if(m_Protocol == PROT_GRIP)
                                {
                                    QByteArray data("!");
                                    //GrIP_Transmit(MSG_REALTIME_CMD, 0, (const uint8_t*)data.constData(), data.length());
                                    Pdu_t p = {(uint8_t*)data.data(), (uint16_t)data.length()};
                                    GrIP_Transmit(MSG_REALTIME_CMD, 0, &p);
                                }

                                m_senderErrorBox->checkBox()->setChecked(false);
                                qApp->beep();
                                int result = m_senderErrorBox->exec();

                                holding = false;
                                errors.clear();
                                if (m_senderErrorBox->checkBox()->isChecked()) m_settings->setIgnoreErrors(true);

                                if(result == QMessageBox::Ignore)
                                {
                                    // Cycle Start/Resume: ~
                                    if(m_Protocol == PROT_GRBL1_1)
                                    {
                                        SerialIf_Write("~");
                                    }
                                    else if(m_Protocol == PROT_GRIP)
                                    {
                                        QByteArray data("~");
                                        //GrIP_Transmit(MSG_REALTIME_CMD, 0, (const uint8_t*)data.constData(), data.length());
                                        Pdu_t p = {(uint8_t*)data.data(), (uint16_t)data.length()};
                                        GrIP_Transmit(MSG_REALTIME_CMD, 0, &p);
                                    }
                                }
                                else
                                    on_cmdFileAbort_clicked();
                            }
                        }

                        // Check transfer complete (last row always blank, last command row = rowcount - 2)
                        if(m_fileProcessedCommandIndex == m_currentModel->rowCount() - 2 || ca.command.contains(QRegExp("M0*2|M30")))
                            m_transferCompleted = true;
                        // Send next program commands
                        else if (!m_fileEndSent && (m_fileCommandIndex < m_currentModel->rowCount()) && !holding)
                            sendNextFileCommands();
                    }

                    if(response.toUpper().contains("ERROR"))
                    {
                        int num = 0;

                        sscanf(response.toUpper().toStdString().c_str(), "ERROR:%d", &num);
                        qDebug() << "Error: " << num;

                        ui->txtConsole->appendPlainText("ERROR: " + GetErrorMsg(num));
                    }

                    // Scroll to first line on "M30" command
                    if (ca.command.contains("M30"))
                        ui->tblProgram->setCurrentIndex(m_currentModel->index(0, 1));

                    // Toolpath shadowing on check mode
                    if(m_statusCaptions.indexOf(ui->txtStatus->text()) == CHECK)
                    {
                        GcodeViewParse *parser = m_currentDrawer->viewParser();
                        QList<LineSegment*> list = parser->getLineSegmentList();

                        if(!m_transferCompleted && m_fileProcessedCommandIndex < m_currentModel->rowCount() - 1)
                        {
                            int i;
                            QList<int> drawnLines;

                            for(i = m_lastDrawnLineIndex; i < list.count() && list.at(i)->getLineNumber()
                                 <= (m_currentModel->data(m_currentModel->index(m_fileProcessedCommandIndex, 4)).toInt()); i++)
                            {
                                drawnLines << i;
                            }

                            if(!drawnLines.isEmpty() && (i < list.count()))
                            {
                                m_lastDrawnLineIndex = i;
                                QVector3D vec = list.at(i)->getEnd();
                                m_toolDrawer.setToolPosition(vec);
                            }

                            foreach(int i, drawnLines)
                            {
                                list.at(i)->setDrawn(true);
                            }

                            if(!drawnLines.isEmpty())
                                m_currentDrawer->update(drawnLines);
                        }
                        else
                        {
                            foreach(LineSegment* s, list)
                            {
                                if(!qIsNaN(s->getEnd().length()))
                                {
                                    m_toolDrawer.setToolPosition(s->getEnd());
                                    break;
                                }
                            }
                        }
                    }

                    response.clear();
                }
                else
                {
                    response.append(data + "; ");
                }
            }
            else
            {
                // Unprocessed responses
                qDebug() << "Floating response:" << data;

                // Handle hardware reset
                if(DataIsReset(data))
                {
                    qDebug() << "Hardware reset";

                    m_processingFile = false;
                    m_transferCompleted = true;
                    m_fileCommandIndex = 0;

                    m_reseting = false;
                    m_homing = false;
                    m_lastGrblStatus = -1;

                    m_updateParserStatus = true;
                    m_statusReceived = true;

                    m_CommandAttributesList.clear();
                    m_CommandQueueList.clear();

                    mCommandsWait.clear();
                    mCommandsSent.clear();

                    updateControlsState();
                }

                ui->txtConsole->appendPlainText(data);
            }
        }
        else if(data.size() > 2)
        {
            // Blank response
            qDebug() << "Unknown: " << data;
        }
    }
}


void frmMain::ProcessGRBL_ETH(QString data)
{
    {
        // Remove CRLF
        data = data.trimmed();

        //qDebug() << "-- " << data << " --";

        // Filter prereset responses
        if(m_reseting)
        {
            qDebug() << "reseting filter:" << data;
            if (!DataIsReset(data))
                return;
            else
            {
                m_reseting = false;
                m_timerStateQuery.setInterval(m_settings->queryStateTime());
            }
        }

        if(data.length() == 0)
        {
            return;
        }

        // Status response
        if(data[0] == '<')
        {
            int status = -1;

            m_statusReceived = true;

            // Update machine coordinates
            static QRegExp mpx;
            if(!m_settings->UseRotaryAxis())
            {
                mpx.setPattern("MPos:([^,]*),([^,]*),([^,^>^|]*)");
            }
            else
            {
                mpx.setPattern("MPos:([^,]*),([^,]*),([^,]*),([^,]*),([^,^>^|]*)");
            }
            if(mpx.indexIn(data) != -1)
            {
                ui->txtMPosX->setText(mpx.cap(1));
                ui->txtMPosY->setText(mpx.cap(2));
                ui->txtMPosZ->setText(mpx.cap(3));
            }
            if(m_settings->UseRotaryAxis())
            {
                // Set A & B
                //qDebug() << "A: " << mpx.cap(4);
                ui->txtMPosA->setText(mpx.cap(4));
                ui->txtMPosB->setText(mpx.cap(5));
            }

            // Status
            static QRegExp stx("<([^,^>^|]*)");
            if (stx.indexIn(data) != -1)
            {
                status = m_status.indexOf(stx.cap(1));

                // Undetermined status
                if (status == -1) status = 0;

                // Update status
                if(status != m_lastGrblStatus)
                {
                    ui->txtStatus->setText(m_statusCaptions[status]);
                    ui->txtStatus->setStyleSheet(QString("background-color: %1; color: %2;").arg(m_statusBackColors[status]).arg(m_statusForeColors[status]));
                }

                // Update controls
                ui->cmdRestoreOrigin->setEnabled(status == IDLE);
                ui->cmdSafePosition->setEnabled(status == IDLE);
                ui->cmdZeroX->setEnabled(status == IDLE);
                ui->cmdZeroY->setEnabled(status == IDLE);
                ui->cmdZeroZ->setEnabled(status == IDLE);
                ui->chkTestMode->setEnabled(status != RUN && !m_processingFile);
                ui->chkTestMode->setChecked(status == CHECK);
                ui->cmdFilePause->setChecked(status == HOLD0 || status == HOLD1 || status == QUEUE);
                ui->cmdSpindle->setEnabled(!m_processingFile || status == HOLD0);
                ui->cmdTouch->setEnabled(status == IDLE);
                ui->cmdHome->setEnabled(status == IDLE || status == ALARM);
#ifdef WINDOWS
                if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7)
                {
                    if (m_taskBarProgress) m_taskBarProgress->setPaused(status == HOLD0 || status == HOLD1 || status == QUEUE);
                }
#endif

                // Update "elapsed time" timer
                if(m_processingFile)
                {
                    QTime time(0, 0, 0);
                    int elapsed = m_startTime.elapsed();
                    ui->glwVisualizer->setSpendTime(time.addMSecs(elapsed));
                }

                // Test for job complete
                if(m_processingFile && m_transferCompleted && ((status == IDLE && m_lastGrblStatus == RUN) || status == CHECK))
                {
                    qDebug() << "job completed:" << m_fileCommandIndex << m_currentModel->rowCount() - 1;

                    // Shadow last segment
                    GcodeViewParse *parser = m_currentDrawer->viewParser();
                    QList<LineSegment*> list = parser->getLineSegmentList();
                    if(m_lastDrawnLineIndex < list.count())
                    {
                        list[m_lastDrawnLineIndex]->setDrawn(true);
                        m_currentDrawer->update(QList<int>() << m_lastDrawnLineIndex);
                    }

                    // Update state
                    m_processingFile = false;
                    m_fileProcessedCommandIndex = 0;
                    m_lastDrawnLineIndex = 0;
                    m_storedParserStatus.clear();

                    updateControlsState();

                    qApp->beep();

                    m_timerStateQuery.stop();
                    m_timerSpindleUpdate.stop();

                    QMessageBox::information(this, qApp->applicationDisplayName(), tr("Job done.\nTime elapsed: %1").arg(ui->glwVisualizer->spendTime().toString("hh:mm:ss")));

                    m_timerStateQuery.setInterval(m_settings->queryStateTime());
                    m_timerSpindleUpdate.start();
                    m_timerStateQuery.start();
                }

                // Store status
                if(status != m_lastGrblStatus)
                {
                    m_lastGrblStatus = status;
                }

                // Abort
                static double x = sNan;
                static double y = sNan;
                static double z = sNan;
                static double a = sNan;
                static double b = sNan;

                if(m_aborting)
                {
                    switch(status)
                    {
                    case IDLE: // Idle
                        if (!m_processingFile && m_resetCompleted)
                        {
                            m_aborting = false;
                            //restoreOffsets();
                            restoreParserState();
                            return;
                        }
                        break;

                    case HOLD0: // Hold
                    case HOLD1:
                    case QUEUE:
                        if (!m_reseting && compareCoordinates(x, y, z))
                        {
                            x = sNan;
                            y = sNan;
                            z = sNan;
                            a = sNan;
                            b = sNan;
                            GrblReset();
                        }
                        else
                        {
                            x = ui->txtMPosX->text().toDouble();
                            y = ui->txtMPosY->text().toDouble();
                            z = ui->txtMPosZ->text().toDouble();
                            a = ui->txtMPosA->text().toDouble();
                            b = ui->txtMPosB->text().toDouble();
                        }
                        break;
                    }
                }
            }

            // Store work offset
            static QVector3D workOffset;
            static double workOffsetAB[2] = {0.0};
            static QRegExp wpx;

            if(!m_settings->UseRotaryAxis())
            {
                wpx.setPattern("WCO:([^,]*),([^,]*),([^,^>^|]*)");
            }
            else
            {
                wpx.setPattern("WCO:([^,]*),([^,]*),([^,]*),([^,]*),([^,^>^|]*)");
            }

            if(wpx.indexIn(data) != -1)
            {
                workOffset = QVector3D(wpx.cap(1).toDouble(), wpx.cap(2).toDouble(), wpx.cap(3).toDouble());

                // Store offsets for rotary axis
                if(m_settings->UseRotaryAxis())
                {
                    workOffsetAB[0] = wpx.cap(4).toDouble();
                    workOffsetAB[1] = wpx.cap(5).toDouble();
                }
            }

            // Update work coordinates
            int prec = m_settings->units() == 0 ? 3 : 4;
            ui->txtWPosX->display(QString::number(ui->txtMPosX->text().toDouble() - workOffset.x(), 'f', prec));
            ui->txtWPosY->display(QString::number(ui->txtMPosY->text().toDouble() - workOffset.y(), 'f', prec));
            ui->txtWPosZ->display(QString::number(ui->txtMPosZ->text().toDouble() - workOffset.z(), 'f', prec));
            if(m_settings->UseRotaryAxis())
            {
                ui->txtWPosA->display(QString::number(ui->txtMPosA->text().toDouble() - workOffsetAB[0], 'f', prec));
                ui->txtWPosB->display(QString::number(ui->txtMPosB->text().toDouble() - workOffsetAB[1], 'f', prec));
            }

            // Update tool position
            QVector3D toolPosition;
            if(!(status == CHECK && m_fileProcessedCommandIndex < m_currentModel->rowCount() - 1))
            {
                toolPosition = QVector3D(toMetric(ui->txtWPosX->value()), toMetric(ui->txtWPosY->value()), toMetric(ui->txtWPosZ->value()));

                m_toolDrawer.setToolPosition(m_codeDrawer->getIgnoreZ() ? QVector3D(toolPosition.x(), toolPosition.y(), 0) : toolPosition);
            }

            // toolpath shadowing
            if(m_processingFile && status != CHECK)
            {
                GcodeViewParse *parser = m_currentDrawer->viewParser();

                bool toolOntoolpath = false;

                QList<int> drawnLines;
                QList<LineSegment*> list = parser->getLineSegmentList();

                for(int i = m_lastDrawnLineIndex; i < list.count() && list.at(i)->getLineNumber() <= (m_currentModel->data(m_currentModel->index(m_fileProcessedCommandIndex, 4)).toInt() + 1); i++)
                {
                    if(list.at(i)->contains(toolPosition))
                    {
                        toolOntoolpath = true;
                        m_lastDrawnLineIndex = i;
                        break;
                    }
                    drawnLines << i;
                }

                if(toolOntoolpath)
                {
                    foreach (int i, drawnLines)
                    {
                        list.at(i)->setDrawn(true);
                    }

                    if(!drawnLines.isEmpty())
                    {
                        m_currentDrawer->update(drawnLines);
                    }
                }
                else if(m_lastDrawnLineIndex < list.count())
                {
                   /* qDebug() << "tool missed:" << list.at(m_lastDrawnLineIndex)->getLineNumber()
                             << m_currentModel->data(m_currentModel->index(m_fileProcessedCommandIndex, 4)).toInt()
                             << m_fileProcessedCommandIndex;*/
                }
            }

            // Get overridings
            static QRegExp ov("Ov:([^,]*),([^,]*),([^,^>^|]*)");
            if(ov.indexIn(data) != -1)
            {
                UpdateOverride(ui->slbFeedOverride, ov.cap(1).toInt(), 0x91);
                UpdateOverride(ui->slbSpindleOverride, ov.cap(3).toInt(), 0x9a);

                int rapid = ov.cap(2).toInt();
                ui->slbRapidOverride->setCurrentValue(rapid);

                int target = ui->slbRapidOverride->isChecked() ? ui->slbRapidOverride->value() : 100;

                if(rapid != target) switch(target)
                {
                case 25:
                    // Rapid override: 25%
                    if(m_Protocol == PROT_GRBL1_1)
                    {
                        SerialIf_Write(QByteArray(1, char(0x97)));
                    }
                    else if(m_Protocol == PROT_GRIP)
                    {
                        //QByteArray data(1, char(0x97));
                        uint8_t c = 0x97;
                        Pdu_t p = {&c, 1};
                        GrIP_Transmit(MSG_REALTIME_CMD, 0, &p);
                    }
                    break;

                case 50:
                    // Rapid override: 50%
                    if(m_Protocol == PROT_GRBL1_1)
                    {
                        SerialIf_Write(QByteArray(1, char(0x96)));
                    }
                    else if(m_Protocol == PROT_GRIP)
                    {
                        //QByteArray data(1, char(0x96));
                        uint8_t c = 0x96;
                        Pdu_t p = {&c, 1};
                        GrIP_Transmit(MSG_REALTIME_CMD, 0, &p);
                    }
                    break;

                case 100:
                    // Rapid override: 100%
                    if(m_Protocol == PROT_GRBL1_1)
                    {
                        SerialIf_Write(QByteArray(1, char(0x95)));
                    }
                    else if(m_Protocol == PROT_GRIP)
                    {
                        QByteArray data(1, char(0x95));
                        uint8_t c = 0x95;
                        Pdu_t p = {&c, 1};
                        GrIP_Transmit(MSG_REALTIME_CMD, 0, &p);
                    }
                    break;
                }

                // Update pins state
                QString pinState;
                static QRegExp pn("Pn:([^|^>]*)");
                if(pn.indexIn(data) != -1)
                {
                    pinState.append(QString(tr("PS: %1")).arg(pn.cap(1)));
                }

                // Process spindle state
                static QRegExp as("A:([^,^>^|]+)");
                if(as.indexIn(data) != -1)
                {
                    QString state = as.cap(1);
                    m_spindleCW = state.contains("S");

                    if(state.contains("S") || state.contains("C"))
                    {
                        m_timerToolAnimation.start(25, this);
                        ui->cmdSpindle->setChecked(true);
                    }
                    else
                    {
                        m_timerToolAnimation.stop();
                        ui->cmdSpindle->setChecked(false);
                    }

                    if(!pinState.isEmpty())
                    {
                        pinState.append(" / ");
                    }

                    pinState.append(QString(tr("AS: %1")).arg(as.cap(1)));
                }
                else
                {
                    m_timerToolAnimation.stop();
                    ui->cmdSpindle->setChecked(false);
                }

                ui->glwVisualizer->setPinState(pinState);
            }

            // Get feed/spindle values
            static QRegExp fs("FS:([^,]*),([^,^|^>]*)");
            if(fs.indexIn(data) != -1)
            {
                ui->glwVisualizer->setSpeedState((QString(tr("F/S: %1 / %2")).arg(fs.cap(1)).arg(fs.cap(2))));
            }

        }
        else if(data.length() > 0)
        {
            // Processed commands
            auto &val = mCommandsSent.front();

            if(mCommandsSent.size() > 0 && !DataIsFloating(data) && !(val.command != "[CTRL+X]" && DataIsReset(data)))
            {
                static QString response; // Full response string
                //qDebug() << "CMD: " << mCommandsSent.front().command;
                //qDebug() << "R: " << data;

                if((mCommandsSent.front().command != "[CTRL+X]" && DataIsEnd(data)) || (mCommandsSent.front().command == "[CTRL+X]" && DataIsReset(data)))
                {
                    response.append(data);

                    // Take command from buffer
                    CommandQueue2 ca = mCommandsSent.front();
                    mCommandsSent.pop_front();
                    QTextBlock tb = ui->txtConsole->document()->findBlockByNumber(ca.consoleIndex);
                    QTextCursor tc(tb);

                    if(m_settings->UseM6() && (ca.command.contains("M6") || ca.command.contains("M06")) && response.contains("ok") && ca.command[0] != ';' && ca.command[0] != '(')
                    {
                        qDebug() << "Waiting for tool change...";

                        int num = -1;
                        if(ca.command.toUpper().contains("T"))
                        {
                            sscanf(ca.command.toUpper().toStdString().c_str(), "%*s T%d", &num);
                            if(num == -1)
                            {
                                sscanf(ca.command.toUpper().toStdString().c_str(), "%*sT%d", &num);
                            }
                        }

                        QString msg = "Confirm tool change: T";
                        msg.append(QString::number(num));
                        int ret = QMessageBox::information(this, qApp->applicationDisplayName(), (msg), QMessageBox::Ok | QMessageBox::Abort);

                        char res[4] = "$T\r";

                        if(ret == QMessageBox::Ok)
                        {
                            if(m_Protocol == PROT_GRBL1_1)
                            {
                                SerialIf_Write(res, strlen(res));
                                //SerialIf_Write("\r", 1);
                            }
                            else if(m_Protocol == PROT_GRIP)
                            {
                                uint8_t ttt[3] = {'$', 'T', '\r'};
                                Pdu_t p = {ttt, 3};
                                GrIP_Transmit(MSG_SYSTEM_CMD, 0, &p);
                                QThread::msleep(10);
                            }
                            ca.command = "$T";
                            mCommandsSent.push_front(ca);
                        }

                        m_jogVector.setZ(0.0);
                        m_toolChangeActive = false;
                    }

                    // Restore absolute/relative coordinate system after jog
                    if(ca.command.toUpper() == "$G" && ca.tableIndex == -2)
                    {
                        if (ui->chkKeyboardControl->isChecked()) m_absoluteCoordinates = response.contains("G90");
                        else if (response.contains("G90")) sendCommand("G90", -1, m_settings->showUICommands());
                    }

                    // Jog
                    if(ca.command.toUpper().contains("$J=") && ca.tableIndex == -2)
                    {
                        jogStep();
                    }

                    if(response.contains(QString("[GC")))
                    {
                        // Update status in visualizer window
                        ui->glwVisualizer->setParserStatus(response.left(response.indexOf("; ")));

                        // Spindle speed
                        QRegExp rx(".*S([\\d\\.]+)");
                        if(rx.indexIn(response) != -1)
                        {
                            double speed = toMetric(rx.cap(1).toDouble()); //RPM in imperial?
                            ui->slbSpindle->setCurrentValue(speed);
                        }
                    }

                    if(ca.command.toUpper().contains(QString("S")))
                    {
                            m_updateParserStatus = true;
                    }

                    // Process parser status
                    if(ca.command.toUpper() == "$G" && ca.tableIndex == -3)
                    {
                        // Update status in visualizer window
                        ui->glwVisualizer->setParserStatus(response.left(response.indexOf("; ")));

                        // Store parser status
                        if (m_processingFile)
                            storeParserState();

                        // Spindle speed
                        QRegExp rx(".*S([\\d\\.]+)");
                        if(rx.indexIn(response) != -1)
                        {
                            double speed = toMetric(rx.cap(1).toDouble()); //RPM in imperial?
                            ui->slbSpindle->setCurrentValue(speed);
                        }
                    }

                    // Store origin
                    if(ca.command == "$#" && ca.tableIndex == -2)
                    {
                        qDebug() << "Received offsets:" << response;
                        QRegExp rx(".*G92:([^,]*),([^,]*),([^\\]]*)");

                        if(rx.indexIn(response) != -1)
                        {
                            if(m_settingZeroX)
                            {
                                m_settingZeroX = false;
                                m_storedX = toMetric(rx.cap(1).toDouble());
                            }
                            else if(m_settingZeroXY)
                            {
                                m_settingZeroXY = false;
                                m_storedY = toMetric(rx.cap(2).toDouble());
                            }
                            else if(m_settingZeroZ)
                            {
                                m_settingZeroZ = false;
                                m_storedZ = toMetric(rx.cap(3).toDouble());
                            }

                            ui->cmdRestoreOrigin->setToolTip(QString(tr("Restore origin: %1, %2, %3\n")).arg(m_storedX).arg(m_storedY).arg(m_storedZ));
                        }
                    }

                    // Homing response
                    if ((ca.command.toUpper() == "$H" || ca.command.toUpper() == "$T") && m_homing)
                        m_homing = false;

                    // Reset complete
                    if(ca.command == "[CTRL+X]")
                    {
                        m_resetCompleted = true;
                        m_updateParserStatus = true;
                        qDebug() << "Reset complete";
                    }

                    // Clear command buffer on "M2" & "M30" command (old firmwares)
                    if((ca.command.contains("M2") || ca.command.contains("M30")) && response.contains("ok") && !response.contains("[Pgm End]"))
                    {
                        m_CommandAttributesList.clear();
                        m_CommandQueueList.clear();
                        mCommandsWait.clear();
                        mCommandsSent.clear();
                    }

                    // Process probing on heightmap mode only from table commands
                    if(ca.command.contains("G38.2") && m_heightMapMode && ca.tableIndex > -1)
                    {
                        // Get probe Z coordinate
                        // "[PRB:0.000,0.000,0.000:0];ok"
                        QRegExp rx(".*PRB:([^,]*),([^,]*),([^]^:]*)");
                        double z = qQNaN();
                        if(rx.indexIn(response) != -1)
                        {
                            qDebug() << "probing coordinates:" << rx.cap(1) << rx.cap(2) << rx.cap(3);
                            z = toMetric(rx.cap(3).toDouble());
                        }

                        static double firstZ;

                        if(m_probeIndex == -1)
                        {
                            firstZ = z;
                            z = 0;
                        }
                        else
                        {
                            // Calculate delta Z
                            z -= firstZ;

                            // Calculate table indexes
                            int row = trunc(m_probeIndex / m_heightMapModel.columnCount());
                            int column = m_probeIndex - row * m_heightMapModel.columnCount();
                            if (row % 2) column = m_heightMapModel.columnCount() - 1 - column;

                            // Store Z in table
                            m_heightMapModel.setData(m_heightMapModel.index(row, column), z, Qt::UserRole);
                            ui->tblHeightMap->update(m_heightMapModel.index(m_heightMapModel.rowCount() - 1 - row, column));
                            updateHeightMapInterpolationDrawer();
                        }

                        m_probeIndex++;
                    }

                    // Change state query time on check mode on
                    if(ca.command.contains(QRegExp("$[cC]")))
                    {
                        m_timerStateQuery.setInterval(response.contains("Enable") ? 1000 : m_settings->queryStateTime());
                    }

                    // Add response to console
                    if(tb.isValid() && tb.text() == ca.command)
                    {
                        bool scrolledDown = ui->txtConsole->verticalScrollBar()->value() == ui->txtConsole->verticalScrollBar()->maximum();

                        // Update text block numbers
                        int blocksAdded = response.count("; ");

                        if(blocksAdded > 0)
                        {
                            for (int i = 0; i < mCommandsSent.size(); i++)
                            {
                                auto it = mCommandsSent.get_at(i);

                                //if (m_CommandAttributesList[i].consoleIndex != -1) m_CommandAttributesList[i].consoleIndex += blocksAdded;
                                if((*it).consoleIndex != -1)
                                {
                                    (*it).consoleIndex += blocksAdded;
                                }
                            }
                        }

                        tc.beginEditBlock();
                        tc.movePosition(QTextCursor::EndOfBlock);

                        tc.insertText(" < " + QString(response).replace("; ", "\r\n"));
                        tc.endEditBlock();

                        if (scrolledDown)
                            ui->txtConsole->verticalScrollBar()->setValue(ui->txtConsole->verticalScrollBar()->maximum());
                    }

                    // Add response to table, send next program commands
                    if(m_processingFile)
                    {
                        // Only if command from table
                        if(ca.tableIndex > -1)
                        {
                            m_currentModel->setData(m_currentModel->index(ca.tableIndex, 2), GCodeItem::Processed);
                            m_currentModel->setData(m_currentModel->index(ca.tableIndex, 3), response);

                            m_fileProcessedCommandIndex = ca.tableIndex;

                            if(ui->chkAutoScroll->isChecked() && ca.tableIndex != -1)
                            {
                                ui->tblProgram->scrollTo(m_currentModel->index(ca.tableIndex + 1, 0));      // TODO: Update by timer
                                ui->tblProgram->setCurrentIndex(m_currentModel->index(ca.tableIndex, 1));
                            }
                        }

#ifdef WINDOWS
                        // Update taskbar progress
                        if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7)
                        {
                            if (m_taskBarProgress) m_taskBarProgress->setValue(m_fileProcessedCommandIndex);
                        }
#endif
                        // Process error messages
                        static bool holding = false;
                        static QString errors;

                        if(ca.tableIndex > -1 && response.toUpper().contains("ERROR") && !m_settings->ignoreErrors())
                        {
                            errors.append(QString::number(ca.tableIndex + 1) + ": " + ca.command + " < " + response + "\n");

                            m_senderErrorBox->setText(tr("Error message(s) received:\n") + errors);

                            if(!holding)
                            {
                                holding = true;         // Hold transmit while messagebox is visible
                                response.clear();

                                // Feed hold: !
                                if(m_Protocol == PROT_GRBL1_1)
                                {
                                    SerialIf_Write("!");
                                }
                                else if(m_Protocol == PROT_GRIP)
                                {
                                    QByteArray data("!");
                                    //GrIP_Transmit(MSG_REALTIME_CMD, 0, (const uint8_t*)data.constData(), data.length());
                                    Pdu_t p = {(uint8_t*)data.data(), (uint16_t)data.length()};
                                    GrIP_Transmit(MSG_REALTIME_CMD, 0, &p);
                                }

                                m_senderErrorBox->checkBox()->setChecked(false);
                                qApp->beep();
                                int result = m_senderErrorBox->exec();

                                holding = false;
                                errors.clear();
                                if (m_senderErrorBox->checkBox()->isChecked()) m_settings->setIgnoreErrors(true);

                                if(result == QMessageBox::Ignore)
                                {
                                    // Cycle Start/Resume: ~
                                    if(m_Protocol == PROT_GRBL1_1)
                                    {
                                        SerialIf_Write("~");
                                    }
                                    else if(m_Protocol == PROT_GRIP)
                                    {
                                        QByteArray data("~");
                                        //GrIP_Transmit(MSG_REALTIME_CMD, 0, (const uint8_t*)data.constData(), data.length());
                                        Pdu_t p = {(uint8_t*)data.data(), (uint16_t)data.length()};
                                        GrIP_Transmit(MSG_REALTIME_CMD, 0, &p);
                                    }
                                }
                                else
                                    on_cmdFileAbort_clicked();
                            }
                        }

                        // Check transfer complete (last row always blank, last command row = rowcount - 2)
                        if(m_fileProcessedCommandIndex == m_currentModel->rowCount() - 2 || ca.command.contains(QRegExp("M0*2|M30")))
                            m_transferCompleted = true;
                        // Send next program commands
                        else if (!m_fileEndSent && (m_fileCommandIndex < m_currentModel->rowCount()) && !holding)
                            sendNextFileCommands();
                    }

                    if(response.toUpper().contains("ERROR"))
                    {
                        int num = 0;

                        sscanf(response.toUpper().toStdString().c_str(), "ERROR:%d", &num);
                        qDebug() << "Error: " << num;

                        ui->txtConsole->appendPlainText("ERROR: " + GetErrorMsg(num));
                    }

                    // Scroll to first line on "M30" command
                    if (ca.command.contains("M30"))
                        ui->tblProgram->setCurrentIndex(m_currentModel->index(0, 1));

                    // Toolpath shadowing on check mode
                    if(m_statusCaptions.indexOf(ui->txtStatus->text()) == CHECK)
                    {
                        GcodeViewParse *parser = m_currentDrawer->viewParser();
                        QList<LineSegment*> list = parser->getLineSegmentList();

                        if(!m_transferCompleted && m_fileProcessedCommandIndex < m_currentModel->rowCount() - 1)
                        {
                            int i;
                            QList<int> drawnLines;

                            for(i = m_lastDrawnLineIndex; i < list.count() && list.at(i)->getLineNumber()
                                 <= (m_currentModel->data(m_currentModel->index(m_fileProcessedCommandIndex, 4)).toInt()); i++)
                            {
                                drawnLines << i;
                            }

                            if(!drawnLines.isEmpty() && (i < list.count()))
                            {
                                m_lastDrawnLineIndex = i;
                                QVector3D vec = list.at(i)->getEnd();
                                m_toolDrawer.setToolPosition(vec);
                            }

                            foreach(int i, drawnLines)
                            {
                                list.at(i)->setDrawn(true);
                            }

                            if(!drawnLines.isEmpty())
                                m_currentDrawer->update(drawnLines);
                        }
                        else
                        {
                            foreach(LineSegment* s, list)
                            {
                                if(!qIsNaN(s->getEnd().length()))
                                {
                                    m_toolDrawer.setToolPosition(s->getEnd());
                                    break;
                                }
                            }
                        }
                    }

                    response.clear();
                }
                else
                {
                    response.append(data + "; ");
                }
            }
            else
            {
                // Unprocessed responses
                qDebug() << "Floating response:" << data;

                // Handle hardware reset
                if(DataIsReset(data))
                {
                    qDebug() << "Hardware reset";

                    m_processingFile = false;
                    m_transferCompleted = true;
                    m_fileCommandIndex = 0;

                    m_reseting = false;
                    m_homing = false;
                    m_lastGrblStatus = -1;

                    m_updateParserStatus = true;
                    m_statusReceived = true;

                    m_CommandAttributesList.clear();
                    m_CommandQueueList.clear();

                    mCommandsWait.clear();
                    mCommandsSent.clear();

                    updateControlsState();
                }

                ui->txtConsole->appendPlainText(data);
            }
        }
        else
        {
            // Blank response
        }
    }
}


void frmMain::ProcessGRBL2()
{
    while(SerialIf_CanReadLine())
    {
        QString data = SerialIf_ReadLine().trimmed();
    }
}
