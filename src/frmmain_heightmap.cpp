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
#include <QFileDialog>

#include "frmmain.h"
#include "ui_frmmain.h"



void frmMain::resetHeightmap()
{
    delete m_heightMapInterpolationDrawer.data();
    m_heightMapInterpolationDrawer.setData(NULL);

    ui->tblHeightMap->setModel(NULL);
    m_heightMapModel.resize(1, 1);

    ui->txtHeightMap->clear();
    m_heightMapFileName.clear();
    m_heightMapChanged = false;
}


void frmMain::addRecentHeightmap(QString fileName)
{
    m_recentHeightmaps.removeAll(fileName);
    m_recentHeightmaps.append(fileName);
    if (m_recentHeightmaps.count() > 5)
    {
        m_recentHeightmaps.takeFirst();
    }
}

void frmMain::on_grpHeightMap_toggled(bool arg1)
{
    ui->widgetHeightMap->setVisible(arg1);
}

QRectF frmMain::borderRectFromTextboxes()
{
    QRectF rect;

    rect.setX(ui->txtHeightMapBorderX->value());
    rect.setY(ui->txtHeightMapBorderY->value());
    rect.setWidth(ui->txtHeightMapBorderWidth->value());
    rect.setHeight(ui->txtHeightMapBorderHeight->value());

    return rect;
}

QRectF frmMain::borderRectFromExtremes()
{
    QRectF rect;

    rect.setX(m_codeDrawer->getMinimumExtremes().x());
    rect.setY(m_codeDrawer->getMinimumExtremes().y());
    rect.setWidth(m_codeDrawer->getSizes().x());
    rect.setHeight(m_codeDrawer->getSizes().y());

    return rect;
}

void frmMain::updateHeightMapBorderDrawer()
{
    if (m_settingsLoading)
        return;

    qDebug() << "Updating border drawer";

    m_heightMapBorderDrawer.setBorderRect(borderRectFromTextboxes());
}

void frmMain::updateHeightMapGrid(double arg1)
{
    if (sender()->property("previousValue").toDouble() != arg1 && !updateHeightMapGrid())
        static_cast<QDoubleSpinBox*>(sender())->setValue(sender()->property("previousValue").toDouble());
    else
        sender()->setProperty("previousValue", arg1);
}

