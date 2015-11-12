#include "DataMotion.h"

using namespace cv;
using namespace std;

// 행렬 구성
//
// Transition Matrix
// |1 0 1 0
// |0 1 0 1
// |0 0 1 0
// |0 0 0 1
//
// 데이터 행렬
// |Vx  Sx
// |Vx  Sy
// |dVx dSx
// |dVy dSy
//
// 측정 행렬
// |Vx  Sx
// |Vx  Sy
// |0   0
// |0   0
//
// |
// |
// |
// |
//




void initKalman(KalmanFilter* KalmanStructure)
{
	// Transition Matrix (A)
	// |1 0 1 0
	// |0 1 0 1
	// |0 0 1 0
	// |0 0 0 1
	KalmanStructure->transitionMatrix = (Mat_<float>(4,4) << 1,0,1,0,  0,1,0,1,  0,0,1,0,  0,0,0,1);

	// Process Noise Covarience Matrix
	// |0.2 0    0.2  0
	// |0   0.2  0    0.2
	// |0   0    0.3  0
	// |0   0    0    0.3
	KalmanStructure->processNoiseCov = (Mat_<float>(4, 4) << 0.2,0,0.2,0, 0,0.2,0,0.2, 0,0,0.3,0, 0,0,0,0.3);

	// Measurement Matrix (H)
	// |1   0    0    0
	// |0   1    0    0
	// |0   0    0    0
	// |0   0    0    0
	KalmanStructure->measurementMatrix = (Mat_<float>(4, 4) << 1,0,0,0,  0,1,0,0,  0,0,0,0,  0,0,0,0);

	// Process Noise covarience Matrix
	// |1e-4  0     0     0
	// |0     1e-4  0     0
	// |0     0     1e-4  0
	// |0     0     0     1e-4
	//setIdentity(KalmanStructure->processNoiseCov, Scalar::all(1e-4));

	// Measurement Noise covariance Matrix
	// |1e-1  0     0     0
	// |0     1e-1  0     0
	// |0     0     1e-1  0
	// |0     0     0     1e-1
	setIdentity(KalmanStructure->measurementNoiseCov, Scalar::all(1e-1));

	// posteri error covarience Matrix
	// |0.1  0    0    0
	// |0    0.1  0    0
	// |0    0    0.1  0
	// |0    0    0    0. 1
	setIdentity(KalmanStructure->errorCovPost, Scalar::all(.1));

	// posteri error covarience Matrix
	// |0.1  0    0    0
	// |0    0.1  0    0
	// |0    0    0.1  0
	// |0    0    0    0. 1
	setIdentity(KalmanStructure->errorCovPre, Scalar::all(.1));

}

MeasurementCS DetectRectangle(Mat* Input_Image, Scalar Colour)
{
	MeasurementCS MeasureData;
	

	//최대 최소의 초기 값은 반대 방향 극한 값으로 설정
	int max_x = 0;
	int max_y = 0;
	int min_x = Cols;
	int min_y = Rows;

	int Step_y = Input_Image->step[0];
	int Step_x = Input_Image->step[1];

	for (int x = 0; x < Cols; x++)
	{
		for (int y = 0; y < Rows; y++)
		{
			for (int Channel = 0; Channel < 3; Channel++)
			{
				
				if (Input_Image->data[y*Step_y + x * Step_x + Channel] == Colour[Channel])
				{
					if (x > max_x)
						max_x = x;

					if (x < min_x)
						min_x = x;

					if (y > max_y)
						max_y = y;

					if (y < min_y)
						min_y = y;
				}
			}
		}
	}

	MeasureData.coor.x = min_x;
	MeasureData.coor.y = min_y;

	MeasureData.coor2.x = max_x;
	MeasureData.coor2.y = max_y;

	return MeasureData;
}

PredictedCS KalmanTracker(MeasurementCS MeasurementData)
{
	PredictedCS PredictData;

	KalmanFilter Tracker(4, 2, 0, CV_32F);
	Mat state(4, 1, CV_32F); /* (x , y , dx , dy) */
	Mat measurement = Mat::zeros(4, 1, CV_32SC1);
	char code = (char)-1;

	initKalman(&Tracker);

	measurement.data[0] = MeasurementData.coor.x;
	measurement.data[1] = MeasurementData.coor.y;

	Tracker.correct(measurement);
	Tracker.predict();

	PredictData.coor.x = Tracker.statePost.data[0];
	PredictData.coor.y = Tracker.statePost.data[1];

	return PredictData;
}