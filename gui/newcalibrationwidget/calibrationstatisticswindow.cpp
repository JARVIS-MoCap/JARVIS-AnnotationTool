/*****************************************************************
 * File:			calibrationstatisticswindow.cpp
 * Created: 	24. July 2021
 * Author:		Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *****************************************************************/

#include "calibrationstatisticswindow.hpp"

#include <QGridLayout>
#include <QGroupBox>


CalibrationStatisticsWindow::CalibrationStatisticsWindow(QList<QString> cameraNames, QList<QList<QString>> cameraPairs, QMap<int,double> intrinsicsReproErrors,
				QMap<int,double> extrinsicsReproErrors, QWidget *parent) :
				m_cameraNames(cameraNames), m_cameraPairs(cameraPairs), m_intrinsicsReproErrors(intrinsicsReproErrors), m_extrinsicsReproErrors(extrinsicsReproErrors), QDialog(parent) {
	setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
	setWindowTitle("Calibration Error Statistics");
	QGridLayout *layout = new QGridLayout(this);


	QLabel *intrinsicsChartLabel = new QLabel("Intrinsics Reprojection Errors");
	intrinsicsChartLabel->setFont(QFont("Sans Serif", 14, QFont::Bold));

	std::vector<double> intrinsicsReproErrorsVec;
	for (const auto& error : intrinsicsReproErrors) {
		intrinsicsReproErrorsVec.push_back(error);
	}
	intrinsicsChartView = new CalibrationChartView(cameraNames, intrinsicsReproErrorsVec);


	QLabel *extrinsicsChartLabel = new QLabel("Extrinsic Reprojection Errors");
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
	extrinsicsChartView = new CalibrationChartView(cameraPairNames, extrinsicsReproErrorsVec);


	layout->addWidget(intrinsicsChartLabel,0,0, Qt::AlignCenter);
	layout->addWidget(extrinsicsChartLabel,0,1, Qt::AlignCenter);
	layout->addWidget(intrinsicsChartView,1,0);
	layout->addWidget(extrinsicsChartView,1,1);


}
