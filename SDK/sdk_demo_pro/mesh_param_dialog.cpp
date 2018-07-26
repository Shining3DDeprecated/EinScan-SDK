

#include "mesh_param_dialog.h"
#include <QDialog>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>
#include <QString>
#include <QHBoxLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QMovie>
#include <qdebug.h>
#include <QButtonGroup>
#include <QLineEdit>
#include <QPainter>
#include <QCheckBox>
#include <QResizeEvent>
#include <iostream>
#include <tchar.h>
#include <QGroupBox>
#include <QIntValidator>
#include <QFileDialog>
#include <QToolTip>
#include <QDateTime>
#include <QStandardPaths>
#include <QRadioButton>
#include "DataCenter.h"

mesh_param_dialog::mesh_param_dialog(QWidget* parent, Qt::WindowFlags flags) :QDialog(parent, flags | Qt::WindowCloseButtonHint)
{
		no_msg_promt = false;
		this->setContentsMargins(2, 2, 2, 2);

		QVBoxLayout* main_layout= new QVBoxLayout();

		init_mesh_group();
		init_simple_group();
		init_fill_hole_group();

		QHBoxLayout* ok_cancel=init_ok_cancel_btn();
		//QHBoxLayout* sharp_smooth=init_smooth_sharpen_group();

		main_layout->addWidget(mesh_group_box);
		main_layout->addWidget(group_simple);
		main_layout->addWidget(hole_group_box);
		//main_layout->addLayout(sharp_smooth);
		main_layout->addLayout(ok_cancel);

		setLayout(main_layout);

		this->adjustSize();

	}
	void mesh_param_dialog::showEvent(QShowEvent *e)
	{
		QDialog::showEvent(e);

		int width = group_simple->width() > hole_group_box->width() ? group_simple->width() : hole_group_box->width();
		group_simple->setFixedWidth(width);
		hole_group_box->setFixedWidth(width);
	}
	/*QHBoxLayout * mesh_param_dialog::init_smooth_sharpen_group()
	{
		smooth_enable = new QCheckBox();
		smooth_enable->setText(tr("Smooth"));
		smooth_enable->setCheckable(true);
		QObject::connect(smooth_enable, SIGNAL(toggled(bool)), SLOT(on_check_smooth(bool)));

		sharp_enable = new QCheckBox();
		sharp_enable->setText(tr("Sharpen"));
		sharp_enable->setCheckable(true);

		QObject::connect(sharp_enable, SIGNAL(toggled(bool)), SLOT(on_check_sharp(bool)));


		QHBoxLayout* h_smooth_sharp = new QHBoxLayout();
		h_smooth_sharp->setContentsMargins(0, 10, 0, 15);
		h_smooth_sharp->setSpacing(20);
		h_smooth_sharp->addWidget(smooth_enable, 0, Qt::AlignCenter);
		h_smooth_sharp->addWidget(sharp_enable, 0, Qt::AlignCenter);
 

		return h_smooth_sharp;
	}*/
	void mesh_param_dialog::simplify_checked(bool state)
	{
		sample_ratio_edit->setEnabled(state);
	}
 
	void  mesh_param_dialog::init_simple_group()
	{
		group_simple = new QGroupBox(tr("Simplify"));

		simplify_enable = new QCheckBox(tr("Simplification ratio"));
		simplify_enable->setCheckable(true);
		simplify_enable->setChecked(true);
		sample_ratio_edit = new QLineEdit();
		sample_ratio_edit->setText("100");

		QHBoxLayout* v_simple = new QHBoxLayout();
		v_simple->addWidget(simplify_enable);
		v_simple->addWidget(sample_ratio_edit);
		group_simple->setLayout(v_simple);

		connect(simplify_enable, SIGNAL(toggled(bool)), this, SLOT(simplify_checked(bool)));
		QObject::connect(sample_ratio_edit, SIGNAL(textChanged(const QString &)), SLOT(on_sample_ratio_change(const QString &)));
	}
	void  mesh_param_dialog::init_fill_hole_group()
	{
		hole_group_box = new QGroupBox(tr("FillHole"));
 
		mark_hole_fill_enable = new QCheckBox(tr("Mark Point"));
		plaint_hole_fill_enable = new QCheckBox(tr("Fill Hole"));
		mark_hole_fill_enable->setCheckable(true);
		plaint_hole_fill_enable->setCheckable(true);
 
		mark_hole_fill_enable->setEnabled((demo_data_center::Instance()->get_whole_marker_count()>0 || SN3D_ALIGN_MODE_GLOABL_MARK_POINT == demo_data_center::Instance()->get_align_mode()) ? true : false);

		plaint_hole_perimeter = new QLineEdit(hole_group_box);
		plaint_hole_label = new QLabel(hole_group_box);
		plaint_hole_label->setText(tr("mm"));

		QObject::connect(plaint_hole_perimeter, SIGNAL(textChanged(const QString &)), SLOT(on_plaint_hole_perimeter(const QString &)));

		QIntValidator* validate = new QIntValidator(plaint_hole_perimeter);
		validate->setBottom(1);

		plaint_hole_perimeter->setValidator(validate);

		int hole_perimeter = 30;
		plaint_hole_perimeter->setText(QString("%1").arg(hole_perimeter));
		plaint_hole_perimeter->setPlaceholderText(QString("%1").arg(hole_perimeter));
		plaint_hole_perimeter->setFixedWidth(100);

		plaint_info_text = new QLabel(hole_group_box);
		plaint_info_text->setWordWrap(true);
		plaint_info_text->setText(tr("Hole-filling:choose 10-100mm perimeter to fill the hole"));

		plaint_info_text->setFixedHeight(60);

		QVBoxLayout*  v_hole_layout = new QVBoxLayout();
		v_hole_layout->addWidget(mark_hole_fill_enable);


		QHBoxLayout* h_hole_layout = new QHBoxLayout();
		h_hole_layout->setContentsMargins(0, 0, 0, 0);
		h_hole_layout->setSpacing(1);
		h_hole_layout->addWidget(plaint_hole_fill_enable, 0, Qt::AlignLeft | Qt::AlignVCenter);
		h_hole_layout->addWidget(plaint_hole_perimeter, 0, Qt::AlignLeft | Qt::AlignVCenter);
		h_hole_layout->addWidget(plaint_hole_label, 0, Qt::AlignLeft | Qt::AlignVCenter);

		v_hole_layout->addLayout(h_hole_layout);
		v_hole_layout->addWidget(plaint_info_text);

		hole_group_box->setLayout(v_hole_layout);

		QObject::connect(mark_hole_fill_enable, SIGNAL(toggled(bool)), SLOT(on_check_hole_maker(bool)));
		QObject::connect(plaint_hole_fill_enable, SIGNAL(toggled(bool)), SLOT(on_check_hole_plaint(bool)));

	 

		connect(simplify_enable, SIGNAL(toggled(bool)), this, SLOT(simplify_checked(bool)));
		hole_group_box->setEnabled(false);
	}

	void  mesh_param_dialog::init_mesh_group()
	{
		mesh_group_box = new QGroupBox(tr("Mesh"));

		mesh_type_watertight = new QRadioButton();
		mesh_type_watertight->setText(tr("Watertight"));
		mesh_type_watertight->setCheckable(true);
		mesh_type_watertight->setChecked(true);

		mesh_type_unwatertight = new QRadioButton();
		mesh_type_unwatertight->setText(tr("UnWatertight"));
		mesh_type_unwatertight->setCheckable(true);

		QButtonGroup* mesh_group = new QButtonGroup();
		mesh_group->addButton(mesh_type_watertight);
		mesh_group->addButton(mesh_type_unwatertight);

		QObject::connect(mesh_type_watertight, SIGNAL(toggled(bool)), this, SLOT(on_check_watertight(bool)));
		QObject::connect(mesh_type_unwatertight, SIGNAL(toggled(bool)), this, SLOT(on_check_unwatertight(bool)));


		mesh_resolution_hight = new QRadioButton();
		mesh_resolution_hight->setText(tr("Hight"));
		mesh_resolution_mid = new QRadioButton();
		mesh_resolution_mid->setText(tr("Middle"));
		mesh_resolution_low = new QRadioButton();
		mesh_resolution_low->setText(tr("Low"));
		mesh_resolution_mid->setChecked(true);

		QButtonGroup* resolution_group = new QButtonGroup();
		resolution_group->addButton(mesh_resolution_hight);
		resolution_group->addButton(mesh_resolution_mid);
		resolution_group->addButton(mesh_resolution_low);

		QHBoxLayout *layout_mesh_type = new QHBoxLayout();
		layout_mesh_type->addWidget(mesh_type_watertight);
		layout_mesh_type->addWidget(mesh_type_unwatertight);

		QHBoxLayout *layout_mesh_resolution = new QHBoxLayout();
		layout_mesh_resolution->addWidget(mesh_resolution_hight);
		layout_mesh_resolution->addWidget(mesh_resolution_mid);
		layout_mesh_resolution->addWidget(mesh_resolution_low);

		QVBoxLayout *layout_mesh_group = new QVBoxLayout();
		layout_mesh_group->addLayout(layout_mesh_type);
		layout_mesh_group->addLayout(layout_mesh_resolution);

		mesh_group_box->setLayout(layout_mesh_group);
		 
	}
	QHBoxLayout * mesh_param_dialog::init_ok_cancel_btn()
	{
		btn_ok = new QPushButton();
		btn_ok->setText(tr("OK"));

	
		btn_cancel = new QPushButton();
		btn_cancel->setText(tr("Cancel"));

		QObject::connect(btn_ok, SIGNAL(clicked()), this, SLOT(on_ok()));
		QObject::connect(btn_cancel, SIGNAL(clicked()), this, SLOT(on_cancel()));

		QHBoxLayout *layout_button = new QHBoxLayout();
		layout_button->addWidget(btn_ok);
		layout_button->addWidget(btn_cancel);
		return layout_button;
	}

	mesh_param_dialog::~mesh_param_dialog()
	{

	}

	 
	void mesh_param_dialog::on_check_watertight(bool flag)
	{
		if (flag)
		{
			mesh_resolution_hight->setEnabled(true);
			mesh_resolution_mid->setEnabled(true);
			mesh_resolution_low->setEnabled(true);
			hole_group_box->setEnabled(false);
		} 
	}
	void mesh_param_dialog::on_check_unwatertight(bool flag)
	{
		if (flag)
		{
			mesh_resolution_hight->setEnabled(false);
			mesh_resolution_mid->setEnabled(false);
			mesh_resolution_low->setEnabled(false);
			hole_group_box->setEnabled(true);
		}
	}
	void mesh_param_dialog::on_check_hole_maker(bool flag)
	{

	}
	void mesh_param_dialog::on_check_hole_plaint(bool flag)
	{

		plaint_hole_perimeter->setEnabled(flag);

	}

	void mesh_param_dialog::on_check_smooth(bool flag)
	{

	}
	void mesh_param_dialog::on_check_sharp(bool flag)
	{

	}

	
	void mesh_param_dialog::on_plaint_hole_perimeter(const QString &text_)
	{
		bool ok_;
		QString msg = tr("Suggest perimeter range 10-100mm");
		int hole_perimeter = text_.toInt(&ok_, 10);
		if (ok_ && hole_perimeter>=1)
		{
			if (hole_perimeter<10 || hole_perimeter>100)
			{
				QPoint  left_top = plaint_hole_perimeter->pos();
				left_top.setY(left_top.y()+5);
				left_top = hole_group_box->mapToGlobal(left_top);

				QRect rt;
				rt.setTopLeft(left_top);
				rt.setHeight(30);
				rt.setWidth(100);

				QToolTip::showText(left_top, msg, hole_group_box, rt, 1000);

			}
		}
		else
		{
			QMessageBox::information(this, tr("Notice"), msg, tr("OK"));
		}

	}
	void mesh_param_dialog::on_sample_ratio_change(const QString &text_)
	{
		bool ok_;
		int percentage_ = text_.toInt(&ok_, 10);
		if (1 <= percentage_&&percentage_ <= 100){
		
	
		}
		else if (100 < percentage_ || percentage_<=0){
			qDebug() << "text_= " << text_ << "percentage_=" << percentage_ << "ok_=" << ok_ << "no_msg_promt=" << no_msg_promt;
			if (no_msg_promt)
			{
				no_msg_promt = false;
				return;
			}
			if (text_.length()>0)
			{
				no_msg_promt = true;
			}
			QMessageBox::information(this, tr("Notice"), tr("Check, Simplification ratio 1-100"), tr("OK"));
			sample_ratio_edit->setText("");
			
		}
	}
	
	void mesh_param_dialog::on_ok()
	{
		QDialog::accept();
	}
	void mesh_param_dialog::on_cancel()
	{
		QDialog::reject();
	}
	void mesh_param_dialog::disableMark()
	{
		mark_hole_fill_enable->setChecked(false);
		mark_hole_fill_enable->setEnabled(false);
	}
	void mesh_param_dialog::get_param_config(SN3D_MESH_PROCESS_PARAM& param)
	{
		param.mesh_type = mesh_type_watertight->isChecked() ? SN3D_MESH_WATERTIGHT : SN3D_MESH_UNWATERTIGHT;
		param.mesh_resolution = SN3D_MESH_MIDDLE;
		if (mesh_resolution_hight->isChecked())
		{
			param.mesh_resolution = SN3D_MESH_HIGHT;
		}
		else if (mesh_resolution_low->isChecked())
		{
			param.mesh_resolution = SN3D_MESH_LOW;
		}

		
		param.simplification_flag = simplify_enable->isChecked();
		param.simplification_ratio = sample_ratio_edit->text().toInt();
		param.smooth_flag = 0; //smooth_enable->isChecked();
		param.sharpen_flag = 0;// sharp_enable->isChecked();
		param.mark_point_fill_hole_flag = mark_hole_fill_enable->isChecked();
		param.plaint_fill_hole_flag = plaint_hole_fill_enable->isChecked();
		param.plaint_fill_hole_perimeter = plaint_hole_perimeter->text().toInt();
		
		qDebug() << "mesh_type=" << param.mesh_type ;
		qDebug() << "mesh_resolution=" << param.mesh_resolution << "simplification_flag=" << param.simplification_flag;
		qDebug() << "simplification_ratio=" << param.simplification_ratio << "mark_point_fill_hole_flag=" << param.mark_point_fill_hole_flag;
		qDebug() << "plaint_fill_hole_perimeter=" << param.plaint_fill_hole_perimeter << "plaint_fill_hole_flag=" << param.plaint_fill_hole_flag;
	}