bool frmMain::updateHeightMapGrid()
{
    if (m_settingsLoading)
        return true;

    qDebug() << "Updating heightmap grid drawer";

    // Grid map changing warning
    bool nan = true;
    for (int i = 0; i < m_heightMapModel.rowCount(); i++)
        for (int j = 0; j < m_heightMapModel.columnCount(); j++)
            if (!qIsNaN(m_heightMapModel.data(m_heightMapModel.index(i, j), Qt::UserRole).toDouble()))
            {
                nan = false;
                break;
            }

    if (!nan && QMessageBox::warning(this, this->windowTitle(), tr("Changing grid settings will reset probe data. Continue?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        return false;

    // Update grid drawer
    QRectF borderRect = borderRectFromTextboxes();
    m_heightMapGridDrawer.setBorderRect(borderRect);
    m_heightMapGridDrawer.setGridSize(QPointF(ui->txtHeightMapGridX->value(), ui->txtHeightMapGridY->value()));
    m_heightMapGridDrawer.setZBottom(ui->txtHeightMapGridZBottom->value());
    m_heightMapGridDrawer.setZTop(ui->txtHeightMapGridZTop->value());

    // Reset model
    int gridPointsX = ui->txtHeightMapGridX->value();
    int gridPointsY = ui->txtHeightMapGridY->value();

    m_heightMapModel.resize(gridPointsX, gridPointsY);
    ui->tblHeightMap->setModel(NULL);
    ui->tblHeightMap->setModel(&m_heightMapModel);
    resizeTableHeightMapSections();

    // Update interpolation
    updateHeightMapInterpolationDrawer(true);

    // Generate probe program
    double gridStepX = gridPointsX > 1 ? borderRect.width() / (gridPointsX - 1) : 0;
    double gridStepY = gridPointsY > 1 ? borderRect.height() / (gridPointsY - 1) : 0;

    qDebug() << "Generating probe program";

    m_programLoading = true;
    m_probeModel.clear();
    m_probeModel.insertRow(0);

    m_probeModel.setData(m_probeModel.index(m_probeModel.rowCount() - 1, 1), QString("G21G90F%1G0Z%2").
                         arg(m_settings->heightmapProbingFeed()).arg(ui->txtHeightMapGridZTop->value()));
    m_probeModel.setData(m_probeModel.index(m_probeModel.rowCount() - 1, 1), QString("G0X0Y0F100"));
    m_probeModel.setData(m_probeModel.index(m_probeModel.rowCount() - 1, 1), QString("G38.2Z%1F10")
                         .arg(ui->txtHeightMapGridZBottom->value()));
    m_probeModel.setData(m_probeModel.index(m_probeModel.rowCount() - 1, 1), QString("G0Z%1F100")
                         .arg(ui->txtHeightMapGridZTop->value()));

    double x, y;

    for (int i = 0; i < gridPointsY; i++)
    {
        y = borderRect.top() + gridStepY * i;

        for (int j = 0; j < gridPointsX; j++)
        {
            x = borderRect.left() + gridStepX * (i % 2 ? gridPointsX - 1 - j : j);
            m_probeModel.setData(m_probeModel.index(m_probeModel.rowCount() - 1, 1), QString("G0X%1Y%2F100")
                                 .arg(x, 0, 'f', 3).arg(y, 0, 'f', 3));
            m_probeModel.setData(m_probeModel.index(m_probeModel.rowCount() - 1, 1), QString("G38.2Z%1F10")
                                 .arg(ui->txtHeightMapGridZBottom->value()));
            m_probeModel.setData(m_probeModel.index(m_probeModel.rowCount() - 1, 1), QString("G0Z%1F100")
                                 .arg(ui->txtHeightMapGridZTop->value()));
        }
    }

    m_programLoading = false;

    if (m_currentDrawer == m_probeDrawer)
        updateParser();

    m_heightMapChanged = true;

    return true;
}

void frmMain::updateHeightMapInterpolationDrawer(bool reset)
{
    if (m_settingsLoading)
        return;

    qDebug() << "Updating interpolation";

    QRectF borderRect = borderRectFromTextboxes();
    m_heightMapInterpolationDrawer.setBorderRect(borderRect);

    QVector<QVector<double>> *interpolationData = new QVector<QVector<double>>;

    int interpolationPointsX = ui->txtHeightMapInterpolationStepX->value();// * (ui->txtHeightMapGridX->value() - 1) + 1;
    int interpolationPointsY = ui->txtHeightMapInterpolationStepY->value();// * (ui->txtHeightMapGridY->value() - 1) + 1;

    double interpolationStepX = interpolationPointsX > 1 ? borderRect.width() / (interpolationPointsX - 1) : 0;
    double interpolationStepY = interpolationPointsY > 1 ? borderRect.height() / (interpolationPointsY - 1) : 0;

    for (int i = 0; i < interpolationPointsY; i++)
    {
        QVector<double> row;
        for (int j = 0; j < interpolationPointsX; j++)
        {
            double x = interpolationStepX * j + borderRect.x();
            double y = interpolationStepY * i + borderRect.y();

            row.append(reset ? qQNaN() : Interpolation::bicubicInterpolate(borderRect, &m_heightMapModel, x, y));
        }

        interpolationData->append(row);
    }

    if (m_heightMapInterpolationDrawer.data() != NULL)
    {
        delete m_heightMapInterpolationDrawer.data();
    }

    m_heightMapInterpolationDrawer.setData(interpolationData);

    // Update grid drawer
    m_heightMapGridDrawer.update();

    // Heightmap changed by table user input
    if (sender() == &m_heightMapModel)
        m_heightMapChanged = true;

    // Reset heightmapped program model
    m_programHeightmapModel.clear();
}

void frmMain::on_chkHeightMapBorderShow_toggled(bool checked)
{
    Q_UNUSED(checked)

    updateControlsState();
}

void frmMain::on_txtHeightMapBorderX_valueChanged(double arg1)
{
    updateHeightMapBorderDrawer();
    updateHeightMapGrid(arg1);
}

void frmMain::on_txtHeightMapBorderWidth_valueChanged(double arg1)
{
    updateHeightMapBorderDrawer();
    updateHeightMapGrid(arg1);
}

void frmMain::on_txtHeightMapBorderY_valueChanged(double arg1)
{
    updateHeightMapBorderDrawer();
    updateHeightMapGrid(arg1);
}

void frmMain::on_txtHeightMapBorderHeight_valueChanged(double arg1)
{
    updateHeightMapBorderDrawer();
    updateHeightMapGrid(arg1);
}

void frmMain::on_chkHeightMapGridShow_toggled(bool checked)
{
    Q_UNUSED(checked)

    updateControlsState();
}

void frmMain::on_txtHeightMapGridX_valueChanged(double arg1)
{
    updateHeightMapGrid(arg1);
}

void frmMain::on_txtHeightMapGridY_valueChanged(double arg1)
{
    updateHeightMapGrid(arg1);
}

void frmMain::on_txtHeightMapGridZBottom_valueChanged(double arg1)
{
    updateHeightMapGrid(arg1);
}

void frmMain::on_txtHeightMapGridZTop_valueChanged(double arg1)
{
    updateHeightMapGrid(arg1);
}

void frmMain::on_cmdHeightMapMode_toggled(bool checked)
{
    // Update flag
    m_heightMapMode = checked;

    // Reset file progress
    m_fileCommandIndex = 0;
    m_fileProcessedCommandIndex = 0;
    m_lastDrawnLineIndex = 0;

    // Reset/restore g-code program modification on edit mode enter/exit
    if (ui->chkHeightMapUse->isChecked())
    {
        on_chkHeightMapUse_clicked(!checked); // Update gcode program parser
    }

    if (checked)
    {
        ui->tblProgram->setModel(&m_probeModel);
        resizeTableHeightMapSections();
        m_currentModel = &m_probeModel;
        m_currentDrawer = m_probeDrawer;
        updateParser();  // Update probe program parser
    }
    else
    {
        m_probeParser.reset();

        if (!ui->chkHeightMapUse->isChecked())
        {
            ui->tblProgram->setModel(&m_programModel);
            connect(ui->tblProgram->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCurrentChanged(QModelIndex,QModelIndex)));
            ui->tblProgram->selectRow(0);

            resizeTableHeightMapSections();
            m_currentModel = &m_programModel;
            m_currentDrawer = m_codeDrawer;

            if (!ui->chkHeightMapUse->isChecked())
            {
                ui->glwVisualizer->updateExtremes(m_codeDrawer);
                updateProgramEstimatedTime(m_currentDrawer->viewParser()->getLineSegmentList());
            }
        }
    }

    // Shadow toolpath
    QList<LineSegment*> list = m_viewParser.getLineSegmentList();
    QList<int> indexes;
    for (int i = m_lastDrawnLineIndex; i < list.count(); i++)
    {
        list[i]->setDrawn(checked);
        list[i]->setIsHightlight(false);
        indexes.append(i);
    }

    // Update only vertex color.
    // If chkHeightMapUse was checked codeDrawer updated via updateParser
    if (!ui->chkHeightMapUse->isChecked())
        m_codeDrawer->update(indexes);

    updateRecentFilesMenu();
    updateControlsState();
}

bool frmMain::saveHeightMap(QString fileName)
{
    QFile file(fileName);
    QDir dir;

    if (file.exists())
        dir.remove(file.fileName());

    if (!file.open(QIODevice::WriteOnly))
        return false;

    QTextStream textStream(&file);
    textStream << ui->txtHeightMapBorderX->text() << ";"
               << ui->txtHeightMapBorderY->text() << ";"
               << ui->txtHeightMapBorderWidth->text() << ";"
               << ui->txtHeightMapBorderHeight->text() << "\r\n";
    textStream << ui->txtHeightMapGridX->text() << ";"
               << ui->txtHeightMapGridY->text() << ";"
               << ui->txtHeightMapGridZBottom->text() << ";"
               << ui->txtHeightMapGridZTop->text() << "\r\n";
    textStream << ui->cboHeightMapInterpolationType->currentIndex() << ";"
               << ui->txtHeightMapInterpolationStepX->text() << ";"
                << ui->txtHeightMapInterpolationStepY->text() << "\r\n";

    for (int i = 0; i < m_heightMapModel.rowCount(); i++)
    {
        for (int j = 0; j < m_heightMapModel.columnCount(); j++)
        {
            textStream << m_heightMapModel.data(m_heightMapModel.index(i, j), Qt::UserRole).toString() << ((j == m_heightMapModel.columnCount() - 1) ? "" : ";");
        }
        textStream << "\r\n";
    }

    file.close();

    m_heightMapChanged = false;

    return true;
}

void frmMain::loadHeightMap(QString fileName)
{
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, this->windowTitle(), tr("Can't open file:\n") + fileName);
        return;
    }

    QTextStream textStream(&file);

    m_settingsLoading = true;

    // Storing previous values
    ui->txtHeightMapBorderX->setValue(qQNaN());
    ui->txtHeightMapBorderY->setValue(qQNaN());
    ui->txtHeightMapBorderWidth->setValue(qQNaN());
    ui->txtHeightMapBorderHeight->setValue(qQNaN());

    ui->txtHeightMapGridX->setValue(qQNaN());
    ui->txtHeightMapGridY->setValue(qQNaN());
    ui->txtHeightMapGridZBottom->setValue(qQNaN());
    ui->txtHeightMapGridZTop->setValue(qQNaN());

    QList<QString> list = textStream.readLine().split(";");
    ui->txtHeightMapBorderX->setValue(list[0].toDouble());
    ui->txtHeightMapBorderY->setValue(list[1].toDouble());
    ui->txtHeightMapBorderWidth->setValue(list[2].toDouble());
    ui->txtHeightMapBorderHeight->setValue(list[3].toDouble());

    list = textStream.readLine().split(";");
    ui->txtHeightMapGridX->setValue(list[0].toDouble());
    ui->txtHeightMapGridY->setValue(list[1].toDouble());
    ui->txtHeightMapGridZBottom->setValue(list[2].toDouble());
    ui->txtHeightMapGridZTop->setValue(list[3].toDouble());

    m_settingsLoading = false;

    updateHeightMapBorderDrawer();

    m_heightMapModel.clear();   // To avoid probe data wipe message
    updateHeightMapGrid();

    list = textStream.readLine().split(";");

    for (int i = 0; i < m_heightMapModel.rowCount(); i++)
    {
        QList<QString> row = textStream.readLine().split(";");
        for (int j = 0; j < m_heightMapModel.columnCount(); j++)
        {
            m_heightMapModel.setData(m_heightMapModel.index(i, j), row[j].toDouble(), Qt::UserRole);
        }
    }

    file.close();

    ui->txtHeightMap->setText(fileName.mid(fileName.lastIndexOf("/") + 1));
    m_heightMapFileName = fileName;
    m_heightMapChanged = false;

    ui->cboHeightMapInterpolationType->setCurrentIndex(list[0].toInt());
    ui->txtHeightMapInterpolationStepX->setValue(list[1].toDouble());
    ui->txtHeightMapInterpolationStepY->setValue(list[2].toDouble());

    updateHeightMapInterpolationDrawer();
}

