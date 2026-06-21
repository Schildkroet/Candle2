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
#include "frmmain.h"
#include "ui_frmmain.h"

static const double PROBE_BACKOFF_MM = 1.0;  // retract after fast touch
static const double PROBE_SLOW_EXTRA = 2.0;  // slow probe travel (backoff + margin)

void frmMain::sendProbeCommands(ProbeDirection dir, const QString &axis, double sign)
{
    m_probeDirection  = dir;
    m_probeSecondPass = false;

    double step     = ui->txtProbeStep->value();
    double feedFast = ui->txtProbeFeedFast->value();
    double feedSlow = ui->txtProbeFeedSlow->value();
    bool   twoPass  = ui->chkProbeTwoPass->isChecked();

    bool show = m_settings->showUICommands();

    // Fast probe
    sendCommand(
        QString("G38.2%1%2F%3")
            .arg(axis)
            .arg(sign * step, 0, 'f', 3)
            .arg(feedFast, 0, 'f', 0),
        -1, show);

    if (twoPass) {
        // Retract 1 mm away from surface using relative mode
        sendCommand(
            QString("G91%1%2G90")
                .arg(axis)
                .arg(-sign * PROBE_BACKOFF_MM, 0, 'f', 3),
            -1, show);
        // Slow precise probe
        sendCommand(
            QString("G38.2%1%2F%3")
                .arg(axis)
                .arg(sign * PROBE_SLOW_EXTRA, 0, 'f', 3)
                .arg(feedSlow, 0, 'f', 0),
            -1, show);
    }
}

void frmMain::on_btnProbeLeft_clicked()  { sendProbeCommands(ProbeLeft,  "X", -1.0); }
void frmMain::on_btnProbeRight_clicked() { sendProbeCommands(ProbeRight, "X", +1.0); }
void frmMain::on_btnProbeFront_clicked() { sendProbeCommands(ProbeFront, "Y", -1.0); }
void frmMain::on_btnProbeBack_clicked()  { sendProbeCommands(ProbeBack,  "Y", +1.0); }
void frmMain::on_btnProbeTop_clicked()   { sendProbeCommands(ProbeTop,   "Z", -1.0); }

void frmMain::on_btnProbeCenterX_clicked()
{
    double center = (m_probeXLeft + m_probeXRight) / 2.0;
    sendCommand(QString("G0X%1").arg(center, 0, 'f', 3),
                -1, m_settings->showUICommands());
}

void frmMain::on_btnProbeCenterY_clicked()
{
    double center = (m_probeYFront + m_probeYBack) / 2.0;
    sendCommand(QString("G0Y%1").arg(center, 0, 'f', 3),
                -1, m_settings->showUICommands());
}
