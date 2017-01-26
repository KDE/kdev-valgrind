/* This file is part of KDevelop
 * Copyright 2011 Sebastien Rannou <mxs@sbrk.org>

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

#ifndef VALGRINDHELGRINDCONFIGPAGE_H_
#define VALGRINDHELGRINDCONFIGPAGE_H_

#include <interfaces/launchconfigurationpage.h>

namespace valgrind
{
namespace Ui
{
class HelgrindConfig;
}

class HelgrindConfigPage : public KDevelop::LaunchConfigurationPage
{
    Q_OBJECT
public:
    explicit HelgrindConfigPage(QWidget* parent = nullptr);
    void          loadFromConfiguration(const KConfigGroup&, KDevelop::IProject * = nullptr) override;
    QIcon         icon(void) const override;
    void          saveToConfiguration(KConfigGroup, KDevelop::IProject * = nullptr) const override;
    QString       title(void) const override;

private:
    Ui::HelgrindConfig    *ui;
};

class HelgrindConfigPageFactory : public KDevelop::LaunchConfigurationPageFactory
{
public:
    HelgrindConfigPageFactory(void);
    ~HelgrindConfigPageFactory(void) override;
    KDevelop::LaunchConfigurationPage* createWidget(QWidget * parent) override;
};
}
#endif /* VALGRINDHELGRINDCONFIGPAGE_H_ */
