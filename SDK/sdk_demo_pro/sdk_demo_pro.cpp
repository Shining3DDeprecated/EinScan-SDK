//#include <vld.h>
#include "sdk_demo_pro.h"
#include <qgroupbox.h>
#include <QLineEdit>
#include <QGridLayout>
#include <QMessageBox>
#include <qcombobox.h>
#include <QFileDialog>
#include "DataCenter.h"
#include <qDebug>
#include <qvalidator.h>
#include <QToolTip>
#include "sn3d_pro_sdk.h"
#include "mesh_param_dialog.h"
#include <time.h>


void CALLBACK scan_demo_callback(void*  handle, int  event_type, int  event_sub_type, void* data, int data_len, void*  user_data)
{
	if (user_data == 0)
		return;

	sdk_demo_pro* sdk_demo = (sdk_demo_pro*)user_data;
	sdk_demo->scan_callback(handle, event_type, event_sub_type, data, data_len);
}
void sdk_demo_pro::post_msg_data_update(int type, int param)
{
	msg_data_t msg_data;
	msg_data.hint = 0;
	msg_data.msg_type = type;
	msg_data.map_data["param"] = param;
	emit sig_handle_msg(msg_data);
}
void sdk_demo_pro::scan_callback(void*  handle, int  event_type, int  event_sub_type, void* data, int data_len)
{
	switch (event_type)
	{
	case SN3D_VIDEO_IMAGE_DATA_READY:
	{
										 
										SN3D_VIDEO_FRAME* video_frm = (SN3D_VIDEO_FRAME*)data;
										msg_data_t msg_data;
										msg_data.hint = video_frm->id;
										msg_data.map_data["width"] = video_frm->video_data.width;
										msg_data.map_data["height"] = video_frm->video_data.height;
										msg_data.msg_type = SN3D_VIDEO_IMAGE_DATA_READY;
										if (video_frm->id == 0 || video_frm->id == 1)
										{
											QImage img((unsigned char*)(video_frm->video_data.data), video_frm->video_data.width, video_frm->video_data.height, QImage::Format_Grayscale8);
											msg_data.map_data["image"] = img.copy();
											emit sig_handle_msg(msg_data);
										}
										else if (video_frm->id == 2)
										{
											int width_step = video_frm->video_data.width * 3;
											unsigned char* imageColor = new unsigned char[width_step*video_frm->video_data.height];
										
											unsigned char* img_data = (unsigned char*)video_frm->video_data.data;
											memcpy(imageColor, img_data, width_step*video_frm->video_data.height);
											QVariant varData;
											varData.setValue<unsigned char*>(imageColor);
											msg_data.map_data["image"] = varData;
										 
											emit sig_handle_msg(msg_data);

										}
										
										break;
	}
	case  SN3D_SCANNER_RT_READY:
	{

								   SN3D_SCANNER_RT* rt = (SN3D_SCANNER_RT*)data;

								   demo_data_center::Instance()->update_scanner_rt(*rt);
								   post_msg_data_update(SN3D_SCANNER_RT_READY, 0);
								   break;
	}

	case  SN3D_PREVIEW_SCAN_MARKPOINT_DATA_READY:
	case  SN3D_CURRENT_MARKPOINT_DATA_READY:
	{
													 SN3D_CLOUD_POINT point = { 0 };
													 int ret = sn3d_get_current_mark_point(handle, point);
													 if (ret != SN3D_RET_NOERROR)
													 {
														// qDebug() << " fail ret" << ret;
														 demo_data_center::Instance()->update_current_mark_point(point);
														 post_msg_data_update(SN3D_CURRENT_MARKPOINT_DATA_READY, 0);
														 return;
													 }

													 if (!demo_data_center::Instance()->allocate_point_memory(point))
													 {
														 qDebug() << " fail allocate_point_memory";
														 return;
													 }
													 ret = sn3d_get_current_mark_point(handle, point);
													 if (ret != SN3D_RET_NOERROR)
													 {
														 qDebug() << " fail ret" << ret;
														 demo_data_center::Instance()->free_point_memory(point);
														 return;
													 }
													 demo_data_center::Instance()->update_current_mark_point(point);
													 post_msg_data_update(SN3D_CURRENT_MARKPOINT_DATA_READY, point.vertex_count);
													 break;
	}
	case SN3D_WHOLE_MARKPOINT_DATA_READY:
	{
												  SN3D_CLOUD_POINT point = { 0 };
												  int ret = sn3d_get_whole_mark_point(handle, point);
												  if (ret != SN3D_RET_NOERROR)
												  {
													  qDebug() << " fail ret" << ret;
													  return;
												  }

												  if (!demo_data_center::Instance()->allocate_point_memory(point))
												  {
													  qDebug() << " fail allocate_point_memory";
													  return;
												  }
												  ret = sn3d_get_whole_mark_point(handle, point);
												  if (ret != SN3D_RET_NOERROR)
												  {
													  qDebug() << " fail ret" << ret;
													  demo_data_center::Instance()->free_point_memory(point);
													  return;
												  }
												  demo_data_center::Instance()->update_whole_mark_point(point);
												  post_msg_data_update(SN3D_WHOLE_MARKPOINT_DATA_READY, point.vertex_count);
												  qDebug() << " SN3D_WHOLE_MARKPOINT_DATA_READY point.vertex_count=" << point.vertex_count;
												  break;
	}
	case  SN3D_CURRENT_SCAN_POINT_CLOUD_READY:
	case  SN3D_PREVIEW_SCAN_POINT_CLOUD_READY:
	{
													   SN3D_CLOUD_POINT point = { 0 };
													   int ret = sn3d_get_current_scan_point_cloud(handle, point);
													   if (ret != SN3D_RET_NOERROR)
													   {
														 //  qDebug() << " fail ret" << ret;
														   demo_data_center::Instance()->update_current_cloud_point(point);
														   post_msg_data_update(SN3D_CURRENT_SCAN_POINT_CLOUD_READY, 0);
														   return;
													   }

													   if (!demo_data_center::Instance()->allocate_point_memory(point))
													   {
														   qDebug() << " fail allocate_point_memory";
														   return;
													   }
													   ret = sn3d_get_current_scan_point_cloud(handle, point);
													   if (ret != SN3D_RET_NOERROR)
													   {
														   qDebug() << " fail ret" << ret;
														   demo_data_center::Instance()->free_point_memory(point);
														   return;
													   }
													 
													   demo_data_center::Instance()->update_current_cloud_point(point);
													   post_msg_data_update(SN3D_CURRENT_SCAN_POINT_CLOUD_READY, point.vertex_count);

													   break;
	}

	case  SN3D_WHOLE_SCAN_POINT_CLOUD_READY:
	{
													 SN3D_CLOUD_POINT point = { 0 };
													 int ret = sn3d_get_current_scan_whole_point_cloud(handle, point);
													 if (ret != SN3D_RET_NOERROR)
													 {
														 qDebug() << " fail ret" << ret;
														 return;
													 }

													 if (!demo_data_center::Instance()->allocate_point_memory(point))
													 {
														 qDebug() << " fail allocate_point_memory";
														 return;
													 }
													 ret = sn3d_get_current_scan_whole_point_cloud(handle, point);
													 if (ret != SN3D_RET_NOERROR)
													 {
														 qDebug() << " fail ret" << ret;
														 demo_data_center::Instance()->free_point_memory(point);
														 return;
													 }
													 demo_data_center::Instance()->update_whole_cloud_point(point);
													 post_msg_data_update(SN3D_WHOLE_SCAN_POINT_CLOUD_READY, point.vertex_count);
													 break;
	}
	case SN3D_DISTANCE_INDECATE:
	case SN3D_SCANNER_DOUBLECLICK:
	case SN3D_SCANNER_CLICK:
	case SN3D_SCANNER_PLUS:
	case SN3D_SCANNER_SUB:
	{
										int * event_data = (int *)data;
										post_msg_data_update(event_type, *event_data);
										break;
	}
	case SN3D_SENSOR_UNPLUGGED:
	{
								  qDebug() << "@@@@ SN3D_SENSOR_UNPLUGGED" ;
	}break;
	case SN3D_CALIBRATION_STATE_DATA:
	{

										SN3D_STATE_CALIBRATE*  calibrate_data = (SN3D_STATE_CALIBRATE*)data;
										
										msg_data_t msg_data;
										msg_data.hint = 0;
										msg_data.msg_type = SN3D_CALIBRATION_STATE_DATA;
										msg_data.map_data["current_calibrate"] = calibrate_data->current_calibrate;
										msg_data.map_data["distance"] = calibrate_data->distance_indicate;
										msg_data.map_data["group"] = calibrate_data->current_group;
										msg_data.map_data["snap_state"] = calibrate_data->snap_state;
										msg_data.map_data["compute_state"] = calibrate_data->compute_state;
										emit sig_handle_msg(msg_data);
										break;
	}

	case  SN3D_MESH_DATA_READY:
	{
								 qDebug() << "get SN3D_MESH_DATA_READY" ;
								 SN3D_TRI_MESH mesh_trim = { 0 };
								 int ret = sn3d_get_current_mesh_data(handle, mesh_trim);
								if (ret != SN3D_RET_NOERROR)
								{
									qDebug() << " fail ret" << ret;
									post_msg_data_update(SN3D_MESH_DATA_READY, -1);
									return;
								}

								if (!demo_data_center::Instance()->allocate_mesh_trim(mesh_trim))
								{
									qDebug() << " fail allocate_point_memory";
									post_msg_data_update(SN3D_MESH_DATA_READY, -1);
									return;
								}
								ret = sn3d_get_current_mesh_data(handle, mesh_trim);
								if (ret != SN3D_RET_NOERROR)
								{
									qDebug() << " fail ret" << ret;
									post_msg_data_update(SN3D_MESH_DATA_READY, -1);
									demo_data_center::Instance()->free_mesh_trim(mesh_trim);
									return;
								}

								qDebug() << " face count=%d" << mesh_trim.face_count;
								demo_data_center::Instance()->update_mesh_trim(mesh_trim);
								post_msg_data_update(SN3D_MESH_DATA_READY, mesh_trim.face_count);
								break;
	}
	default:

		break;
	}
}
sdk_demo_pro::sdk_demo_pro(QWidget *parent)
: QDialog(parent, Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint)
{
	handle = 0;

	cur_point_count = 0;
	whole_point_count = 0;
	distance_value = 0;
	marker_point_count = 0;
	whole_marker_point_count = 0;

	current_calibrate_type = SN3D_CALIBRATE_INVALID;
	current_camera_group = SN3D_CALIBRATE_INVALID;

	qRegisterMetaType<msg_data_t>();
	qRegisterMetaType<unsigned char*>();

	QGridLayout* main_layout = new QGridLayout();

	main_layout->addLayout(init_camera_video(), 0, 0, 1, 4);
	main_layout->addWidget(init_scanner(), 1, 0);
	main_layout->addWidget(init_calibrate(), 1, 1);
	main_layout->addWidget(init_scan(), 1, 2);

	this->setLayout(main_layout);

	QObject::connect(open_scanner, SIGNAL(clicked()), SLOT(on_open_scanner()));
	QObject::connect(close_scanner, SIGNAL(clicked()), SLOT(on_close_scanner()));
	QObject::connect(start_calibrate, SIGNAL(clicked()), SLOT(on_start_calibrate()));
	QObject::connect(skip_calibrate, SIGNAL(clicked()), SLOT(on_skip_calibrate()));

	QObject::connect(get_scan_param, SIGNAL(clicked()), SLOT(on_get_scan_param()));
	QObject::connect(set_scan_param, SIGNAL(clicked()), SLOT(on_set_scan_param()));
	//QObject::connect(align_mode_feature, SIGNAL(toggled(bool)), SLOT(on_feature_toggled(bool)));
	//QObject::connect(align_mode_markpoint, SIGNAL(toggled(bool)), SLOT(on_markpoint_toggled(bool)));
	QObject::connect(align_mode_global_mark_point, SIGNAL(toggled(bool)), SLOT(on_global_mark_point_toggled(bool)));

	QObject::connect(start_scan, SIGNAL(clicked()), SLOT(on_start_scan()));
	QObject::connect(pause_scan, SIGNAL(clicked()), SLOT(on_pause_scan()));
	QObject::connect(finish_scan, SIGNAL(clicked()), SLOT(on_finish_scan()));
	QObject::connect(cancel_scan, SIGNAL(clicked()), SLOT(on_cancel_scan()));
	QObject::connect(mesh_data, SIGNAL(clicked()), SLOT(on_mesh_data()));
	QObject::connect(save_whole_data, SIGNAL(clicked()), SLOT(on_save_whole_data()));
	QObject::connect(save_whole_mark_data, SIGNAL(clicked()), SLOT(on_save_whole_mark_data()));
	
	QObject::connect(delete_point, SIGNAL(clicked()), SLOT(on_delete_point()));
	QObject::connect(brightness_list, SIGNAL(currentIndexChanged(int)), SLOT(on_brightness_list_change(int)));
	QObject::connect(this, SIGNAL(sig_handle_msg(msg_data_t)), SLOT(handle_msg(msg_data_t)));

	current_ui_sate = ui_state_ui_init;
	change_ui_state(ui_state_ui_init);

	setFixedHeight(800);
}

