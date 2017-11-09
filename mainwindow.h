#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui
{
    class MainWindow;
}

struct File
{
    int id;
    QString fileName;
    QString text;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

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
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
