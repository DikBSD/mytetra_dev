#ifndef SHORTCUTSETTINGSSCREEN_H
#define SHORTCUTSETTINGSSCREEN_H

#include <QDialog>
#include <QWidget>
#include <QAction>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include "controllers/shortcutSettings/ShortcutSettingsController.h"

class ShortcutSettingsScreen : public QDialog
{
    Q_OBJECT
public:
    ShortcutSettingsScreen(QWidget *parent = nullptr);
    ~ShortcutSettingsScreen();

protected slots:

    void onShortcutSelect(const QModelIndex &index);
    void onShortcutKeysChange(const QString &text);

protected:

    ShortcutSettingsController *shortcutSettingsController;

    QPushButton *buttonGrabShortcut;
    QPushButton *buttonResetShortcutToDefault;
    QPushButton *buttonResetAllShortcutsToDefault;

    QDialogButtonBox *dialogButtonBox; // Кнопки Ok и Cancel

    QGroupBox *shortcutBox;
    QLabel *commandLabel;
    QLabel *commandValueLabel;
    QLabel *desctiptionLabel;
    QLabel *desctiptionValueLabel;
    QLabel *shortcutLabel;
    QLineEdit *shortcutValueLineEdit;
    QHBoxLayout *shortcutLineLayout; // Слой с полем сочетания клавиш, кнопками Grab и Reset to default
    QGridLayout *shortcutLayout; // Слой внутри объединяющего прямоугольника настройки шортката

    QVBoxLayout *screenLayout;

    // Данные о текущем выбранном шорткате
    // Раздел, команда, описание, клавиши
    ShortcutSettingsController::ShortcutData shortcutData;

    void setupUI(void);
    void setupSignals(void);
    void assembly(void);

};

#endif // SHORTCUTSETTINGSSCREEN_H