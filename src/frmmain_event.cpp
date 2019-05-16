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
#include <QScrollBar>
#include <QMimeData>

#include "frmmain.h"
#include "ui_frmmain.h"

#include "interface/SerialInterface.h"


bool frmMain::eventFilter(QObject *obj, QEvent *event)
{
    // Main form events
    if (obj == this || obj == ui->tblProgram || obj == ui->cboJogStep || obj == ui->cboJogFeed)
    {
        // Jog on keyboard control
        if (!m_processingFile && ui->chkKeyboardControl->isChecked() && (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease)
                && !static_cast<QKeyEvent*>(event)->isAutoRepeat())
        {

            switch (static_cast<QKeyEvent*>(event)->key())
            {
            case Qt::Key_4:
                if (event->type() == QEvent::KeyPress) emit ui->cmdXMinus->pressed(); else emit ui->cmdXMinus->released();
                break;

            case Qt::Key_6:
                if (event->type() == QEvent::KeyPress) emit ui->cmdXPlus->pressed(); else emit ui->cmdXPlus->released();
                break;

            case Qt::Key_8:
                if (event->type() == QEvent::KeyPress) emit ui->cmdYPlus->pressed(); else emit ui->cmdYPlus->released();
                break;

            case Qt::Key_2:
                if (event->type() == QEvent::KeyPress) emit ui->cmdYMinus->pressed(); else emit ui->cmdYMinus->released();
                break;

            case Qt::Key_9:
                if (event->type() == QEvent::KeyPress) emit ui->cmdZPlus->pressed(); else emit ui->cmdZPlus->released();
                break;

            case Qt::Key_3:
                if (event->type() == QEvent::KeyPress) emit ui->cmdZMinus->pressed(); else emit ui->cmdZMinus->released();
                break;
            }
        }

        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

            if (!m_processingFile && keyEvent->key() == Qt::Key_ScrollLock && obj == this)
            {
                ui->chkKeyboardControl->toggle();
                if (!ui->chkKeyboardControl->isChecked())
                    ui->cboCommand->setFocus();
            }

            if (!m_processingFile && ui->chkKeyboardControl->isChecked())
            {
                if (keyEvent->key() == Qt::Key_7)
                {
                    ui->cboJogStep->setCurrentPrevious();
                }
                else if (keyEvent->key() == Qt::Key_1)
                {
                    ui->cboJogStep->setCurrentNext();
                }
                else if (keyEvent->key() == Qt::Key_Minus)
                {
                    ui->cboJogFeed->setCurrentPrevious();
                }
                else if (keyEvent->key() == Qt::Key_Plus)
                {
                    ui->cboJogFeed->setCurrentNext();
                }
                else if (keyEvent->key() == Qt::Key_5)
                {
                    on_cmdStop_clicked();
                }
                else if (keyEvent->key() == Qt::Key_0)
                {
                    on_cmdSpindle_clicked(!ui->cmdSpindle->isChecked());
                }
                else if (keyEvent->key() == Qt::Key_Asterisk)
                {
                    ui->slbSpindle->setSliderPosition(ui->slbSpindle->sliderPosition() + 1);
                }
                else if (keyEvent->key() == Qt::Key_Slash)
                {
                    ui->slbSpindle->setSliderPosition(ui->slbSpindle->sliderPosition() - 1);
                }
            }

            if (obj == ui->tblProgram && m_processingFile)
            {
                if (keyEvent->key() == Qt::Key_PageDown || keyEvent->key() == Qt::Key_PageUp || keyEvent->key() == Qt::Key_Down || keyEvent->key() == Qt::Key_Up)
                {
                    ui->chkAutoScroll->setChecked(false);
                }
            }
        }

    }
    else if (obj == ui->splitPanels && event->type() == QEvent::Resize)
    {
        // Splitter events
        // Resize splited widgets
        onPanelsSizeChanged(ui->scrollAreaWidgetContents->sizeHint());
    }
    else if (obj == ui->splitPanels->handle(1))
    {
        // Splitter handle events
        int minHeight = getConsoleMinHeight();
        switch (event->type())
        {
        case QEvent::MouseButtonPress:
            // Store current console group box minimum & real heights
            m_storedConsoleMinimumHeight = ui->grpConsole->minimumHeight();
            m_storedConsoleHeight = ui->grpConsole->height();

            // Update splited sizes
            ui->splitPanels->setSizes(QList<int>() << ui->scrollArea->height() << ui->grpConsole->height());

            // Set new console mimimum height
            ui->grpConsole->setMinimumHeight(qMax(minHeight, ui->splitPanels->height()
                - ui->scrollAreaWidgetContents->sizeHint().height() - ui->splitPanels->handleWidth() - 4));
            break;

        case QEvent::MouseButtonRelease:
            // Store new console minimum height if height was changed with split handle
            if (ui->grpConsole->height() != m_storedConsoleHeight)
            {
                ui->grpConsole->setMinimumHeight(ui->grpConsole->height());
            }
            else
            {
                ui->grpConsole->setMinimumHeight(m_storedConsoleMinimumHeight);
            }
            break;

        case QEvent::MouseButtonDblClick:
            // Switch to min/max console size
            if (ui->grpConsole->height() == minHeight || !ui->scrollArea->verticalScrollBar()->isVisible())
            {
                ui->splitPanels->setSizes(QList<int>() << ui->scrollArea->minimumHeight()
                << ui->splitPanels->height() - ui->splitPanels->handleWidth() - ui->scrollArea->minimumHeight());
            }
            else
            {
                ui->grpConsole->setMinimumHeight(minHeight);
                onPanelsSizeChanged(ui->scrollAreaWidgetContents->sizeHint());
            }
            break;

        default:
            break;
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

void frmMain::showEvent(QShowEvent *se)
{
    Q_UNUSED(se)

    placeVisualizerButtons();

#ifdef WINDOWS
    if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7) {
        if (m_taskBarButton == NULL) {
            m_taskBarButton = new QWinTaskbarButton(this);
            m_taskBarButton->setWindow(this->windowHandle());
            m_taskBarProgress = m_taskBarButton->progress();
        }
    }
#endif

    ui->glwVisualizer->setUpdatesEnabled(true);

    resizeCheckBoxes();
}

void frmMain::hideEvent(QHideEvent *he)
{
    Q_UNUSED(he)

    ui->glwVisualizer->setUpdatesEnabled(false);
}

void frmMain::resizeEvent(QResizeEvent *re)
{
    Q_UNUSED(re)

    placeVisualizerButtons();
    resizeCheckBoxes();
    resizeTableHeightMapSections();
}

void frmMain::timerEvent(QTimerEvent *te)
{
    if (te->timerId() == m_timerToolAnimation.timerId())
    {
        m_toolDrawer.rotate((m_spindleCW ? -40 : 40) * (double)(ui->slbSpindle->currentValue()) / (ui->slbSpindle->maximum()));
    }
    else
    {
        QMainWindow::timerEvent(te);
    }
}

void frmMain::closeEvent(QCloseEvent *ce)
{
    bool mode = m_heightMapMode;
    m_heightMapMode = false;

    if (!saveChanges(m_heightMapMode))
    {
        ce->ignore();
        m_heightMapMode = mode;

        return;
    }

    if (m_processingFile && QMessageBox::warning(this, this->windowTitle(), tr("File sending in progress. Terminate and exit?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
    {
        ce->ignore();
        m_heightMapMode = mode;

        return;
    }

    if (SerialIf_IsOpen())
    {
        SerialIf_Close();
    }

    if (m_CommandQueueList.length() > 0)
    {
        m_CommandAttributesList.clear();
        m_CommandQueueList.clear();
    }

    mCommandsWait.clear();
    mCommandsSent.clear();
}

void frmMain::dragEnterEvent(QDragEnterEvent *dee)
{
    if (m_processingFile)
    {
        return;
    }

    if (dee->mimeData()->hasFormat("text/plain") && !m_heightMapMode)
    {
        dee->acceptProposedAction();
    }
    else if (dee->mimeData()->hasFormat("text/uri-list") && dee->mimeData()->urls().count() == 1)
    {
        QString fileName = dee->mimeData()->urls().at(0).toLocalFile();

        if ((!m_heightMapMode && isGCodeFile(fileName)) || (m_heightMapMode && isHeightmapFile(fileName)))
        {
            dee->acceptProposedAction();
        }
    }
}

void frmMain::dropEvent(QDropEvent *de)
{
    QString fileName = de->mimeData()->urls().at(0).toLocalFile();

    if (!m_heightMapMode)
    {
        if (!saveChanges(false))
        {
            return;
        }

        if (!fileName.isEmpty())
        {
            // Load dropped g-code file
            addRecentFile(fileName);
            updateRecentFilesMenu();
            loadFile(fileName);
        }
        else
        {
            // Load dropped text
            m_programFileName.clear();
            m_fileChanged = true;
            loadFile(de->mimeData()->text().split("\n"));
        }
    }
    else
    {
        if (!saveChanges(true))
            return;

        // Load dropped heightmap file
        addRecentHeightmap(fileName);
        updateRecentFilesMenu();
        loadHeightMap(fileName);
    }
}
