/*
 * Candle 2 — Machine Settings (GRBL $$ editor)
 *
 * Dynamic editor for GRBL firmware settings. Parses the controller's response
 * to `$$`, presents each entry in a table, and writes back only changed values
 * via `$N=value` commands. Works with GRBL, GRBL-Advanced, FluidNC, or any
 * variant that uses the `$N=value (description)` reply format.
 */
#ifndef MACHINESETTINGSDIALOG_H
#define MACHINESETTINGSDIALOG_H

#include <QDialog>
#include <QList>
#include <QPair>
#include <QString>
#include <QStringList>

class QTableWidget;
class QPushButton;
class QLabel;

class MachineSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MachineSettingsDialog(QWidget *parent = nullptr);

    // Clear the table and show a "loading" hint. Call before emitting the $$
    // query so the dialog visibly reflects in-flight state.
    void beginRefresh();

    // Parse the joined response string (as produced by frmMain's response
    // accumulator, semicolon-separated) and populate the table. Safe to call
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
        int n;
        QString description;
        QString originalValue;
    };

    QTableWidget *m_table;
    QPushButton  *m_btnRefresh;
    QPushButton  *m_btnApply;
    QPushButton  *m_btnRestoreDefaults;
    QPushButton  *m_btnClose;
    QLabel       *m_lblStatus;

    QList<Row> m_rows;
    QStringList m_accumulatedErrors;
};

#endif // MACHINESETTINGSDIALOG_H