QBoxLayout* sdk_demo_pro::init_camera_video()
{
	//camera video data
	left_camera_video = new QLabel();
	right_camera_video = new QLabel();
	colour_camera_video = new QLabel();
	left_camera_video->setFixedSize(QSize(360, 270));
	right_camera_video->setFixedSize(QSize(360, 270));
	colour_camera_video->setFixedSize(QSize(360, 270));
	left_camera_video->setStyleSheet("QLabel{background-color:rgb(0,0,0);}");
	right_camera_video->setStyleSheet("QLabel{background-color:rgb(0,0,0);}");
	colour_camera_video->setStyleSheet("QLabel{background-color:rgb(0,0,0);}");

	QHBoxLayout* h_layout = new QHBoxLayout();
	h_layout->addWidget(left_camera_video);
	h_layout->addWidget(right_camera_video);
	h_layout->addWidget(colour_camera_video);

	return h_layout;
}

QWidget* sdk_demo_pro::init_scanner()
{
	device_type = new QCheckBox();
	device_type->setText(tr("EinScan-Plus"));
	init_type_list = new QComboBox();

	QStringList items;
	items << tr("Calibrate") <<tr("RapidScan") << tr("HDScan") ;
	init_type_list->addItems(items);

	init_type_list->setItemData(0, SN3D_INIT_CALIBRATE);
	init_type_list->setItemData(1, SN3D_INIT_RAPIDSCAN);
	init_type_list->setItemData(2, SN3D_INIT_HD_SCAN);
	init_type_list->setCurrentIndex(0);

	//open close scanner
	open_scanner = new QPushButton();
	open_scanner->setText(tr("OpenScanner"));
	close_scanner = new QPushButton();
	close_scanner->setText(tr("CloseScanner"));
	scaner_prompt = new QLabel();
	QVBoxLayout* v_layout = new QVBoxLayout();
	v_layout->addWidget(device_type);
	v_layout->addWidget(init_type_list);
	v_layout->addWidget(open_scanner);
	v_layout->addWidget(close_scanner);
	v_layout->addWidget(scaner_prompt);

	scanner_group = new QGroupBox();
	scanner_group->setTitle(tr("Scanner"));
	scanner_group->setLayout(v_layout);
	return scanner_group;
}

QWidget* sdk_demo_pro::init_calibrate()
{
	//calibrate

	start_calibrate = new QPushButton();
	start_calibrate->setText(tr("Snap"));


	skip_calibrate = new QPushButton();
	skip_calibrate->setText(tr("SkipCalibrate"));

	QVBoxLayout* v_layout = new QVBoxLayout();
	v_layout->addWidget(start_calibrate);
	v_layout->addWidget(skip_calibrate);

	calibrate_prompt = new QLabel();

	calibrate_type = new QLabel();
	calibrate_state = new QLabel();

	calibrate_type->setFixedHeight(40);
	calibrate_state->setFixedHeight(40);
	calibrate_prompt->setFixedHeight(40);

	v_layout->addWidget(calibrate_type);
	v_layout->addWidget(calibrate_state);
	v_layout->addWidget(calibrate_prompt);

	v_layout->addWidget(init_calibrate_distance());


	calibrate_group = new QGroupBox();
	calibrate_group->setTitle(tr("Calibrate"));
	calibrate_group->setLayout(v_layout);
	calibrate_group->setFixedWidth(400);
	return calibrate_group;

}

