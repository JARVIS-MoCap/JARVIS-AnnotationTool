/*****************************************************************
 * File:			exportdatasetwidget.cpp
 * Created: 	30. July 2021
 * Author:		Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *****************************************************************/

#include "exportdatasetwidget.hpp"

#include <QGridLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QGroupBox>
#include <QDirIterator>
#include <QThread>
#include <QTextStream>
#include <QWebEngineView>


ExportDatasetWidget::ExportDatasetWidget(QWidget *parent) : QWidget(parent) {
	this->setMinimumSize(600,600);
	settings = new QSettings();
	QGridLayout *layout = new QGridLayout(this);
	m_errorMsg = new QErrorMessage();

	QWebEngineView *edit = new QWebEngineView();
	//edit->setStyleSheet("background-color: white;");

	QTextDocument *doc = new QTextDocument();
	QFile f1("/home/timo/Documents/AnnotationTool/build/github-markdown.css");
	f1.open(QFile::ReadOnly | QFile::Text);
	QTextStream in1(&f1);
	doc->setDefaultStyleSheet(in1.readAll());

	edit->setDocument(doc);

	//QFile f("/home/timo/Documents/AnnotationTool/build/test.md");
	QFile f("/home/timo/Documents/AnnotationTool/build/test.html");

	f.open(QFile::ReadOnly | QFile::Text);
	QTextStream in(&f);

	//doc->setMarkdown(in.readAll(), QTextDocument::MarkdownDialectGitHub);
	QString html = in.readAll();
	QString css = in1.readAll();
	edit->setHtml(html);

	//doc->setDefaultStyleSheet("body { color : green; background-color : black; }");
  //doc->setHtml("<body>test 123</body>");

	layout->addWidget(edit,0,0);
}
