/*
 * Candle 2 — Machine Settings (GRBL $$ editor)
 */
#include "machinesettingsdialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QTableWidget>
#include <QTableWidgetItem>

namespace
{
    enum Column { ColN = 0, ColDescription, ColValue, ColCount };
}


MachineSettingsDialog::MachineSettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Machine Settings"));
    resize(560, 520);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(ColCount);
    m_table->setHorizontalHeaderLabels({tr("#"), tr("Description"), tr("Value")});
    m_table->verticalHeader()->setVisible(false);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::DoubleClicked |
                             QAbstractItemView::SelectedClicked |
                             QAbstractItemView::EditKeyPressed |
                             QAbstractItemView::AnyKeyPressed);
    m_table->horizontalHeader()->setSectionResizeMode(ColDescription, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(ColN, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(ColValue, QHeaderView::ResizeToContents);

    m_lblStatus = new QLabel(this);
    m_lblStatus->setWordWrap(true);

    m_btnRefresh         = new QPushButton(tr("&Refresh"), this);
    m_btnApply           = new QPushButton(tr("&Apply"), this);
    m_btnRestoreDefaults = new QPushButton(tr("Restore &Defaults"), this);
    m_btnClose           = new QPushButton(tr("&Close"), this);

    m_btnClose->setDefault(true);

    connect(m_btnRefresh,         &QPushButton::clicked, this, &MachineSettingsDialog::onRefreshClicked);
    connect(m_btnApply,           &QPushButton::clicked, this, &MachineSettingsDialog::onApplyClicked);
    connect(m_btnRestoreDefaults, &QPushButton::clicked, this, &MachineSettingsDialog::onRestoreDefaultsClicked);
    connect(m_btnClose,           &QPushButton::clicked, this, &QDialog::accept);

    auto *buttons = new QHBoxLayout;
    buttons->addWidget(m_btnRefresh);
    buttons->addWidget(m_btnRestoreDefaults);
    buttons->addStretch();
    buttons->addWidget(m_btnApply);
    buttons->addWidget(m_btnClose);

    auto *root = new QVBoxLayout(this);
    root->addWidget(m_table);
    root->addWidget(m_lblStatus);
    root->addLayout(buttons);
}


void MachineSettingsDialog::beginRefresh()
{
    m_rows.clear();
    m_table->setRowCount(0);
    clearErrors();
    setStatus(tr("Querying controller…"));
}


void MachineSettingsDialog::appendError(const QString &msg)
{
    m_accumulatedErrors.append(msg);
    setStatus(m_accumulatedErrors.join('\n'), true);
}


void MachineSettingsDialog::clearErrors()
{
    m_accumulatedErrors.clear();
}


void MachineSettingsDialog::populate(const QString &joinedResponse)
{
    m_rows.clear();
    m_table->setRowCount(0);

    // frmMain concatenates response lines with "; ". Normalize to newlines,
    // then pull each "$N=value (description)" off independently so stray "ok"
    // markers and blank lines don't confuse the parser.
    static const QRegularExpression rxLine(
        R"(^\s*\$(\d+)\s*=\s*([^\s(]+)\s*(?:\(([^)]*)\))?\s*$)");

    const QStringList lines = joinedResponse.split(QRegularExpression("[;\\r\\n]"),
                                                   Qt::SkipEmptyParts);

    for (const QString &raw : lines)
    {
        const auto m = rxLine.match(raw.trimmed());
        if (!m.hasMatch())
            continue;

        Row row;
        row.n             = m.captured(1).toInt();
        row.originalValue = m.captured(2);
        row.description   = m.captured(3);
        m_rows.append(row);
    }

    m_table->setRowCount(m_rows.size());
    for (int i = 0; i < m_rows.size(); ++i)
    {
        const Row &r = m_rows.at(i);

        auto *itemN = new QTableWidgetItem(QString("$%1").arg(r.n));
        itemN->setFlags(itemN->flags() & ~Qt::ItemIsEditable);
        itemN->setTextAlignment(Qt::AlignCenter);

        auto *itemDesc = new QTableWidgetItem(r.description);
        itemDesc->setFlags(itemDesc->flags() & ~Qt::ItemIsEditable);

        auto *itemValue = new QTableWidgetItem(r.originalValue);
        itemValue->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        m_table->setItem(i, ColN,           itemN);
        m_table->setItem(i, ColDescription, itemDesc);
        m_table->setItem(i, ColValue,       itemValue);
    }

    if (m_rows.isEmpty())
    {
        // Nothing parsed. Either the controller isn't a `$$`-compatible GRBL,
        // or it rejected the query (typical cause: ALARM state — the firmware
        // silently drops $$ until cleared via $X).
        QString raw = joinedResponse.trimmed();
        if (raw.endsWith("; ok")) raw.chop(4);
        if (raw.isEmpty()) raw = tr("(empty)");

        setStatus(tr("No settings returned. Controller said: \"%1\".\n"
                     "If the machine is in Alarm state, unlock with $X in the "
                     "console (or home it), then click Refresh.").arg(raw), true);
    }
    else
    {
        setStatus(tr("Loaded %1 setting(s). Edit the Value column, then click Apply.")
                      .arg(m_rows.size()));
    }
}


void MachineSettingsDialog::setStatus(const QString &msg, bool isError)
{
    m_lblStatus->setText(msg);
    m_lblStatus->setStyleSheet(isError ? QStringLiteral("color: red;") : QString());
}


void MachineSettingsDialog::onRefreshClicked()
{
    emit refreshRequested();
}


void MachineSettingsDialog::onApplyClicked()
{
    if (m_rows.isEmpty())
    {
        setStatus(tr("Nothing loaded yet — click Refresh."), true);
        return;
    }

    QList<QPair<int, QString>> changes;
    for (int i = 0; i < m_rows.size(); ++i)
    {
        const QString current = m_table->item(i, ColValue)->text().trimmed();
        if (current == m_rows.at(i).originalValue)
            continue;

        if (current.isEmpty())
        {
            setStatus(tr("$%1 has an empty value — fix or Refresh to revert.").arg(m_rows.at(i).n), true);
            return;
        }

        changes.append({m_rows.at(i).n, current});
    }

    if (changes.isEmpty())
    {
        setStatus(tr("No changes to apply."));
        return;
    }

    setStatus(tr("Applying %1 change(s)…").arg(changes.size()));
    emit applyRequested(changes);
}


void MachineSettingsDialog::onRestoreDefaultsClicked()
{
    const auto answer = QMessageBox::warning(
        this,
        tr("Restore Defaults"),
        tr("Reset all machine settings to the firmware defaults?\n\n"
           "This sends $RST=$ to the controller and cannot be undone."),
        QMessageBox::Yes | QMessageBox::Cancel,
        QMessageBox::Cancel);

    if (answer != QMessageBox::Yes)
        return;

    setStatus(tr("Restoring defaults…"));
    emit restoreDefaultsRequested();
}