QWidget* sdk_demo_pro::init_scan()
{
	//video  brightness
	QLabel* brightness = new QLabel();
	brightness->setText(tr("AdjustBrightness"));
	brightness_list = new QComboBox();
	for (int i = 0; i <= 16; i++)
	{
		brightness_list->addItem(QString("%1").arg(i), i);
	}

	QHBoxLayout* brightness_layout = new QHBoxLayout();
	brightness_layout->addWidget(brightness);
	brightness_layout->addWidget(brightness_list);

	//
	QLabel* near_min = new QLabel();
	near_min->setText(tr("near_min(>=200.0):"));
	near_min_edit = new QLineEdit();
	QDoubleValidator* near_min_validate = new QDoubleValidator(near_min_edit);
	near_min_validate->setBottom(200.0);
	near_min_validate->setTop(800.0);
	near_min_edit->setValidator(near_min_validate);
	//
	QLabel* far_max = new QLabel();
	far_max->setText(tr("far_max:(<=800.0)"));
	far_max_edit = new QLineEdit();
	QDoubleValidator* far_max_validate = new QDoubleValidator(far_max_edit);
	far_max_validate->setBottom(200.0);
	far_max_validate->setTop(800.0);
	far_max_edit->setValidator(far_max_validate);
	QPushButton* get_scan_range = new QPushButton();
	get_scan_range->setText(tr("GetScanRange"));
	QPushButton* set_scan_range = new QPushButton();
	set_scan_range->setText(tr("SetScanRange"));


	set_scan_range->hide();
	get_scan_range->hide();
	near_min->hide();
	near_min_edit->hide();
	far_max->hide();
	far_max_edit->hide();

	QHBoxLayout* h_scan_range = new QHBoxLayout();
	h_scan_range->addWidget(get_scan_range);
	h_scan_range->addWidget(set_scan_range);
	h_scan_range->addWidget(near_min);
	h_scan_range->addWidget(near_min_edit);
	h_scan_range->addWidget(far_max);
	h_scan_range->addWidget(far_max_edit);
	

	QObject::connect(get_scan_range, SIGNAL(clicked()), SLOT(on_get_scan_range()));
	QObject::connect(set_scan_range, SIGNAL(clicked()), SLOT(on_set_scan_range()));

	//scan param group
	QGroupBox* scan_param_group = new QGroupBox();
	scan_param_group->setTitle(tr("ScanParam"));

	//
	get_scan_param = new QPushButton();
	get_scan_param->setText(tr("GetScanParam"));
	set_scan_param = new QPushButton();
	set_scan_param->setText(tr("SetScanParam"));

	resolution_label = new QLabel();
	resolution_label->setText(tr("Resolution ( 0.5-3.0 )"));
	point_resolution_value = new QLineEdit();

	QDoubleValidator* resolution_validate = new QDoubleValidator(point_resolution_value);
	resolution_validate->setBottom(0.2);
	resolution_validate->setTop(3.0);
	point_resolution_value->setValidator(resolution_validate);
	QObject::connect(point_resolution_value, SIGNAL(textChanged(const QString &)), SLOT(resolution_value_change(const QString &)));

	texture = new QCheckBox();
	texture->setText(tr("Texture"));

	//scan param  align mode 

	align_mode_global_mark_point = new QRadioButton();
	align_mode_global_mark_point->setText(tr("GlobalMarkPoint"));
 
	
	align_mode_feature = new QRadioButton();
	align_mode_feature->setText(tr("Feature"));

	align_mode_markpoint = new QRadioButton();

	align_mode_markpoint->setText(tr("MarkPoint"));

	align_group = new QGroupBox();
	align_group->setFixedHeight(45);
	align_group->setTitle(tr("AlignMode"));

	QHBoxLayout* h_layout = new QHBoxLayout();
	h_layout->addWidget(align_mode_feature);
	h_layout->addWidget(align_mode_markpoint);
	h_layout->addWidget(align_mode_global_mark_point);

	align_group->setLayout(h_layout);

	QGridLayout* scan_paran_layout = new QGridLayout();
	scan_paran_layout->setSpacing(2);
	scan_paran_layout->setContentsMargins(3, 1, 3, 1);

	scan_paran_layout->addWidget(resolution_label, 0, 0);
	scan_paran_layout->addWidget(point_resolution_value, 0, 1);
	scan_paran_layout->addWidget(texture, 1, 0, 1, 2);
	 
	scan_paran_layout->addWidget(align_group, 2, 0, 1, 2);

	scan_paran_layout->addWidget(get_scan_param, 3, 0);
	scan_paran_layout->addWidget(set_scan_param, 3, 1);

	scan_param_group->setLayout(scan_paran_layout);
	scan_param_group->setFixedHeight(125);

	//scan control
	start_scan = new QPushButton();
	start_scan->setText(tr("StartPreview"));
	pause_scan = new QPushButton();
	pause_scan->setText(tr("PauseScan"));
	finish_scan = new QPushButton();
	finish_scan->setText(tr("FinishScan"));
	cancel_scan = new QPushButton();
	cancel_scan->setText(tr("CancelScan"));
	mesh_data = new QPushButton();
	mesh_data->setText(tr("Mesh"));

	//export data 
	save_whole_data = new QPushButton();
	save_whole_data->setText(tr("SaveCloudPoint"));

	save_whole_mark_data = new QPushButton();
	save_whole_mark_data->setText(tr("SaveWholeMarkPoint"));

	/*save_mesh_data = new QPushButton();
	save_mesh_data->setText(tr("SaveMeshData"));*/
	//delete point group 
	delete_point = new QPushButton();
	delete_point->setText(tr("DeletePoint"));

	point_index0_start = new QLineEdit();
	point_index0_end = new QLineEdit();
	point_index1_start = new QLineEdit();
	point_index1_end = new QLineEdit();
	point_index2_start = new QLineEdit();
	point_index2_end = new QLineEdit();

	//point index >=0
	QIntValidator* start0_validate = new QIntValidator(point_index0_start);
	start0_validate->setBottom(0);
	point_index0_start->setValidator(start0_validate);

	QIntValidator* end0_validate = new QIntValidator(point_index0_end);
	end0_validate->setBottom(0);
	point_index0_end->setValidator(start0_validate);

	QIntValidator* start1_validate = new QIntValidator(point_index1_start);
	start1_validate->setBottom(0);
	point_index1_start->setValidator(start0_validate);

	QIntValidator* end1_validate = new QIntValidator(point_index1_end);
	end1_validate->setBottom(0);
	point_index1_end->setValidator(end1_validate);

	QIntValidator* start2_validate = new QIntValidator(point_index2_start);
	start2_validate->setBottom(0);
	point_index2_start->setValidator(start2_validate);

	QIntValidator* end2_validate = new QIntValidator(point_index2_end);
	end2_validate->setBottom(0);
	point_index2_end->setValidator(end2_validate);

	//delete point index layout
	QGridLayout* point_index_data = new QGridLayout();

	QLabel* index0 = new QLabel();
	index0->setText("IndexGroup:");
	point_index_data->addWidget(index0, 0, 0);
	point_index_data->addWidget(point_index0_start, 0, 1);
	QLabel*index0_span = new QLabel();
	index0_span->setText("--");
	point_index_data->addWidget(index0_span, 0, 2);
	point_index_data->addWidget(point_index0_end, 0, 3);

	//
	QLabel* index1 = new QLabel();
	index1->setText("IndexGroup:");
	point_index_data->addWidget(index1, 1, 0);
	point_index_data->addWidget(point_index1_start, 1, 1);
	QLabel*index1_span = new QLabel();
	index1_span->setText("--");
	point_index_data->addWidget(index0_span, 1, 2);
	point_index_data->addWidget(point_index1_end, 1, 3);

	QLabel* index2 = new QLabel();
	index2->setText("IndexGroup:");
	point_index_data->addWidget(index2, 2, 0);
	point_index_data->addWidget(point_index2_start, 2, 1);
	QLabel*index2_span = new QLabel();
	index2_span->setText("--");
	point_index_data->addWidget(index2_span, 2, 2);
	point_index_data->addWidget(point_index2_end, 2, 3);

	point_index_data->addWidget(delete_point, 3, 0, 1, 4);


	QGroupBox* delete_point_group = new QGroupBox();
	delete_point_group->setTitle(tr("Deleted Point"));
	delete_point_group->setLayout(point_index_data);
	delete_point_group->setFixedHeight(140);


	//ui layout
	QVBoxLayout* v_layout = new QVBoxLayout();

	v_layout->setSpacing(2);
	v_layout->addLayout(brightness_layout);
	v_layout->addLayout(h_scan_range);
	v_layout->addWidget(scan_param_group);

	QHBoxLayout* scan_ctrl = new QHBoxLayout;
	scan_ctrl->addWidget(start_scan);
	scan_ctrl->addWidget(pause_scan);
	scan_ctrl->addWidget(finish_scan);
	scan_ctrl->addWidget(cancel_scan);
	scan_ctrl->addWidget(mesh_data);

	v_layout->addLayout(scan_ctrl);
	 

	QHBoxLayout* save_data = new QHBoxLayout;
	save_data->addWidget(save_whole_data);
	save_data->addWidget(save_whole_mark_data);
	//save_data->addWidget(save_mesh_data);
	v_layout->addLayout(save_data);

	v_layout->addWidget(delete_point_group);

	//scan data infomation
	scan_point_count = new QLabel();
	scan_point_count->setFixedHeight(60);
	scan_distance = new QLabel();
	scan_distance->setFixedHeight(40);
	v_layout->addWidget(scan_distance);
	v_layout->addWidget(scan_point_count);

	//scan group
	scan_group = new QGroupBox();
	scan_group->setTitle(tr("Scan"));
	scan_group->setLayout(v_layout);

	return scan_group;

}

