/*
 * Candle 2 — Machine Settings (GRBL $$ editor)
 */
#include "machinesettingsdialog.h"

#include <algorithm>

#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QTabWidget>
#include <QTableWidget>
#include <QTableWidgetItem>


// ---------------------------------------------------------------------------
// Static registry of known GRBL settings
// ---------------------------------------------------------------------------
//
// Covers the settings shipped by GRBL 1.1 and the additional ones common to
// GRBL-Advanced forks (same $N numbers, same semantics). Anything not listed
// here falls back to a generic text editor on the "Other" tab using the
// firmware's own description string — so adding new settings doesn't require
// a code change to make them editable.

namespace
{
    using Kind  = MachineSettingsDialog::Kind;
    using Group = MachineSettingsDialog::Group;

    struct Info
    {
        int     n;
        Group   group;
        Kind    kind;
        const char *label;
        const char *unit;   // optional suffix shown after the label
    };

    // Registry derived from GRBL-RV Report_GrblSettings() and Settings_StoreGlobalSetting().
    // $N numbers and semantics come directly from the firmware source — not from generic GRBL 1.1.
    static const Info kRegistry[] = {
        // ---- Control / invert ----
        {   0, Group::Motion,  Kind::Generic,     "Step pulse",                  "µs"       },
        {   2, Group::Invert,  Kind::BitmaskXYZ,  "Step port invert",            nullptr    },
        {   3, Group::Invert,  Kind::BitmaskXYZ,  "Direction port invert",       nullptr    },
        {   4, Group::Invert,  Kind::Bool,        "Step enable invert",          nullptr    },
        {   5, Group::Invert,  Kind::Bool,        "Limit pin invert",            nullptr    },
        {   6, Group::Invert,  Kind::Bool,        "Probe pin invert",            nullptr    },
        // ---- Motion ----
        {   1, Group::Motion,  Kind::Generic,     "Step idle delay",             "ms"       },
        {  11, Group::Motion,  Kind::Generic,     "Junction deviation",          "mm"       },
        {  12, Group::Motion,  Kind::Generic,     "Arc tolerance",               "mm"       },
        {  14, Group::Motion,  Kind::Generic,     "Tool change mode",            nullptr    },
        {  33, Group::Motion,  Kind::Bool,        "Lathe mode",                  nullptr    },
        {  35, Group::Motion,  Kind::Bool,        "Enable M7 mist coolant",      nullptr    },
        {  37, Group::Motion,  Kind::Bool,        "Backlash compensation",       nullptr    },
        {  38, Group::Motion,  Kind::Bool,        "Enable multi-axis",           nullptr    },
        {  41, Group::Motion,  Kind::Bool,        "Force init alarm",            nullptr    },
        // ---- Report ----
        {  10, Group::Report,  Kind::Generic,     "Status report mask",          nullptr    },
        {  13, Group::Report,  Kind::Bool,        "Report inches",               nullptr    },
        {  34, Group::Report,  Kind::Bool,        "Buffer sync on NVM write",    nullptr    },
        // ---- Limits ----
        {  20, Group::Limits,  Kind::Bool,        "Soft limits",                 nullptr    },
        {  21, Group::Limits,  Kind::Bool,        "Hard limits",                 nullptr    },
        {  36, Group::Limits,  Kind::Bool,        "Force hard limit check",      nullptr    },
        {  42, Group::Limits,  Kind::Bool,        "Check limits at init",        nullptr    },
        // ---- Homing ----
        {  22, Group::Homing,  Kind::Bool,        "Homing cycle enable",         nullptr    },
        {  23, Group::Homing,  Kind::BitmaskXYZ,  "Homing direction invert",     nullptr    },
        {  24, Group::Homing,  Kind::Generic,     "Homing feed",                 "mm/min"   },
        {  25, Group::Homing,  Kind::Generic,     "Homing seek",                 "mm/min"   },
        {  26, Group::Homing,  Kind::Generic,     "Homing debounce",             "ms"       },
        {  27, Group::Homing,  Kind::Generic,     "Homing pull-off",             "mm"       },
        {  39, Group::Homing,  Kind::Bool,        "Homing init lock",            nullptr    },
        {  40, Group::Homing,  Kind::Bool,        "Homing force set origin",     nullptr    },
        // ---- Spindle ----
        {  15, Group::Spindle, Kind::Generic,     "Encoder PPR",                 nullptr    },
        {  30, Group::Spindle, Kind::Generic,     "Max spindle speed",           "RPM"      },
        {  31, Group::Spindle, Kind::Generic,     "Min spindle speed",           "RPM"      },
        {  32, Group::Spindle, Kind::Bool,        "Laser mode",                  nullptr    },
        // ---- Axes — linear (X/Y/Z) ----
        { 100, Group::Axes,    Kind::Generic,     "X steps per mm",              "step/mm"  },
        { 101, Group::Axes,    Kind::Generic,     "Y steps per mm",              "step/mm"  },
        { 102, Group::Axes,    Kind::Generic,     "Z steps per mm",              "step/mm"  },
        { 110, Group::Axes,    Kind::Generic,     "X max rate",                  "mm/min"   },
        { 111, Group::Axes,    Kind::Generic,     "Y max rate",                  "mm/min"   },
        { 112, Group::Axes,    Kind::Generic,     "Z max rate",                  "mm/min"   },
        { 120, Group::Axes,    Kind::Generic,     "X acceleration",              "mm/s²"    },
        { 121, Group::Axes,    Kind::Generic,     "Y acceleration",              "mm/s²"    },
        { 122, Group::Axes,    Kind::Generic,     "Z acceleration",              "mm/s²"    },
        { 130, Group::Axes,    Kind::Generic,     "X max travel",                "mm"       },
        { 131, Group::Axes,    Kind::Generic,     "Y max travel",                "mm"       },
        { 132, Group::Axes,    Kind::Generic,     "Z max travel",                "mm"       },
        { 140, Group::Axes,    Kind::Generic,     "X backlash",                  "mm"       },
        { 141, Group::Axes,    Kind::Generic,     "Y backlash",                  "mm"       },
        { 142, Group::Axes,    Kind::Generic,     "Z backlash",                  "mm"       },
        // ---- Axes — rotary (A/B, only when multi-axis enabled) ----
        { 103, Group::Axes,    Kind::Generic,     "A steps per deg",             "step/°"   },
        { 104, Group::Axes,    Kind::Generic,     "B steps per deg",             "step/°"   },
        { 113, Group::Axes,    Kind::Generic,     "A max rate",                  "°/min"    },
        { 114, Group::Axes,    Kind::Generic,     "B max rate",                  "°/min"    },
        { 123, Group::Axes,    Kind::Generic,     "A acceleration",              "°/s²"     },
        { 124, Group::Axes,    Kind::Generic,     "B acceleration",              "°/s²"     },
        { 133, Group::Axes,    Kind::Generic,     "A max travel",                "°"        },
        { 134, Group::Axes,    Kind::Generic,     "B max travel",                "°"        },
        { 143, Group::Axes,    Kind::Generic,     "A backlash",                  "°"        },
        { 144, Group::Axes,    Kind::Generic,     "B backlash",                  "°"        },
    };

