/*
 * Candle 2 — Machine Settings (GRBL $$ editor)
 *
 * Editor for GRBL firmware settings. Parses the controller's `$$` reply,
 * presents each entry in a tabbed, grouped UI with friendly labels and
 * type-appropriate editors (checkbox for booleans, XYZ checkboxes for
 * invert bitmasks, plain text for numbers and unknown entries). Writes
 * back only changed values via `$N=value` commands.
 *
 * Unknown `$N` values fall back to the firmware's own description on an
 * "Other" tab — this keeps the dialog forward-compatible with GRBL-Advanced,
 * FluidNC, and any fork that adds settings we don't know about yet.
 */
#ifndef MACHINESETTINGSDIALOG_H
#define MACHINESETTINGSDIALOG_H

#include <QDialog>
#include <QHash>
#include <QList>
#include <QMap>
#include <QPair>
#include <QString>
#include <QStringList>

class QCheckBox;
class QLineEdit;
class QPushButton;
class QLabel;
class QTabWidget;
class QTableWidget;
class QWidget;

class MachineSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    // Kind + Group are public so the static registry in the .cpp (which lives
    // in an anonymous namespace) can reference them. They're descriptive of
    // each setting's editor behavior, not implementation state worth hiding.
    enum class Kind
    {
        Generic,      // plain text — numbers, bitmasks we don't know about
        Bool,         // 0/1 → QCheckBox
        BitmaskXYZ,   // low three bits → X/Y/Z checkboxes; higher bits preserved
    };

    enum class Group
    {
        Motion, Invert, Report, Limits, Homing, Spindle, Axes, Other
    };

    explicit MachineSettingsDialog(QWidget *parent = nullptr);

    // Clear the tables and show a "loading" hint. Call before emitting the $$
    // query so the dialog visibly reflects in-flight state.
    void beginRefresh();

    // Parse the joined response string (as produced by frmMain's response
    // accumulator, semicolon-separated) and populate the editor. Safe to call
    // multiple times — replaces existing rows.
    void populate(const QString &joinedResponse);

    // Update the status strip at the bottom of the dialog (e.g. after Apply).
    void setStatus(const QString &msg, bool isError = false);

    // Append an error line to the status strip. Used when multiple $N=value
    // writes may each emit their own error — we want every one visible, not
    // just the last one.
    void appendError(const QString &msg);

    // Discard any accumulated error lines. Called at the start of Apply /
    // Refresh / Restore so old errors don't bleed into the next cycle.
    void clearErrors();

signals:
    // User clicked Refresh (or dialog needs a fresh $$ dump).
    void refreshRequested();

    // User clicked Apply. List contains (settingNumber, newValue) for rows
    // whose value differs from what $$ originally reported.
    void applyRequested(const QList<QPair<int, QString>> &changes);

    // User clicked "Restore Defaults" and confirmed. Caller should send
    // `$RST=$` then re-query.
    void restoreDefaultsRequested();

private slots:
    void onRefreshClicked();
    void onApplyClicked();
    void onRestoreDefaultsClicked();

private:
    struct Row
    {
        int     n;
        QString originalValue;
        QString label;          // friendly label if known, else firmware description
        Kind    kind;
        Group   group;

        // Editor widget — exactly one of these is used depending on kind.
        QLineEdit                *lineEdit = nullptr;
        QCheckBox                *checkBox = nullptr;
        QList<QCheckBox*>         bitChecks;          // size 3 for BitmaskXYZ (X/Y/Z)
        int                       preservedHighBits = 0;  // bitmask bits 3+, round-tripped as-is
    };

    void initTabs();
    void addRowToTab(Row &row, const QString &n, const QString &description);
    QString currentValue(const Row &r) const;

    QTabWidget   *m_tabs;
    QMap<Group, QTableWidget*> m_tablesByGroup;

    QPushButton  *m_btnRefresh;
    QPushButton  *m_btnApply;
    QPushButton  *m_btnRestoreDefaults;
    QPushButton  *m_btnClose;
    QLabel       *m_lblStatus;

    QList<Row>    m_rows;
    QStringList   m_accumulatedErrors;
};

#endif // MACHINESETTINGSDIALOG_H
