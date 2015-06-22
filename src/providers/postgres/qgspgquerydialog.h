/***************************************************************************
                          qgspgquerydialog.h
                          ------------------
    begin                : June 2015
    copyright            : (C) 2015 by Nyall Dawson
    email                : nyall dot dawson at gmail dot com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef QGSPGQUERYDIALOG_H
#define QGSPGQUERYDIALOG_H
#include "ui_qgsdbquerydialog.h"
#include "qgisgui.h"
#include <QDialog>

class QgsPgQueryDialog : public QDialog, private Ui::QgsDbQueryDialog
{
    Q_OBJECT
  public:
    //! Constructor
    QgsPgQueryDialog( const QString& connectionName, QWidget *parent = 0, Qt::WindowFlags fl = QgisGui::ModalDialogFlags );
    //! Destructor
    ~QgsPgQueryDialog();

  private slots:

    void on_mClearButton_clicked();
    void on_mExecuteButton_clicked();
    void on_mRetrieveColumnsButton_clicked();

  private:

    void initCompleter();

    QString mConnName;

    static const QStringList mSqlDictionary;

    QFont getMonospaceFont();
};

#endif //  QGSPGQUERYDIALOG_H