sdk_demo_pro::~sdk_demo_pro()
{

}

int sdk_demo_pro::handle_update_video(msg_data_t msg_data)
{

	int imgW_ = msg_data.map_data["width"].toInt();
	int imgH_ = msg_data.map_data["height"].toInt();

	QImage  picture;
	unsigned char* img_data = NULL;
	if (msg_data.hint == 2)//color camera
	{
		QImage testStep(imgW_, imgH_, QImage::Format_RGB888);

		int width_step = testStep.bytesPerLine();
		 
		img_data=msg_data.map_data["image"].value<unsigned char*>();
	
		unsigned char u = 0;
		for (int i = 0; i < imgW_; i++)
		{
			for (int j = 0; j < imgH_; j++)
			{
                u = (unsigned char)img_data[j*width_step + i * 3 + 2];
				img_data[j*width_step + i * 3 + 2] = (unsigned char)img_data[j*width_step + i * 3 + 0];
				img_data[j*width_step + i * 3 + 0] = u;
			}
		}
		picture = QImage(img_data, imgW_, imgH_, QImage::Format_RGB888);
			 
	}
	else// left right camera
	{
		picture = msg_data.map_data["image"].value<QImage>();
	}
	
	QLabel* video_wnd = left_camera_video;
	if (msg_data.hint == 1)//right cam
	{
		video_wnd = right_camera_video;
	}
	else if (msg_data.hint == 2)
	{
		video_wnd = colour_camera_video;
	}
	int nWidth = video_wnd->width();
	int nHeight = video_wnd->height();
	QPixmap pixmap = QPixmap::fromImage(picture);
	pixmap = pixmap.scaled(QSize(nWidth, nHeight), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

	QMatrix left_matrix_;
	left_matrix_.rotate(180);

	pixmap = pixmap.transformed(left_matrix_, Qt::SmoothTransformation);

	video_wnd->setPixmap(pixmap);

	if (img_data != NULL)
		delete[] img_data;
	img_data = NULL;

	return 0;
}
int sdk_demo_pro::change_ui_state(int state)
{
	switch (state)
	{
	case ui_state_ui_init:
	{

			calibrate_group->setEnabled(false);
			scan_group->setEnabled(false);
			close_scanner->setEnabled(false);
			open_scanner->setEnabled(true);
			device_type->setEnabled(true);
			cur_face_count = 0;
	}
		break;
	case ui_state_open_scanner_ok:
	{
			if (current_type == SN3D_INIT_CALIBRATE)
			{

				calibrate_group->setEnabled(true);
				scan_group->setEnabled(false);
				close_scanner->setEnabled(true);
				open_scanner->setEnabled(false);
				device_type->setEnabled(false);
				start_calibrate->setEnabled(true);
				skip_calibrate->setEnabled(true);
				start_calibrate->setProperty("calibrate_paused", false);
				cur_face_count = 0;
			}
			else if (current_type == SN3D_INIT_RAPIDSCAN || current_type == SN3D_INIT_HD_SCAN)
			{
				start_scan->setText(tr("StartPreview"));
				cur_face_count = 0;
				calibrate_group->setEnabled(false);
				scan_group->setEnabled(true);
				close_scanner->setEnabled(true);
				open_scanner->setEnabled(false);
				device_type->setEnabled(false);
 
				align_group->setEnabled(true);

				set_scan_param->setEnabled(true);
				get_scan_param->setEnabled(true);
				start_scan->setEnabled(true);
				pause_scan->setEnabled(false);
				finish_scan->setEnabled(false);
				cancel_scan->setEnabled(false);
				save_whole_data->setEnabled(false);
				save_whole_mark_data->setEnabled(false);
				delete_point->setEnabled(false);
				point_index0_start->setEnabled(false);
				point_index0_end->setEnabled(false);
				point_index1_start->setEnabled(false);
				point_index1_end->setEnabled(false);
				point_index2_start->setEnabled(false);
				point_index2_end->setEnabled(false);
				mesh_data->setEnabled(false);

				if (current_type == SN3D_INIT_HD_SCAN)
				{
					texture->hide();
					align_mode_feature->hide();
					resolution_label->setText(tr("Resolution ( 0.2-3.0 )"));
				 
				}
				else
				{
					texture->show();
					align_mode_feature->show();
					resolution_label->setText(tr("Resolution ( 0.5-3.0 )"));
					if (device_type->isChecked())
						resolution_label->setText(tr("Resolution ( 0.7-3.0 )"));
				 
				}
			}

	}
		break;
	case ui_state_scanner_closed:
	{

		calibrate_group->setEnabled(false);
		scan_group->setEnabled(false);
		close_scanner->setEnabled(false);
		open_scanner->setEnabled(true);
		device_type->setEnabled(true);
	}
		break;
	case ui_state_preview_scaning:
	{
		cur_face_count = 0;
		align_group->setEnabled(false);
		set_scan_param->setEnabled(false);
		start_scan->setEnabled(true);
		start_scan->setText(tr("StartScan"));
		pause_scan->setEnabled(false);
		finish_scan->setEnabled(false);
		cancel_scan->setEnabled(false);
		save_whole_data->setEnabled(false);
		save_whole_mark_data->setEnabled(false);
		delete_point->setEnabled(false);
		point_index0_start->setEnabled(false);
		point_index0_end->setEnabled(false);
		point_index1_start->setEnabled(false);
		point_index1_end->setEnabled(false);
		point_index2_start->setEnabled(false);
		point_index2_end->setEnabled(false);
		mesh_data->setEnabled(false);

		break;
	}
	case ui_state_scanning:
	{
            cur_face_count = 0;
			align_group->setEnabled(false);
			set_scan_param->setEnabled(false);
			start_scan->setEnabled(false);
			pause_scan->setEnabled(true);
			finish_scan->setEnabled(true);
			cancel_scan->setEnabled(true);
			save_whole_data->setEnabled(false);
			save_whole_mark_data->setEnabled(false);
			delete_point->setEnabled(false);
			point_index0_start->setEnabled(false);
			point_index0_end->setEnabled(false);
			point_index1_start->setEnabled(false);
			point_index1_end->setEnabled(false);
			point_index2_start->setEnabled(false);
			point_index2_end->setEnabled(false);
			mesh_data->setEnabled(false);

			break;
	}
	case ui_state_scanning_paused:
	{
			align_group->setEnabled(false);
            set_scan_param->setEnabled(false);
			start_scan->setEnabled(true);
			pause_scan->setEnabled(false);
			finish_scan->setEnabled(true);
			cancel_scan->setEnabled(true);
			save_whole_data->setEnabled(true);
			save_whole_mark_data->setEnabled(true);
			delete_point->setEnabled(true);
			point_index0_start->setEnabled(true);
			point_index0_end->setEnabled(true);
			point_index1_start->setEnabled(true);
			point_index1_end->setEnabled(true);
			point_index2_start->setEnabled(true);
			point_index2_end->setEnabled(true);
			mesh_data->setEnabled(false);

			save_whole_data->setText(tr("save (not optimize point cloud)"));

			break;
	}
	case ui_state_scanning_finished:

	{       
             align_group->setEnabled(false);
			set_scan_param->setEnabled(false);
			start_scan->setEnabled(true);
			pause_scan->setEnabled(false);
			finish_scan->setEnabled(false);

			cancel_scan->setEnabled(whole_point_count>0?true:false);
			save_whole_data->setEnabled(whole_point_count>0 ? true : false);
			save_whole_mark_data->setEnabled(whole_point_count>0 ? true : false);
			mesh_data->setEnabled(whole_point_count>0 ? true : false);
			delete_point->setEnabled(false);
			point_index0_start->setEnabled(false);
			point_index0_end->setEnabled(false);
			point_index1_start->setEnabled(false);
			point_index1_end->setEnabled(false);
			point_index2_start->setEnabled(false);
			point_index2_end->setEnabled(false);
			close_scanner->setEnabled(true);
			save_whole_data->setText(tr("save (optimize point cloud)"));

			break;
	}
	case ui_state_scanning_canceled:
	{
            align_group->setEnabled(true);
			set_scan_param->setEnabled(true);
			start_scan->setEnabled(true);
			pause_scan->setEnabled(false);
			finish_scan->setEnabled(false);
			cancel_scan->setEnabled(false);
			save_whole_data->setEnabled(false);
			save_whole_mark_data->setEnabled(false);
			delete_point->setEnabled(false);
			point_index0_start->setEnabled(false);
			point_index0_end->setEnabled(false);
			point_index1_start->setEnabled(false);
			point_index1_end->setEnabled(false);
			point_index2_start->setEnabled(false);
			point_index2_end->setEnabled(false);
			mesh_data->setEnabled(false);
			break;
	}
	case ui_state_mesh_processing:
{
			cur_face_count = 0;
			align_group->setEnabled(false);
			set_scan_param->setEnabled(false);
			start_scan->setEnabled(false);
			pause_scan->setEnabled(false);
			finish_scan->setEnabled(false);
			cancel_scan->setEnabled(false);
			save_whole_data->setEnabled(false);
			save_whole_mark_data->setEnabled(false);
			delete_point->setEnabled(false);
			point_index0_start->setEnabled(false);
			point_index0_end->setEnabled(false);
			point_index1_start->setEnabled(false);
			point_index1_end->setEnabled(false);
			point_index2_start->setEnabled(false);
			point_index2_end->setEnabled(false);
			mesh_data->setEnabled(false);

			close_scanner->setEnabled(false);
			break;
	}


	}

	current_ui_sate = state;
	return 0;
}

void sdk_demo_pro::on_open_scanner()
{
	current_type = init_type_list->currentData().toInt();

	bool einscan_plus = device_type->isChecked();
	SN3D_INIT_DATA init_data = { 0 };
	init_data.device_type = "EinScan-Pro";
	if (einscan_plus)
		init_data.device_type = "EinScan-Plus";
	int ret = 0;
	if (handle != 0)
	{
		ret = sn3d_close(handle);
		handle = 0;
		show_error(ret);
	}
	qDebug() << "init_type=" << current_type;
	ret = sn3d_Initialize(current_type, &init_data, handle);
	
	if (ret == SN3D_RET_NOERROR)
	{
		change_ui_state(ui_state_open_scanner_ok);
		//
		sn3d_regist_callback(handle, scan_demo_callback, this);
		if (current_type != SN3D_INIT_CALIBRATE)
		{
			int min_brigthness = 0;
			int max_brigthness = 16;

			if (SN3D_RET_NOERROR == sn3d_get_brightness_range(handle, min_brigthness, max_brigthness))
			{
				brightness_list->clear();
				for (int i = min_brigthness; i <= max_brigthness; i++)
				{
					brightness_list->addItem(QString("%1").arg(i), i);
				}
			}
			//get scan config data 
			update_brightness();
			update_scan_param(true);
		}
	}
	show_error(ret, false);
}
void sdk_demo_pro::on_close_scanner()
{
	int ret = 0;
	if (handle != 0)
	{
		ret = sn3d_close(handle);
		handle = 0;
		show_error(ret,false);
	}
	change_ui_state(ui_state_scanner_closed);
	reset_calibrate_distance();
	demo_data_center::Instance()->clear_data();
	clear_show_data();
	
}
void sdk_demo_pro::on_start_calibrate()
{
	int ret = 0;
	if (handle != 0)
	{
		ret = sn3d_start_calibrate(handle);
		show_error(ret);
		bool pause=start_calibrate->property("calibrate_paused").toBool();
	/*	if (pause)
		reset_calibrate_distance();*/
		start_calibrate->setProperty("calibrate_paused", false);
		if (ret == SN3D_RET_NOERROR)
		{
			//start_calibrate->setEnabled(false);
		}
	}
}

void sdk_demo_pro::on_skip_calibrate()
{
	int ret = 0;
	if (handle != 0)
	{
		ret = sn3d_skip_calibrate(handle);
		show_error(ret);
		if (ret == SN3D_RET_NOERROR)
		{
			start_calibrate->setEnabled(true);
			start_calibrate->setProperty("calibrate_paused", false);
		}
		reset_calibrate_distance();
	}
}

void sdk_demo_pro::resolution_value_change(const QString& text)
{
	QString msg = tr("Suggest value range 0.5-3.0");
	double minValue = 0.5;
	if (current_type == SN3D_INIT_HD_SCAN)
	{
		msg = tr("Suggest value range 0.2-3.0");
		minValue = 0.2;
	}
	bool ok = false;
	

	double value = text.toDouble(&ok);
	 
	if (!ok || value < minValue || value>3.0)
	{
		QPoint  left_top = point_resolution_value->pos();
		left_top.setY(left_top.y() + 5);
		left_top = point_resolution_value->parentWidget()->mapToGlobal(left_top);

		QRect rt;
		rt.setTopLeft(left_top);
		rt.setHeight(30);
		rt.setWidth(100);

		QToolTip::showText(left_top, msg, point_resolution_value, rt, 1000);
	}

}
void sdk_demo_pro::on_get_scan_param()
{
	if (handle)
	{
		int ret = update_scan_param();
		show_error(ret);
	}
}
void sdk_demo_pro::on_global_mark_point_toggled(bool toggle)
{
	if (align_mode_global_mark_point->property("not_popup").toInt() == 1 && toggle)
	{
		align_mode_global_mark_point->setProperty("not_popup", 0);
		return;
	}
	if (!toggle)
		return;

	QString filename_ = QFileDialog::getOpenFileName(this, tr("import global mark point file"), "", tr("(*.txt)"));
	if (!filename_.isNull()){

		QFile file(filename_);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QMessageBox::warning(nullptr, tr("failed"), tr("Open file failed"));
			return;
		}

		QByteArray data = file.readLine();

		data = file.readLine();
		data = file.readLine();
		data = file.readLine();

		std::vector<SN3D_POINT_DATA> points;
		std::vector<SN3D_POINT_DATA> normals;
		int index = 0;
		while (data.length() > 0)
		{
			SN3D_POINT_DATA pt = { 0 };
			SN3D_POINT_DATA pt_normal = { 0 };
			int count = sscanf(data.data_ptr()->data(), "%f %f %f %f %f %f", &pt.x, &pt.y, &pt.z, &pt_normal.x, &pt_normal.y, &pt_normal.z);
			points.push_back(pt);
			if (count == 6)
				normals.push_back(pt_normal);

			qDebug() << "index=" << index << " x=" << pt.x << "y=" << pt.y << "z=" << pt.z << " nx=" << pt_normal.x << "ny=" << pt_normal.y << "nz=" << pt_normal.z;
			index++;
			data = file.readLine();
		}

		SN3D_CLOUD_POINT  mark_point = { 0 };

		qDebug() << "index=" << index << " size=" << points.size();
		if (points.size() > 0)
		{
			mark_point.vertex_count = points.size();
			mark_point.vertex_data = &points[0];
		}
		if (normals.size() > 0)
		{
			mark_point.norma_count = normals.size();
			mark_point.norma_data = &normals[0];
		}

		int ret = sn3d_import_global_mark_point(handle, mark_point);

		show_error(ret);
		if (ret == SN3D_RET_NO_DATA_ERROR)
		demo_data_center::Instance()->set_align_mode(SN3D_ALIGN_MODE_MARK_POINT);
	}
	else
	{
		QMessageBox::warning(NULL, tr("Warning"), tr("Global align mode must import global markpoint data"));
		align_mode_markpoint->setChecked(true);
		demo_data_center::Instance()->set_align_mode(SN3D_ALIGN_MODE_MARK_POINT);
	}
}
 
