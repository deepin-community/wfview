
#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QTableWidget>
#include <QMenu>
#include <QMouseEvent>
#include <QItemDelegate>
#include <QComboBox>
#include <QCheckBox>
#include <QRegularExpression>
#include <QLineEdit>
#include <QRegularExpressionValidator>
#include <QRegularExpression>

class tableWidget : public QTableWidget
{
    Q_OBJECT

public:
    explicit tableWidget(QWidget* parent = 0);
    void editing(bool val) { editingEnabled = val; };

signals:
    void rowAdded(int row);
    void rowDeleted(quint32 num);

protected:
    void mouseReleaseEvent(QMouseEvent *event);
    bool editingEnabled = true;
};


class tableEditor : public QItemDelegate
{
    Q_OBJECT
public:
    explicit tableEditor(QString validExp = "", QObject *parent = 0);
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    mutable QLineEdit *edit;
    QString validExp;
};



class tableCombobox : public QItemDelegate
{
    Q_OBJECT
public:
    explicit tableCombobox(QAbstractItemModel* model, bool sort=false, QObject *parent = 0);
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *combo, const QModelIndex &index) const override;
    void setModelData(QWidget *combo, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *combo, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    mutable QComboBox *combo;
    mutable QComboBox *modelCombo = Q_NULLPTR;

private slots:
    void setData(int val);

private:
    QAbstractItemModel* modelData;
};


class tableCheckbox : public QItemDelegate
{
    Q_OBJECT
public:
    explicit tableCheckbox(QObject *parent = 0);
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *combo, const QModelIndex &index) const override;
    void setModelData(QWidget *combo, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *combo, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // TABLEWIDGET_H
