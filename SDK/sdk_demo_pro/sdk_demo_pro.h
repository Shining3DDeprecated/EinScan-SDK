#ifndef SDK_DEMO_PRO_H
#define SDK_DEMO_PRO_H

#include <QtWidgets/QMainWindow>
#include "ui_sdk_demo_pro.h"
#include <qlabel.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <QRadioButton>
#include <qdialog.h>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "msg_define.h"
#include <qstackedwidget.h>

class sdk_demo_pro : public QDialog
{
	Q_OBJECT

public:
	sdk_demo_pro(QWidget *parent = 0);
	~sdk_demo_pro();

	enum
	{
		ui_state_ui_init,
		ui_state_open_scanner_ok,
		ui_state_scanner_closed,
		ui_state_scanning,
		ui_state_scanning_paused,
		ui_state_scanning_finished,
		ui_state_scanning_canceled,
		ui_state_mesh_processing,
		ui_state_preview_scaning,
	};

	void scan_callback(void*  handle, int  event_type, int  event_sub_type, void* data, int data_len);

signals:
	void sig_handle_msg(msg_data_t msg_data);

protected:
	//scan message 
	int handle_update_video(msg_data_t msg_data);					 
	int handle_update_scanner_rt(msg_data_t msg_data);				 
	int handle_update_current_cloud_point(msg_data_t msg_data);		 
	int handle_update_whole_cloud_point(msg_data_t msg_data);		 
	int handle_update_current_mark_cloud_point(msg_data_t msg_data);
	int handle_update_whole_mark_cloud_point(msg_data_t msg_data);
	int handle_update_mesh_data(msg_data_t msg_data);
	//calibrate 
	int handle_calibrate_msg(msg_data_t msg_data);
	int handle_update_distance_indecate(msg_data_t msg_data);

	//scanner device message
	int handle_scanner_double_click_msg(msg_data_t msg_data);
	int handle_scanner_click_msg(msg_data_t msg_data);
	int handle_scanner_plus_sub_msg(msg_data_t msg_data);
 
	//
	int change_ui_state(int state);
	void post_msg_data_update(int type, int param);
 
protected slots:
    void handle_msg(msg_data_t msg_data);

	//
    void on_open_scanner();
	void on_close_scanner();

	//calibrate
	void on_start_calibrate();
	void on_skip_calibrate();

	//scan param 
	void on_get_scan_param();
	void on_set_scan_param();
	void on_global_mark_point_toggled(bool toggle);
	void resolution_value_change(const QString &);
	void on_brightness_list_change(int index);
	void on_get_scan_range();
	void on_set_scan_range();

	//scan control
	void on_start_scan();
	void on_pause_scan();
	void on_finish_scan();
	void on_cancel_scan();
	void on_delete_point();

	//mesh
	void on_mesh_data();

	//export data 
	void on_save_whole_data();
	void on_save_whole_mark_data();
	//void on_save_mesh_data();
 
protected:

	void set_calibrate_state(int calibrate_type, int distance_index = -1, int group = -1);

	int update_brightness();
	int update_scan_param(bool first=false);

	void show_error(int err_code, bool only_show_err = true);
	void show_data();
	void clear_show_data();
private:
	Ui::sdk_demo_proClass ui;

	QBoxLayout* init_camera_video();
	QWidget*	init_scanner();
 
	QWidget*	init_calibrate();
	QWidget*	init_scan();
	QWidget*	init_calibrate_distance();
	void		reset_calibrate_distance();

	//camera video data
	QLabel*		left_camera_video;
	QLabel*		right_camera_video;
	QLabel*		colour_camera_video;


	//open close scanner
	QGroupBox*   scanner_group;
	QPushButton* open_scanner;
	QPushButton* close_scanner;
	QLabel*      scaner_prompt;
	QComboBox*   init_type_list;
	QCheckBox*   device_type;
	//calibrate
	QGroupBox*    calibrate_group;
	QLabel*       calibrate_type;
	QLabel*       calibrate_state;
	QLabel*       calibrate_prompt;
	QPushButton*  start_calibrate;
	QPushButton*  skip_calibrate;
	int           current_calibrate_type;
	int           current_camera_group;
 
	//calbrate distance show
	QLabel*       camera_distance[5];
	QLabel*       hd_distance[12];
	QLabel*       wb_distance[12];
	QStackedWidget* calibrate_state_manager;

 
	//scan 
	QGroupBox*    scan_group;

	//scan param 
	QComboBox*    brightness_list;
	QPushButton*  get_scan_param;
	QPushButton*  set_scan_param;
	QLineEdit*    point_resolution_value;
	QCheckBox*    texture;
	QLabel*       resolution_label;
	QLineEdit*		far_max_edit;
	QLineEdit*      near_min_edit;

	//scan align mode param 
	QRadioButton* align_mode_feature;
	QRadioButton* align_mode_markpoint;
	QRadioButton* align_mode_global_mark_point;
	QGroupBox*    align_group;

	//scan control 
	QPushButton*  start_scan;
	QPushButton*  pause_scan;
	QPushButton*  finish_scan;
	QPushButton*  cancel_scan;
	QPushButton*  save_whole_data;
	QPushButton*  save_whole_mark_data;
	//QPushButton*  save_mesh_data;
	QPushButton*  mesh_data;

	//delete point group
	QPushButton*  delete_point;
	QLineEdit*    point_index0_start;
	QLineEdit*    point_index0_end;
	QLineEdit*    point_index1_start;
	QLineEdit*    point_index1_end;
	QLineEdit*    point_index2_start;
	QLineEdit*    point_index2_end;

	//data infomation
	QLabel*       scan_point_count;
	QLabel*       scan_distance;


	int           cur_point_count;
	int           whole_point_count;
	int           distance_value;
	int           marker_point_count;
	int           whole_marker_point_count;
	int           cur_face_count;
	//
	void*			handle;
	int             current_type;
	int             current_ui_sate;
};

#endif // SDK_DEMO_PRO_H