void sdk_demo_pro::on_set_scan_param()
{
	if (handle)
	{
		SN3D_SCAN_PARAM param = { 0 };
		param.resolution = point_resolution_value->text().toDouble();
		param.flag_texture = texture->isChecked() ? 1 : 0;
	
		if (current_type == SN3D_INIT_HD_SCAN)
		{
			param.align_mode = SN3D_ALIGN_MODE_MARK_POINT; //only surpport mark point
			if (align_mode_global_mark_point->isChecked())
			{
				param.align_mode = SN3D_ALIGN_MODE_GLOABL_MARK_POINT;
			}
			param.flag_texture = 0;// not surpport texture 
		}
		else
		{
			int align_mode = SN3D_ALIGN_MODE_FEATURE;
			if (align_mode_markpoint->isChecked())
			{
				align_mode = SN3D_ALIGN_MODE_MARK_POINT;
			}
			else if (align_mode_global_mark_point->isChecked())
			{
				align_mode = SN3D_ALIGN_MODE_GLOABL_MARK_POINT;
			}
			param.align_mode = align_mode;

		}
		demo_data_center::Instance()->set_align_mode(param.align_mode);
		int ret = sn3d_set_scan_param(handle, &param);

		qDebug() << "ret=" << ret << "resolution=" << param.resolution << "flag_texture=" << param.flag_texture << "align_mode=" << param.align_mode;

		show_error(ret);
		clear_show_data();
		start_scan->setText(tr("StartPreview"));
	}
}

