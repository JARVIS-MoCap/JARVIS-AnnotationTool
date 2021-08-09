/*****************************************************************
 * File:			helpwindow.cpp
 * Created: 	08. August 2021
 * Author:		Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *****************************************************************/

#include "helpwindow.hpp"

#include <QGridLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QGroupBox>
#include <QDirIterator>
#include <QThread>
#include <QTextStream>


HelpWindow::HelpWindow(QWidget *parent) : QWidget(parent, Qt::Window) {
	this->setMinimumSize(1100,800);
	QGridLayout *layout = new QGridLayout(this);
	mainSplitter = new QSplitter(this);
	layout->addWidget(mainSplitter,0,0);

	textBrowser = new QTextBrowser();

	tableOfContents = new QTreeWidget();
	tableOfContents->setColumnCount(1);
	tableOfContents->setHeaderLabel("Table of Contents");
	tableOfContents->setMaximumSize(300,100000);
	connect(tableOfContents, &QTreeWidget::itemDoubleClicked, this, &HelpWindow::itemSlectedSlot);

	QTreeWidgetItem *gettingStartedItem = new QTreeWidgetItem(tableOfContents, QStringList(QString("Getting Started")));
	QTreeWidgetItem *introductionItem = new QTreeWidgetItem(gettingStartedItem, QStringList(QString("Introduction")));
	QTreeWidgetItem *exampleItem = new QTreeWidgetItem(gettingStartedItem, QStringList(QString("Exploring the Example Dataset")));
	QTreeWidgetItem *ownDataItem = new QTreeWidgetItem(gettingStartedItem, QStringList(QString("Working with your own Data")));
	gettingStartedItem->addChild(introductionItem);
	gettingStartedItem->addChild(exampleItem);
	gettingStartedItem->addChild(ownDataItem);
	QTreeWidgetItem *calibrationItem = new QTreeWidgetItem(tableOfContents, QStringList(QString("Calibration")));
	QTreeWidgetItem *recordingCalibrationItem = new QTreeWidgetItem(calibrationItem, QStringList(QString("Recording Calibration Videos")));
	QTreeWidgetItem *createCalibrationItem = new QTreeWidgetItem(calibrationItem, QStringList(QString("Create Calibration Files")));
	calibrationItem->addChild(recordingCalibrationItem);
	calibrationItem->addChild(createCalibrationItem);
	gettingStartedItem->setExpanded(true);
	calibrationItem->setExpanded(true);

	tableOfContents->addTopLevelItem(gettingStartedItem);
	tableOfContents->addTopLevelItem(calibrationItem);

	m_contentMap[introductionItem] = "introduction.html";
	m_contentMap[exampleItem] = "example.html";
	m_contentMap[ownDataItem] = "owndata.html";
	m_contentMap[recordingCalibrationItem] = "recordingcalibration.html";
	m_contentMap[createCalibrationItem] = "createcalibration.html";


	mainSplitter->addWidget(tableOfContents);
	mainSplitter->addWidget(textBrowser);

}

void HelpWindow::itemSlectedSlot(QTreeWidgetItem *item, int column) {
	auto it = m_contentMap.find(item);
	if (it != m_contentMap.end()) {
		setDocument(m_contentMap[item]);
	}


}

void HelpWindow::setDocument(const QString &path) {
	QTextDocument *doc = new QTextDocument();
	//doc->setBaseUrl(QUrl("C:\\Users\\timoh\\Documents\\AnnotationTool\\help\\"));
	std::cout << doc->baseUrl().path().toStdString() << std::endl;
	QFile f1("/github-markdown.css");
	f1.open(QFile::ReadOnly | QFile::Text);
	QTextStream in1(&f1);
	doc->setDefaultStyleSheet(in1.readAll());
	textBrowser->setDocument(doc);

	QFile f(path);
	f.open(QFile::ReadOnly | QFile::Text);
	QTextStream in(&f);
	QString html = in.readAll();
	doc->setHtml(html);

}
