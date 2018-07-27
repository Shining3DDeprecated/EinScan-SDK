#include "DataCenter.h"
#include <qdebug.h>
//#include <vld.h>
#include <time.h>
demo_data_center* demo_data_center::_instance=0;

demo_data_center::demo_data_center() :_mutx(QMutex::Recursive)
{
	memset(&_scanner_rt, 0, sizeof(_scanner_rt));
	memset(&_current_point, 0, sizeof(SN3D_CLOUD_POINT));
	memset(&_whole_point, 0, sizeof(SN3D_CLOUD_POINT));
	memset(&_mark_point, 0, sizeof(SN3D_CLOUD_POINT));
	memset(&_whole_mark_point, 0, sizeof(SN3D_CLOUD_POINT));
	memset(&_mesh_trim, 0, sizeof(SN3D_TRI_MESH));
	current_align_mode = SN3D_ALIGN_MODE_FEATURE;
}
demo_data_center::~demo_data_center()
{

}
 
demo_data_center* demo_data_center::Instance()
{
	if (_instance == 0)
		_instance = new demo_data_center();

	return _instance;
}
void    demo_data_center::UnInstance()
{
	if (_instance != 0)
		delete _instance;

	_instance = 0;
}
bool demo_data_center::allocate_point_memory(SN3D_CLOUD_POINT& point)
{
	 
	if (point.vertex_count > 0)
	{
		point.vertex_data = new SN3D_POINT_DATA[point.vertex_count];
	}
	if (point.norma_count > 0)
	{
		point.norma_data = new SN3D_POINT_DATA[point.norma_count];
	}
	if (point.vertex_color_count > 0)
	{
		point.vertex_color_data = new SN3D_POINT_DATA[point.vertex_color_count];
	}

	if (point.vertex_color_data == NULL && point.norma_data == NULL && point.vertex_data == NULL)
	{
		printf("\r\n  vertex_count=%d norma_data=%d vertex_color_count=%d", point.vertex_count, point.norma_count, point.vertex_color_count);
		return false;
	}
		

	return true;

}
bool demo_data_center::allocate_mesh_trim(SN3D_TRI_MESH& mesh_trim)
{

	if (mesh_trim.vertex_count > 0)
	{
		mesh_trim.vertex_data = new SN3D_POINT_DATA[mesh_trim.vertex_count];
		mesh_trim.norma_data = new SN3D_POINT_DATA[mesh_trim.vertex_count];
		mesh_trim.vertex_color_data = new SN3D_POINT_DATA[mesh_trim.vertex_count];
	}
	if (mesh_trim.face_count > 0)
	{
		mesh_trim.face_ids = new SN3D_TRI_FACE[mesh_trim.face_count];
	}
 
	if (mesh_trim.vertex_color_data == NULL || mesh_trim.norma_data == NULL || mesh_trim.vertex_data == NULL || mesh_trim.face_ids==NULL)
	{
		printf("\r\n  vertex_count=%d ", mesh_trim.vertex_count);
		return false;
	}

	return true;

}
void demo_data_center::free_mesh_trim(SN3D_TRI_MESH& mesh_trim)
{
	if (mesh_trim.vertex_data != NULL)
	{
		delete[] mesh_trim.vertex_data;
		mesh_trim.vertex_data = NULL;
	}
	if (mesh_trim.norma_data != NULL)
	{
		delete[] mesh_trim.norma_data;
		mesh_trim.norma_data = NULL;
	}
	if (mesh_trim.vertex_color_data != NULL)
	{
		delete[] mesh_trim.vertex_color_data;
		mesh_trim.vertex_color_data = NULL;
	}

	mesh_trim.vertex_count = 0;
	if (mesh_trim.face_ids != NULL)
	{
		delete[] mesh_trim.face_ids;
		mesh_trim.face_ids = NULL;
	}

	mesh_trim.face_count = 0;
	 
}
void demo_data_center::get_scanner_rt(SN3D_SCANNER_RT& rt)
{
	QMutexLocker lock(&_mutx);
	rt= _scanner_rt ;
}
void demo_data_center::update_scanner_rt(SN3D_SCANNER_RT& rt)
{
	QMutexLocker lock(&_mutx);
	_scanner_rt = rt;
}
void demo_data_center::update_current_cloud_point(SN3D_CLOUD_POINT& point)
{
	QMutexLocker lock(&_mutx);
	free_point_memory(_current_point);
	_current_point = point;

	static int current_index = 0;

	char current_file[128] = { 0 };
	sprintf(current_file, "./curernt_pointcloud%d.asc", current_index);
	FILE* data_file = fopen(current_file, "w");
	if (data_file == NULL)
		return ;
	{		 
		for (int i = 0; i<_current_point.vertex_count; i++)
		{
			fprintf(data_file, "%f    %f    %f", _current_point.vertex_data[i].x, _current_point.vertex_data[i].y, _current_point.vertex_data[i].z);

			if (_current_point.norma_data)
				fprintf(data_file, "    %f    %f    %f", _current_point.norma_data[i].x, _current_point.norma_data[i].y, _current_point.norma_data[i].z);

			if (_current_point.vertex_color_data)
				fprintf(data_file, "    %f    %f    %f", _current_point.vertex_color_data[i].x, _current_point.vertex_color_data[i].y, _current_point.vertex_color_data[i].z);

			fprintf(data_file, "\n");
		}
	}

	fclose(data_file);

	current_index++;
}
int demo_data_center::delete_point(int start0, int end0, int start1, int end1, int start2, int end2, SN3D_CLOUD_POINT& remain)
{
	QMutexLocker lock(&_mutx);
	 
	//debug test 
	//double max_x = 0;
	//double min_x = 0;

	//double max_y = 0;
	//double min_y = 0;

	//double max_z = 0;
	//double min_z = 0;

 //
	//for (int i = 0; i < _whole_point.vertex_count; i++)
	//{
	//	if (i == 0)
	//	{
	//		min_z=max_z = _whole_point.vertex_data[i].z;
	//		min_y=max_y = _whole_point.vertex_data[i].y;
	//		min_x = max_x = _whole_point.vertex_data[i].x;
	//	}
	//	if (_whole_point.vertex_data[i].z>max_z) max_z = _whole_point.vertex_data[i].z;

	//	if (_whole_point.vertex_data[i].z<min_z) min_z = _whole_point.vertex_data[i].z;

	//	if (_whole_point.vertex_data[i].y>max_y) max_y = _whole_point.vertex_data[i].y;

	//	if (_whole_point.vertex_data[i].y<min_y) min_y = _whole_point.vertex_data[i].y;

	//	if (_whole_point.vertex_data[i].x>max_x) max_x = _whole_point.vertex_data[i].x;

	//	if (_whole_point.vertex_data[i].x<min_x) min_x = _whole_point.vertex_data[i].x;

	//}

	//printf("\r\n maxz=%f minz=%f maxy=%f miny=%f maxx=%f minx=%f", max_z, min_z, max_y, min_y, max_x, min_x);
	//double center = (max_z + min_z) / 2;
	//double len = (max_z - min_z) / 4;
	//max_z = center + len;
	//min_z = center - len;

	////
	//center = (max_y + min_y) / 2;
	//len = (max_y - min_y) / 4;
	//max_y = center + len;
	//min_y = center - len;
	////
	//center = (max_x + min_x) / 2;
	//len = (max_x - min_x) / 4;

	//max_x = center + len;
	//min_x = center - len;
	//int count = 0;
	//for (int i = 0; i < _whole_point.vertex_count; i++)
	//{
	//	if (_whole_point.vertex_data[i].z <= max_z
	//		&& _whole_point.vertex_data[i].z >= min_z
	//		&& _whole_point.vertex_data[i].y <= max_y
	//		&& _whole_point.vertex_data[i].y >= min_y
	//		&& _whole_point.vertex_data[i].x <= max_x
	//		&& _whole_point.vertex_data[i].x >= min_x
	//		)
	//	{
	//		
	//	}
	//	else
	//	{
	//		count++;
	//	}
	//}
	////
	//printf("\r\n delete count=%d", count);
	//remain.vertex_count = count;
	//if (_whole_point.norma_count > 0)
	//{
	//	remain.norma_count = count;
	//}

	//if (_whole_point.vertex_color_count > 0)
	//{
	//	remain.vertex_color_count = count;
	//}

	//allocate_point_memory(remain);
	//int index = 0;
	//for (int i = 0; i < _whole_point.vertex_count; i++)
	//{
	//	if (_whole_point.vertex_data[i].z <= max_z
	//		&& _whole_point.vertex_data[i].z >= min_z
	//		&& _whole_point.vertex_data[i].y <= max_y
	//		&& _whole_point.vertex_data[i].y >= min_y
	//		&& _whole_point.vertex_data[i].x <= max_x
	//		&& _whole_point.vertex_data[i].x >= min_x
	//		)
	//	{
	//	
	//	}
	//	else
	//	{
	//		remain.vertex_data[index] = _whole_point.vertex_data[i];
	//		if (remain.norma_data)
	//			remain.norma_data[index] = _whole_point.norma_data[i];
	//		if (remain.vertex_color_data)
	//			remain.vertex_color_data[index] = _whole_point.vertex_color_data[i];

	//		index++;
	//	}
	//}

	//free_point_memory(_whole_point);
	//_whole_point = remain;
	//return 0;
	//

	int remain_count = 0;
	for (int i = 0; i < _whole_point.vertex_count; i++)
	{
		bool del = (_whole_point.vertex_data[i].id >= start0  && _whole_point.vertex_data[i].id <= end0) 
			|| (_whole_point.vertex_data[i].id >= start1  && _whole_point.vertex_data[i].id <= end1) 
			|| (_whole_point.vertex_data[i].id >= start2 && _whole_point.vertex_data[i].id <= end2);
		if (!del)
		{
			remain_count++;
		}
	}

	remain.vertex_count = remain_count;

	if (_whole_point.norma_count > 0)
	{
		remain.norma_count = remain_count;
	}

	if (_whole_point.vertex_color_count > 0)
	{
		remain.vertex_color_count = remain_count;
	}

	if (remain_count>0 && remain_count<=_whole_point.vertex_count)
	{
		allocate_point_memory(remain);
		int j = 0;
		for (int i = 0; i < _whole_point.vertex_count; i++)
		{
			bool del = (_whole_point.vertex_data[i].id >= start0  && _whole_point.vertex_data[i].id <= end0)
				|| (_whole_point.vertex_data[i].id >= start1  && _whole_point.vertex_data[i].id <= end1)
				|| (_whole_point.vertex_data[i].id >= start2 && _whole_point.vertex_data[i].id <= end2);
			if (!del)
			{
				remain.vertex_data[j] = _whole_point.vertex_data[i];
				if (remain.norma_data)
					remain.norma_data[j] = _whole_point.norma_data[i];
				if (remain.vertex_color_data)
					remain.vertex_color_data[j] = _whole_point.vertex_color_data[i];

				j++;
			}
		}
	}

	free_point_memory(_whole_point);
	_whole_point = remain;
	
	return 0;
}
 
