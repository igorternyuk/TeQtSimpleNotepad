#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QTextDocument>
#include <QList>
#include <QMap>
#include "settingsuntil.h"

namespace Ui
{
    class MainWindow;
}

class QLabel;
class QTextEdit;
class FindReplaceDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void closeEvent(QCloseEvent*) override;

private slots:
    void mark_unsaved_text_changes_on_tab();
    void on_action_about_program_triggered();
    void on_action_about_Qt_triggered();
    void on_action_open_triggered();
    void on_action_save_triggered();
    void on_action_close_triggered();
    void on_action_quit_triggered();
    void on_action_copy_triggered();
    void on_action_cut_triggered();
    void on_action_paste_triggered();
    void on_action_select_all_triggered();
    void on_action_clear_Selection_triggered();
    void on_action_new_file_triggered();
    void on_action_choose_font_triggered();
    void on_action_choose_font_color_triggered();
    void on_action_choose_background_color_triggered();
    void on_action_find_and_replace_triggered();
    void on_action_next_search_result_triggered();
    void on_action_previous_search_result_triggered();
    void on_action_undo_triggered();
    void on_action_redo_triggered();
    void on_action_save_as_triggered();
    void on_action_text_properties_triggered();
    void on_actionExport_to_pdf_triggered();
    void on_action_save_all_files_triggered();
    void on_action_close_all_files_triggered();
    void update_cursor_info();
    int create_new_tab(const QString &title,
                       const QString &pathToFile = QString(),
                       const QString &text = QString());
    bool open_files(const QStringList &listOfFiles);
    bool open_file(const QString &path);
    bool save_file(int index);
    bool save_file_as(int index);
    bool save_text_to_file(const QString &filePath, const QString &text);
    void on_tabWidget_tabCloseRequested(int index);
    void save_all_current_session_files();
    void load_all_last_session_files();
    void load_settings();
    void save_settings();

private:
    enum class SettingsKey
    {
        WINDOW_SIZE,
        WINDOW_POS,
        CURRENT_TAB,
        UNTITLED_TAB_INDEX_MAX
    };

    QMap<SettingsKey, QString> mKeys
    {
        { SettingsKey::WINDOW_SIZE, "WindowSize" },
        { SettingsKey::WINDOW_POS, "WindowPos" },
        { SettingsKey::CURRENT_TAB, "Current tab" },
        { SettingsKey::UNTITLED_TAB_INDEX_MAX, "UntitledTabIndexMax"}
    };

    enum class SettingsGroup
    {
        MainWindowGeometry,
        TabWidget
    };

    QMap<SettingsGroup, QString> mSettingsGroups
    {
        { SettingsGroup::MainWindowGeometry, "WindowSize" },
        { SettingsGroup::MainWindowGeometry, "WindowPos" },
        { SettingsGroup::TabWidget, "Current tab" },
        { SettingsGroup::TabWidget, "UntitledTabIndexMax"}
    };

    const QString SESSION_FILE_PATH { "lastSession.dat" };

    Ui::MainWindow *ui;
    QLabel *mLblTextInfo;
    QList<QTextEdit*> mListOfTextEdits;
    FindReplaceDialog *mFindReplaceDialog;
    int mUntitledTabIndexMax = 0;
};

#endif // MAINWINDOW_H
