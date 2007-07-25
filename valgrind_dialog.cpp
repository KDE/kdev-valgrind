/* This file is part of KDevelop
   Copyright (C) 2006 Hamish Rodda <rodda@kde.org>
   Copyright (C) 2002 Harald Fernengel <harry@kdevelop.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#include "valgrind_dialog.h"

#include <QLayout>
#include <QCheckBox>
#include <QRegExp>
#include <QStackedWidget>

#include <kprocess.h>
#include <klocale.h>
#include <kurlrequester.h>
#include <klineedit.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>

#include "ui_valgrind_dialog.h"


ValgrindDialog::ValgrindDialog( Type type, QWidget* parent )
  : KDialog( parent )
  , m_type(type)
{
  w = new Ui::ValgrindDialog();
  QWidget* top = new QWidget(this);
  w->setupUi(top);
  w->valExecutableEdit->setUrl(KUrl( "valgrind" ));
w->executableEdit->setFocus();
  w->stack->setCurrentWidget(type == Memcheck ? w->memcheckWidget : w->callgrindWidget);
  setMainWidget( top );
  connect( w->executableEdit->lineEdit(),  SIGNAL( textChanged( const QString &)), this, SLOT( valgrindTextChanged()));
  connect( w->valExecutableEdit->lineEdit(), SIGNAL( textChanged( const QString &)), this, SLOT( valgrindTextChanged()));
  connect( w->ctExecutableEdit->lineEdit(),  SIGNAL( textChanged( const QString &)), this, SLOT( valgrindTextChanged()));
  connect( w->kcExecutableEdit->lineEdit(), SIGNAL( textChanged( const QString &)), this, SLOT( valgrindTextChanged()));
  enableButtonOk( false );
}


ValgrindDialog::~ValgrindDialog()
{
}

void ValgrindDialog::valgrindTextChanged()
{
    if (m_type == Memcheck)
        enableButtonOk( !w->valExecutableEdit->lineEdit()->text().isEmpty() &&  !w->executableEdit->lineEdit()->text().isEmpty() );
    else if (m_type == Calltree)
        enableButtonOk( !w->executableEdit->lineEdit()->text().isEmpty() &&  !w->ctExecutableEdit->lineEdit()->text().isEmpty() && !w->kcExecutableEdit->lineEdit()->text().isEmpty() );
}

QString ValgrindDialog::executableName() const
{
  return w->executableEdit->url().toLocalFile();
}

void ValgrindDialog::setExecutable( const QString& url )
{
  w->executableEdit->setUrl( url );
}

QString ValgrindDialog::valExecutable() const
{
  return w->valExecutableEdit->url().toLocalFile();
}

QString ValgrindDialog::parameters() const
{
  return w->paramEdit->text();
}

void ValgrindDialog::setParameters( const QString& params )
{
  w->paramEdit->setText( params );
}

void ValgrindDialog::setValExecutable( const QString& ve )
{
  QString vUrl = ve;
  if ( vUrl.isEmpty() ) {
    vUrl = KStandardDirs::findExe( "valgrind" );
  }
  if ( vUrl.isEmpty() ) {
    KMessageBox::sorry( this, i18n( "Could not find valgrind in your $PATH. Please make "
                                    "sure it is installed properly." ),
                        i18n( "Valgrind Not Found" ) );
    w->valExecutableEdit->setUrl( KUrl("valgrind") );
  } else {
    w->valExecutableEdit->setUrl( vUrl );
  }
}

static const QString leakCheckParam( "--leak-check=yes" );
static const QString reachableParam( "--show-reachable=yes" );
static const QString childrenParam( "--trace-children=yes" );

QString ValgrindDialog::valParams() const
{
  QString params;
  if (isNewValgrindVersion())
    params = QString::fromLatin1( "--tool=memcheck " );
  params += w->valParamEdit->text();
  if ( w->memleakBox->isChecked() )
    params += ' ' + leakCheckParam;
  if ( w->reachableBox->isChecked() )
    params += ' ' + reachableParam;
  if ( w->childrenBox->isChecked() )
    params += ' ' + childrenParam;

  return params;
}

void ValgrindDialog::setValParams( const QString& params )
{
  QString myParams = params;
  if ( myParams.contains( leakCheckParam ) )
    w->memleakBox->setChecked( true );
  if ( myParams.contains( reachableParam ) )
    w->reachableBox->setChecked( true );
  if ( myParams.contains( childrenParam ) )
    w->childrenBox->setChecked( true );

  myParams = myParams.replace( QRegExp( leakCheckParam ), "" );
  myParams = myParams.replace( QRegExp( reachableParam ), "" );
  myParams = myParams.replace( QRegExp( childrenParam ), "" );
  myParams = myParams.replace( "--tool=memcheck ", "" );
  myParams = myParams.trimmed();
  w->valParamEdit->setText( myParams );
}

QString ValgrindDialog::ctExecutable() const
{
  return w->ctExecutableEdit->url().toLocalFile();
}

void ValgrindDialog::setCtExecutable( const QString& ce )
{
  QString vUrl = ce;
  if ( vUrl.isEmpty() ) {
    vUrl = KStandardDirs::findExe( "calltree" );
  }
  if ( vUrl.isEmpty() ) {
    KMessageBox::sorry( this, i18n( "Could not find calltree in your $PATH. Please make "
                                    "sure it is installed properly." ),
                        i18n( "Calltree Not Found" ) );
    w->ctExecutableEdit->setUrl( KUrl("calltree") );
  } else {
    w->ctExecutableEdit->setUrl( vUrl );
  }
}

QString ValgrindDialog::ctParams() const
{
  QString params = w->ctParamEdit->text();
  if ( w->ctChildrenBox->isChecked() )
    params += ' ' + childrenParam;

  return params;
}

void ValgrindDialog::setCtParams( const QString& params )
{
  QString myParams = params;
  if ( myParams.contains( childrenParam ) )
    w->ctChildrenBox->setChecked( true );

  myParams = myParams.replace( QRegExp( childrenParam ), "" );
  myParams = myParams.trimmed();
  w->ctParamEdit->setText( myParams );
}

QString ValgrindDialog::kcExecutable( ) const
{
  return w->kcExecutableEdit->url().toLocalFile();
}

void ValgrindDialog::setKcExecutable( const QString& ke )
{
  QString vUrl = ke;
  if ( vUrl.isEmpty() ) {
    vUrl = KStandardDirs::findExe( "kcachegrind" );
  }
  if ( vUrl.isEmpty() ) {
    KMessageBox::sorry( this, i18n( "Could not find kcachegrind in your $PATH. Please make "
                                    "sure it is installed properly." ),
                        i18n( "KCachegrind Not Found" ) );
    w->kcExecutableEdit->setUrl( KUrl("kcachegrind") );
  } else {
    w->kcExecutableEdit->setUrl( vUrl );
  }
}

bool ValgrindDialog::isNewValgrindVersion( ) const
{
  KProcess proc;
  proc.setShellCommand("test \"valgrind-20\" == `valgrind --version | awk -F \\. '{print $1$2}'`");
  int ec = proc.execute();
  if (ec >= 0)
    return ec;
  return true;
}

// kate: space-indent on; indent-width 2; tab-width 2; show-tabs on;

#include "valgrind_dialog.moc"

