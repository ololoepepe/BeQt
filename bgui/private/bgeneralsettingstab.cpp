#include "bgeneralsettingstab.h"

#include "../bcore/bcore.h"

#include <QVariantMap>
#include <QString>
#include <QLocale>
#include <QFormLayout>
#include <QComboBox>
#include <QList>

const QLocale _m_En = QLocale(QLocale::English, QLocale::UnitedStates);

//

QString textFromLocale(const QLocale &locale)
{
    QString language = QLocale::languageToString( locale.language() );
    QString country = (locale.country() != QLocale::AnyCountry) ? QLocale::countryToString( locale.country() ) : "";
    QString name = locale.name();
    return language + (!country.isEmpty() ? " (" + country + ")" : "") + " [" + name + "]";
}

//

const QString BGeneralSettingsTab::Id = "beqt/bgui/general";
const QString BGeneralSettingsTab::IdLocale = "locale";

//

BGeneralSettingsTab::BGeneralSettingsTab(const QVariantMap &settings)
{
    QLocale currentLocale = settings.value(IdLocale).toLocale();
    QList<QLocale> availableLocales = BCore::availableLocales();
    _m_flt = new QFormLayout(this);
      _m_flt->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
      _m_cmboxLanguage = new QComboBox;
        _m_cmboxLanguage->setEditable(false);
        if (!availableLocales.contains(_m_En) && currentLocale != _m_En)
            _m_cmboxLanguage->addItem(textFromLocale(_m_En), _m_En);
        if ( !availableLocales.contains(currentLocale) )
            _m_cmboxLanguage->addItem(textFromLocale(currentLocale), currentLocale);
        for (int i = 0; i < availableLocales.size(); ++i)
        {
            const QLocale &l = availableLocales.at(i);
            _m_cmboxLanguage->addItem(textFromLocale(l), l);
        }
        int ind = _m_cmboxLanguage->findData(currentLocale);
        if (ind >= 0)
            _m_cmboxLanguage->setCurrentIndex(ind);
      _m_flt->addRow("Language:", _m_cmboxLanguage);
}

//

QVariantMap BGeneralSettingsTab::valueMap() const
{
    QVariantMap m;
    m.insert( IdLocale, _m_cmboxLanguage->itemData( _m_cmboxLanguage->currentIndex() ).toLocale() );
    return m;
}

QString BGeneralSettingsTab::title() const
{
    return tr("General", "title");
}
