#include "StyleManager.hpp"

#include "base/Qv2rayBase.hpp"
#include "common/QvHelpers.hpp"

#include <QApplication>
#include <QColor>
#include <QPalette>
#include <QStyleFactory>

namespace Qv2ray::ui::styles
{
    QvStyleManager::QvStyleManager(QObject *parent) : QObject(parent)
    {
        ReloadStyles();
    }

    void QvStyleManager::ReloadStyles()
    {
        styles.clear();
        for (const auto &key : QStyleFactory::keys())
        {
            LOG(MODULE_UI, "Found factory style: " + key)
            QvStyle style;
            style.Name = key;
            style.Type = QvStyle::QVSTYLE_FACTORY;
            styles.insert(key, style);
        }

        for (const auto &styleDir : Qv2rayAssetsPaths("uistyles"))
        {
            for (const auto &file : GetFileList(QDir(styleDir)))
            {
                QFileInfo fileInfo(styleDir + "/" + file);
                if (fileInfo.suffix() == "css" || fileInfo.suffix() == "qss" || fileInfo.suffix() == "qvstyle")
                {
                    LOG(MODULE_UI, "Found QSS style at: \"" + fileInfo.absoluteFilePath() + "\"")
                    QvStyle style;
                    style.Name = fileInfo.baseName();
                    style.qssPath = fileInfo.absoluteFilePath();
                    style.Type = QvStyle::QVSTYLE_QSS;
                    styles.insert(style.Name, style);
                }
            }
        }
    }

    bool QvStyleManager::ApplyStyle(const QString &style)
    {
        if (!styles.contains(style))
            return false;
        const auto &s = styles[style];
        switch (s.Type)
        {
            case QvStyle::QVSTYLE_QSS:
            {
                LOG(MODULE_UI, "Applying UI QSS style: " + s.qssPath)
                qvApp->setStyle(QStyleFactory::create("fusion"));
                qvApp->processEvents();
                //
                const auto content = StringFromFile(s.qssPath);
                qvApp->setStyleSheet(content);
                return true;
            }
            case QvStyle::QVSTYLE_FACTORY:
            {
                LOG(MODULE_UI, "Applying UI style: " + s.Name)
                qvApp->setStyle(QStyleFactory::create(s.Name));
                qvApp->setStyleSheet("");
                return true;
            }
            default: return false;
        }
    }
} // namespace Qv2ray::ui::styles