/*******************************************************************************
 * File:			  settingswindow.hpp
 * Created: 	  23. October 2020
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:   2021 Timo Hueser
 * License:     LGPL v3.0
 ******************************************************************************/

#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include "globals.hpp"
#include "dataset.hpp"
#include "colormap.hpp"

#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QSettings>
#include <QGroupBox>
#include <QSpinBox>
#include <QSlider>
#include <QCheckBox>
#include <QComboBox>


class SettingsWindow : public QWidget {
	Q_OBJECT

	public:
		explicit SettingsWindow(QWidget *parent = nullptr);

	signals:
		void imageTranformationChanged(int hueFactor, int saturationFactor,
					int brightnessFactor);
		void keypointSizeChanged(int value);
		void keypointShapeChanged(const QString& entity, KeypointShape shape);
		void colorMapChanged(const QString& entity, ColorMap::ColorMapType type,
					QColor color);
		void minViewsChanged(int val);
		void errorThresholdChanged(float val);
		void boneLengthErrorThresholdChanged(float val);

	public slots:
		void datasetLoadedSlot();

	private:
		void loadSettings();
		QImage createColorMapPreview(ColorMap::ColorMapType type, 
					QColor color = QColor(0,0,255));

		QSettings *settings;
		QGroupBox *imageSettingsBox;
		QCheckBox *grayScaleToggle;
		QSlider *hueSlider;
		QSpinBox *hueBox;
		QPushButton *hueResetButton;
		QSlider *saturationSlider;
		QSpinBox *saturationBox;
		QPushButton *saturationResetButton;
		QSlider *brightnessSlider;
		QSpinBox *brightnessBox;
		QPushButton *brightnessResetButton;

		QGroupBox *annotationSettingsBox;
		QSpinBox *keypointSizeEdit;
		QWidget *entitySettingsWidget;
		QGridLayout *entitysettingslayout;
		QList<QComboBox*> keypointShapeCombosList;
		QList<QComboBox*> colorMapCombosList;
		QList<ColorMap::ColorMapType> colorMapTypeList;
		QList<QLabel*> colormapPreviewsList;
		QList<QPushButton*> colorChooserButtonsList;
		QList<QColor> colorsList;

		QGroupBox *reprojectionSettingsBox;
		QSpinBox *minViewsEdit;
		QDoubleSpinBox *errorThresholdEdit;
		QDoubleSpinBox *boneLengthErrorThresholdEdit;

	private slots:
		void imageTranformationChangedSlot();
		void grayScaleToggledSlot(int state);
		void hueResetClickedSlot();
		void saturationResetClickedSlot();
		void brightnessResetClickedSlot();
		void keypointShapeChangedSlot(int index);
		void colorMapChangedSlot(int index);
		void colorChooserClickedSlot();
		void minViewsChangedSlot(int val);
		void errorThresholdChangedSlot(double val);
		void boneLengthErrorThresholdChangedSlot(double val);

};

#endif