    const Info *lookup(int n)
    {
        for (const auto &info : kRegistry)
            if (info.n == n) return &info;
        return nullptr;
    }

    const char *groupTitle(Group g)
    {
        switch (g) {
        case Group::Motion:  return "Motion";
        case Group::Invert:  return "Invert";
        case Group::Report:  return "Report";
        case Group::Limits:  return "Limits";
        case Group::Homing:  return "Homing";
        case Group::Spindle: return "Spindle";
        case Group::Axes:    return "Axes";
        case Group::Other:   return "Other";
        }
        return "Other";
    }

    // Ordered list of groups — drives tab order.
    const Group kGroupOrder[] = {
        Group::Motion, Group::Invert, Group::Report, Group::Limits,
        Group::Homing, Group::Spindle, Group::Axes, Group::Other,
    };

    enum Column { ColN = 0, ColLabel, ColValue, ColCount };
}


// ---------------------------------------------------------------------------
// MachineSettingsDialog
// ---------------------------------------------------------------------------

MachineSettingsDialog::MachineSettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Machine Settings"));
    resize(620, 560);

    m_tabs = new QTabWidget(this);
    initTabs();

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
    root->addWidget(m_tabs);
    root->addWidget(m_lblStatus);
    root->addLayout(buttons);
}


void MachineSettingsDialog::initTabs()
{
    for (Group g : kGroupOrder)
    {
        auto *table = new QTableWidget(this);
        table->setColumnCount(ColCount);
        table->setHorizontalHeaderLabels({tr("#"), tr("Setting"), tr("Value")});
        table->verticalHeader()->setVisible(false);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers); // editors are always-on widgets
        table->horizontalHeader()->setSectionResizeMode(ColLabel, QHeaderView::Stretch);
        table->horizontalHeader()->setSectionResizeMode(ColN,     QHeaderView::ResizeToContents);
        table->horizontalHeader()->setSectionResizeMode(ColValue, QHeaderView::ResizeToContents);

        m_tablesByGroup[g] = table;
        m_tabs->addTab(table, tr(groupTitle(g)));
    }
}


