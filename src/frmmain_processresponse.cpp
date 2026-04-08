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
#include <QRegularExpression>
#include <QTextBlock>
#include <QScrollBar>
#include <QThread>

#include "frmmain.h"
#include "ui_frmmain.h"

#include "interface/SerialInterface.h"
#include "GrIP/GrIP.h"


// ---------------------------------------------------------------------------
// Thin wrappers that feed received data into the shared ProcessResponse logic
// ---------------------------------------------------------------------------

void frmMain::ProcessGRBL1_1()
{
    while (SerialIf_CanReadLine())
    {
        QString data = SerialIf_ReadLine().trimmed();
        ProcessResponse(data);
    }
}

void frmMain::ProcessGRBL_ETH(QString data)
{
    ProcessResponse(data.trimmed());
}

void frmMain::ProcessGRBL2()
{
    while (SerialIf_CanReadLine())
    {
        SerialIf_ReadLine(); // not yet implemented
    }
}


// ---------------------------------------------------------------------------
// ProcessResponse — shared GRBL response handler
// ---------------------------------------------------------------------------

void frmMain::ProcessResponse(const QString &data)
{
    qDebug() << "<" << data << ">";

    // Filter pre-reset responses
    if (m_reseting)
    {
        qDebug() << "reseting filter:" << data;
        if (!DataIsReset(data))
            return;

        m_reseting = false;
        m_timerStateQuery.setInterval(m_settings->queryStateTime());
    }

    if (data.isEmpty())
        return;

    // -----------------------------------------------------------------------
    // Status report  <...>
    // -----------------------------------------------------------------------
    if (data[0] == '<')
    {
        int status = -1;

        m_statusReceived = true;

        // Machine coordinates
        const QRegularExpression mpx(m_settings->UseRotaryAxis()
            ? "MPos:([^,]*),([^,]*),([^,]*),([^,]*),([^,>|]*)"
            : "MPos:([^,]*),([^,]*),([^,>|]*)");
        auto mpxMatch = mpx.match(data);
        if (mpxMatch.hasMatch())
        {
            ui->txtMPosX->setText(mpxMatch.captured(1));
            ui->txtMPosY->setText(mpxMatch.captured(2));
            ui->txtMPosZ->setText(mpxMatch.captured(3));
        }
        if (m_settings->UseRotaryAxis())
        {
            ui->txtMPosA->setText(mpxMatch.captured(4));
            ui->txtMPosB->setText(mpxMatch.captured(5));
        }

        // Status string
        static const QRegularExpression stx("<([^,>|]*)");
        auto stxMatch = stx.match(data);
        if (stxMatch.hasMatch())
        {
            status = m_status.indexOf(stxMatch.captured(1));
            if (status == -1) status = 0;

            if (status != m_lastGrblStatus)
            {
                ui->txtStatus->setText(m_statusCaptions[status]);
                ui->txtStatus->setStyleSheet(QString("background-color: %1; color: %2;")
                    .arg(m_statusBackColors[status]).arg(m_statusForeColors[status]));
            }

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
                if (m_taskBarProgress)
                    m_taskBarProgress->setPaused(status == HOLD0 || status == HOLD1 || status == QUEUE);
            }
#endif

            // Elapsed time
            if (m_processingFile)
            {
                QTime time(0, 0, 0);
                ui->glwVisualizer->setSpendTime(time.addMSecs(m_startTime.elapsed()));
            }

            // Job complete
            if (m_processingFile && m_transferCompleted && ((status == IDLE && m_lastGrblStatus == RUN) || status == CHECK))
            {
                qDebug() << "job completed:" << m_fileCommandIndex << m_currentModel->rowCount() - 1;

                GcodeViewParse *parser = m_currentDrawer->viewParser();
                QList<LineSegment*> list = parser->getLineSegmentList();
                if (m_lastDrawnLineIndex < list.count())
                {
                    list[m_lastDrawnLineIndex]->setDrawn(true);
                    m_currentDrawer->update(QList<int>() << m_lastDrawnLineIndex);
                }

                m_processingFile = false;
                m_fileProcessedCommandIndex = 0;
                m_lastDrawnLineIndex = 0;
                m_storedParserStatus.clear();

                updateControlsState();
                qApp->beep();

                m_timerStateQuery.stop();
                m_timerSpindleUpdate.stop();

                QMessageBox::information(this, qApp->applicationDisplayName(),
                    tr("Job done.\nTime elapsed: %1").arg(ui->glwVisualizer->spendTime().toString("hh:mm:ss")));

                m_timerStateQuery.setInterval(m_settings->queryStateTime());
                m_timerSpindleUpdate.start();
                m_timerStateQuery.start();
            }

            if (status != m_lastGrblStatus)
                m_lastGrblStatus = status;

            // Abort logic
            if (m_aborting)
            {
                switch (status)
                {
                case IDLE:
                    if (!m_processingFile && m_resetCompleted)
                    {
                        m_aborting = false;
                        restoreParserState();
                        return;
                    }
                    break;

                case HOLD0:
                case HOLD1:
                case QUEUE:
                    if (!m_reseting && compareCoordinates(m_abortX, m_abortY, m_abortZ))
                    {
                        m_abortX = sNan;
                        m_abortY = sNan;
                        m_abortZ = sNan;
                        m_abortA = sNan;
                        m_abortB = sNan;
                        GrblReset();
                    }
                    else
                    {
                        m_abortX = ui->txtMPosX->text().toDouble();
                        m_abortY = ui->txtMPosY->text().toDouble();
                        m_abortZ = ui->txtMPosZ->text().toDouble();
                        m_abortA = ui->txtMPosA->text().toDouble();
                        m_abortB = ui->txtMPosB->text().toDouble();
                    }
                    break;
                }
            }
        }

        // Work offset
        const QRegularExpression wpx(m_settings->UseRotaryAxis()
            ? "WCO:([^,]*),([^,]*),([^,]*),([^,]*),([^,>|]*)"
            : "WCO:([^,]*),([^,]*),([^,>|]*)");
        auto wpxMatch = wpx.match(data);
        if (wpxMatch.hasMatch())
        {
            m_workOffset = QVector3D(wpxMatch.captured(1).toDouble(), wpxMatch.captured(2).toDouble(), wpxMatch.captured(3).toDouble());
            if (m_settings->UseRotaryAxis())
            {
                m_workOffsetAB[0] = wpxMatch.captured(4).toDouble();
                m_workOffsetAB[1] = wpxMatch.captured(5).toDouble();
            }
        }

        // Work coordinates
        int prec = m_settings->units() == 0 ? 3 : 4;
        ui->txtWPosX->display(QString::number(ui->txtMPosX->text().toDouble() - m_workOffset.x(), 'f', prec));
        ui->txtWPosY->display(QString::number(ui->txtMPosY->text().toDouble() - m_workOffset.y(), 'f', prec));
        ui->txtWPosZ->display(QString::number(ui->txtMPosZ->text().toDouble() - m_workOffset.z(), 'f', prec));
        if (m_settings->UseRotaryAxis())
        {
            ui->txtWPosA->display(QString::number(ui->txtMPosA->text().toDouble() - m_workOffsetAB[0], 'f', prec));
            ui->txtWPosB->display(QString::number(ui->txtMPosB->text().toDouble() - m_workOffsetAB[1], 'f', prec));
        }

        // Tool position
        QVector3D toolPosition;
        if (!(status == CHECK && m_fileProcessedCommandIndex < m_currentModel->rowCount() - 1))
        {
            toolPosition = QVector3D(toMetric(ui->txtWPosX->value()), toMetric(ui->txtWPosY->value()), toMetric(ui->txtWPosZ->value()));
            m_toolDrawer.setToolPosition(m_codeDrawer->getIgnoreZ() ? QVector3D(toolPosition.x(), toolPosition.y(), 0) : toolPosition);
        }

        // Toolpath shadowing
        if (m_processingFile && status != CHECK)
        {
            GcodeViewParse *parser = m_currentDrawer->viewParser();
            bool toolOntoolpath = false;
            QList<int> drawnLines;
            QList<LineSegment*> list = parser->getLineSegmentList();

            for (int i = m_lastDrawnLineIndex;
                 i < list.count() && list.at(i)->getLineNumber() <= (m_currentModel->data(m_currentModel->index(m_fileProcessedCommandIndex, 4)).toInt() + 1);
                 i++)
            {
                if (list.at(i)->contains(toolPosition))
                {
                    toolOntoolpath = true;
                    m_lastDrawnLineIndex = i;
                    break;
                }
                drawnLines << i;
            }

            if (toolOntoolpath)
            {
                foreach (int i, drawnLines) list.at(i)->setDrawn(true);
                if (!drawnLines.isEmpty()) m_currentDrawer->update(drawnLines);
            }
            else if (m_lastDrawnLineIndex < list.count())
            {
                qDebug() << "tool missed:" << list.at(m_lastDrawnLineIndex)->getLineNumber()
                         << m_currentModel->data(m_currentModel->index(m_fileProcessedCommandIndex, 4)).toInt()
                         << m_fileProcessedCommandIndex;
            }
        }

        // Override values
        static const QRegularExpression ov("Ov:([^,]*),([^,]*),([^,>|]*)");
        auto ovMatch = ov.match(data);
        if (ovMatch.hasMatch())
        {
            UpdateOverride(ui->slbFeedOverride, ovMatch.captured(1).toInt(), 0x91);
            UpdateOverride(ui->slbSpindleOverride, ovMatch.captured(3).toInt(), 0x9a);

            int rapid = ovMatch.captured(2).toInt();
            ui->slbRapidOverride->setCurrentValue(rapid);

            int target = ui->slbRapidOverride->isChecked() ? ui->slbRapidOverride->value() : 100;

            if (rapid != target) switch (target)
            {
            case 25:
                if (m_Protocol == PROT_GRBL1_1) { SerialIf_Write(QByteArray(1, char(0x97))); }
                else if (m_Protocol == PROT_GRIP) { uint8_t c = 0x97; Pdu_t p = {&c, 1}; GrIP_Transmit(MSG_REALTIME_CMD, 0, &p); }
                break;
            case 50:
                if (m_Protocol == PROT_GRBL1_1) { SerialIf_Write(QByteArray(1, char(0x96))); }
                else if (m_Protocol == PROT_GRIP) { uint8_t c = 0x96; Pdu_t p = {&c, 1}; GrIP_Transmit(MSG_REALTIME_CMD, 0, &p); }
                break;
            case 100:
                if (m_Protocol == PROT_GRBL1_1) { SerialIf_Write(QByteArray(1, char(0x95))); }
                else if (m_Protocol == PROT_GRIP) { uint8_t c = 0x95; Pdu_t p = {&c, 1}; GrIP_Transmit(MSG_REALTIME_CMD, 0, &p); }
                break;
            }

            // Pin state
            QString pinState;
            static const QRegularExpression pn("Pn:([^|>]*)");
            auto pnMatch = pn.match(data);
            if (pnMatch.hasMatch())
                pinState.append(QString(tr("PS: %1")).arg(pnMatch.captured(1)));

            // Spindle / accessory state
            static const QRegularExpression as("A:([^,>|]+)");
            auto asMatch = as.match(data);
            if (asMatch.hasMatch())
            {
                QString state = asMatch.captured(1);
                m_spindleCW = state.contains("S");

                if (state.contains("S") || state.contains("C"))
                {
                    m_timerToolAnimation.start(25, this);
                    ui->cmdSpindle->setChecked(true);
                }
                else
                {
                    m_timerToolAnimation.stop();
                    ui->cmdSpindle->setChecked(false);
                }

                if (!pinState.isEmpty()) pinState.append(" / ");
                pinState.append(QString(tr("AS: %1")).arg(asMatch.captured(1)));
            }
            else
            {
                m_timerToolAnimation.stop();
                ui->cmdSpindle->setChecked(false);
            }

            ui->glwVisualizer->setPinState(pinState);
        }

        // Feed/spindle readout
        static const QRegularExpression fs("FS:([^,]*),([^,|>]*)");
        auto fsMatch = fs.match(data);
        if (fsMatch.hasMatch())
            ui->glwVisualizer->setSpeedState(QString(tr("F/S: %1 / %2")).arg(fsMatch.captured(1)).arg(fsMatch.captured(2)));
    }

    // -----------------------------------------------------------------------
    // Command response (non-status, non-empty)
    // -----------------------------------------------------------------------
    else if (data.length() > 0)
    {
        if (mCommandsSent.size() > 0 && !DataIsFloating(data) &&
            !(mCommandsSent.front().command != "[CTRL+X]" && DataIsReset(data)))
        {
            if ((mCommandsSent.front().command != "[CTRL+X]" && DataIsEnd(data)) ||
                (mCommandsSent.front().command == "[CTRL+X]" && DataIsReset(data)))
            {
                m_response.append(data);

                CommandQueue2 ca = mCommandsSent.front();
                mCommandsSent.pop_front();
                QTextBlock tb = ui->txtConsole->document()->findBlockByNumber(ca.consoleIndex);
                QTextCursor tc(tb);

                // Tool change (M6/M06)
                if (m_settings->UseM6() && (ca.command.contains("M6") || ca.command.contains("M06")) &&
                    m_response.contains("ok") && ca.command[0] != ';' && ca.command[0] != '(')
                {
                    qDebug() << "Waiting for tool change...";

                    int toolNum = -1;
                    if (ca.command.toUpper().contains("T"))
                    {
                        sscanf(ca.command.toUpper().toStdString().c_str(), "%*s T%d", &toolNum);
                        if (toolNum == -1)
                            sscanf(ca.command.toUpper().toStdString().c_str(), "%*sT%d", &toolNum);
                    }

                    QString msg = toolNum >= 0
                        ? QString(tr("Confirm tool change: T%1")).arg(toolNum)
                        : tr("Confirm tool change");

                    int ret = QMessageBox::information(this, qApp->applicationDisplayName(), msg,
                                                       QMessageBox::Ok | QMessageBox::Abort);

                    if (ret == QMessageBox::Ok)
                    {
                        if (m_Protocol == PROT_GRBL1_1)
                        {
                            const char res[] = "$T\r";
                            SerialIf_Write(res, strlen(res));
                        }
                        else if (m_Protocol == PROT_GRIP)
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

                // Restore coord system after jog
                if (ca.command.toUpper() == "$G" && ca.tableIndex == -2)
                {
                    if (ui->chkKeyboardControl->isChecked())
                        m_absoluteCoordinates = m_response.contains("G90");
                    else if (m_response.contains("G90"))
                        sendCommand("G90", -1, m_settings->showUICommands());
                }

                // Jog step
                if (ca.command.toUpper().contains("$J=") && ca.tableIndex == -2)
                    jogStep();

                // Parser status from [GC:...]
                static const QRegularExpression rxSpindleSpeed(".*S([\\d\\.]+)");
                if (m_response.contains(QString("[GC")))
                {
                    ui->glwVisualizer->setParserStatus(m_response.left(m_response.indexOf("; ")));

                    auto m = rxSpindleSpeed.match(m_response);
                    if (m.hasMatch())
                        ui->slbSpindle->setCurrentValue(toMetric(m.captured(1).toDouble()));
                }

                if (ca.command.toUpper().contains(QString("S")))
                    m_updateParserStatus = true;

                // $G response (parser status update)
                if (ca.command.toUpper() == "$G" && ca.tableIndex == -3)
                {
                    ui->glwVisualizer->setParserStatus(m_response.left(m_response.indexOf("; ")));

                    if (m_processingFile) storeParserState();

                    auto m = rxSpindleSpeed.match(m_response);
                    if (m.hasMatch())
                        ui->slbSpindle->setCurrentValue(toMetric(m.captured(1).toDouble()));
                }

                // $# response (work offsets / zero storage)
                if (ca.command == "$#" && ca.tableIndex == -2)
                {
                    qDebug() << "Received offsets:" << m_response;
                    static const QRegularExpression rxG92(".*G92:([^,]*),([^,]*),([^\\]]*)");
                    auto g92Match = rxG92.match(m_response);
                    if (g92Match.hasMatch())
                    {
                        if (m_settingZeroX)      { m_settingZeroX = false;  m_storedX = toMetric(g92Match.captured(1).toDouble()); }
                        else if (m_settingZeroXY){ m_settingZeroXY = false; m_storedY = toMetric(g92Match.captured(2).toDouble()); }
                        else if (m_settingZeroZ) { m_settingZeroZ = false;  m_storedZ = toMetric(g92Match.captured(3).toDouble()); }

                        ui->cmdRestoreOrigin->setToolTip(
                            QString(tr("Restore origin: %1, %2, %3\n")).arg(m_storedX).arg(m_storedY).arg(m_storedZ));
                    }
                }

                // Homing / $T
                if ((ca.command.toUpper() == "$H" || ca.command.toUpper() == "$T") && m_homing)
                    m_homing = false;

                // Reset complete
                if (ca.command == "[CTRL+X]")
                {
                    m_resetCompleted = true;
                    m_updateParserStatus = true;
                    qDebug() << "Reset complete";
                }

                // M2 / M30 — clear command buffers (old firmware)
                if ((ca.command.contains("M2") || ca.command.contains("M30")) &&
                    m_response.contains("ok") && !m_response.contains("[Pgm End]"))
                {
                    m_CommandAttributesList.clear();
                    m_CommandQueueList.clear();
                    mCommandsWait.clear();
                    mCommandsSent.clear();
                }

                // Probing (heightmap)
                if (ca.command.contains("G38.2") && m_heightMapMode && ca.tableIndex > -1)
                {
                    static const QRegularExpression rxPRB(".*PRB:([^,]*),([^,]*),([^,:\\]]*)");
                    double z = qQNaN();
                    auto prbMatch = rxPRB.match(m_response);
                    if (prbMatch.hasMatch())
                    {
                        qDebug() << "probing coordinates:" << prbMatch.captured(1) << prbMatch.captured(2) << prbMatch.captured(3);
                        z = toMetric(prbMatch.captured(3).toDouble());
                    }

                    if (m_probeIndex == -1)
                    {
                        m_probeFirstZ = z;
                        z = 0;
                    }
                    else
                    {
                        z -= m_probeFirstZ;

                        int row = trunc(m_probeIndex / m_heightMapModel.columnCount());
                        int column = m_probeIndex - row * m_heightMapModel.columnCount();
                        if (row % 2) column = m_heightMapModel.columnCount() - 1 - column;

                        m_heightMapModel.setData(m_heightMapModel.index(row, column), z, Qt::UserRole);
                        ui->tblHeightMap->update(m_heightMapModel.index(m_heightMapModel.rowCount() - 1 - row, column));
                        updateHeightMapInterpolationDrawer();
                    }

                    m_probeIndex++;
                }

                // Check mode timer adjustment
                if (ca.command.contains(QRegularExpression("\\$[cC]")))
                    m_timerStateQuery.setInterval(m_response.contains("Enable") ? 1000 : m_settings->queryStateTime());

                // Update console
                if (tb.isValid() && tb.text() == ca.command)
                {
                    bool scrolledDown = ui->txtConsole->verticalScrollBar()->value()
                                        == ui->txtConsole->verticalScrollBar()->maximum();

                    int blocksAdded = m_response.count("; ");
                    if (blocksAdded > 0)
                    {
                        for (int i = 0; i < mCommandsSent.size(); i++)
                        {
                            auto it = mCommandsSent.get_at(i);
                            if ((*it).consoleIndex != -1)
                                (*it).consoleIndex += blocksAdded;
                        }
                    }

                    tc.beginEditBlock();
                    tc.movePosition(QTextCursor::EndOfBlock);
                    tc.insertText(" < " + QString(m_response).replace("; ", "\r\n"));
                    tc.endEditBlock();

                    if (scrolledDown)
                        ui->txtConsole->verticalScrollBar()->setValue(
                            ui->txtConsole->verticalScrollBar()->maximum());
                }

                // File processing
                if (m_processingFile)
                {
                    if (ca.tableIndex > -1)
                    {
                        m_currentModel->setData(m_currentModel->index(ca.tableIndex, 2), GCodeItem::Processed);
                        m_currentModel->setData(m_currentModel->index(ca.tableIndex, 3), m_response);
                        m_fileProcessedCommandIndex = ca.tableIndex;

                        if (ui->chkAutoScroll->isChecked())
                        {
                            ui->tblProgram->scrollTo(m_currentModel->index(ca.tableIndex + 1, 0));
                            ui->tblProgram->setCurrentIndex(m_currentModel->index(ca.tableIndex, 1));
                        }
                    }

#ifdef WINDOWS
                    if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7)
                    {
                        if (m_taskBarProgress) m_taskBarProgress->setValue(m_fileProcessedCommandIndex);
                    }
#endif

                    // Error handling
                    if (ca.tableIndex > -1 && m_response.toUpper().contains("ERROR") && !m_settings->ignoreErrors())
                    {
                        m_errorMessages.append(QString::number(ca.tableIndex + 1) + ": " + ca.command + " < " + m_response + "\n");
                        m_senderErrorBox->setText(tr("Error message(s) received:\n") + m_errorMessages);

                        if (!m_holdingOnError)
                        {
                            m_holdingOnError = true;
                            m_response.clear();

                            if (m_Protocol == PROT_GRBL1_1)       { SerialIf_Write("!"); }
                            else if (m_Protocol == PROT_GRIP) { QByteArray d("!"); Pdu_t p = {(uint8_t*)d.data(), (uint16_t)d.length()}; GrIP_Transmit(MSG_REALTIME_CMD, 0, &p); }

                            m_senderErrorBox->checkBox()->setChecked(false);
                            qApp->beep();
                            int result = m_senderErrorBox->exec();

                            m_holdingOnError = false;
                            m_errorMessages.clear();

                            if (m_senderErrorBox->checkBox()->isChecked())
                            {
                                m_settings->setIgnoreErrors(true);
                                saveSettings();
                            }

                            if (result == QMessageBox::Ignore)
                            {
                                if (m_Protocol == PROT_GRBL1_1)       { SerialIf_Write("~"); }
                                else if (m_Protocol == PROT_GRIP) { QByteArray d("~"); Pdu_t p = {(uint8_t*)d.data(), (uint16_t)d.length()}; GrIP_Transmit(MSG_REALTIME_CMD, 0, &p); }
                            }
                            else
                            {
                                on_cmdFileAbort_clicked();
                            }
                        }
                    }

                    // Transfer complete?
                    if (m_fileProcessedCommandIndex == m_currentModel->rowCount() - 2 ||
                        ca.command.contains(QRegularExpression("M0*2|M30")))
                    {
                        m_transferCompleted = true;
                    }
                    else if (!m_fileEndSent && (m_fileCommandIndex < m_currentModel->rowCount()) && !m_holdingOnError)
                    {
                        sendNextFileCommands();
                    }
                }

                // Log error code description
                if (m_response.toUpper().contains("ERROR"))
                {
                    int num = 0;
                    sscanf(m_response.toUpper().toStdString().c_str(), "ERROR:%d", &num);
                    qDebug() << "Error:" << num;
                    ui->txtConsole->appendPlainText("ERROR: " + GetErrorMsg(num));
                }

                // Scroll to top on M30
                if (ca.command.contains("M30"))
                    ui->tblProgram->setCurrentIndex(m_currentModel->index(0, 1));

                // Toolpath shadowing in check mode
                if (m_statusCaptions.indexOf(ui->txtStatus->text()) == CHECK)
                {
                    GcodeViewParse *parser = m_currentDrawer->viewParser();
                    QList<LineSegment*> list = parser->getLineSegmentList();

                    if (!m_transferCompleted && m_fileProcessedCommandIndex < m_currentModel->rowCount() - 1)
                    {
                        int i;
                        QList<int> drawnLines;
                        for (i = m_lastDrawnLineIndex;
                             i < list.count() && list.at(i)->getLineNumber()
                                 <= m_currentModel->data(m_currentModel->index(m_fileProcessedCommandIndex, 4)).toInt();
                             i++)
                        {
                            drawnLines << i;
                        }

                        if (!drawnLines.isEmpty() && i < list.count())
                        {
                            m_lastDrawnLineIndex = i;
                            m_toolDrawer.setToolPosition(list.at(i)->getEnd());
                        }

                        foreach (int idx, drawnLines) list.at(idx)->setDrawn(true);
                        if (!drawnLines.isEmpty()) m_currentDrawer->update(drawnLines);
                    }
                    else
                    {
                        foreach (LineSegment *s, list)
                        {
                            if (!qIsNaN(s->getEnd().length()))
                            {
                                m_toolDrawer.setToolPosition(s->getEnd());
                                break;
                            }
                        }
                    }
                }

                m_response.clear();
            }
            else
            {
                m_response.append(data + "; ");
            }
        }
        else
        {
            // Floating / unprocessed response
            qDebug() << "Floating response:" << data;

            if (DataIsReset(data))
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
}