void demo_data_center::update_whole_mark_point(SN3D_CLOUD_POINT& point)
{
	QMutexLocker lock(&_mutx);
	free_point_memory(_whole_mark_point);
	_whole_mark_point = point;
}

void demo_data_center::update_current_mark_point(SN3D_CLOUD_POINT& point)
{
	QMutexLocker lock(&_mutx);
	free_point_memory(_mark_point);
	_mark_point = point;
}
void demo_data_center::update_whole_cloud_point(SN3D_CLOUD_POINT& point)
{
	QMutexLocker lock(&_mutx);
	free_point_memory(_whole_point);
	_whole_point = point;

}
void demo_data_center::update_mesh_trim(SN3D_TRI_MESH& mesh_trim)
{
	QMutexLocker lock(&_mutx);
	free_mesh_trim(_mesh_trim);
	_mesh_trim = mesh_trim;

	//save to asc for debug
	//time_t tNow = time(NULL);
	//tm* now = localtime(&tNow);
	//char temp_file[256] = { 0 };
	//sprintf(temp_file, "./%02d-%02d-%02d.asc", now->tm_hour, now->tm_min, now->tm_sec);

	//FILE* data_file = fopen(temp_file, "w");
	//if (data_file == NULL)
	//	return ;
	// 
	//for (int i = 0; i<_mesh_trim.vertex_count; i++)
	//{
	//	fprintf(data_file, "%f    %f    %f    ", _mesh_trim.vertex_data[i].x, _mesh_trim.vertex_data[i].y, _mesh_trim.vertex_data[i].z);
	//	fprintf(data_file, "%f    %f    %f", _mesh_trim.vertex_color_data[i].x, _mesh_trim.vertex_color_data[i].y, _mesh_trim.vertex_color_data[i].z);
	//	fprintf(data_file, "\n");
	//}
	// 
	//fclose(data_file);

}
void demo_data_center::clear_data()
{
	QMutexLocker lock(&_mutx);
	free_point_memory(_whole_point);
	free_point_memory(_mark_point);
	free_point_memory(_current_point);
	free_point_memory(_whole_mark_point);
	free_mesh_trim(_mesh_trim);
}
void  demo_data_center::set_align_mode(int  align_mode)
{
	current_align_mode = align_mode;
}
int   demo_data_center::get_align_mode()
{
	return current_align_mode;
}
void demo_data_center::free_point_memory(SN3D_CLOUD_POINT& point)
{
	if (point.norma_count > 0 && point.norma_data != NULL)
	{
		//printf("\r\n point.norma_data=%d", (int)point.norma_data);
		delete[] point.norma_data;
	}
		
	point.norma_count = 0;
	point.norma_data = NULL;
	if (point.vertex_count > 0 && point.vertex_data != NULL)
	{
		delete[] point.vertex_data;
		//printf("\r\n point.vertex_data=%d", (int)point.vertex_data);
	}
		
	point.vertex_count = 0;
	point.vertex_data = NULL;

	if (point.vertex_color_count > 0 && point.vertex_color_count != NULL)
	{
		delete[] point.vertex_color_data;
		//printf("\r\n point.vertex_color_data=%d", (int)point.vertex_color_data);
	}
		
	point.vertex_color_count = 0;
	point.vertex_color_data = NULL;
}
int demo_data_center::get_whole_marker_count()
{
	QMutexLocker lock(&_mutx);
	return _whole_mark_point.vertex_count;
}
int  demo_data_center::save_whole_mark_point(char* path)
{
	FILE* data_file = fopen(path, "w");
	if (data_file == NULL)
		return -1;
	/*fprintf(data_file, "%d rows\n", _whole_mark_point.vertex_count);
	fprintf(data_file, "%d columns\n",4);
	fprintf(data_file, "#\n");*/
	{
		QMutexLocker lock(&_mutx);

		for (int i = 0; i<_whole_mark_point.vertex_count; i++)
		{
			fprintf(data_file, "%f    %f    %f",
				_whole_mark_point.vertex_data[i].x, _whole_mark_point.vertex_data[i].y, _whole_mark_point.vertex_data[i].z);

			if (_whole_mark_point.norma_data)
				fprintf(data_file, "    %f    %f    %f", _whole_mark_point.norma_data[i].x, _whole_mark_point.norma_data[i].y, _whole_mark_point.norma_data[i].z);

			fprintf(data_file, "\n");
		}
	}

	fclose(data_file);

	return 0;
}
int demo_data_center::save_whole_point(char* path)
{
	FILE* data_file = fopen(path, "w");
	if (data_file == NULL)
		return -1;
	{
		QMutexLocker lock(&_mutx);
		for (int i = 0; i<_whole_point.vertex_count; i++)
		{
			fprintf(data_file, "%f    %f    %f", _whole_point.vertex_data[i].x, _whole_point.vertex_data[i].y, _whole_point.vertex_data[i].z);

			if (_whole_point.norma_data)
				fprintf(data_file, "    %f    %f    %f", _whole_point.norma_data[i].x, _whole_point.norma_data[i].y, _whole_point.norma_data[i].z);

			if (_whole_point.vertex_color_data)
				fprintf(data_file, "    %f    %f    %f", _whole_point.vertex_color_data[i].x, _whole_point.vertex_color_data[i].y, _whole_point.vertex_color_data[i].z);

			fprintf(data_file, "\n");
		}
	}

	fclose(data_file);

	return 0;
}