void sdk_demo_pro::on_start_scan()
{
	if (handle)
	{
		
		int ret = sn3d_start_scan(handle);
		
		if (ret == SN3D_RET_NOERROR)
		{
			if (start_scan->text()==tr("StartScan"))
			  change_ui_state(ui_state_scanning);
			else if (start_scan->text() == tr("StartPreview"))
				change_ui_state(ui_state_preview_scaning);
		}
		else
		{
			show_error(ret);
		}
		
	}
}
void sdk_demo_pro::on_pause_scan()
{
	if (handle)
	{
		int ret = sn3d_pause_scan(handle);
		show_error(ret);
		if (ret == SN3D_RET_NOERROR)
			change_ui_state(ui_state_scanning_paused);
	}
}
void sdk_demo_pro::on_finish_scan()
{
	if (handle)
	{
		int ret = sn3d_finish_scan(handle);
		show_error(ret);
		if (ret == SN3D_RET_NOERROR)
			change_ui_state(ui_state_scanning_finished);
	}
}
void sdk_demo_pro::on_cancel_scan()
{
	if (handle)
	{
		int ret = sn3d_abandon_scan(handle);
		show_error(ret);
		 
		if (ret == SN3D_RET_NOERROR)
			change_ui_state(ui_state_scanning_canceled);

	 
		QApplication::processEvents();
		clear_show_data();

	}
}
void sdk_demo_pro::on_mesh_data()
{
	if (handle)
	{
		mesh_param_dialog mesh_dlg;
		if (mesh_dlg.exec() != 1)
		{
			return;
		}
		
		change_ui_state(ui_state_mesh_processing);

		/*SN3D_SCAN_PARAM scan_param = { 0 };
		int ret = sn3d_get_scan_param(handle, scan_param);
		if (ret == SN3D_RET_NOERROR)
		{
			if (scan_param.align_mode != SN3D_ALIGN_MODE_MARK_POINT)
				mesh_dlg.disableMark();
		}*/


		SN3D_MESH_PROCESS_PARAM param = { 0 };

	
		mesh_dlg.get_param_config(param);

		int ret = sn3d_mesh_process(handle, param);
		show_error(ret);

		if (ret != SN3D_RET_NOERROR)
		{
			change_ui_state(ui_state_scanning_finished);
		}
	}
}
void sdk_demo_pro::on_save_whole_data()
{
	QString filename_ = QFileDialog::getSaveFileName(this, tr("save file"), "", tr("(*.asc)"));
	if (!filename_.isNull()){
		demo_data_center::Instance()->save_whole_point(filename_.toLocal8Bit().data());
	}
}
//void sdk_demo_pro::on_save_mesh_data()
//{
//	QString filename_ = QFileDialog::getSaveFileName(this, tr("save file"), "", tr("(*.ply)"));
//	if (!filename_.isNull()){
//		demo_data_center::Instance()->save_whole_point(filename_.toLocal8Bit().data());
//	}
//}

void sdk_demo_pro::on_save_whole_mark_data()
{
	QString filename_ = QFileDialog::getSaveFileName(this, tr("save file"), "", tr("(*.txt)"));
	if (!filename_.isNull()){
		demo_data_center::Instance()->save_whole_mark_point(filename_.toLocal8Bit().data());
	}
}
void sdk_demo_pro::on_delete_point()
{
	if (handle)
	{
		bool ok = false;
		int start0 = point_index0_start->text().toInt(&ok);
		if (!ok)
			start0 = -1;
		int end0 = point_index0_end->text().toInt(&ok);
		if (!ok)
			end0 = -1;

		int start1 = point_index1_start->text().toInt(&ok);
		if (!ok)
			start1 = -1;
		int end1 = point_index1_end->text().toInt(&ok);
		if (!ok)
			end1 = -1;

		int start2 = point_index2_start->text().toInt(&ok);
		if (!ok)
			start2 = -1;
		int end2 = point_index2_end->text().toInt(&ok);
		if (!ok)
			end2 = -1;

		SN3D_CLOUD_POINT remain_point = { 0 };
		int result = demo_data_center::Instance()->delete_point(start0, end0, start1, end1, start2, end2, remain_point);

		if (result == 0)
		{
			int ret = sn3d_update_cloud_point(handle, remain_point);
			show_error(ret);
			whole_point_count = remain_point.vertex_count;
			show_data();

		
		}
	
	}
}