void frmMain::on_chkHeightMapInterpolationShow_toggled(bool checked)
{
    Q_UNUSED(checked)

    updateControlsState();
}

void frmMain::on_cmdHeightMapLoad_clicked()
{
    if (!saveChanges(true))
    {
        return;
    }

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open"), m_lastFolder, tr("Heightmap files (*.map)"));

    if (fileName != "")
    {
        addRecentHeightmap(fileName);
        loadHeightMap(fileName);

        // If using heightmap
        if (ui->chkHeightMapUse->isChecked() && !m_heightMapMode)
        {
            // Restore original file
            on_chkHeightMapUse_clicked(false);
            // Apply heightmap
            on_chkHeightMapUse_clicked(true);
        }

        updateRecentFilesMenu();
        updateControlsState(); // Enable 'cmdHeightMapMode' button
    }
}

void frmMain::on_txtHeightMapInterpolationStepX_valueChanged(double arg1)
{
    Q_UNUSED(arg1)

    updateHeightMapInterpolationDrawer();
}

void frmMain::on_txtHeightMapInterpolationStepY_valueChanged(double arg1)
{
    Q_UNUSED(arg1)

    updateHeightMapInterpolationDrawer();
}

void frmMain::on_chkHeightMapUse_clicked(bool checked)
{
    // Reset table view
    QByteArray headerState = ui->tblProgram->horizontalHeader()->saveState();
    ui->tblProgram->setModel(NULL);

    CancelException cancel;

    if (checked)
        try {

        // Prepare progress dialog
        QProgressDialog progress(tr("Applying heightmap..."), tr("Abort"), 0, 0, this);
        progress.setWindowModality(Qt::WindowModal);
        progress.setFixedHeight(progress.sizeHint().height());
        progress.show();
        progress.setStyleSheet("QProgressBar {text-align: center; qproperty-format: \"\"}");

        // Performance test
        QTime time;

        // Set current model to prevent reseting heightmap cache
        m_currentModel = &m_programHeightmapModel;

        // Update heightmap-modificated program if not cached
        if (m_programHeightmapModel.rowCount() == 0)
        {
            // Modifying linesegments
            QList<LineSegment*> *list = m_viewParser.getLines();
            QRectF borderRect = borderRectFromTextboxes();
            double x, y, z;
            QVector3D point;

            progress.setLabelText(tr("Subdividing segments..."));
            progress.setMaximum(list->count() - 1);
            time.start();

            for (int i = 0; i < list->count(); i++)
            {
                if (!list->at(i)->isZMovement())
                {
                    QList<LineSegment*> subSegments = subdivideSegment(list->at(i));

                    if (subSegments.count() > 0)
                    {
                        delete list->at(i);
                        list->removeAt(i);
                        foreach (LineSegment* subSegment, subSegments) list->insert(i++, subSegment);
                        i--;
                    }
                }

                if (progress.isVisible() && (i % PROGRESSSTEP == 0))
                {
                    progress.setMaximum(list->count() - 1);
                    progress.setValue(i);
                    qApp->processEvents();

                    if (progress.wasCanceled())
                        throw cancel;
                }
            }

            qDebug() << "Subdivide time: " << time.elapsed();

            progress.setLabelText(tr("Updating Z-coordinates..."));
            progress.setMaximum(list->count() - 1);
            time.start();

            for (int i = 0; i < list->count(); i++)
            {
                if (i == 0)
                {
                    x = list->at(i)->getStart().x();
                    y = list->at(i)->getStart().y();
                    z = list->at(i)->getStart().z() + Interpolation::bicubicInterpolate(borderRect, &m_heightMapModel, x, y);
                    list->at(i)->setStart(QVector3D(x, y, z));
                }
                else
                    list->at(i)->setStart(list->at(i - 1)->getEnd());

                x = list->at(i)->getEnd().x();
                y = list->at(i)->getEnd().y();
                z = list->at(i)->getEnd().z() + Interpolation::bicubicInterpolate(borderRect, &m_heightMapModel, x, y);
                list->at(i)->setEnd(QVector3D(x, y, z));

                if (progress.isVisible() && (i % PROGRESSSTEP == 0))
                {
                    progress.setValue(i);
                    qApp->processEvents();

                    if (progress.wasCanceled())
                        throw cancel;
                }
            }

            qDebug() << "Z update time (interpolation): " << time.elapsed();

            progress.setLabelText(tr("Modifying G-code program..."));
            progress.setMaximum(m_programModel.rowCount() - 2);
            time.start();

            // Modifying g-code program
            QString command;
            QStringList args;
            int line;
            QString newCommand;
            GCodeItem item;
            int lastSegmentIndex = 0;
            int lastCommandIndex = -1;

            // Search strings
            QString coords("XxYyZzIiJjKkRr");
            QString g("Gg");
            QString m("Mm");

            char codeChar;          // Single code char G1 -> G
            float codeNum;          // Code number      G1 -> 1

            QString lastCode;
            bool isLinearMove;
            bool hasCommand;
            bool isMachineCoords;

            m_programLoading = true;
            for (int i = 0; i < m_programModel.rowCount() - 1; i++) {
                command = m_programModel.data().at(i).command;
                line = m_programModel.data().at(i).line;
                isLinearMove = false;
                hasCommand = false;
                isMachineCoords = false;

                if (line < 0 || line == lastCommandIndex || lastSegmentIndex == list->count() - 1) {
                    item.command = command;
                    m_programHeightmapModel.data().append(item);
                } else {
                    // Get commands args
                    args = m_programModel.data().at(i).args;
                    newCommand.clear();

                    // Parse command args
                    foreach (QString arg, args) {                   // arg examples: G1, G2, M3, X100...
                        codeChar = arg.at(0).toLatin1();            // codeChar: G, M, X...
                        if (!coords.contains(codeChar)) {           // Not parameter
                            codeNum = arg.mid(1).toDouble();
                            if (g.contains(codeChar)) {             // 'G'-command
                                // Store 'G0' & 'G1'
                                if (codeNum == 0.0f || codeNum == 1.0f) {
                                    lastCode = arg;
                                    isLinearMove = true;            // Store linear move
                                }

                                // Replace 'G2' & 'G3' with 'G1'
                                if (codeNum == 2.0f || codeNum == 3.0f) {
                                    newCommand.append("G1");
                                    isLinearMove = true;
                                // Drop plane command for arcs
                                } else if (codeNum != 17.0f && codeNum != 18.0f && codeNum != 19.0f) {
                                    newCommand.append(arg);
                                }

                                if (codeNum == 53.0f) {
                                    isMachineCoords = true;
                                }

                                hasCommand = true;                  // Command has 'G'
                            } else {
                                if (m.contains(codeChar))
                                    hasCommand = true;              // Command has 'M'
                                newCommand.append(arg);       // Other commands
                            }
                        }
                    }

                    // Find first linesegment by command index
                    for (int j = lastSegmentIndex; j < list->count(); j++) {
                        if (list->at(j)->getLineNumber() == line) {
                            if (!qIsNaN(list->at(j)->getEnd().length()) && ((isLinearMove && !isMachineCoords) || (!hasCommand && !lastCode.isEmpty()))) {
                                // Create new commands for each linesegment with given command index
                                while ((j < list->count()) && (list->at(j)->getLineNumber() == line)) {

                                    point = list->at(j)->getEnd();
                                    if (!list->at(j)->isAbsolute()) point -= list->at(j)->getStart();
                                    if (!list->at(j)->isMetric()) point /= 25.4;

                                    item.command = newCommand + QString("X%1Y%2Z%3")
                                            .arg(point.x(), 0, 'f', 3).arg(point.y(), 0, 'f', 3).arg(point.z(), 0, 'f', 3);
                                    m_programHeightmapModel.data().append(item);

                                    if (!newCommand.isEmpty()) newCommand.clear();
                                    j++;
                                }
                            // Copy original command if not G0 or G1
                            } else {
                                item.command = command;
                                m_programHeightmapModel.data().append(item);
                            }

                            lastSegmentIndex = j;
                            break;
                        }
                    }
                }
                lastCommandIndex = line;

                if (progress.isVisible() && (i % PROGRESSSTEP == 0)) {
                    progress.setValue(i);
                    qApp->processEvents();
                    if (progress.wasCanceled()) throw cancel;
                }
            }
            m_programHeightmapModel.insertRow(m_programHeightmapModel.rowCount());
        }
        progress.close();

        qDebug() << "Program modification time: " << time.elapsed();

        ui->tblProgram->setModel(&m_programHeightmapModel);
        ui->tblProgram->horizontalHeader()->restoreState(headerState);

        connect(ui->tblProgram->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCurrentChanged(QModelIndex,QModelIndex)));

        m_programLoading = false;

        // Update parser
        m_currentDrawer = m_codeDrawer;
        updateParser();

        // Select first row
        ui->tblProgram->selectRow(0);
    }
    catch (CancelException)
    {
        // Cancel modification
        m_programHeightmapModel.clear();
        m_currentModel = &m_programModel;

        ui->tblProgram->setModel(&m_programModel);
        ui->tblProgram->horizontalHeader()->restoreState(headerState);

        connect(ui->tblProgram->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCurrentChanged(QModelIndex,QModelIndex)));
        ui->tblProgram->selectRow(0);

        ui->chkHeightMapUse->setChecked(false);

        return;
    }
    else
    {                                        // Restore original program
        m_currentModel = &m_programModel;

        ui->tblProgram->setModel(&m_programModel);
        ui->tblProgram->horizontalHeader()->restoreState(headerState);

        connect(ui->tblProgram->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCurrentChanged(QModelIndex,QModelIndex)));

        // Store changes flag
        bool fileChanged = m_fileChanged;

        // Update parser
        updateParser();

        // Select first row
        ui->tblProgram->selectRow(0);

        // Restore changes flag
        m_fileChanged = fileChanged;
    }

    // Update groupbox title
    ui->grpHeightMap->setProperty("overrided", checked);
    style()->unpolish(ui->grpHeightMap);
    ui->grpHeightMap->ensurePolished();

    // Update menu
    ui->actFileSaveTransformedAs->setVisible(checked);
}

