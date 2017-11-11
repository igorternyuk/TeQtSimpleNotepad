#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QTextDocument>
#include <QList>

namespace Ui
{
    class MainWindow;
}

//typedef FileData FileData;
struct FileData
{
    QString fullPath;
    QString text;
    bool isSaved;
};

class QLabel;
class QComboBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QLabel *mLblTexyInfo;
    QComboBox *mFileSelector;
    QList<FileData> mFilesData;
    int mCurrentFileIndex{0};

private slots:
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
    void on_action_find_triggered();
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
    void update_file_view(int index);
    bool open_file(const QString &path);
    bool save_file(int index);
    void close_file(int index);
    void syncronize_combobox_and_file_list();

    void on_actionAdd_tab_triggered();
    void on_tabWidget_tabCloseRequested(int index);
};

#endif // MAINWINDOW_H
