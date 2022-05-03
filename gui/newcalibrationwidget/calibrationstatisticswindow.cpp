/*****************************************************************
	* File:			  calibrationstatisticswindow.cpp
	* Created: 	  05. August 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2022 Timo Hueser
	* License:    LGPL v2.1
	*****************************************************************/

#include "calibrationstatisticswindow.hpp"

#include <QGridLayout>
#include <QGroupBox>


CalibrationStatisticsWindow::CalibrationStatisticsWindow(QList<QString> cameraNames,
			QList<QList<QString>> cameraPairs, QMap<int,double> intrinsicsReproErrors,
			QMap<int,double> extrinsicsReproErrors, QWidget *parent) :
		 	QDialog(parent), m_cameraNames(cameraNames), m_cameraPairs(cameraPairs),
			m_intrinsicsReproErrors(intrinsicsReproErrors), m_extrinsicsReproErrors(extrinsicsReproErrors) {
	setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
	setWindowTitle("Calibration Error Statistics");
	QGridLayout *layout = new QGridLayout(this);

	QLabel *intrinsicsChartLabel;
	if (intrinsicsReproErrors.size() > 0) {
		intrinsicsChartLabel = new QLabel("Intrinsics Reprojection Errors");
		intrinsicsChartLabel->setFont(QFont("Sans Serif", 14, QFont::Bold));

		std::vector<double> intrinsicsReproErrorsVec;
		for (const auto& error : intrinsicsReproErrors) {
			intrinsicsReproErrorsVec.push_back(error);
		}
		intrinsicsChartView = new CalibrationChartView(cameraNames, intrinsicsReproErrorsVec);
	}

	QLabel *extrinsicsChartLabel;
	if (extrinsicsReproErrors.size() > 0) {
		extrinsicsChartLabel = new QLabel("Extrinsic Reprojection Errors");
		extrinsicsChartLabel->setFont(QFont("Sans Serif", 14, QFont::Bold));

		QList<QString> cameraPairNames;
		for (const auto& pair : cameraPairs) {
			if (pair.size() == 2) {
				cameraPairNames.append(pair[0] + " --> " + pair[1]);
			}
			else {
				cameraPairNames.append(pair[0] + " --> " + pair[1] + " --> " + pair[2]);
			}
		}
		std::vector<double> extrinsicsReproErrorsVec;
		for (const auto& error : extrinsicsReproErrors) {
			extrinsicsReproErrorsVec.push_back(error);
		}
		extrinsicsChartView = new CalibrationChartView(cameraPairNames, extrinsicsReproErrorsVec,
																									 "interactive");
	}

	closeButton = new QPushButton("Close");
	closeButton->setMinimumSize(30,30);
	connect(closeButton, &QPushButton::clicked, this, &CalibrationStatisticsWindow::accept);

	if (intrinsicsReproErrors.size() > 0) {
		layout->addWidget(intrinsicsChartLabel,0,0, Qt::AlignCenter);
		layout->addWidget(intrinsicsChartView,1,0);
	}
	if (extrinsicsReproErrors.size() > 0) {
		layout->addWidget(extrinsicsChartLabel,0,1, Qt::AlignCenter);
		layout->addWidget(extrinsicsChartView,1,1);
	}
	if (intrinsicsReproErrors.size() > 0 && extrinsicsReproErrors.size() > 0) {
		layout->addWidget(closeButton,2,0,1,2, Qt::AlignRight);
	}
	else {
		layout->addWidget(closeButton,2,0, Qt::AlignRight);
	}

}