void MachineSettingsDialog::beginRefresh()
{
    for (auto *table : m_tablesByGroup)
        table->setRowCount(0);
    m_rows.clear();
    clearErrors();
    setStatus(tr("Querying controller…"));
}


void MachineSettingsDialog::populate(const QString &joinedResponse)
{
    // Reset all tables + rows before re-populating. We preserve the tab order
    // and hide any tab whose group ends up empty for this controller.
    for (auto *table : m_tablesByGroup)
        table->setRowCount(0);
    m_rows.clear();

    static const QRegularExpression rxLine(
        R"(^\s*\$(\d+)\s*=\s*([^\s(]+)\s*(?:\(([^)]*)\))?\s*$)");

    const QStringList lines = joinedResponse.split(QRegularExpression("[;\\r\\n]"),
                                                   Qt::SkipEmptyParts);

    for (const QString &raw : lines)
    {
        const auto match = rxLine.match(raw.trimmed());
        if (!match.hasMatch())
            continue;

        Row row;
        row.n             = match.captured(1).toInt();
        row.originalValue = match.captured(2);

        const Info *info = lookup(row.n);
        if (info)
        {
            row.group = info->group;
            row.kind  = info->kind;
            row.label = info->unit
                ? QString("%1 (%2)").arg(info->label, info->unit)
                : QString::fromLatin1(info->label);
        }
        else
        {
            row.group = Group::Other;
            row.kind  = Kind::Generic;
            row.label = match.captured(3);   // fall back to firmware's own description
            if (row.label.isEmpty())
                row.label = tr("Unknown");
        }

        addRowToTab(row, match.captured(1), match.captured(3));
        m_rows.append(row);
    }

    // Hide empty tabs for a cleaner look on minimal firmwares.
    for (int i = m_tabs->count() - 1; i >= 0; --i)
    {
        auto *table = qobject_cast<QTableWidget*>(m_tabs->widget(i));
        if (table && table->rowCount() == 0)
            m_tabs->setTabVisible(i, false);
        else if (table)
            m_tabs->setTabVisible(i, true);
    }

    if (m_rows.isEmpty())
    {
        QString raw = joinedResponse.trimmed();
        if (raw.endsWith("; ok")) raw.chop(4);
        if (raw.isEmpty()) raw = tr("(empty)");

        setStatus(tr("No settings returned. Controller said: \"%1\".\n"
                     "If the machine is in Alarm state, unlock with $X in the "
                     "console (or home it), then click Refresh.").arg(raw), true);
    }
    else
    {
        setStatus(tr("Loaded %1 setting(s) across %2 tab(s). "
                     "Edit and click Apply — only changed rows are sent.")
                      .arg(m_rows.size())
                      .arg(std::count_if(std::begin(kGroupOrder), std::end(kGroupOrder),
                          [&](Group g) { return m_tablesByGroup.value(g)->rowCount() > 0; })));
    }
}


