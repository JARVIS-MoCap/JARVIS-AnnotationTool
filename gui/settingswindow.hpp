/*******************************************************************************
 * File:			  settingswindow.hpp
 * Created: 	  23. October 2020
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:   2022 Timo Hueser
 * License:     LGPL v2.1
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
#include <QTabWidget>


class SettingsWindow : public QWidget {
	Q_OBJECT

	public:
		explicit SettingsWindow(QWidget *parent = nullptr);

	signals:
		void imageTranformationChanged(int hueFactor, int saturationFactor,
					int brightnessFactor, int contrastFactor);
		void alwaysShowLabelsToggled(bool toggle);
		void labelFontColorChanged(QColor color);
		void labelBackroundColorChanged(QColor color);
		void keypointSizeChanged(int value);
		void keypointShapeChanged(const QString& entity, KeypointShape shape);
		void colorMapChanged(const QString& entity, ColorMap::ColorMapType type,
					QColor color);
		void minViewsChanged(int val);
		void errorThresholdChanged(float val);
		void boneLengthErrorThresholdChanged(float val);

	public slots:
		void datasetLoadedSlot();
		void brightnessChangedSlot(int brightnessFactor);

	private:
		bool m_initialized = false;
		void loadSettings();
		QImage createColorMapPreview(ColorMap::ColorMapType type,
					QColor color = QColor(0,0,255));

		QImage createColorPreview(QColor color);

		QColor m_labelFontColor = QColor(255,255,255);
		QColor m_labelBackgroundColor = QColor(32, 100, 164,125);
		int m_backgroundAlpha = 125;


		QSettings *settings;
		QTabWidget *tabWidget;

		QWidget *imageSettingsWidget;
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
		QSlider *contrastSlider;
		QSpinBox *contrastBox;
		QPushButton *contrastResetButton;

		QWidget *annotationSettingsWidget;
		QSpinBox *keypointSizeEdit;
		QCheckBox *alwaysShowLabelsCheckbox;
		QPushButton *fontColorChooserButton;
		QLabel *fontColorPreview;
		QPushButton *backgroundColorChooserButton;
		QLabel *backgroundColorPreview;

		QWidget *entitySettingsWidget;
		QGridLayout *entitysettingslayout;
		QList<QComboBox*> keypointShapeCombosList;
		QList<QComboBox*> colorMapCombosList;
		QList<ColorMap::ColorMapType> colorMapTypeList;
		QList<QLabel*> colormapPreviewsList;
		QList<QPushButton*> colorChooserButtonsList;
		QList<QColor> colorsList;
		QList<QWidget*> singleEntityWidgetList;

		QWidget *reprojectionSettingsWidget;
		QSpinBox *minViewsEdit;
		QDoubleSpinBox *errorThresholdEdit;
		QDoubleSpinBox *boneLengthErrorThresholdEdit;

		QWidget *infoWidget;

	private slots:
		void imageTranformationChangedSlot();
		void grayScaleToggledSlot(int state);
		void hueResetClickedSlot();
		void saturationResetClickedSlot();
		void brightnessResetClickedSlot();
		void contrastResetClickedSlot();
		void alwaysShowLabelsToggledSlot(int state);
		void fontColorChooserButtonClickedSlot();
		void backgroundColorChooserButtonClickedSlot();
		void keypointShapeChangedSlot(int index);
		void colorMapChangedSlot(int index);
		void colorChooserClickedSlot();
		void minViewsChangedSlot(int val);
		void errorThresholdChangedSlot(double val);
		void boneLengthErrorThresholdChangedSlot(double val);

};

#endif