void sdk_demo_pro::on_brightness_list_change(int index)
{
	if (handle)
	{
		int value = brightness_list->itemData(index).toInt();
		int ret = sn3d_set_brightness(handle, value);
		show_error(ret);
	}
}
void sdk_demo_pro::handle_msg(msg_data_t msg_data)
{
	switch (msg_data.msg_type)
	{
	case SN3D_VIDEO_IMAGE_DATA_READY:
		handle_update_video(msg_data);
		break;
	case SN3D_DISTANCE_INDECATE:
		handle_update_distance_indecate(msg_data);
		break;
	case SN3D_SCANNER_RT_READY:
		handle_update_scanner_rt(msg_data);
		break;
	case SN3D_CURRENT_MARKPOINT_DATA_READY:
		handle_update_current_mark_cloud_point(msg_data);
		break;
	case SN3D_WHOLE_MARKPOINT_DATA_READY:
		handle_update_whole_mark_cloud_point(msg_data);
		break;
	case SN3D_CURRENT_SCAN_POINT_CLOUD_READY:
		handle_update_current_cloud_point(msg_data);
		break;
	case SN3D_WHOLE_SCAN_POINT_CLOUD_READY:
		handle_update_whole_cloud_point(msg_data);
		break;
	case SN3D_CALIBRATION_STATE_DATA:
		handle_calibrate_msg(msg_data);
		break;
	case SN3D_SCANNER_DOUBLECLICK:
		handle_scanner_double_click_msg(msg_data);
		break;
	case SN3D_SCANNER_CLICK:
		handle_scanner_click_msg(msg_data);
		break;
	case SN3D_SCANNER_PLUS:
	case SN3D_SCANNER_SUB:
		handle_scanner_plus_sub_msg(msg_data);
		break;
	case SN3D_MESH_DATA_READY:
		handle_update_mesh_data(msg_data);
		break;

	default:
		break;
	}
}
int sdk_demo_pro::handle_update_scanner_rt(msg_data_t msg_data)
{
	SN3D_SCANNER_RT rt = { 0 };
	demo_data_center::Instance()->get_scanner_rt(rt);

	//qDebug() << "\r\n rot=" << rt.rotate[0] << " " << rt.rotate[1] << " " << rt.rotate[2]
	//	<< " " << rt.rotate[3] << " " << rt.rotate[4] << " " << rt.rotate[5]
	//	<< " " << rt.rotate[6] << " " << rt.rotate[7] << " " << rt.rotate[9]
	//	<< "trans= " << rt.trans[0] << " " << rt.trans[1] << " " << rt.trans[2];

	return 0;
}
int sdk_demo_pro::handle_update_current_cloud_point(msg_data_t msg_data)
{
	cur_point_count = msg_data.map_data["param"].toInt();
	show_data();


	return 0;
}
int sdk_demo_pro::handle_update_whole_cloud_point(msg_data_t msg_data)
{
	whole_point_count = msg_data.map_data["param"].toInt();
	show_data();
	return 0;
}
int sdk_demo_pro::handle_update_current_mark_cloud_point(msg_data_t msg_data)
{
	marker_point_count = msg_data.map_data["param"].toInt();
	show_data();
	return 0;
}
int sdk_demo_pro::handle_update_mesh_data(msg_data_t msg_data)
{
	cur_face_count = msg_data.map_data["param"].toInt();
	show_data();
	change_ui_state(ui_state_scanning_finished);
	if (cur_face_count>0)
	    QMessageBox::information(this, tr("Notice"), tr("Mesh  Success"), tr("OK"));
	else
		QMessageBox::information(this, tr("Notice"), tr("Mesh  Failed"), tr("OK"));
	return 0;
}
int sdk_demo_pro::handle_update_whole_mark_cloud_point(msg_data_t msg_data)
{
	whole_marker_point_count = msg_data.map_data["param"].toInt();
	show_data();
	return 0;
}
int sdk_demo_pro::handle_update_distance_indecate(msg_data_t msg_data)
{
	distance_value = msg_data.map_data["param"].toInt();
	/*printf("\nhandle_update_distance_indecate distance_value=%d", distance_value);*/
	show_data();
	return 0;
}
int sdk_demo_pro::handle_scanner_double_click_msg(msg_data_t msg_data)
{
	qDebug() << "scanner double click ";
	return 0;
}
int sdk_demo_pro::handle_scanner_click_msg(msg_data_t msg_data)
{
	int pause_flag = msg_data.map_data["param"].toInt();
	if (pause_flag == 0)
		on_start_scan();
	else
		on_pause_scan();

	return 0;
}
int sdk_demo_pro::handle_scanner_plus_sub_msg(msg_data_t msg_data)
{
	int current_index = brightness_list->currentIndex();
	if (msg_data.msg_type == SN3D_SCANNER_PLUS)
	{
		current_index++;
	}
	else
	{
		current_index--;
	}
	if (current_index <= 0) current_index = 0;
	if (current_index >= brightness_list->count())
		current_index = brightness_list->count() - 1;

	brightness_list->setCurrentIndex(current_index);
	return 0;
}
int sdk_demo_pro::handle_calibrate_msg(msg_data_t msg_data)
{
	int current_calibrate = msg_data.map_data["current_calibrate"].toInt();
	int distance = msg_data.map_data["distance"].toInt();
	int group = msg_data.map_data["group"].toInt();
	int snap_state = msg_data.map_data["snap_state"].toInt();
	int compute_state = msg_data.map_data["compute_state"].toInt();


	QString str_compute_state = tr(" ");
 
	QString cur_calibrate_type = tr("Current: Camera Calibrate");
	if (current_calibrate == SN3D_CALIBRATE_STATE_HD)
	{
		cur_calibrate_type = tr("Current: HD Calibrate");
	}
	else if (current_calibrate == SN3D_CALIBRATE_STATE_WB)
	{
		cur_calibrate_type = tr("Current: WB Calibrate");
	}
	else if (current_calibrate == SN3D_CALIBRATE_STATE_EXIT)
	{
		cur_calibrate_type = tr("Current: Calibrate Finished ");
		start_calibrate->setEnabled(false);
		skip_calibrate->setEnabled(false);
		str_compute_state = tr("Please Close The Device");
	}


	QString str_snap_sate = tr(" ");
	if (snap_state == SN3D_CALIBRATE_SNAP_STATE_ON)
	{
		str_snap_sate = tr("Snap On");
		//start_calibrate->setEnabled(false);
		start_calibrate->setProperty("calibrate_paused", false);
		
	}
	else if (snap_state == SN3D_CALIBRATE_SNAP_STATE_OFF)
	{
		str_snap_sate = tr("Snap Off");
		start_calibrate->setEnabled(true);
		start_calibrate->setProperty("calibrate_paused",true);
	}

	if (compute_state == SN3D_CALIBRATE_COMPUTING)
	{
		str_compute_state = tr("Computing ");
		skip_calibrate->setEnabled(false);
		start_calibrate->setEnabled(false);
		start_calibrate->setProperty("calibrate_paused", false);
		close_scanner->setEnabled(false);
	}
	else if (compute_state == SN3D_CALIBRATE_COMPUTE_SUCCESS)
	{
		str_compute_state = tr("Compute sucesse ");
		skip_calibrate->setEnabled(true);
		start_calibrate->setEnabled(true);
		start_calibrate->setProperty("calibrate_paused", false);
		close_scanner->setEnabled(true);
	}
	else if (compute_state == SN3D_CALIBRATE_COMPUTE_FAILED)
	{
		str_compute_state = tr("Compute failed ");
		skip_calibrate->setEnabled(true);
		start_calibrate->setEnabled(true);
		close_scanner->setEnabled(true);
		start_calibrate->setProperty("calibrate_paused", false);
	}

	if (current_calibrate == SN3D_CALIBRATE_STATE_CAMERA && group != SN3D_CALIBRATE_INVALID )
	{
		cur_calibrate_type += QString(" %1/5").arg(group);
	}
	 
	calibrate_state->setText(str_snap_sate);
	calibrate_type->setText(cur_calibrate_type);
	calibrate_prompt->setText(str_compute_state);

	set_calibrate_state(current_calibrate, distance, group);

	if (current_calibrate != current_calibrate_type)
	{
		current_calibrate_type = current_calibrate;
		reset_calibrate_distance();
		
	}
	if (group != SN3D_CALIBRATE_INVALID && group != current_camera_group)
	{
		current_camera_group = group;
		QApplication::processEvents();
		_sleep(1000);
		reset_calibrate_distance();
		start_calibrate->setProperty("calibrate_paused", false);
	}
	return 0;
}
void sdk_demo_pro::set_calibrate_state(int calibrate_type, int distance_index, int group)
{
	calibrate_state_manager->setCurrentIndex(calibrate_type);

	if (calibrate_type == SN3D_CALIBRATE_STATE_CAMERA && distance_index >= 0 && distance_index <= 4)
	{
		for (int i = 0; i < 5; i++)
		{
			QString str_color="background-color:rgb(199,199,199);";
			if (camera_distance[i]->property("state").toInt() == 1)
			{
				str_color = "background-color:rgb(0,255,0);";
			}
			camera_distance[i]->setStyleSheet(str_color);
		}
		camera_distance[4 - distance_index]->setProperty("state", 1);
		camera_distance[4 - distance_index]->setStyleSheet("background-color:rgb(0,0,255);");
	}

	if (calibrate_type == SN3D_CALIBRATE_STATE_HD && distance_index >= 0 && distance_index <= 11)
	{
		for (int i = 0; i < 7; i++)
		{
			QString str_color = "background-color:rgb(199,199,199);";
			if (hd_distance[i]->property("state").toInt() == 1)
			{
				str_color = "background-color:rgb(0,255,0);";
			}
			hd_distance[i]->setStyleSheet(str_color);
		}

		hd_distance[6 - distance_index]->setStyleSheet("background-color:rgb(0,0,255);");
		hd_distance[6 - distance_index]->setProperty("state", 1);
	}

	if (calibrate_type == SN3D_CALIBRATE_STATE_WB && distance_index >= 0 && distance_index <= 11)
	{
		for (int i = 0; i < 7; i++)
		{
			wb_distance[i]->setStyleSheet("background-color:rgb(199,199,199);");
		}
		qDebug() << "distance_index=" << distance_index;
		wb_distance[6 - distance_index]->setStyleSheet("background-color:rgb(0,0,255);");
		 
	}

}