QList<LineSegment*> frmMain::subdivideSegment(LineSegment* segment)
{
    QList<LineSegment*> list;

    QRectF borderRect = borderRectFromTextboxes();

    double interpolationStepX = borderRect.width() / (ui->txtHeightMapInterpolationStepX->value() - 1);
    double interpolationStepY = borderRect.height() / (ui->txtHeightMapInterpolationStepY->value() - 1);

    double length;

    QVector3D vec = segment->getEnd() - segment->getStart();

    if (qIsNaN(vec.length()))
        return QList<LineSegment*>();

    if (fabs(vec.x()) / fabs(vec.y()) < interpolationStepX / interpolationStepY)
        length = interpolationStepY / (vec.y() / vec.length());
    else
        length = interpolationStepX / (vec.x() / vec.length());

    length = fabs(length);

    if (qIsNaN(length))
    {
        qDebug() << "ERROR length:" << segment->getStart() << segment->getEnd();

        return QList<LineSegment*>();
    }

    QVector3D seg = vec.normalized() * length;
    int count = trunc(vec.length() / length);

    if (count == 0) return QList<LineSegment*>();

    for (int i = 0; i < count; i++)
    {
        LineSegment* line = new LineSegment(segment);
        line->setStart(i == 0 ? segment->getStart() : list[i - 1]->getEnd());
        line->setEnd(line->getStart() + seg);
        list.append(line);
    }

    if (list.count() > 0 && list.last()->getEnd() != segment->getEnd())
    {
        LineSegment* line = new LineSegment(segment);
        line->setStart(list.last()->getEnd());
        line->setEnd(segment->getEnd());
        list.append(line);
    }

    return list;
}

void frmMain::on_cmdHeightMapCreate_clicked()
{
    ui->cmdHeightMapMode->setChecked(true);
    on_actFileNew_triggered();
}

void frmMain::on_cmdHeightMapBorderAuto_clicked()
{
    QRectF rect = borderRectFromExtremes();

    if (!qIsNaN(rect.width()) && !qIsNaN(rect.height()))
    {
        ui->txtHeightMapBorderX->setValue(rect.x());
        ui->txtHeightMapBorderY->setValue(rect.y());
        ui->txtHeightMapBorderWidth->setValue(rect.width());
        ui->txtHeightMapBorderHeight->setValue(rect.height());
    }
}
