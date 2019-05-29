#ifndef QT_PROJECT_WIZARD_CONTENT_PATH_H
#define QT_PROJECT_WIZARD_CONTENT_PATH_H

#include <set>

#include "QtProjectWizardContent.h"
#include "QtLocationPicker.h"

class SourceGroupSettings;

class QtProjectWizardContentPath
	: public QtProjectWizardContent
{
	Q_OBJECT

public:
	QtProjectWizardContentPath(QtProjectWizardWindow* window);

	// QtSettingsWindow implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual bool check() override;

protected:
	void setTitleString(const QString& title);
	void setHelpString(const QString& help);
	void setPlaceholderString(const QString& placeholder);

	void setFileEndings(const std::set<std::wstring>& fileEndings);
	void setAllowEmpty(bool allowEmpty);

	QtLocationPicker* m_picker;

	bool m_makePathRelativeToProjectFileLocation;

private:
	virtual std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() = 0;

	QString m_titleString;
	QString m_helpString;
	QString m_placeholderString;
	std::set<std::wstring> m_fileEndings;
	bool m_allowEmpty;
};

#endif // QT_PROJECT_WIZARD_CONTENT_PATH_H