void MachineSettingsDialog::addRowToTab(Row &row, const QString &nStr, const QString &fwDesc)
{
    auto *table = m_tablesByGroup.value(row.group);
    if (!table)
        return;

    const int r = table->rowCount();
    table->insertRow(r);

    auto *itemN = new QTableWidgetItem(QString("$%1").arg(nStr));
    itemN->setTextAlignment(Qt::AlignCenter);
    itemN->setFlags(itemN->flags() & ~Qt::ItemIsEditable);
    table->setItem(r, ColN, itemN);

    auto *itemLabel = new QTableWidgetItem(row.label);
    itemLabel->setFlags(itemLabel->flags() & ~Qt::ItemIsEditable);
    if (!fwDesc.isEmpty() && fwDesc != row.label)
        itemLabel->setToolTip(fwDesc);  // keep firmware's description as a hover hint
    table->setItem(r, ColLabel, itemLabel);

    // Build the editor widget appropriate for this Kind.
    switch (row.kind) {
    case Kind::Bool:
    {
        auto *cb = new QCheckBox(table);
        cb->setChecked(row.originalValue.toInt() != 0);
        cb->setStyleSheet("margin-left: 6px;");
        row.checkBox = cb;
        table->setCellWidget(r, ColValue, cb);
        break;
    }
    case Kind::BitmaskXYZ:
    {
        const int value = row.originalValue.toInt();
        row.preservedHighBits = value & ~0b111;   // keep bits 3+ so we don't clobber A/B on forks

        auto *container = new QWidget(table);
        auto *lay = new QHBoxLayout(container);
        lay->setContentsMargins(4, 0, 4, 0);
        lay->setSpacing(8);

        const QStringList axes{"X", "Y", "Z"};
        for (int i = 0; i < axes.size(); ++i)
        {
            auto *cb = new QCheckBox(axes.at(i), container);
            cb->setChecked((value >> i) & 1);
            lay->addWidget(cb);
            row.bitChecks.append(cb);
        }
        lay->addStretch();
        table->setCellWidget(r, ColValue, container);
        break;
    }
    case Kind::Generic:
    {
        auto *edit = new QLineEdit(row.originalValue, table);
        edit->setAlignment(Qt::AlignRight);
        row.lineEdit = edit;
        table->setCellWidget(r, ColValue, edit);
        break;
    }
    }

    table->resizeRowToContents(r);
}


QString MachineSettingsDialog::currentValue(const Row &r) const
{
    switch (r.kind) {
    case Kind::Bool:
        return r.checkBox && r.checkBox->isChecked() ? QStringLiteral("1") : QStringLiteral("0");

    case Kind::BitmaskXYZ: {
        int value = r.preservedHighBits;
        for (int i = 0; i < r.bitChecks.size(); ++i)
            if (r.bitChecks.at(i)->isChecked())
                value |= (1 << i);
        return QString::number(value);
    }

    case Kind::Generic:
        return r.lineEdit ? r.lineEdit->text().trimmed() : r.originalValue;
    }
    return r.originalValue;
}


void MachineSettingsDialog::setStatus(const QString &msg, bool isError)
{
    m_lblStatus->setText(msg);
    m_lblStatus->setStyleSheet(isError ? QStringLiteral("color: red;") : QString());
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
    for (const Row &r : std::as_const(m_rows))
    {
        const QString current = currentValue(r);
        if (current == r.originalValue)
            continue;

        if (current.isEmpty())
        {
            setStatus(tr("$%1 has an empty value — fix or Refresh to revert.").arg(r.n), true);
            return;
        }

        changes.append({r.n, current});
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
