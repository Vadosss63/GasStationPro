#pragma once

#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QWidget>

class ServiceMenuDialog : public QWidget {
  Q_OBJECT
public:
  explicit ServiceMenuDialog(QWidget *parent = nullptr);
  void readLogFile();

public slots:
  void showDialog();

private:
  void createWidget();
  void showSum(int sum_);
  QLabel *sum = nullptr;
  QListWidget *chequeList = nullptr;
};