void sdk_demo_pro::reset_calibrate_distance()
{
	for (int i = 0; i < 5; i++)
	{
		camera_distance[i]->setStyleSheet("background-color:rgb(199,199,199);");
		camera_distance[i]->setProperty("state", 0);
	}
	for (int i = 0; i < 7; i++)
	{
		hd_distance[i]->setStyleSheet("background-color:rgb(199,199,199);");
		hd_distance[i]->setProperty("state", 0);
	}
	for (int i = 0; i < 7; i++)
	{
		hd_distance[i]->setProperty("state", 0);
		wb_distance[i]->setStyleSheet("background-color:rgb(199,199,199);");
	 
	}
}
QWidget*  sdk_demo_pro::init_calibrate_distance()
{
	QWidget*  cam_widget = new  QWidget();
	QVBoxLayout* v_cam = new QVBoxLayout();
	for (int i = 0; i < 5; i++)
	{
		camera_distance[i] = new QLabel();

		camera_distance[i]->setFixedHeight(30);
		camera_distance[i]->setStyleSheet("background-color:rgb(199,199,199);");
		v_cam->addWidget(camera_distance[i]);
		
	}
	v_cam->addStretch();
	cam_widget->setLayout(v_cam);

	QWidget*  hd_widget = new  QWidget();
	QVBoxLayout* v_hd = new QVBoxLayout();
	v_hd->setSpacing(2);
	for (int i = 0; i < 7; i++)
	{
		hd_distance[i] = new QLabel();
		hd_distance[i]->setFixedHeight(15);
		hd_distance[i]->setStyleSheet("background-color:rgb(199,199,199);");
		v_hd->addWidget(hd_distance[i]);
	
	}
	v_hd->addStretch();
	hd_widget->setLayout(v_hd);

	QWidget*  wb_widget = new  QWidget();
	QVBoxLayout* v_wb = new QVBoxLayout();
	v_wb->setSpacing(2);
	for (int i = 0; i < 7; i++)
	{
		wb_distance[i] = new QLabel();
		wb_distance[i]->setFixedHeight(15);
		wb_distance[i]->setStyleSheet("background-color:rgb(199,199,199);");
		v_wb->addWidget(wb_distance[i]);
	
	}
	v_wb->addStretch();
	wb_widget->setLayout(v_wb);
	calibrate_state_manager = new QStackedWidget();
	calibrate_state_manager->setFixedHeight(400);
	calibrate_state_manager->addWidget(cam_widget);
	calibrate_state_manager->addWidget(hd_widget);
	calibrate_state_manager->addWidget(wb_widget);
	calibrate_state_manager->setCurrentIndex(0);

	return calibrate_state_manager;
}
void sdk_demo_pro::show_data()
{
	QString point = QString("current point=%1 whole point=%2\ncurrent mark point=%3 whole mark point=%4 triangles=%5").arg(cur_point_count).arg(whole_point_count).arg(marker_point_count).arg(whole_marker_point_count).arg(cur_face_count);

	QString dis;
	if (distance_value == 0)
	{
		dis = tr("Too Close");
	}
	if (distance_value == 11)
	{
		dis = tr("Too Far");
	}
	if (distance_value == 12)
	{
		dis = tr("Track Lost");
	}
	if (distance_value >= 1 && distance_value <= 10)
	{
		dis = QString("distance normal=%1").arg(distance_value);
	}

	scan_distance->setText(dis);
	scan_point_count->setText(point);

}
int sdk_demo_pro::update_brightness()
{
	int ret = 0;
	if (handle)
	{
		int brightness = 0;
		ret = sn3d_get_brightness(handle, brightness);
		qDebug() << "ret=" << ret << "brightness=" << brightness;
		brightness_list->setCurrentIndex(brightness);
	}
	return ret;
}

void sdk_demo_pro::clear_show_data()
{
	demo_data_center::Instance()->clear_data();
	cur_point_count=0;
	whole_point_count=0;
	distance_value=6;
	marker_point_count=0;
	whole_marker_point_count=0;
	cur_face_count = 0;
	show_data();
}
int sdk_demo_pro::update_scan_param(bool first )
{
	int ret = 0;
	if (handle)
	{
		SN3D_SCAN_PARAM param = { 0 };
		ret = sn3d_get_scan_param(handle, param);

		qDebug() << "ret=" << ret << "resolution=" << param.resolution << "flag_texture=" << param.flag_texture << "align_mode=" << param.align_mode;
		point_resolution_value->setText(QString("%1").arg(param.resolution));
		if (param.align_mode == SN3D_ALIGN_MODE_FEATURE)
			align_mode_feature->setChecked(true);
		else if (param.align_mode == SN3D_ALIGN_MODE_GLOABL_MARK_POINT)
		{
			if (first)//if first select global_mark_point mode ,but not load global marker points
			{
				align_mode_markpoint->setChecked(true);

				SN3D_SCAN_PARAM param_set = param;
				param_set.align_mode = SN3D_ALIGN_MODE_MARK_POINT;
				sn3d_set_scan_param(handle, &param_set);
			}
			else
			{
				align_mode_global_mark_point->setProperty("not_popup", 1);
				align_mode_global_mark_point->setChecked(true);
				align_mode_global_mark_point->setProperty("not_popup", 0);
			}
			
		}
		else
			align_mode_markpoint->setChecked(true);

		if (param.flag_texture == 1)
			texture->setChecked(true);
		else
			texture->setChecked(false);
		float near_min = 200;
		float far_max = 800.0;
		sn3d_get_near_far_dis(handle, near_min, far_max);
		far_max_edit->setText(QString("%1").arg(far_max));
		near_min_edit->setText(QString("%1").arg(near_min));
	}
	return ret;
}
void sdk_demo_pro::on_get_scan_range()
{
	float near_min = 200;
	float far_max = 800.0;
	if (handle)
	{
		int ret = sn3d_get_near_far_dis(handle, near_min, far_max);
		far_max_edit->setText(QString("%1").arg(far_max));
		near_min_edit->setText(QString("%1").arg(near_min));
		show_error(ret, true);
	}

}
void sdk_demo_pro::on_set_scan_range()
{
	float far_max = far_max_edit->text().toFloat();
	float near_min = near_min_edit->text().toFloat();
	if (handle)
	{
		int ret = sn3d_set_near_far_dis(handle, near_min, far_max);
		show_error(ret, false);
	}
	
}
void sdk_demo_pro::show_error(int err_code,bool only_show_err)
{
	QString str_err = tr("execute ok");
	switch (err_code)
	{
	case SN3D_RET_PARAM_ERROR:
		str_err = tr("param error");
		break;
	case SN3D_RET_ORDER_ERROR:
		str_err = tr("invoke order error");
		break;
	case SN3D_RET_TIME_OUT_ERROR:
		str_err = tr("invoke time out error");
		break;
	case SN3D_RET_NOT_SUPPORT_ERROR:
		str_err = tr("not support command error");
		break;
	case SN3D_RET_NO_DEVICE_ERROR:
		str_err = tr("not connect device error");
		break;
	case SN3D_RET_DEVICE_LICENSE_ERROR:
		str_err = tr("license error");
		break;
	case SN3D_RET_GPU_ERROR:
		str_err = tr("Graphics card incompatible with scanner OR the graphics driver is out-of-date! \nPlease follow the prompts and try again:\n(1) Ensure graphics card is equivalent to NVIDIA GTX series 660 or higher;\n(2) Update driver to latest version.");
		break;
	case SN3D_RET_INNER_ERROR:
		str_err = tr("SDK inner Error");
		break;
	case SN3D_RET_NOT_CALIBRATE_ERROR:
		str_err = tr("No Calibrate Data");
		break;
	case SN3D_RET_LOST_CONFIG_FILE_ERROR:
		str_err = tr("Config File Lost");
		break;
	case SN3D_RET_NO_DATA_ERROR:
		str_err = tr("No Scan Data");
		break;
	case SN3D_RET_LOST_CALIBRATE_FILE_ERROR:
		str_err = tr("Calibrate File Lost");
		break;
	case SN3D_RET_NO_GLOBAL_MARK_POINT_PARAM_ERROR:
		str_err = tr("no global mark point data");
		break;
	default:
		break;

	}

	if (only_show_err)
	{
		if (err_code != SN3D_RET_NOERROR)
			QMessageBox::warning(NULL, tr("Warning"), str_err);
	}
	else
	{
		QMessageBox::warning(NULL, tr("Warning"), str_err);
	}
	

}

