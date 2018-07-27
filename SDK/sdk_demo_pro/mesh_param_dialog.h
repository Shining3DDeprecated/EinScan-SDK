#ifndef s_dialog_H
#define s_dialog_H


#include <iostream>
#include <fstream>
#include <vector>
#include <windows.h>

#include <QObject>
#include <QPushButton>
#include <QDialog>
#include <QLabel>
#include <QString>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QProgressBar>
#include <qmovie.h>
#include <QSplitter>
#include <QDialog>
#include <QLineEdit>
#include <QSlider> 
#include <QCheckBox>
#include <QTimer>
#include <map>
#include <QGroupBox>
#include <QRegExpValidator>
#include <QFileDialog>
#include <QSlider>
#include <QRadioButton>
#include "sn3d_pro_sdk.h"

class   mesh_param_dialog : public QDialog
{
	Q_OBJECT
public:
	mesh_param_dialog(QWidget* parent = 0, Qt::WindowFlags flags = 0);
	virtual ~mesh_param_dialog();

	void get_param_config(SN3D_MESH_PROCESS_PARAM& param);
	void disableMark();
protected slots:
	
	void on_sample_ratio_change(const QString &text_);
	void on_plaint_hole_perimeter(const QString &text_);
	void on_ok();
	void on_cancel();
	void on_check_hole_maker(bool flag);
	void on_check_hole_plaint(bool flag);

	void on_check_watertight(bool flag);
	void on_check_unwatertight(bool flag);

	void on_check_smooth(bool flag);
	void on_check_sharp(bool flag);
	void simplify_checked(bool state);
 
protected:
	virtual void showEvent(QShowEvent *);
	QHBoxLayout * init_ok_cancel_btn();
	void init_mesh_group();
	void init_simple_group();
	void init_fill_hole_group();
	//QHBoxLayout* init_smooth_sharpen_group();
 
  
	//mesh group
	QGroupBox*  mesh_group_box;

	QRadioButton* mesh_type_watertight;
	QRadioButton* mesh_type_unwatertight;

	QRadioButton* mesh_resolution_hight;
	QRadioButton* mesh_resolution_mid;
	QRadioButton* mesh_resolution_low;
	//simple
	QGroupBox* group_simple;
    QLineEdit* sample_ratio_edit;
	QCheckBox* simplify_enable;

	//fill hole
	QGroupBox*  hole_group_box;
	QCheckBox*  mark_hole_fill_enable;
	QCheckBox*  plaint_hole_fill_enable;
	QLineEdit*  plaint_hole_perimeter;
	QLabel   *  plaint_hole_label;
	QLabel*     plaint_info_text;
	
	//smooth sharpen 
	/*QCheckBox* smooth_enable;
	QCheckBox* sharp_enable;*/
		
	//ok cancel 
	QPushButton* btn_ok;
	QPushButton* btn_cancel;

	//
	bool       no_msg_promt;//消息框跳两次问题
};


#endif 